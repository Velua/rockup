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

    eventsdb.emplace(owner, [&](auto &row) {
        row.eventid = eventid;
        row.stakeamount = stakeamt;
        row.maxatt = maxatt;
        row.eventowner = owner;
    });
}

void rockup::reqticket(name attendee, name eventid, name ticketid)
{

    event_index eventsdb(_code, _code.value);
    ticket_index ticketdb(_code, _code.value);

    auto itr = eventsdb.find(eventid.value);
    eosio_assert(itr != eventsdb.end(), "event does not exist");
    auto itr2 = ticketdb.find(ticketid.value);
    eosio_assert(itr2 == ticketdb.end(), "ticket id already exists");

    ticketdb.emplace(_self, [&](auto &row) {
        row.ticketid = ticketid;
        row.eventid = eventid;
        row.attendee = attendee;
        row.paid = false;
    });
}

void rockup::rollcall(name ticketid, name eventowner, bool attended)
{
    require_auth(eventowner);
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

    ticket_index ticketdb("rockup"_n, "rockup"_n.value);
    event_index eventdb("rockup"_n, "rockup"_n.value);

    auto itr = ticketdb.find(ticketid.value);
    eosio_assert(itr != ticketdb.end(), "ticket does not exist");
    eosio_assert(!itr->paid, "ticket already paid for");

    auto itr2 = eventdb.find(itr->eventid.value);
    eosio_assert(itr2 != eventdb.end(), "event does not exist");
    print("hello world");
    eosio_assert(itr2->stakeamount == quantity, "incorrect eos amount sent");

    bool seatavailable = itr2->maxatt > itr2->att;
    eosio_assert(seatavailable, "no more seats available");

    ticketdb.modify(itr, _self, [&](auto &row) {
        row.paid = true;
    });

    eventdb.modify(itr2, _self, [&](auto &row) {
        row.att = itr2->att + 1;
    });
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
            EOSIO_DISPATCH_HELPER(rockup, (init)(createevent)(testreset)(reqticket))
        }
    }
}