
importScripts('socket-handler.js');

var socket;
var messageHandler = function(data){
  postMessage({data:true,message:data});
  //ack messages that don't require users answer here
  var obj = JSON.parse(data);
  if(!obj.block){    
    socket.acknowledge(obj, -1);
  }
}
var logHandler = function(message, type) {
  postMessage({log:true,type:type,message:message});
}
  
function onMessage(event){
  if(event.data.command == 'connect'){   
    var url = event.data.url; 
    socket = new SocketHandler(url, messageHandler, logHandler);
    socket.connect();    
  } else if(event.data.command == 'acknowledge'){
      socket.acknowledge(event.data.obj, event.data.ret);
  } else if(event.data.command == 'disconnect'){
    //no need to acually disconnect at the moment
    socket.destroy();
    postMessage('done');
  }
  
}
self.addEventListener('message', onMessage, false);


