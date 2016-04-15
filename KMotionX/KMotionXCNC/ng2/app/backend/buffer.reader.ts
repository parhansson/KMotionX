
declare var StringView: any;

export class BufStreamReader{
  littleEndian = true;
  dv:DataView;
  off = 0;
  
  constructor(private buf:ArrayBuffer){
    this.dv = new DataView(this.buf);
  }
  
  skip(bytesToSkip:number){
    this.off = this.off + bytesToSkip;
  }
  byte(){
    var val = this.dv.getUint8(this.off);
    this.off = this.off+1;
    return val; 
  }
  short(){
    var val = this.dv.getUint16(this.off,this.littleEndian);
    this.off = this.off+2;
    return val; 
  }
  int(){
    var val = this.dv.getUint32(this.off,this.littleEndian);
    this.off = this.off+4;
    return val; 
  }
  double(){
    var val = this.dv.getFloat64(this.off,this.littleEndian);
    this.off = this.off+8;
    return val; 
  }
  string():string{
    var len = this.int();
    var val = new StringView(this.dv.buffer,'utf-8',this.off,len).valueOf();
    this.off = this.off+len;
    return val;
    //stringView new StringView(arrayBuffer input, optional DOMString encoding, optional unsigned long bytethis.offset, optional unsigned long length);
  }
  
}