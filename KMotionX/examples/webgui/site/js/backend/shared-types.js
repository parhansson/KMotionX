var LOG_TYPE = {
  'NONE':0,
  'SEND':1,
  'RECEIVE':2,
  'ERROR':3,
  'PING':4
}

function BufStreamReader(buf){
  var littleEndian = true;
  var dv = new DataView(buf);
  var off = 0;
  this.skip = function(bytesToSkip){
    off = off + bytesToSkip;
  }
  this.byte = function(){
    var val = dv.getUint8(off,littleEndian);
    off = off+1;
    return val; 
  }
  this.short = function(){
    var val = dv.getUint16(off,littleEndian);
    off = off+2;
    return val; 
  }
  this.int = function(){
    var val = dv.getUint32(off,littleEndian);
    off = off+4;
    return val; 
  }
  this.double = function(){
    var val = dv.getFloat64(off,littleEndian);
    off = off+8;
    return val; 
  }
  
}