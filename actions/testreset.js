const { sendTransaction, getErrorDetail, getTable } = require(`../utils`)

const { CONTRACT_ACCOUNT } = process.env

async function action() {
    try {
        const transaction = await sendTransaction({
            name: `testreset`,
            actor: CONTRACT_ACCOUNT,
            data: { eventid: 'eos21' },
        })

        const tickets = await getTable('tickets', 'eos21')
        if (tickets.rows.length > 0) throw "Tickets werent removed";
        const events = await getTable('events')
        if (events.rows.length > 0) throw "Event/s still exists"
        console.log(`SUCCESS`)
        console.log(
            transaction.processed.action_traces
                .map(trace => `${trace.console}${trace.inline_traces.map(t => `\n\t${t.console}`)}`)
                .join(`\n`),
        )
    } catch (error) {
        console.error(`${getErrorDetail(error)}`)
    }
}

action()
