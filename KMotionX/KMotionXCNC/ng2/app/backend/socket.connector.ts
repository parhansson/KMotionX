
//import {LogLevel} from '../../log/log.level';
import {LogLevel} from '../log/log.level';

export interface SocketMessageHandler {
  onSocketLog(data: any, logType: number): void;
  onSocketMessage(data: any): void;
}

export class SocketConnector {

  websocket: WebSocket;
  connectTimeout: number = null;
  url: string;
  constructor(private socketMessageHandler:SocketMessageHandler) {
  }

  connect(url: string) {
    this.url = url;
    this.socketMessageHandler.onSocketLog('Websocket connecting...', LogLevel.NONE);
    this.websocket = new WebSocket(this.url);
    this.websocket.binaryType = "arraybuffer";
    this.websocket.onopen = this.onopen.bind(this);
    this.websocket.onclose = this.onclose.bind(this);
    this.websocket.onerror = this.onerror.bind(this);
    this.websocket.onmessage = this.onmessage.bind(this);

  }

  reconnect() {
    if (this.connectTimeout == null) {
      this.connectTimeout = setInterval(this.connect.bind(this,this.url), 5000);
    }
  }

  onclose(ev) {
    this.socketMessageHandler.onSocketLog('Websocket closed. (' + ev.code + ")" + ev.reason, LogLevel.NONE);
    this.reconnect();
  }
  onerror(ev) {
    this.socketMessageHandler.onSocketLog("Websocket error.", LogLevel.ERROR);
    this.reconnect();
  }
  onopen(ev) {
    if (this.connectTimeout != null) {
      clearInterval(this.connectTimeout);
      this.connectTimeout = null;
    }
    this.socketMessageHandler.onSocketLog('Websocket connected.', LogLevel.NONE);
    this.sendMessage('KMotionX');
  }
  onmessage(ev: MessageEvent) {
    if (!ev.data) {
      this.socketMessageHandler.onSocketLog('Ping', LogLevel.PING);
    } else {
      //Data might be binary Blob or ArrayBuffer
      if (ev.data instanceof Blob) {
        //this.socketMessageHandler.onSocketLog('Blob', LOG_TYPE.RECEIVE);
      } else if (ev.data instanceof ArrayBuffer) {
        //this.socketMessageHandler.onSocketLog('ArrayBuffer', LOG_TYPE.RECEIVE);
      } else {
        this.socketMessageHandler.onSocketLog(ev.data, LogLevel.RECEIVE);
      }

      this.socketMessageHandler.onSocketMessage(ev.data);
    }
  }

  destroy() {
    this.websocket.onclose = function() { }; // disable onclose handler first
    this.websocket.close();
  }

  sendMessage(message) {
    this.socketMessageHandler.onSocketLog(message, LogLevel.SEND);
    this.websocket.send(message);
  }
}
