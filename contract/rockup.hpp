#include <string>

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/name.hpp>

#define EOS_SYMBOL symbol("EOS", 4)

CONTRACT rockup : public eosio::contract
{
  using contract::contract;

public:
  struct init
  {
    init(){};
    eosio::name name;
    EOSLIB_SERIALIZE(init, (name))
  };

  ACTION init(eosio::name name);
  ACTION testreset();
  ACTION createevent(eosio::name owner, eosio::name eventid, eosio::asset stakeamt, uint64_t maxatt);
  ACTION reqticket(eosio::name attendee, eosio::name eventid, eosio::name ticketid);
  ACTION rollcall(eosio::name ticketid, bool attended);
  void transfer(eosio::name from, eosio::name to, eosio::asset quantity, std::string memo);

private:
  TABLE event
  {
    eosio::name eventid;
    eosio::asset stakeamount;
    uint64_t maxatt;
    uint64_t att;
    eosio::name eventowner;

    uint64_t primary_key() const { return eventid.value; }
  };
  typedef eosio::multi_index<"events"_n, event> event_index;

  TABLE ticket
  {
    eosio::name ticketid;
    eosio::name attendee;
    eosio::name eventid;
    bool paid;

    uint64_t primary_key() const { return ticketid.value; }
  };
  typedef eosio::multi_index<"tickets"_n, ticket> ticket_index;
};