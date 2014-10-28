  var websocket;

  var CB_STATUS = 0; //Non blocking callback. Called from the interpreter in different thread
  var CB_COMPLETE = 1; //Non blocking callback. Called from the interpreter in different thread
  var CB_ERR_MSG = 2; //Non blocking callback
  var CB_CONSOLE = 3; //Non blocking callback, event though it has return value??
  var CB_USER = 4; //Blocking callback. Called from the interpreter in different thread
  var CB_USER_M_CODE = 5; //Blocking callback. Called from the interpreter in different thread
  var CB_STATE = 6; //Non blocking callback. Updates gui with current state from server. 
  var CB_MESSAGEBOX = 7; //Blocking callback. However there is no need to block OK only boxes.

  var RAW_NONE = 0;
  var RAW_SEND = 1;
  var RAW_REC = 2;
  var RAW_ERROR = 3;
  var RAW_PING = 4;

  function connectWS() {
      raw('CONNECTING...', RAW_NONE);
      var url = 'ws://' + location.host + '/ws';


      websocket = new WebSocket(url);
      websocket.onopen = function(ev) {
          raw('CONNECTED', RAW_NONE);
          sendMessage('KMotionX');
      };
      websocket.onclose = function(ev) {
          raw('DISCONNECTED', RAW_NONE);
          window.setTimeout(function() {
              connectWS()
          }, 5000);
      };
      websocket.onmessage = receiveMessage;

      websocket.onerror = function(ev) {
          raw(ev.data, RAW_ERROR);
      };
  };


  function sendMessage(message) {
      raw(message, RAW_SEND);
      websocket.send(message);
  }

  function receiveMessage(ev) {

      if (!ev.data) {
          raw('', RAW_PING);
      } else {
          setTimeout(function() {
              raw(ev.data, RAW_REC);
              var obj = JSON.parse(ev.data);

              var id = obj.id;
              var type = obj.type;
              var ret = -1;
              if (type == CB_USER || type == CB_USER_M_CODE) { //blocking call
                  ret = blockOk(obj);
                  sendAck(obj, ret);
              } else if (type == CB_MESSAGEBOX) {
                  ret = block(obj);
                  sendAck(obj, ret);
              } else {
                  sendAck(obj, ret);

                  if (type == CB_STATUS || type == CB_COMPLETE) { // interpreter status update
                      stat(obj);
                  } else if (type == CB_ERR_MSG) {
                      err(obj);
                  } else if (type == CB_CONSOLE) {
                      console(obj);
                  } else if (type == CB_STATE) {
                      state(obj);

                  }

              }
          }, 0);


      }

  }
  var sendAck = function(obj, ret) {

      if (obj.type != null) { //not sure if type ever is not null
          sendMessage("CB_ACK:" + obj.id + ":" + obj.type + ":" + ret + ":");
      }
  }
  var state = function(obj) {
      toggleButton("#feed_hold_btn", obj.data.feedHold == 1);
      toggleButton("#simulate_btn", obj.data.simulate == 1);
      toggleButton("#code_apply", obj.data.interpreting == 1);
      //only load if different from loaded
      if (obj.data.file != "" && obj.data.file != $("#code_apply").data("file")) {
          loadFile(obj.data.file);
      }
  };

  var raw = function(message, type) {


      var div = document.createElement('div');
      var style = '';
      if (type == RAW_NONE) {

      } else if (type == RAW_SEND) {
          style = '<span style="color: green; ">SENT: </span> '
      } else if (type == RAW_REC) {
          style = '<span style="color: blue; ">RECEIVED: </span> '
      } else if (type == RAW_ERROR) {
          style = '<span style="color: red; ">ERROR: </span> '
      } else if (type == RAW_PING) {
          style = '<span style="color: blue; ">PING...</span> '
      }

      div.innerHTML = style + message;
      var logNode = document.getElementById('output');
      logNode.appendChild(div);
      addScrollTo(logNode, div);

  };

  var err = function(obj) {
      logText('error', 'red', obj.data);
  };

  var console = function(obj) {
      logText('console', 'blue', obj.data);
  };

  var stat = function(obj) {
      //select gcode row
      lineTA.toLine(obj.data.line);
      drawStatus(obj.data.message);
      logText('status', 'green', obj.data.line + ": " + obj.data.message);


  };

  var blockOk = function(obj) {
      alert(obj.data);
      return 96;
  };

  var block = function(obj) {
      alert(obj.data.message);
      return 96;
  };

  function logText(elementId, color, text) {
      var logNode = document.getElementById(elementId);
      var div = document.createElement('div');
      div.innerHTML = '<span style="color: ' + color + ';">' + text + ' </span>';
      logNode.appendChild(div);
      addScrollTo(logNode, div);
  }

  function addScrollTo(text, add) {
      var pos = add.offsetTop;
      text.scrollTop = pos;
  }