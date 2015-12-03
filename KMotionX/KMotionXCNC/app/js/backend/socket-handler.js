
function SocketHandler(url, messageHandler, logHandler) {
    this.url = url;
    var defaultHandler = function(data) {
      //console doesn't work in web worker
      console.error("Unknown object. No handler registered", data);
    }
    
    this.messageHandler = messageHandler || defaultHandler;
    this.logHandler = logHandler;
    this.connectTimeout = null;
}

SocketHandler.prototype.connect = function() {
    this.logHandler('Websocket connecting...', LOG_TYPE.NONE);
    this.websocket = new WebSocket(this.url);
    this.websocket.binaryType = "arraybuffer";
    this.websocket.onopen = this.onopen.bind(this);
    this.websocket.onclose = this.onclose.bind(this);
    this.websocket.onerror = this.onerror.bind(this);
    this.websocket.onmessage = this.onmessage.bind(this);

}

SocketHandler.prototype.reconnect = function() {
  if(this.connectTimeout == null){
    this.connectTimeout = setInterval(this.connect.bind(this), 5000);
  }  
}

SocketHandler.prototype.onclose = function(ev) {
  this.logHandler('Websocket closed. (' + ev.code +")" +ev.reason, LOG_TYPE.NONE);
  this.reconnect();
}
SocketHandler.prototype.onerror = function(ev) {
  this.logHandler("Websocket error.", LOG_TYPE.ERROR);
  this.reconnect();
}
SocketHandler.prototype.onopen = function(ev) {
  if(this.connectTimeout != null){
    clearInterval(this.connectTimeout);
    this.connectTimeout = null;
  }
  this.logHandler('Websocket connected.', LOG_TYPE.NONE);
  this.sendMessage('KMotionX');
}
SocketHandler.prototype.onmessage = function(ev) {

  if (!ev.data) {
      this.logHandler('', LOG_TYPE.PING);
  } else {
      //Data might be binary Blob or ArrayBuffer
      if(ev.data instanceof Blob){
      
      } else if(ev.data instanceof ArrayBuffer){
      
      } else {
        this.logHandler(ev.data, LOG_TYPE.RECEIVE);              
      }            
      
      this.messageHandler(ev.data);
  }

}

SocketHandler.prototype.destroy = function() {
  this.websocket.onclose = function () {}; // disable onclose handler first
  this.websocket.close();  
}

SocketHandler.prototype.sendMessage = function(message) {
    this.logHandler(message, LOG_TYPE.SEND);
    this.websocket.send(message);
}
SocketHandler.prototype.acknowledge = function(obj, ret) {

    if (obj.type != null) { //not sure if type ever is not null
        this.sendMessage("CB_ACK: " + obj.id + " " + obj.type + " " + ret);
    }
}

