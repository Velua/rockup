const { sendTransaction, getTable, getBalance } = require(`../utils`);

const { CONTRACT_ACCOUNT } = process.env;
const wait = require('waait')

describe(`contract`, () => {
  beforeAll(async () => {
    await sendTransaction({ name: `testreset`, data: { eventid: 'newid' } });
    jest.setTimeout(10000)
  });

  const grace = 15
  const now = Math.floor((new Date).getTime() / 1000);
  const etime = now + 30


  test(`test2 can create an event`, async () => {
    await sendTransaction({
      name: `createevent`,
      actor: `test2`,
      data: {
        owner: `test2`,
        eventid: `newid`,
        stakeamt: "5.0000 EOS",
        maxatt: 10,
        inviteonly: false,
        about: "whatever",
        grace,
        etime
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
        inviteonly: 0,
        grace,
        etime
      }
    ]);
  })

  test(`test 1 can request a ticket for valid event`, async () => {
    await sendTransaction({
      name: `reqticket`,
      actor: `test1`,
      data: {
        attendee: `test1`,
        eventid: `newid`,
        ticketid: "ap41"
      }
    });

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
      { ticketid: "ap41", attendee: "test1", eventid: "newid", paid: 0 }
    ]);
  });

  test(`test1 can pay for ticket`, async () => {


    await sendTransaction({
      account: "eosio.token",
      name: "transfer",
      actor: "test1",
      data: {
        from: "test1",
        to: "rockup",
        quantity: "5.0000 EOS",
        memo: "ap41:newid"
      }
    });

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
      { ticketid: "ap41", attendee: "test1", eventid: "newid", paid: 1 }
    ]);

  })

  test(`test1 can pull out before deadline`, async () => {

    const beforeBalance = await getBalance('test1')

    const beforeTable = await getTable("events");

    expect(beforeTable.rows).toEqual([
      {
        eventid: "newid",
        stakeamount: "5.0000 EOS",
        maxatt: 10,
        eventowner: "test2",
        att: 1,
        open: 1,
        inviteonly: 0,
        grace,
        etime
      }
    ]);

    await sendTransaction({
      name: 'wipeticket',
      actor: 'test1',
      data: {
        ticketid: 'ap41',
        eventid: 'newid'
      }
    })

    const afterBalance = await getBalance('test1')
    expect(afterBalance).toBe(beforeBalance + 5)

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([]);

    const afterTable = await getTable("events");

    expect(afterTable.rows).toEqual([
      {
        eventid: "newid",
        stakeamount: "5.0000 EOS",
        maxatt: 10,
        eventowner: "test2",
        att: 0,
        open: 1,
        inviteonly: 0,
        grace,
        etime
      }
    ]);

  })

  test(`test 3 can request a ticket for valid event`, async () => {
    await sendTransaction({
      name: `reqticket`,
      actor: `test3`,
      data: {
        attendee: `test3`,
        eventid: `newid`,
        ticketid: "ap42"
      }
    });

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
      { ticketid: "ap42", attendee: "test3", eventid: "newid", paid: 0 }
    ]);
  });

  test(`test3 can pay for ticket`, async () => {

    await sendTransaction({
      account: "eosio.token",
      name: "transfer",
      actor: "test3",
      data: {
        from: "test3",
        to: "rockup",
        quantity: "5.0000 EOS",
        memo: "ap42:newid"
      }
    });

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
      { ticketid: "ap42", attendee: "test3", eventid: "newid", paid: 1 }
    ]);

  })




  test(`test3 cant pull out after deadline`, async () => {

    const now = Math.floor((new Date).getTime() / 1000);
    const timeTillDeadline = etime - grace - now;
    const timeToWait = timeTillDeadline * 1000 + 1000
    await wait(timeToWait)

    expect.assertions(3)
    const beforeBalance = await getBalance('test3')

    try {
      await sendTransaction({
        name: 'wipeticket',
        actor: 'test3',
        data: {
          ticketid: 'ap42',
          eventid: 'newid'
        }
      })
    } catch (e) {
      expect(e.message).toBe(`assertion failure with message: too late to cancel`)
    }

    const afterBalance = await getBalance('test3')
    expect(afterBalance).toBe(beforeBalance)

    const tableResult = await getTable('tickets', 'newid');
    expect(tableResult.rows).toEqual([
      { ticketid: "ap42", attendee: "test3", eventid: "newid", paid: 1 }
    ])


  })

})