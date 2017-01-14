
import { LogLevel } from '../log/log-level';

export interface SocketMessageHandler {
  onSocketLog(data: string, logType: number): void;
  onSocketMessage(data: ArrayBuffer | Blob | string): void;
}

export class SocketConnector
//implements MessagePort
{

  reconnectDelayMs = 5000;
  websocket: WebSocket;
  connectTimeout: number = null;
  url: string;

  constructor(
    private socketMessageHandler: SocketMessageHandler,
    private handshakeMessage?: string) {
  }

  connect(url: string) {
    this.url = url;
    this.log('Websocket connecting...', LogLevel.INFO);
    this.websocket = new WebSocket(this.url);
    this.websocket.binaryType = 'arraybuffer';
    this.websocket.onopen = this.onopen.bind(this);
    this.websocket.onclose = this.onclose.bind(this);
    this.websocket.onerror = this.onerror.bind(this);
    this.websocket.onmessage = this.onmessage.bind(this);
  }

  reconnect() {
    if (this.connectTimeout == null) {
      this.connectTimeout = setInterval(this.connect.bind(this, this.url), this.reconnectDelayMs);
    }
  }

  onclose(ev: CloseEvent) {
    this.log('Websocket closed. (' + ev.code + ')' + ev.reason, LogLevel.INFO);
    this.reconnect();
  }

  onerror(ev: Event) {
    this.log('Websocket error.', LogLevel.ERROR);
    this.reconnect();
  }

  onopen(ev: Event) {
    if (this.connectTimeout != null) {
      clearInterval(this.connectTimeout);
      this.connectTimeout = null;
    }
    this.log('Websocket connected.', LogLevel.INFO);
    if (this.handshakeMessage) {
      this.sendMessage(this.handshakeMessage);
    }
  }

  onmessage(ev: MessageEvent) {
    if (!ev.data) {
      this.log('Ping', LogLevel.INFO);
    } else {
      //Data might be binary Blob or ArrayBuffer
      if (ev.data instanceof Blob) {
        //this.log('Blob', LOG_TYPE.RECEIVE);
      } else if (ev.data instanceof ArrayBuffer) {
        //this.log('ArrayBuffer', LOG_TYPE.RECEIVE);
      } else {
        this.log(ev.data, LogLevel.RECEIVE);
      }

      this.socketMessageHandler.onSocketMessage(ev.data);
    }
  }

  destroy() {
    this.websocket.onclose = function () { }; // disable onclose handler first
    this.websocket.close();
  }

  sendMessage(message: string) {
    this.log(message, LogLevel.SEND);
    this.websocket.send(message);
  }

  private log(data: string, logType: number) {
    this.socketMessageHandler.onSocketLog(data, logType);
  }
}
