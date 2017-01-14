
export class BufferReader {
  littleEndian = true;
  dv: DataView;
  off = 0;

  constructor(private buf: ArrayBuffer) {
    this.dv = new DataView(this.buf);
  }

  skip(bytesToSkip: number) {
    this.off = this.off + bytesToSkip;
  }
  byte() {
    var val = this.dv.getUint8(this.off);
    this.off += 1;
    return val;
  }
  short() {
    var val = this.dv.getUint16(this.off, this.littleEndian);
    this.off += 2;
    return val;
  }
  int() {
    var val = this.dv.getUint32(this.off, this.littleEndian);
    this.off += 4;
    return val;
  }
  double() {
    var val = this.dv.getFloat64(this.off, this.littleEndian);
    this.off += 8;
    return val;
  }
  string(): string {
    var len = this.int();
    var val = this.readUtf8String(new Uint8Array(this.dv.buffer), this.off, len);
    //TODO len is not always the same as byteLen on utf-8;
    this.off += len;
    return val;
  }

  private readUtf8String(array: Uint8Array, offset: number, chars: number) {
    let out: string, i: number, len: number;
    let char1: number, char2: number, char3: number;

    out = '';
    len = array.byteLength;
    i = offset;
    while (i < len) {
      char1 = array[i++];
      switch (char1 >> 4) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
          // 0xxxxxxx
          out += String.fromCharCode(char1);
          break;
        case 12: case 13:
          // 110x xxxx   10xx xxxx
          char2 = array[i++];
          out += String.fromCharCode(((char1 & 0x1F) << 6) | (char2 & 0x3F));
          break;
        case 14:
          // 1110 xxxx  10xx xxxx  10xx xxxx
          char2 = array[i++];
          char3 = array[i++];
          out += String.fromCharCode(
            ((char1 & 0x0F) << 12) |
            ((char2 & 0x3F) << 6) |
            ((char3 & 0x3F) << 0));
          break;
      }
      if (out.length >= chars) {
        return out;
      }
    }

    return out;
  }
}
