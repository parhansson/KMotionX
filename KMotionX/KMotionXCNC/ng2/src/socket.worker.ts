import { SocketMessageBroker } from './app/backend/socket/socket.message.broker'

const SocketWorker = self as any

class Wrapper {
    constructor() { }

    postMessage(message, ports, transfer) {
        postMessage(message, ports, transfer);
    }
}

let smb = new SocketMessageBroker(new Wrapper());

onmessage = function (ev) {
    smb.onmessage(ev);
}

export default SocketWorker

