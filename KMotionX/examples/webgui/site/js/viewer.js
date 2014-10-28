var svg;
var g;
function initSVG(){
    $("#svgviewer").svg();
    svg = $('#svgviewer').svg('get'); 
    svg.configure({width: '200mm', height: '200mm', viewBox: '0 0 100 100'}, true);
    g = svg.group({stroke: 'blue', strokeWidth: "0.1", transform: "translate(50,10)"});
    //svg.circle(130, 75, 50, {fill: 'none', stroke: 'red', strokeWidth: 3});
}
var lastX = 0;
var lastY = 0;

function drawStatus(message){
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
	 			svg.line(g,lastX, lastY, values[0], values[1], options); 			
 			} else if(type == "ellipse"){
	 			svg.ellipse(g, values[0], values[1], values[2], values[3], {fill: 'yellow'});
 			}


 			lastX = values[0];
 			lastY = values[1];
 			

 		}
 	}
 	
 	
 
}

function drawIntro(svg) { 
    svg.circle(75, 75, 50, 
        {fill: 'none', stroke: 'red', strokeWidth: 3}); 
    var g = svg.group({stroke: 'black', strokeWidth: 2}); 
    svg.line(g, 15, 75, 135, 75); 
    svg.line(g, 75, 15, 75, 135); 
}