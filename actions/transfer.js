const { sendTransaction, getErrorDetail } = require(`../utils`)

const { CONTRACT_ACCOUNT } = process.env

async function action() {
    try {
        await sendTransaction({
            account: `eosio.token`,
            name: `transfer`,
            actor: `test2`,
            data: {
                from: `test2`,
                to: 'rockup',
                quantity: `5.0000 EOS`,
                memo: 'www',
            },
        })
        console.log(`SUCCESS`)
    } catch (error) {
        console.error(`${getErrorDetail(error)}`)
    }
}

action()
