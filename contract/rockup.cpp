#include "./rockup.hpp"

using namespace eosio;
using namespace std;

void rockup::createevent(name owner, name eventid, asset stakeamt, uint64_t maxatt, bool inviteonly, string about, uint32_t grace, uint32_t etime)
{
    require_auth(owner);
    event_index eventsdb(_code, _code.value);
    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr == eventsdb.end(), "event already exists");

    eosio_assert(stakeamt.symbol.is_valid(), "invalid quantity");
    eosio_assert(stakeamt.amount > 0, "only positive quantity allowed");
    eosio_assert(stakeamt.symbol == EOS_SYMBOL, "only EOS tokens allowed");

    eosio_assert(now() < etime, "etime in the past");

    eventsdb.emplace(owner, [&](auto &row) {
        row.eventid = eventid;
        row.stakeamount = stakeamt;
        row.maxatt = maxatt;
        row.eventowner = owner;
        row.open = true;
        row.inviteonly = inviteonly;
        row.grace = grace;
        row.etime = etime;
    });

    action(
        std::vector<permission_level>(),
        "dfuseiohooks"_n,
        "event"_n,
        std::make_tuple(
            // Parameter `auth_key`
            std::string(""),
            // Parameter `data`
            std::string("event=" + eventid.to_string() + "&about=" + about)))
        .send_context_free();
}

void rockup::closeevent(name eventid)
{
    event_index eventsdb(_code, _code.value);
    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    require_auth(itr->eventowner);

    eventsdb.modify(itr, same_payer, [&](auto &row) {
        row.open = false;
    });
}

void rockup::reqticket(name attendee, name eventid, name ticketid)
{
    // Do we need this?
    // require_auth(attendee);

    event_index eventsdb(_code, _code.value);
    ticket_index ticketdb(_code, eventid.value);

    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    eosio_assert(itr->open, "cannot create ticket for closed event");
    if (itr->inviteonly)
    {
        eosio_assert(has_auth(itr->eventowner), "invite only event");
    }
    auto itr2 = ticketdb.find(ticketid.value);
    eosio_assert(itr2 == ticketdb.end(), "ticket id already exists");

    ticketdb.emplace(itr->inviteonly ? itr->eventowner : attendee, [&](auto &row) {
        row.ticketid = ticketid;
        row.eventid = eventid;
        row.attendee = attendee;
        row.paid = false;
    });
}

void rockup::rollcall(name ticketid, name eventid, bool attended)
{
    ticket_index ticketsdb(_code, eventid.value);
    auto itr = ticketsdb.find(ticketid.value);
    eosio_assert(itr != ticketsdb.end(), "ticket does not exist");
    eosio_assert(itr->paid, "cannot role call on a unpaid ticket");

    event_index eventsdb(_code, _code.value);
    auto itr2 = eventsdb.find(eventid.value);
    eosio_assert(itr2 != eventsdb.end(), "event does not exist");
    eosio_assert(!itr2->open, "event must be closed for rollcall");
    require_auth(itr2->eventowner);

    name to = attended ? itr->attendee : itr2->eventowner;
    string memo = attended ? "Thanks for coming!" : "Event Recovery";

    action(permission_level{_self, "active"_n}, "eosio.token"_n, "transfer"_n, std::make_tuple(_self, to, itr2->stakeamount, memo))
        .send();

    ticketsdb.erase(itr);
}

void rockup::transfer(name from, name to, asset quantity, string memo)
{
    if (from == _self)
    {
        // we're sending money, do nothing additional
        return;
    }

    eosio_assert(to == _self, "contract is not involved in this transfer");
    eosio_assert(quantity.symbol.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "only positive quantity allowed");
    eosio_assert(quantity.symbol == EOS_SYMBOL, "only EOS tokens allowed");

    size_t pos = memo.find(":");
    eosio_assert(pos != string::npos, "Invalid memo");
    name ticketid = name{memo.substr(0, pos)};
    name eventid = name{memo.substr(pos + 1)};

    ticket_index ticketdb("rockup.xyz"_n, eventid.value);
    event_index eventdb("rockup.xyz"_n, "rockup.xyz"_n.value);

    auto itr = ticketdb.find(ticketid.value);
    eosio_assert(itr != ticketdb.end(), "ticket does not exist");
    eosio_assert(!itr->paid, "ticket already paid for");

    auto itr2 = eventdb.find(itr->eventid.value);
    eosio_assert(itr2 != eventdb.end(), "event does not exist");
    eosio_assert(itr2->stakeamount == quantity, "incorrect eos amount sent");
    eosio_assert(itr2->open, "event is closed");

    bool seatavailable = itr2->maxatt > itr2->att;
    eosio_assert(seatavailable, "no more seats available");

    ticketdb.modify(itr, same_payer, [&](auto &row) {
        row.paid = true;
    });

    eventdb.modify(itr2, same_payer, [&](auto &row) {
        row.att = itr2->att + 1;
    });
}

void rockup::wipeticket(name ticketid, name eventid)
{
    ticket_index ticketsdb(_code, eventid.value);
    auto itr = ticketsdb.find(ticketid.value);
    eosio_assert(itr != ticketsdb.end(), "ticket does not exist");

    event_index eventsdb(_code, _code.value);
    auto itr2 = eventsdb.find(eventid.value);
    eosio_assert(itr2 != eventsdb.end(), "event does not exist");

    eosio_assert(has_auth(itr->attendee) || !itr2->open, "only attendee can wipe ticket before event finished");
    if (itr->paid)
    {
        eosio_assert(now() + itr2->grace < itr2->etime, "too late to cancel");
        action(permission_level{_self, "active"_n}, "eosio.token"_n, "transfer"_n, std::make_tuple(_self, itr->attendee, itr2->stakeamount, string("Cancelled ticket")))
            .send();

        eventsdb.modify(itr2, same_payer, [&](auto &row) {
            row.att = itr2->att - 1;
        });
    }
    ticketsdb.erase(itr);
}

void rockup::we(name eventid)
{

    event_index eventsdb(_code, _code.value);
    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    eosio_assert(!itr->open, "event must be closed");

    ticket_index ticketsdb(_code, eventid.value);
    auto itr2 = ticketsdb.begin();
    eosio_assert(itr2 == ticketsdb.end(), "ticket still exists");

    eventsdb.erase(itr);
}

// void rockup::testreset(name eventid)
// {
//     require_auth(_self);
//     event_index eventsdb(_code, _code.value);
//     ticket_index ticketdb(_code, eventid.value);

//     auto itr = eventsdb.begin();
//     while (itr != eventsdb.end())
//     {
//         itr = eventsdb.erase(itr);
//     }

//     auto itr2 = ticketdb.begin();
//     while (itr2 != ticketdb.end())
//     {
//         itr2 = ticketdb.erase(itr2);
//     }
// }

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action)
{
    if (code == "eosio.token"_n.value && action == "transfer"_n.value)
    {
        eosio::execute_action(eosio::name(receiver), eosio::name(code), &rockup::transfer);
    }
    else if (code == receiver)
    {
        switch (action)
        {
            EOSIO_DISPATCH_HELPER(rockup, (createevent)(closeevent)(rollcall)(reqticket)(wipeticket)(we))
        }
    }
}