
var Unit = {
    inch: "inch",
    mm: "mm"
}

function SocketHandler(url, handlers, logNodeId) {
    this.url = url;
    this.loggingOn = true;
    this.handlers = handlers || {};
    if(logNodeId){ //If logNodeId is set the SocketHandler will log to that element e.g a div instead of console
	    this.logNode = document.getElementById(logNodeId);
    }
    this.CB_Type = [
        'STATUS', // 0: Non blocking callback. Called from the interpreter in different thread
        'COMPLETE', // 1: Non blocking callback. Called from the interpreter in different thread
        'ERR_MSG', // 2: Non blocking callback
        'CONSOLE', // 3: Non blocking callback, event though it has return value??
        'USER', // 4: Blocking callback. Called from the interpreter in different thread
        'USER_M_CODE', // 5: Blocking callback. Called from the interpreter in different thread
        'STATE', // 6: Non blocking callback. Updates gui with current state from server.
        'MESSAGEBOX' // 7: Blocking callback. However there is no need to block OK only boxes.

    ];
    this.LOG_TYPE = {
    	'NONE':0,
    	'SEND':1,
    	'RECEIVE':2,
    	'ERROR':3,
    	'PING':4
    }

    this.defaultHandler = function(obj) {
        console.error("Unknown object. No handler registered", obj);
    }
    this.connect();

}
SocketHandler.prototype.connect = function() {
    var _this = this;
    _this.log('CONNECTING...', _this.LOG_TYPE.NONE);

    this.websocket = new WebSocket(this.url);
    this.websocket.onopen = function(ev) {
        _this.log('CONNECTED', _this.LOG_TYPE.NONE);
        _this.sendMessage('KMotionX');
    };
    this.websocket.onclose = function(ev) {
        _this.log('DISCONNECTED', _this.LOG_TYPE.NONE);
        window.setTimeout(function() {
            _this.connect();
        }, 5000);
    };
    this.websocket.onerror = function(ev) {
        _this.log(ev.data, _this.LOG_TYPE.ERROR);
    };

    this.websocket.onmessage = function(ev) {

        if (!ev.data) {
            _this.log('', _this.LOG_TYPE.PING);
        } else {
            setTimeout(function() {
                _this.log(ev.data, _this.LOG_TYPE.RECEIVE);
                var obj = JSON.parse(ev.data);
                var handler = _this.handlers[_this.CB_Type[obj.type]] || _this.defaultHandler;
                var ret = handler(obj);
                _this.acknowledge(obj, ret);
            }, 0);

        }

    }

}

SocketHandler.prototype.sendMessage = function(message) {
    this.log(message, this.LOG_TYPE.SEND);
    this.websocket.send(message);
}
SocketHandler.prototype.acknowledge = function(obj, ret) {

    if (obj.type != null) { //not sure if type ever is not null
        this.sendMessage("CB_ACK:" + obj.id + ":" + obj.type + ":" + ret + ":");
    }
}

SocketHandler.prototype.log = function(message, type) {
	if(!this.loggingOn) return;
    if (this.logNode) {
        var style = '';
        var fragment=document.createDocumentFragment();
        var div = document.createElement('div');
        fragment.appendChild(div);
        if (type == this.LOG_TYPE.NONE) {

        } else {
          var sp = document.createElement('span');
          if (type == this.LOG_TYPE.SEND) {
            sp.style.color = 'green';
            sp.appendChild(document.createTextNode("SENT: "));
          } else if (type == this.LOG_TYPE.RECEIVE) {
            sp.style.color = 'blue';
            sp.appendChild(document.createTextNode("RECEIVED: "));
          } else if (type == this.LOG_TYPE.ERROR) {
            sp.style.color = 'red';
            sp.appendChild(document.createTextNode("ERROR: "));
          } else if (type == this.LOG_TYPE.PING) {
            sp.style.color = 'blue';          
            sp.appendChild(document.createTextNode("PING..."));          
          }
          div.appendChild(sp);
        } 
        div.appendChild(document.createTextNode(message));

        this.logNode.appendChild(fragment);
    	this.logNode.scrollTop = this.logNode.scrollHeight;
    } else {
        if (type == this.LOG_TYPE.NONE) {
			console.info('---',message);
        } else if (type == this.LOG_TYPE.SEND) {
        	console.info('SENT',message);
        } else if (type == this.LOG_TYPE.RECEIVE) {
            console.info('RECEIVED',message);
        } else if (type == this.LOG_TYPE.ERROR) {
            console.info('ERROR',message);
        } else if (type == this.LOG_TYPE.PING) {
            console.info('PING...',message);
        }

    }
};


