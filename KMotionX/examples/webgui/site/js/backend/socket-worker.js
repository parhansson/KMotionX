
importScripts('shared-types.js');
importScripts('socket-handler.js');

var socket;
function messageHandler(data){
  if(data instanceof ArrayBuffer){
    var status = parseStatus(data);      
    postMessage({status:true,message:status});

  } else if(data instanceof Blob){
    var reader = new FileReader();
    reader.addEventListener("loadend", function() {
       // reader.result contains the contents of blob as a typed array
      var status = parseStatus(reader.result);      
      postMessage({status:true,message:status});
    });
    reader.readAsArrayBuffer(data);
  } else {
    try{
      var obj = JSON.parse(data);
      postMessage({data:true,message:obj});
      //ack messages that don't require users answer here
      if(!obj.block){    
        socket.acknowledge(obj, -1);
      }
      
    } catch(e){
      console.log(data);
      logHandler("Error handling message: " + data, LOG_TYPE.ERROR);
      throw e;
    }
  }
  
}

function parseStatus(arraybuffer){
  
  var r = new BufStreamReader(arraybuffer);
  //var /*int*/ VersionAndSize = r.int();  //bits 16-23 = version, bits 0-15 = size in words
  var /*short*/ Size = r.short();      //bits 0-15 = size in words
  var /*short*/ Version = r.short();   //bits 16-23 = version
  //int ADC[N_ADCS+2*N_ADCS_SNAP]; //[24]*4
  r.skip(96);
  //int DAC[N_DACS];               //[8]*4
  r.skip(32);
  //int PWM[N_PWMS+2*N_PWMS_SNAP]; //[16]*4
  r.skip(64);
  
  //double Position[N_CHANNELS];   //[8]*8
  var positions = [];
  for (var i=0; i<8; i++) {
    positions[i] = r.double();
  }

  //double Dest[N_CHANNELS];       //[8]*8
  var dests = [];
  for (var i=0; i<8; i++) {
    dests[i] = r.double();
  }
 
  r.skip(8);
  //unsigned char OutputChan0[N_CHANNELS]; //[8]*1

  var /*int*/ InputModes = r.int();         // 4 bits for each axis 
  var /*int*/ InputModes2 = r.int();        // 4 bits for each axis 
  
  var /*int*/ OutputModes = r.int();        // 4 bits for each axis 
  var /*int*/ OutputModes2 = r.int();       // 4 bits for each axis 
  var /*int*/ Enables = r.int();            // 1 bit  for each axis 
  var /*int*/ AxisDone = r.int();           // 1 bit  for each axis
  
  //int BitsDirection[2];// KMotion - 64 bits of I/O direction 1 = output
  var /*int*/ BitsDirection0 = r.int();
  var /*int*/ BitsDirection1 = r.int();
  
  //int BitsState[2];    // KMotion - 64 bits of state lsb=I/O bit0
  var /*int*/ BitsState0 = r.int();
  var /*int*/ BitsState1 = r.int();
  
  
  var /*int*/ SnapBitsDirection0 = r.int();   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 0
  var /*int*/ SnapBitsDirection1 = r.int();   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 1
  var /*int*/ SnapBitsState0 = r.int();       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 0
  var /*int*/ SnapBitsState1 = r.int();       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 1
  var /*int*/ KanalogBitsStateInputs = r.int();   // Kanalog - 16 bits 128-143
  var /*int*/ KanalogBitsStateOutputs = r.int();  // Kanalog - 24 bits 144-167
  
  var /*int*/ RunOnStartUp = r.int();           // word Bits 1-7 selects which threads to execute on startup   
  var /*int*/ ThreadActive = r.int();           // one bit for each thread, 1=active, bit 0 - primary
  var /*int*/ StopImmediateState = r.int();     // Status of Stop Coordinated Motion Immediately
  
  var /*double*/ TimeStamp = r.double();        // Time in seconds (since KFlop boot) this status was aquired
  
  //int PC_comm[N_PC_COMM_PERSIST];// 8 persist Variables constantly uploaded to send misc commands/data to PC
  r.skip(32);
  
  var /*int*/ VirtualBits  = r.int();           // Virtual I/O bits simulated in memory
  var /*int*/ VirtualBitsEx0 = r.int();         // only upload 32 1024 Expanded Virtual Bits 
  
  //size = 464
  var ThreadActiveArr = toBitArr(ThreadActive, 8);
  
  var dros = [];
  for (var i=0; i<6; i++) {
    dros[i] = r.double();
  }
  
  
  var status = {
      version: Version,
      size:Size,
      position: positions,
      dest: dests,
      inputModes: decodeBits(InputModes,16,4).concat(decodeBits(InputModes2,16,4)),
      outputModes: decodeBits(OutputModes,16,4).concat(decodeBits(OutputModes2,16,4)),
      enables:toBitArr(Enables, 8),
      axisDone: toBitArr(AxisDone, 8),
      bitsDirection : toBitArr(BitsDirection0, 32).concat(toBitArr(BitsDirection1, 32)).join(', '),
      bitsState: toBitArr(BitsState0, 32).concat(toBitArr(BitsState1, 32)).join(', '),
      runOnStartUp: toBitArr(RunOnStartUp, 8),
      threadActive: toBitArr(ThreadActive, 8),
      stopImmediateState: StopImmediateState,
      timeStamp: TimeStamp,
      dro:dros
      
  };
  
  return status;
}

function toBitArr(value, size){
  var bitArr = [];
  for(var i = 0;i<size;i++) {
    bitArr[i] =  value >> i & 1; 
  }
  return bitArr;
}

function decodeBits(value, size, bits){
  //Need to handle sign bit hence the & and xor
  var andOp = 0xFFFFFFFF << bits & 0x7FFFFFFF ^ 0x7FFFFFFF;
  var valueArr = [];
  var parts = size/bits;
  for(var i = 0;i<parts;i++) {
    valueArr[i] =  value >> (bits*i) & andOp; 
  }
  return valueArr;  
}

function logHandler(message, type) {
  postMessage({log:true,type:type,message:message});
}
  
function onMessage(event){
  if(event.data.command == 'connect'){   
    var url = event.data.url; 
    socket = new SocketHandler(url, messageHandler, logHandler);
    socket.connect();    
  } else if(event.data.command == 'acknowledge'){
      socket.acknowledge(event.data.obj, event.data.ret);
  } else if(event.data.command == 'disconnect'){
    //no need to acually disconnect at the moment
    socket.destroy();
    postMessage('done');
  }
  
}
self.addEventListener('message', onMessage, false);


