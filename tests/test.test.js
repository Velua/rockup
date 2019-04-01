const { sendTransaction, getTable } = require(`../utils`);

const { CONTRACT_ACCOUNT } = process.env;

describe(`contract`, () => {
  beforeAll(async () => {
    await sendTransaction({ name: `testreset` });
  });

  test(`test1 can create an event`, async () => {
    await sendTransaction({
      name: `createevent`,
      actor: `test1`,
      data: {
        owner: `test1`,
        eventid: `eos21`,
        stakeamt: "5.0000 EOS",
        maxatt: 10
      }
    });

    const tableResult = await getTable("events");

    expect(tableResult.rows).toEqual([
      {
        eventid: "eos21",
        stakeamount: "5.0000 EOS",
        maxatt: 10,
        eventowner: "test1",
        att: 0
      }
    ]);
  });

  test(`test1 cannot create a new event with an existing id`, async () => {
    // expect.assertions(1);
    try {
      await sendTransaction({
        name: `createevent`,
        actor: `test1`,
        data: {
          owner: `test1`,
          eventid: `eos21`,
          stakeamt: "5.0000 EOS",
          maxatt: 12
        }
      });
    } catch (e) {
      expect(e.message).toBe(
        "assertion failure with message: event already exists"
      );
      const tableResult = await getTable("events");
      expect(tableResult.rows).toEqual([
        {
          eventid: "eos21",
          stakeamount: "5.0000 EOS",
          maxatt: 10,
          eventowner: "test1",
          att: 0
        }
      ]);
    }
  });

  test(`test 2 can request a ticket for valid event`, async () => {
    await sendTransaction({
      name: `reqticket`,
      actor: `test2`,
      data: {
        attendee: `test2`,
        eventid: `eos21`,
        ticketid: "ap41"
      }
    });

    const tableResult = await getTable("tickets");
    expect(tableResult.rows).toEqual([
      { ticketid: "ap41", attendee: "test2", eventid: "eos21", paid: 0 }
    ]);
  });

  test(`test 2 cannot request a ticket for an event that does not exist`, async () => {
    expect.assertions(1);
    try {
      await sendTransaction({
        name: `reqticket`,
        actor: `test2`,
        data: {
          attendee: `test2`,
          eventid: `eos55`,
          ticketid: "ap41"
        }
      });
    } catch (e) {
      expect(e.message).toBe(
        "assertion failure with message: event does not exist"
      );
    }
  });

  test(`test 2 cant pay for his ticket sending the wrong amount`, async () => {
    expect.assertions(2);

    try {
      await sendTransaction({
        account: "eosio.token",
        name: "transfer",
        actor: "test2",
        data: {
          from: "test2",
          to: "rockup",
          quantity: "4.9999 EOS",
          memo: "ap41"
        }
      });
    } catch (e) {
      expect(e.message).toBe(
        "assertion failure with message: incorrect eos amount sent"
      );
    }

    try {
      await sendTransaction({
        account: "eosio.token",
        name: "transfer",
        actor: "test2",
        data: {
          from: "test2",
          to: "rockup",
          quantity: "6.0000 EOS",
          memo: "ap41"
        }
      });
    } catch (e) {
      expect(e.message).toBe(
        "assertion failure with message: incorrect eos amount sent"
      );
    }
  });

  test(`test 2 cant pay for his ticket sending wrong amount`, async () => {
    // Confirm attenance for event is 0
    const eventsTable = await getTable("events");
    const beforeEvent = eventsTable.rows[0];
    expect(beforeEvent.att).toBe(0);

    // Confirm ticket has not been paid for yet
    const ticketsTable = await getTable("tickets");
    const beforeTicket = ticketsTable.rows[0];
    expect(beforeTicket.paid).toBe(0);

    await sendTransaction({
      account: "eosio.token",
      name: "transfer",
      actor: "test2",
      data: {
        from: "test2",
        to: "rockup",
        quantity: "5.0000 EOS",
        memo: "ap41"
      }
    });

    // Confirm ticket has now been paid for
    const tableResult = await getTable("tickets");
    expect(tableResult.rows).toEqual([
      { ticketid: "ap41", attendee: "test2", eventid: "eos21", paid: 1 }
    ]);

    // Confirm attendance for the event has gone up
    const afterEventsTable = await getTable("events");
    const afterEvent = afterEventsTable.rows[0];
    expect(afterEvent.att).toBe(1);
  });

  test(`test 2 cannot pay for the same ticket twice`, async () => {
    expect.assertions(1);
    try {
      await sendTransaction({
        account: "eosio.token",
        name: "transfer",
        actor: "test2",
        data: {
          from: "test2",
          to: "rockup",
          quantity: "5.0000 EOS",
          memo: "ap41"
        }
      });
    } catch (e) {
      expect(e.message).toBe(
        `assertion failure with message: ticket already paid for`
      );
    }
  });
});
