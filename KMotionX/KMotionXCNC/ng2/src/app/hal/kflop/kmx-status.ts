import { Status } from '../status'

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
  gcodeFile: string = ''
  gcodeFileTimestamp: number = 0;
  machineSettingsFile: string = ''
  machineSettingsFileTimestamp: number = 0;

  constructor() { }

  copyFrom(from: KmxStatus) {
    for (let key in from) {
      //copy all the fields
      this[key] = from[key];
    }
  }
}
