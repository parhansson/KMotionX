
import { SocketConnector, SocketMessageHandler, JsonSerializer } from '../../util'
import { KmxStatusParser, KmxStatus, ControlMessage } from '../../hal/kflop'
import { LogMessage, TextMessage } from './messages'

export class SocketMessageBroker implements SocketMessageHandler {

  socket: SocketConnector
  kmxStatusStream: KmxStatusParser
  serializer: JsonSerializer

  constructor(private messagePort: any) {
    //messagePort.onmessage = this.onmessage.bind(this);
    this.serializer = new JsonSerializer()
    this.kmxStatusStream = new KmxStatusParser()
    this.socket = new SocketConnector(this, 'KMotionX')
    this.post(new TextMessage('WorkerReady'))
  }

  onSocketMessage(data: ArrayBuffer | Blob | string) {
    if (data instanceof ArrayBuffer) {
      this.onBinaryMessage(data)
    } else if (data instanceof Blob) {
      let reader = new FileReader();
      // reader.result contains the contents of blob as a typed array
      reader.addEventListener('loadend', this.onBinaryMessage.bind(this, reader.result));
      reader.readAsArrayBuffer(data);
    } else {
      if (data !== 'KMotionX') {
        //try{ //try catch disables optimization in chrome
        let ctrlMessage = JSON.parse(data) as ControlMessage;
        this.post(new ControlMessage(ctrlMessage));
        //ack messages that don't require users answer here
        if (ctrlMessage.payload.block === false) {
          this.acknowledge(ctrlMessage.id, -1);
        }

        //} catch(e){
        //  console.log(data);
        //  logHandler("Error handling message: " + data, LOG_TYPE.ERROR);
        //  throw e;
        //}
      }
    }
  }



  onSocketLog(message: string, type: number) {
    this.post(new LogMessage(message, type));

  }

  onmessage(event: MessageEvent) {
    if (event.data.command == 'connect') {
      let url = event.data.url;
      this.socket.connect(url);
    } else if (event.data.command == 'acknowledge') {
      this.acknowledge(event.data.id, event.data.ret);
    } else if (event.data.command == 'disconnect') {
      //no need to acually disconnect at the moment
      this.socket.destroy();
      this.post(new TextMessage('done'));
    }

  }

  private onBinaryMessage(data: ArrayBuffer) {
    let status = this.kmxStatusStream.readBuffer(data);
    this.post(status);
  }

  private acknowledge(id, ret) {
    this.socket.sendMessage(JSON.stringify({ type: 'CB_ACK', id: id, returnValue: ret }));
  }

  private post(payload: TextMessage | LogMessage | KmxStatus | ControlMessage) {
    this.messagePort.postMessage(this.serializer.serialize(payload));
  }

}