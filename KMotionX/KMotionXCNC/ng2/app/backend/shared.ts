
export class IOPin {
  state: boolean = false
  output: boolean = true
  constructor(public name: string, public description: string, public number?: number) { }
}

export class Connector {
  constructor(public name: string, public pins: IOPin[]) { }
}


export class KmxStatus {
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
  machineSettingsFile: string = ""

  constructor() { }

  copyFrom(from: KmxStatus) {
    for (var key in from) {
      //copy all the fields
      this[key] = from[key];
    }
  }

  getConnector(name: string) {
    if (name === 'JP7') {
      let pins: IOPin[] = []
      pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
      pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
      pins.push(new IOPin('VDD12', '+12 Volts Output'))
      pins.push(new IOPin('RESET#', 'Power up Reset (low true) output'))
      pins.push(new IOPin('IO44', 'Gen Purpose LVTTL I/O (3.3V Only)', 44))
      pins.push(new IOPin('IO45', 'Gen Purpose LVTTL I/O (3.3V Only)', 45))
      pins.push(new IOPin('IO0', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 0 Input Phase A', 0))
      pins.push(new IOPin('IO1', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 0 Input Phase B', 1))
      pins.push(new IOPin('IO2', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 1 Input Phase A', 2))
      pins.push(new IOPin('IO3', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 1 Input Phase B', 3))
      pins.push(new IOPin('IO4', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 2 Input Phase A', 4))
      pins.push(new IOPin('IO5', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 2 Input Phase B', 5))
      pins.push(new IOPin('IO6', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 3 Input Phase A', 6))
      pins.push(new IOPin('IO7', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 3 Input Phase B', 7))
      pins.push(new IOPin('IO8', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 Home     (or Step 0 output)', 8))
      pins.push(new IOPin('IO9', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 Home     (or Dir  0 output)', 9))
      pins.push(new IOPin('I10', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 2 Home     (or Step 1 output)', 10))
      pins.push(new IOPin('I11', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 3 Home     (or Dir 1 output)', 11))
      pins.push(new IOPin('I12', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 + Limit   (or Step 2 output)', 12))
      pins.push(new IOPin('I13', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 - Limit    (or Dir  2 output)', 13))
      pins.push(new IOPin('I14', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 + Limit   (or Step 3 output)', 14))
      pins.push(new IOPin('I15', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 - Limit    (or Dir  3 output)', 15))
      pins.push(new IOPin('VDD5', '+5 Volts Output'))
      pins.push(new IOPin('VDD5', '+5 Volts Output'))
      pins.push(new IOPin('GND', 'Digital Ground'))
      pins.push(new IOPin('GND', 'Digital Ground'))
      let connector = new Connector(name, pins);

      for (let pin of connector.pins) {
        if (pin.number !== undefined) {
          pin.state = this.bitsState[pin.number]
          pin.output = this.bitsDirection[pin.number]
        }
      }
      return connector
    }
    if (name === 'JP4') {
      let pins: IOPin[] = []
      pins.push(new IOPin('VDD5', '+5 Volts Output'))
      pins.push(new IOPin('VDD12', '+12 Volts Output'))
      pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
      pins.push(new IOPin('RESET#', 'Power up Reset (low true) output'))
      pins.push(new IOPin('IO16', 'Gen Purpose LVTTL I/O (3.3V Only)', 16))
      pins.push(new IOPin('IO17', 'Gen Purpose LVTTL I/O (3.3V Only)', 17))
      pins.push(new IOPin('IO18', 'Gen Purpose LVTTL I/O (3.3V Only)', 18))
      pins.push(new IOPin('GND', 'Digital Ground'))
      pins.push(new IOPin('GND', 'Digital Ground'))
      pins.push(new IOPin('IO19', 'Gen Purpose LVTTL I/O (3.3V Only)', 19))
      pins.push(new IOPin('IO20', 'Gen Purpose LVTTL I/O (3.3V Only)', 20))
      pins.push(new IOPin('IO21', 'Gen Purpose LVTTL I/O (3.3V Only)', 21))
      pins.push(new IOPin('IO22', 'Gen Purpose LVTTL I/O (3.3V Only)', 22))
      pins.push(new IOPin('IO23', 'Gen Purpose LVTTL I/O (3.3V Only)', 23))
      pins.push(new IOPin('IO24', 'Gen Purpose LVTTL I/O (3.3V Only)', 24))
      pins.push(new IOPin('IO25', 'Gen Purpose LVTTL I/O (3.3V Only)', 25))


      let connector = new Connector(name, pins);

      for (let pin of connector.pins) {
        if (pin.number !== undefined) {
          pin.state = this.bitsState[pin.number]
          pin.output = this.bitsDirection[pin.number]
        }
      }
      return connector
    }

    return null
  }
}