function ViewerSVG(selector){
    $(selector).svg();
    this.svg = $(selector).svg('get');
    this.svg.configure({width: '200mm', height: '200mm', viewBox: '0 0 200 200'}, true);
    this.g = this.svg.group({stroke: 'blue', strokeWidth: "0.1", transform: "translate(100,100) scale(1,-1)"});
    this.svg.line(this.g,-1, -1, 1, 1, {stroke: 'red', strokeWidth: "0.1"});
    this.svg.line(this.g,-1, 1, 1, -1, {stroke: 'red', strokeWidth: "0.1"});
	this.lastX = 0;
	this.lastY = 0;

}


ViewerSVG.prototype.draw = function (message){
 //-STRAIGHT_FEED(41.6810, 8.3160, 6.0000, 0.0000)@1414170869.333
// ARC_FEED(0.0000, 50.0000, 0.0000, 25.0000, -1, 10.0000)@1414270937.597
	var type = "";
	var options = {};
	if(message.indexOf("STRAIGHT_TRAVERSE") > -1){
		type = "line";
		options = {stroke: 'green'};
	} else if(message.indexOf("STRAIGHT_FEED") > -1){
		type = "line";
	} else if(message.indexOf("ARC_FEED") > -1){
		type = "ellipse";
	}
	if(type == ""){
		return;
	}

 	var start = message.indexOf("(");
 	if(start > -1){
 		var end = message.indexOf(")@");
 		if(end > -1){
 			var res = message.substring(start+1, end);
 			var values = res.split(", ");
 			if(type == "line"){
	 			this.svg.line(this.g,this.lastX, this.lastY, values[0], values[1], options);
 			} else if(type == "ellipse"){
	 			//this.svg.ellipse(this.g, values[0], values[1], values[2], values[3], {fill: 'yellow'});
	 			var x = values[0];
	 			var y = values[1];
	 			var i = values[2];
                var j = values[3];
                var flag = values[4];
                var z = values[5];
	 			this.svg.path(this.g, "M0,0 a150,150 0 0,0 -150,150", {fill: 'none'});
	 			/*
	 			<path d="M275,175 v-150 a150,150 0 0,0 -150,150 z"
	 		        fill="yellow" stroke="blue" stroke-width="5" />
	 		        */
 			}
 			this.lastX = values[0];
 			this.lastY = values[1];
 		}
 	}
}

function GCode(gcode){
  if(Array.isArray(gcode)){
    this.lines = gcode;
    this.text = gcode.join('\n');;
  } else {
    this.text = gcode;   
    this.lines = gcode.split('\n');;
  }
}

function LineSelectingTextArea(nodeId) {
    this.tarea = document.getElementById(nodeId);
}

LineSelectingTextArea.prototype.init = function(gcode) {
	this.tarea.value = gcode.text;
    this.linePos = [];

    // calculate start/end
    var startPos = 0;
    var endPos = 0;//gcode.length; //this.tarea.value.length;
    var lineLength;
    for (var x = 0; x < gcode.lines.length; x++) {
        lineLength = gcode.lines[x].length;
        endPos = startPos + lineLength;
        this.linePos[x] = [startPos, endPos];

        startPos += (lineLength + 1);
    }

}

LineSelectingTextArea.prototype.select = function(lineNum) {
    if (this.tarea == null) {
        return;
    }
    var pos = this.linePos[lineNum];
    var startPos = pos[0];
    var endPos = pos[1];

    // do selection
    // Chrome / Firefox

    if (typeof(this.tarea.selectionStart) != "undefined") {
        //this.tarea.focus();
        this.tarea.selectionStart = startPos;
        this.tarea.selectionEnd = endPos;

        // we need to scroll to this row but scrolls are in pixels,
        // so we need to know a row's height, in pixels
        var lineHeight = this.tarea.clientHeight / 17;//tarea.rows;

        // scroll !!
        if(this.tarea.scrollTop != "undefined"){
	        this.tarea.scrollTop = lineHeight * lineNum;
        }
        

        return true;
    }

    // IE
    if (document.selection && document.selection.createRange) {
        this.tarea.focus();
        this.tarea.select();
        var range = document.selection.createRange();
        range.collapse(true);
        range.moveEnd("character", endPos);
        range.moveStart("character", startPos);
        range.select();
        return true;
    }
    return false;
}
