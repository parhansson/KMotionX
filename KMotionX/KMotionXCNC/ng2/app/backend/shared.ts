export class Message<T> {
  isText: boolean = false
  isJson: boolean = false
  isStatus: boolean = false
  isLog: boolean = false
  message: T
  constructor(message: T) {
    this.message = message
  }

}
export class TextMessage extends Message<string>{

  constructor(message: string) {
    super(message)
    this.isText = true
  }
}
export class JsonMessage extends Message<any>{
  constructor(message: any) {
    super(message)
    this.isJson = true
  }
}
export class StatusMessage extends Message<KmxStatus>{
  constructor(message: KmxStatus) {
    super(message)
    this.isStatus = true
  }
}

export class LogMessage extends Message<any>{
  constructor(message: any, public type: number) {
    super(message)
    this.isLog = true

  }
}

export interface Status {
  bitsDirection: boolean[]
  bitsState: boolean[]
  dro: number[]
  currentLine: number
  gcodeFile: string
}
export class KmxStatus implements Status {
  timeStamp: number = 0
  version: number = 0
  size: number = 0
  position: number[] = []
  dest: number[] = []
  inputModes: number[] = []
  outputModes: number[] = []
  enables: boolean[] = []
  axisDone: boolean[] = []
  bitsDirection: boolean[] = []
  bitsState: boolean[] = []
  runOnStartUp: boolean[] = []
  threadActive: boolean[] = []
  stopImmediateState: number = -1
  dro: number[] = []
  feedHold: boolean = false
  connected: boolean = false
  simulating: boolean = true
  interpreting: boolean = false
  currentLine: number = 0
  gcodeFile: string = ""
  gcodeFileTimestamp: number = 0;
  machineSettingsFile: string = ""
  machineSettingsFileTimestamp: number  = 0;

  constructor() { }

  copyFrom(from: KmxStatus) {
    for (var key in from) {
      //copy all the fields
      this[key] = from[key];
    }
  }
}