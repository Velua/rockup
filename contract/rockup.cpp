#include "./rockup.hpp"

using namespace eosio;
using namespace std;

void rockup::init(name name)
{
    require_auth(_self);
}

void rockup::createevent(name owner, name eventid, asset stakeamt, uint64_t maxatt)
{
    require_auth(owner);
    event_index eventsdb(_code, _code.value);
    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr == eventsdb.end(), "event already exists");

    eosio_assert(stakeamt.symbol.is_valid(), "invalid quantity");
    eosio_assert(stakeamt.amount > 0, "only positive quantity allowed");
    eosio_assert(stakeamt.symbol == EOS_SYMBOL, "only EOS tokens allowed");

    eventsdb.emplace(owner, [&](auto &row) {
        row.eventid = eventid;
        row.stakeamount = stakeamt;
        row.maxatt = maxatt;
        row.eventowner = owner;
        row.open = true;
    });
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
    require_auth(attendee);

    event_index eventsdb(_code, _code.value);
    ticket_index ticketdb(_code, _code.value);

    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    eosio_assert(itr->open, "cannot create ticket for closed event");
    auto itr2 = ticketdb.find(ticketid.value);
    eosio_assert(itr2 == ticketdb.end(), "ticket id already exists");

    ticketdb.emplace(attendee, [&](auto &row) {
        row.ticketid = ticketid;
        row.eventid = eventid;
        row.attendee = attendee;
        row.paid = false;
    });
}

void rockup::rollcall(name ticketid, bool attended)
{

    ticket_index ticketsdb(_code, _code.value);
    auto itr = ticketsdb.find(ticketid.value);
    eosio_assert(itr != ticketsdb.end(), "ticket does not exist");
    eosio_assert(itr->paid, "cannot role call on a unpaid ticket");

    event_index eventsdb(_code, _code.value);
    auto itr2 = eventsdb.find(itr->eventid.value);
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

    name ticketid = name{memo};

    ticket_index ticketdb("rockup.xyz"_n, "rockup.xyz"_n.value);
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

void rockup::wipeticket(name ticketid)
{
    ticket_index ticketsdb(_code, _code.value);
    auto itr = ticketsdb.find(ticketid.value);
    eosio_assert(itr != ticketsdb.end(), "ticket does not exist");
    eosio_assert(!itr->paid, "cannot wipe a paid ticket");

    event_index eventsdb(_code, _code.value);
    auto itr2 = eventsdb.find(itr->eventid.value);
    eosio_assert(itr2 != eventsdb.end(), "event does not exist");

    eosio_assert(has_auth(itr->attendee) || !itr2->open, "only attendee or event owner can wipe closed");

    ticketsdb.erase(itr);
}

void rockup::wipeevent(name eventid)
{

    event_index eventsdb(_code, _code.value);
    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    eosio_assert(!itr->open, "event must be closed");


    ticket_index ticketsdb(_code, _code.value);
    auto eventidindex = ticketsdb.get_index<name("byevent")>();
    auto itr2 = eventidindex.lower_bound(eventid.value);

    eosio_assert(itr2 == eventidindex.end(), "ticket still exists");

    eventsdb.erase(itr);
}

void rockup::testreset()
{
    require_auth(_self);
    event_index eventsdb(_code, _code.value);
    ticket_index ticketdb(_code, _code.value);

    auto itr = eventsdb.begin();
    while (itr != eventsdb.end())
    {
        itr = eventsdb.erase(itr);
    }

    auto itr2 = ticketdb.begin();
    while (itr2 != ticketdb.end())
    {
        itr2 = ticketdb.erase(itr2);
    }
}

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
            EOSIO_DISPATCH_HELPER(rockup, (createevent)(closeevent)(rollcall)(reqticket)(wipeticket)(wipeevent))
        }
    }
}