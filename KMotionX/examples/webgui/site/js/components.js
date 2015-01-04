
var Unit = {
    inch: "inch",
    mm: "mm"
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



