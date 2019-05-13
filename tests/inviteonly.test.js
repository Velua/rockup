const { sendTransaction, getTable, getBalance } = require(`../utils`);

const { CONTRACT_ACCOUNT } = process.env;

describe(`contract`, () => {
  beforeAll(async () => {
    await sendTransaction({ name: `testreset`, data: { eventid: 'newid' } });
    jest.setTimeout(10000)
  });


  test(`test2 can create an event`, async() => {
    await sendTransaction({
        name: `createevent`,
        actor: `test2`,
        data: {
          owner: `test2`,
          eventid: `newid`,
          stakeamt: "5.0000 EOS",
          maxatt: 10,
          inviteonly: true
        }
      });
  
      const tableResult = await getTable("events");
  
      expect(tableResult.rows).toEqual([
        {
          eventid: "newid",
          stakeamount: "5.0000 EOS",
          maxatt: 10,
          eventowner: "test2",
          att: 0,
          open: 1,
          inviteonly: 1
        }
      ]);
  })

  test(`test1 cannot req ticket for the event`, async() => {
    expect.assertions(2)
    try {
        await sendTransaction({
        name: `reqticket`,
        actor: `test1`,
        data: {
            attendee: `test1`,
              eventid: `newid`,
              ticketid: "fefe"
            }
          });
    } catch(e) {
        expect(e.message).toBe("assertion failure with message: invite only event")
    }
    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
    ]);
  })

  test(`test2 can reqticket for test1`, async() => {
    await sendTransaction({
        name: `reqticket`,
        actor: `test2`,
        data: {
            attendee: `test1`,
              eventid: `newid`,
              ticketid: "fefe"
            }
          });

          const tableResult = await getTable('tickets', 'newid');
          expect(tableResult.rows).toEqual([
            { ticketid: "fefe", attendee: "test1", eventid: "newid", paid: 0 }
          ]);
  })

  test(`test1 can pay for the ticket test2 made for him`, async()=> {
      await sendTransaction({
          name: 'transfer',
          actor: 'test1',
          account: 'eosio.token',
          data: {
              from: 'test1',
              to: CONTRACT_ACCOUNT,
              quantity: '5.0000 EOS',
              memo: 'fefe:newid'
          }
      })


      const tableResult = await getTable('tickets', 'newid');
      expect(tableResult.rows).toEqual([
        { ticketid: "fefe", attendee: "test1", eventid: "newid", paid: 1 }
      ]);
  })


})