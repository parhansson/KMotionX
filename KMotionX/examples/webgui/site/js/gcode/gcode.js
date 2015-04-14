
var Unit = {
    inch: "inch",
    mm: "mm"
}

var GCode ={};


GCode.Source = function (gcode){
  if(Array.isArray(gcode)){
    this.lines = gcode;
    this.text = gcode.join('\n');;
  } else {
    this.text = gcode;   
    this.lines = gcode.split('\n');;
  }
}

GCode.Curve3 = function ( startPoint, endPoint, args, clockWise ) {
  
  
  var centerX = startPoint.x + args.I;
  var centerY = startPoint.y + args.J;
  var centerZ = startPoint.z + args.K; // Helical not correct implemented yet, i guess...
  var startAngle;
  var endAngle;
  var radius = Math.sqrt((args.I * args.I) + (args.J * args.J));  //should use pythagoras
  if(startPoint.x == endPoint.x && startPoint.y == endPoint.y){
    //full circle
    startAngle = 0;
    endAngle = Math.PI*2;
  } else {
    startAngle = -Math.atan2(endPoint.y-centerY, endPoint.x-centerX);
    endAngle   = Math.atan2(args.J, args.I);
  }

  //console.info("Curve ax, ay, radius, startangle, endangle",aX, aY, radius,aStartAngle,  aEndAngle);
  
  this.init(
      centerX,  centerY,                  // ax, aY
      radius, radius,           // xRadius, yRadius
      startAngle,  endAngle,  // aStartAngle, aEndAngle
      clockWise                     // aClockwise
  );
  this.sZ = startPoint.z;
  this.height = endPoint.z - startPoint.z;  
  
}

GCode.Curve3.prototype.init = function ( aX, aY, xRadius, yRadius, aStartAngle, aEndAngle, aClockwise ) {
  //Kept this function since it is the same as the THREE.EllipseCurve
  this.aX = aX;
  this.aY = aY;

  this.xRadius = xRadius;
  this.yRadius = yRadius;

  this.aStartAngle = aStartAngle;
  this.aEndAngle = aEndAngle;

  this.aClockwise = aClockwise;

};

GCode.Curve3.prototype.getPoint = function ( t , h) {

  var deltaAngle = this.aEndAngle - this.aStartAngle;

  if ( deltaAngle < 0 ) deltaAngle += Math.PI * 2;
  if ( deltaAngle > Math.PI * 2 ) deltaAngle -= Math.PI * 2;

  var angle;

  if ( this.aClockwise === true ) {

      angle = this.aEndAngle + ( 1 - t ) * ( Math.PI * 2 - deltaAngle );

  } else {

      angle = this.aStartAngle + t * deltaAngle;

  }
  
  var vector = new THREE.Vector3();

  vector.x = this.aX + this.xRadius * Math.cos( angle );
  vector.y = this.aY + this.yRadius * Math.sin( angle );
  vector.z = this.sZ + h;
  return vector;

};  



GCode.Curve3.prototype.getPoints = function ( divisions ) {

  if ( ! divisions ) divisions = 5;

  var d, pts = [];
  
  var h = 0;
  var hdelta = this.height / (divisions + 1);
  
  for ( d = 0; d <= divisions; d ++ ) { 
      pts.push( this.getPoint( d / divisions, h ) );
      h+=hdelta;
  }
  //console.info("ARC height sz ez delta",this.height, this.sZ, h-hdelta, hdelta);
  return pts;

};



