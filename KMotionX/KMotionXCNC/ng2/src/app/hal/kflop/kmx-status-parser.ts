import { BufferReader } from '../../util'
import { KmxStatus } from './kmx-status'

export class KmxStatusParser {

  readBuffer(arraybuffer: ArrayBuffer) {
    let r = new BufferReader(arraybuffer);
    //let /*int*/ VersionAndSize = r.int();  //bits 16-23 = version, bits 0-15 = size in words
    let /*short*/ Size = r.short();      //bits 0-15 = size in words
    let /*short*/ Version = r.short();   //bits 16-23 = version
    //int ADC[N_ADCS+2*N_ADCS_SNAP]; //[24]*4
    r.skip(96);
    //int DAC[N_DACS];               //[8]*4
    r.skip(32);
    //int PWM[N_PWMS+2*N_PWMS_SNAP]; //[16]*4
    r.skip(64);

    //double Position[N_CHANNELS];   //[8]*8
    let positions: number[] = [];
    for (let i = 0; i < 8; i++) {
      positions.push(r.double());
    }

    //double Dest[N_CHANNELS];       //[8]*8
    let dests: number[] = [];
    for (let i = 0; i < 8; i++) {
      dests.push(r.double());
    }

    r.skip(8);
    //unsigned char OutputChan0[N_CHANNELS]; //[8]*1

    let /*int*/ InputModes = r.int();         // 4 bits for each axis 
    let /*int*/ InputModes2 = r.int();        // 4 bits for each axis 

    let /*int*/ OutputModes = r.int();        // 4 bits for each axis 
    let /*int*/ OutputModes2 = r.int();       // 4 bits for each axis 
    let /*int*/ Enables = r.int();            // 1 bit  for each axis 
    let /*int*/ AxisDone = r.int();           // 1 bit  for each axis

    //int BitsDirection[2];// KMotion - 64 bits of I/O direction 1 = output
    let /*int*/ BitsDirection0 = r.int();
    let /*int*/ BitsDirection1 = r.int();

    //int BitsState[2];    // KMotion - 64 bits of state lsb=I/O bit0
    let /*int*/ BitsState0 = r.int();
    let /*int*/ BitsState1 = r.int();


    let /*int*/ SnapBitsDirection0 = r.int();   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 0
    let /*int*/ SnapBitsDirection1 = r.int();   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 1
    let /*int*/ SnapBitsState0 = r.int();       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 0
    let /*int*/ SnapBitsState1 = r.int();       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 1
    let /*int*/ KanalogBitsStateInputs = r.int();   // Kanalog - 16 bits 128-143
    let /*int*/ KanalogBitsStateOutputs = r.int();  // Kanalog - 24 bits 144-167

    let /*int*/ RunOnStartUp = r.int();           // word Bits 1-7 selects which threads to execute on startup   
    let /*int*/ ThreadActive = r.int();           // one bit for each thread, 1=active, bit 0 - primary
    let /*int*/ StopImmediateState = r.int();     // Status of Stop Coordinated Motion Immediately

    let /*double*/ TimeStamp = r.double();        // Time in seconds (since KFlop boot) this status was aquired

    //int PC_comm[N_PC_COMM_PERSIST];// 8 persist Variables constantly uploaded to send misc commands/data to PC
    r.skip(32);

    let /*int*/ VirtualBits = r.int();           // Virtual I/O bits simulated in memory
    let /*int*/ VirtualBitsEx0 = r.int();         // only upload 32 1024 Expanded Virtual Bits 

    //size = 464
    let ThreadActiveArr = this.toBitArr(ThreadActive, 8);

    let dros: number[] = [];
    for (let i = 0; i < 6; i++) {
      dros.push(r.double());
    }
    let connected = r.byte() > 0;
    let simulate = r.byte() > 0;
    let interpreting = r.byte() > 0;
    let currentLine = r.int();
    let gcodeFile = r.string();
    let gcodeFileTimestamp = r.int();
    let machineSettingsFile = r.string();
    let machineSettingsFileTimestamp = r.int();


    let status = new KmxStatus()
    status.timeStamp = TimeStamp;
    status.version = Version;
    status.size = Size;
    status.position = positions;
    status.dest = dests;
    status.inputModes = this.decodeBits(InputModes, 16, 4).concat(this.decodeBits(InputModes2, 16, 4));
    status.outputModes = this.decodeBits(OutputModes, 16, 4).concat(this.decodeBits(OutputModes2, 16, 4));
    status.enables = this.toBitArr(Enables, 8);
    status.axisDone = this.toBitArr(AxisDone, 8);
    status.bitsDirection = this.toBitArr(BitsDirection0, 32).concat(this.toBitArr(BitsDirection1, 32))
    status.bitsState = this.toBitArr(BitsState0, 32).concat(this.toBitArr(BitsState1, 32))
    status.runOnStartUp = this.toBitArr(RunOnStartUp, 8);
    status.threadActive = this.toBitArr(ThreadActive, 8);
    status.stopImmediateState = StopImmediateState;
    status.dro = dros;
    status.feedHold = StopImmediateState > 0
    status.connected = connected;
    status.simulating = simulate;
    status.interpreting = interpreting;
    status.currentLine = currentLine;
    status.gcodeFile = gcodeFile;
    status.gcodeFileTimestamp = gcodeFileTimestamp;
    status.machineSettingsFile = machineSettingsFile;
    status.machineSettingsFileTimestamp = machineSettingsFileTimestamp;

    return status;
  }

  toBitArr(value: number, size: number) {
    let bitArr: boolean[] = [];
    for (let i = 0; i < size; i++) {
      bitArr[i] = (value >> i & 1) == 1;
    }
    return bitArr;
  }

  decodeBits(value: number, size: number, bits: number) {
    //Need to handle sign bit hence the & and xor
    let andOp = 0xFFFFFFFF << bits & 0x7FFFFFFF ^ 0x7FFFFFFF;
    let valueArr: number[] = [];
    let parts = size / bits;
    for (let i = 0; i < parts; i++) {
      valueArr[i] = value >> (bits * i) & andOp;
    }
    return valueArr;
  }

}

