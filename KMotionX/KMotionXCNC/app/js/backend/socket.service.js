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
        STATE: stateHandler,// 6: Non blocking callback. Updates gui with current state from server.
        MESSAGEBOX: messageHandler// 7: Blocking callback. However there is no need to block OK only boxes.
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
      //socketWorker = new Worker("js/backend/socket-worker.js");
      //socketWorker.onmessage = workerMessage;
      KMX.Util.getSingletonWorker("js/backend/socket-worker.js", workerMessage)
      .then(
        function(socketWorker) {
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
        var handler = socketHandlers[obj.type];// || _this.defaultHandler;
        var ret = handler(obj);
        if(obj.block){
          //only ack messages that require users answer here
          acknowledge(obj, ret);              
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
        });        
      } else if(data.log){
        logHandler(data.message, data.type);
      }
    }
    
    function acknowledge(obj, ret){
      socketWorker.postMessage({command:'acknowledge',obj:obj,ret:ret});
    } 

    
    
    function statusHandler(obj) {
      kmxLogger.log('status', 'Line: '+ obj.data.line + " - " + obj.data.message);
    }
    function completeHandler(obj) {
      kmxLogger.log('status', 'Done Line: '+ obj.data.line + " Status: " + obj.data.status + " Sequence " + obj.data.sequence + " - " + obj.data.message);
    }
    function errorMessageHandler(obj) {
      kmxLogger.log('error', obj.data);
    }
    function consoleHandler(obj) {
      kmxLogger.log('console', obj.data);
    }
    
    function userHandler(obj) {
      if (confirm('USR: ' + obj.data + ' ?')) {
        return 0; // zero is true for this callback
      } else {
          return 1;
      }
    }
    function userMCodeHandler(obj) {
      if (confirm('Are you sure you want to continue after M' + obj.data + ' ?')) {
        return 0; // zero is true for this callback
      } else {
          return 1;
      }
    }
    
    function stateHandler(obj) {
      //Event is coming outside of angular.
      //enter digest cycle with scope.apply
      $rootScope.$apply(function(){
        SocketService.data.gcodeFile = obj.data.file;
        SocketService.data.machineSettings = obj.data.machine;
      }); 
      //TODO listen for machine configuration changes?
    }
    
    function messageHandler(obj) {
      alert(obj.data.message);
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