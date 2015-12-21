(function() {
  angular.module('KMXBackend')
    .factory('SocketService', socketHandler); 

  socketHandler.$inject = ['$rootScope','kmxLogger'];
  
  function socketHandler($rootScope,kmxLogger) {
    var socketWorker;
    //var messageCount = 0;
    var socketHandlers = {
        
        STATUS: statusHandler,// 0: Non blocking callback. Called from the interpreter in different thread
        COMPLETE: completeHandler,// 1: Non blocking callback. Called from the interpreter in different thread
        ERR_MSG: errorMessageHandler,// 2: Non blocking callback
        CONSOLE: consoleHandler,// 3: Non blocking callback, event though it has return value??
        USER: userHandler,// 4: Blocking callback. Called from the interpreter in different thread
        USER_M_CODE: userMCodeHandler,// 5: Blocking callback. Called from the interpreter in different thread
        MESSAGEBOX: messageHandler// 6: Blocking callback. However there is no need to block OK only boxes.
    }    
    var SocketService = {
        acknowledge: acknowledge,
        init: init,
        data: {
          raw: {},
          interpreting:false,
          feedHold:false,
          dro:[],
          timeStamp:"N/A",
          connected:false,
          simulating:false,
          currentLine:-1
        }
    }
    return SocketService; 
    
    
    function init(){
      if(socketWorker){
        return;
      }
      var url = 'ws://' + window.location.host + '/ws';
      KMX.Util.getSingletonWorker("js/backend/socket-worker.js", workerMessage)
      .then(
        function(worker) {
          socketWorker = worker; 
          socketWorker.postMessage({command:'connect',url:url}) 
        },function(reason){
          console.error(reason);
        });
      
      
      //does not seem to work, at least not in chrome
//      window.onbeforeunload = function(){
//        socketWorker.postMessage({command:'disconnect'})
//      }
      
    }
    function workerMessage(event) {
      //messageCount++;
      //if(messageCount%5 == 0){
        //console.info("Messages received", messageCount);
      //}
      var data = event.data; 
      if(data.data){
        var obj = data.message;
        if(obj.KMX){
          return;
        }
        var handler = socketHandlers[obj.payload.type];// || _this.defaultHandler;
        var ret = handler(obj.payload);
        if(obj.payload.block === true){
          //only ack messages that require users answer here
          acknowledge(obj.id, ret);              
        }
      } else if(data.status){
        //var json = angular.toJson(data.message,true);
        //logHandler(json, LOG_TYPE.NONE);
        //console.log(json);
        
        //Event is coming outside of angular.
        //enter digest cycle with scope.apply
        $rootScope.$apply(function(){
          var raw = data.message;
          //SocketService.data.raw = raw;
          SocketService.data.interpreting = raw.interpreting,
          SocketService.data.feedHold = raw.stopImmediateState > 0;//pause
          SocketService.data.dro = raw.dro;
          SocketService.data.timeStamp = raw.timeStamp;
          SocketService.data.connected = raw.connected;
          SocketService.data.simulating = raw.simulate;
          SocketService.data.currentLine = raw.currentLine;
          SocketService.data.gcodeFile = raw.gcodeFile;
          SocketService.data.machineSettings = raw.machineSettingsFile;
        });        
      } else if(data.log){
        logHandler(data.message, data.type);
      }
    }
    
    function acknowledge(id, ret){
      socketWorker.postMessage({command:'acknowledge',id:id,ret:ret});
    } 

    function statusHandler(payload) {
      kmxLogger.log('status', 'Line: '+ payload.line + " - " + payload.message);
    }
    function completeHandler(payload) {
      kmxLogger.log('status', 'Done Line: '+ payload.line + " Status: " + payload.status + " Sequence " + payload.sequence + " - " + payload.message);
    }
    function errorMessageHandler(payload) {
      kmxLogger.log('error', payload.message);
    }
    function consoleHandler(payload) {
      kmxLogger.log('console', payload.message);
    }
    
    function userHandler(payload) {
      if (confirm('USR: ' + payload.message + ' ?')) {
        return 0; // zero is true for this callback
      } else {
          return 1;
      }
    }
    function userMCodeHandler(payload) {
      if (confirm('Are you sure you want to continue after M' + payload.code + ' ?')) {
        return 0; // zero is true for this callback
      } else {
          return 1;
      }
    }
    
    function messageHandler(payload) {
      alert(payload.message);
      return 0;
    }
    
    function logHandler(message, type) {
      this.logNode = document.getElementById('output');
      
      if (this.logNode) {
          var style = '';
          var fragment=document.createDocumentFragment();
          var div = document.createElement('div');
          fragment.appendChild(div);
          if (type == LOG_TYPE.NONE) {

          } else {
            var sp = document.createElement('span');
            if (type == LOG_TYPE.SEND) {
              sp.style.color = 'green';
              sp.appendChild(document.createTextNode("SENT: "));
            } else if (type == LOG_TYPE.RECEIVE) {
              sp.style.color = 'blue';
              sp.appendChild(document.createTextNode("RECEIVED: "));
            } else if (type == LOG_TYPE.ERROR) {
              sp.style.color = 'red';
              sp.appendChild(document.createTextNode("ERROR: "));
            } else if (type == LOG_TYPE.PING) {
              sp.style.color = 'blue';          
              sp.appendChild(document.createTextNode("PING..."));          
            }
            div.appendChild(sp);
          } 
          div.appendChild(document.createTextNode(message));

          kmxLogger.logFragment('output',fragment);
      } else {
          if (type == LOG_TYPE.NONE) {
              console.info('---',message);
          } else if (type == LOG_TYPE.SEND) {
              console.info('SENT',message);
          } else if (type == LOG_TYPE.RECEIVE) {
              console.info('RECEIVED',message);
          } else if (type == LOG_TYPE.ERROR) {
              console.info('ERROR',message);
          } else if (type == LOG_TYPE.PING) {
              console.info('PING...',message);
          }

      }
    }    
    
  }
  
})();