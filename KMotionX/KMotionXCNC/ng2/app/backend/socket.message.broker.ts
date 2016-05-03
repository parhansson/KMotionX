
import {SocketConnector,SocketMessageHandler} from './socket.connector'
import {KmxStatusStream} from './kmx.status'
import {KmxStatus} from './shared'
//
//declare function postMessage(data: any): void;
export interface PostMessageCallback { (data: any, transfer?:any[]): void }
export class SocketMessageBroker implements SocketMessageHandler {

  socket: SocketConnector;
  kmxStatusStream: KmxStatusStream;

  constructor(private postMessageCallback:PostMessageCallback) {
    this.kmxStatusStream = new KmxStatusStream();
    this.socket = new SocketConnector(this);
    this.postMessageCallback('WorkerReady');
  }
  
  onSocketMessage(data: any){
    if (data instanceof ArrayBuffer) {
      this.onBinaryMessage(data)
    } else if (data instanceof Blob) {
      var reader = new FileReader();
      // reader.result contains the contents of blob as a typed array
      reader.addEventListener("loadend", this.onBinaryMessage.bind(this, reader.result));
      reader.readAsArrayBuffer(data);
    } else {
      if (data !== "KMotionX") {      
        //try{ //try catch disables optimization in chrome
        var obj = JSON.parse(data);
        this.postMessageCallback({ data: true, message: obj });
        //ack messages that don't require users answer here
        if (obj.payload.block === false) {
          this.acknowledge(obj.id, -1);
        }
      
        //} catch(e){
        //  console.log(data);
        //  logHandler("Error handling message: " + data, LOG_TYPE.ERROR);
        //  throw e;
        //}
      }
    }    
  }
 
  onBinaryMessage(data:ArrayBuffer){
    var status = this.kmxStatusStream.readBuffer(data);
    this.postMessageCallback({ status: true, message: status });    
  }
  
  onSocketLog(message: any, type: number){
    this.postMessageCallback({ log: true, type: type, message: message });
    
  }

  onMessage(event: MessageEvent) {
    if (event.data.command == 'connect') {
      var url = event.data.url;
      this.socket.connect(url);
    } else if (event.data.command == 'acknowledge') {
      this.acknowledge(event.data.id, event.data.ret);
    } else if (event.data.command == 'disconnect') {
      //no need to acually disconnect at the moment
      this.socket.destroy();
      this.postMessageCallback('done');
    }

  }
  acknowledge(id, ret) {
    this.socket.sendMessage(JSON.stringify({ type: "CB_ACK", id: id, returnValue: ret }));
  }
  
}