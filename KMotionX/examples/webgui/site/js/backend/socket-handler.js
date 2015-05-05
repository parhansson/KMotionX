
function SocketHandler(url, messageHandler, logHandler) {
    this.url = url;
    var defaultHandler = function(data) {
      //console doesn't work in web worker
      console.error("Unknown object. No handler registered", data);
    }
    
    this.messageHandler = messageHandler || defaultHandler;
    this.logHandler = logHandler;
}

SocketHandler.prototype.connect = function() {
    var _this = this;
    _this.logHandler('CONNECTING...', LOG_TYPE.NONE);

    this.websocket = new WebSocket(this.url);
    this.websocket.binaryType = "arraybuffer";
    this.websocket.onopen = function(ev) {
        _this.logHandler('CONNECTED', LOG_TYPE.NONE);
        _this.sendMessage('KMotionX');
    };
    this.websocket.onclose = function(ev) {
        _this.logHandler('DISCONNECTED', LOG_TYPE.NONE);
        setTimeout(function() {
            _this.connect();
        }, 5000);
    };
    this.websocket.onerror = function(ev) {
        _this.logHandler(ev.data, LOG_TYPE.ERROR);
    };

    this.websocket.onmessage = function(ev) {

        if (!ev.data) {
            _this.logHandler('', LOG_TYPE.PING);
        } else {
            //Data might be binary Blob or ArrayBuffer
            if(ev.data instanceof Blob){
            
            } else if(ev.data instanceof ArrayBuffer){
            
            } else {
              _this.logHandler(ev.data, LOG_TYPE.RECEIVE);              
            }            
            
            _this.messageHandler(ev.data);
        }

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

