import { SocketMessageBroker } from './app/backend/socket.message.broker'


class Wrapper {
    constructor(){}

    postMessage(message, ports,transfer ) {
        postMessage(message, ports,transfer);
    }
}

let smb =  new SocketMessageBroker(new Wrapper());

onmessage = function(ev){
    smb.onmessage(ev);
}



