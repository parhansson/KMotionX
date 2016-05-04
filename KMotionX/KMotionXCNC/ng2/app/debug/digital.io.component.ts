import {Component, Injectable, Pipe, PipeTransform, Input} from '@angular/core';
import {NgClass} from '@angular/common';
import {SocketService} from '../backend/socket.service'
import {KmxStatus} from '../backend/shared'


export class IOPin {
  state: boolean = false
  output: boolean = true
  constructor(public name: string, public description: string, public number?: number) { }
}

export class Connector {
  constructor(public name: string, public pins: IOPin[]) { }
}

// @Pipe({
//   name: 'connectorFilter',
//   pure: false
// })
// @Injectable()
// export class ConnectorPipe implements PipeTransform {
//   transform(items: any[], args: any[]): any {
//     if (items instanceof Array) {

//       // filter items array, items which match and return true will be kept, false will be filtered out
//       if ('JP7' === args as any as string) {
//         return items.map((value,index)=>{return {IO:index,value:value} }).filter((item, index) => { return index >= 0 && index <= 15 || index === 44 || index === 45 });
//       }
//       if ('JP4' === args as any as string) {
//         return items.map((value,index)=>{return {IO:index,value:value} }).filter((item, index) => { return index >= 16 && index <= 25 });
//       }
//       return []//items.filter((item) => { return false });
//     }
//   }
// }


@Component({
  selector: 'connector',
  directives: [NgClass],
  //pipes: [ConnectorPipe],
  template: `
            {{getConnector().name}}
            <div *ngIf="single" class="well">
              <div class="row">
                <div class="iopin outer_circle" *ngFor="let pin of getConnectorPins(); let pinNumber = index" [title]="(pinNumber + 1) + ' - ' + pin.name + ' - ' + pin.description">
                  <div class="circle" [ngClass]="{'iopin-output': pin.output, 'iopin-input': !pin.output}">
                    <span class="glyphicon" [ngClass]="{'glyphicon-resize-full': pin.output, 'glyphicon-resize-small': !pin.output}" ></span>
                  </div>
                </div>
              </div>
            </div>
            <div *ngIf="!single" class="well">
              <div class="row">
                <div class="iopin outer_circle" *ngFor="let pin of getConnectorPins(true); let pinNumber = index" [title]="((pinNumber + 1)*2) + ' - ' + pin.name + ' - ' + pin.description">
                  <div class="circle" [ngClass]="{'iopin-output': pin.output, 'iopin-input': !pin.output}">
                    <span class="glyphicon" [ngClass]="{'glyphicon-resize-full': pin.output, 'glyphicon-resize-small': !pin.output}" ></span>
                  </div>
                </div>
              </div>
              <div class="row">
                <div class="iopin outer_circle" *ngFor="let pin of getConnectorPins(false); let pinNumber = index" [title]="((pinNumber *2 )+1) + ' - ' + pin.name + ' - ' + pin.description">
                  <div class="circle" [ngClass]="{'iopin-output': pin.output, 'iopin-input': !pin.output}">
                    <span class="glyphicon" [ngClass]="{'glyphicon-resize-full': pin.output, 'glyphicon-resize-small': !pin.output}" ></span>
                  </div>
                </div>
              </div>
            </div>
            `,
  styles: [`
      .iopin {
        display: inline-block;
        width: 40px;
        height: 40px;
        margin: 1em;
      }

      .iopin-output {
        color:#FF0000;
      }
      .iopin-input {
        color:#00FF00;
      }
      .outer_circle {
          height:20px;
          line-height:20px;
          width:20px;
          border-radius:50%;
          background-color:#000000;
          position:relative;
          margin:0 auto;
          cursor:pointer;
          }
          
      .outer_circle:after {
        position:clear
      }

      .circle {
          background-color:#aeaeae;
          border-radius:50%;
          height:14px;
          line-height:10px;
          width:14px;
          position:absolute;
          margin:3px;
          }
          
     `]
})
export class ConnectorComponent {
  intStatus: KmxStatus
  @Input() name: string
  @Input() single: boolean
  jp7 = new Connector("JP7 - GPIO", [])
  jp4 = new Connector("JP4 - Aux #0", [])
  jp6 = new Connector("JP6 - Aux #1", [])
  jp5 = new Connector("JP5 - GPIO #1", []);

  constructor(private socketService: SocketService) {
    this.intStatus = this.socketService.data
    this.jp7.pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
    this.jp7.pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
    this.jp7.pins.push(new IOPin('VDD12', '+12 Volts Output'))
    this.jp7.pins.push(new IOPin('RESET#', 'Power up Reset (low true) output'))
    this.jp7.pins.push(new IOPin('IO44', 'Gen Purpose LVTTL I/O (3.3V Only)', 44))
    this.jp7.pins.push(new IOPin('IO45', 'Gen Purpose LVTTL I/O (3.3V Only)', 45))
    this.jp7.pins.push(new IOPin('IO0', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 0 Input Phase A', 0))
    this.jp7.pins.push(new IOPin('IO1', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 0 Input Phase B', 1))
    this.jp7.pins.push(new IOPin('IO2', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 1 Input Phase A', 2))
    this.jp7.pins.push(new IOPin('IO3', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 1 Input Phase B', 3))
    this.jp7.pins.push(new IOPin('IO4', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 2 Input Phase A', 4))
    this.jp7.pins.push(new IOPin('IO5', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 2 Input Phase B', 5))
    this.jp7.pins.push(new IOPin('IO6', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 3 Input Phase A', 6))
    this.jp7.pins.push(new IOPin('IO7', 'Gen Purpose LVTTL I/O (5V Tolerant) or Encoder 3 Input Phase B', 7))
    this.jp7.pins.push(new IOPin('IO8', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 Home     (or Step 0 output)', 8))
    this.jp7.pins.push(new IOPin('IO9', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 Home     (or Dir  0 output)', 9))
    this.jp7.pins.push(new IOPin('I10', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 2 Home     (or Step 1 output)', 10))
    this.jp7.pins.push(new IOPin('I11', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 3 Home     (or Dir 1 output)', 11))
    this.jp7.pins.push(new IOPin('I12', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 + Limit   (or Step 2 output)', 12))
    this.jp7.pins.push(new IOPin('I13', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 0 - Limit    (or Dir  2 output)', 13))
    this.jp7.pins.push(new IOPin('I14', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 + Limit   (or Step 3 output)', 14))
    this.jp7.pins.push(new IOPin('I15', 'Gen Purpose LVTTL I/O (5V Tolerant) or Axis 1 - Limit    (or Dir  3 output)', 15))
    this.jp7.pins.push(new IOPin('VDD5', '+5 Volts Output'))
    this.jp7.pins.push(new IOPin('VDD5', '+5 Volts Output'))
    this.jp7.pins.push(new IOPin('GND', 'Digital Ground'))
    this.jp7.pins.push(new IOPin('GND', 'Digital Ground'))


    this.jp4.pins.push(new IOPin('VDD5', '+5 Volts Output'))
    this.jp4.pins.push(new IOPin('VDD12', '+12 Volts Output'))
    this.jp4.pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
    this.jp4.pins.push(new IOPin('RESET#', 'Power up Reset (low true) output'))
    this.jp4.pins.push(new IOPin('IO16', 'Gen Purpose LVTTL I/O (3.3V Only)', 16))
    this.jp4.pins.push(new IOPin('IO17', 'Gen Purpose LVTTL I/O (3.3V Only)', 17))
    this.jp4.pins.push(new IOPin('IO18', 'Gen Purpose LVTTL I/O (3.3V Only)', 18))
    this.jp4.pins.push(new IOPin('GND', 'Digital Ground'))
    this.jp4.pins.push(new IOPin('GND', 'Digital Ground'))
    this.jp4.pins.push(new IOPin('IO19', 'Gen Purpose LVTTL I/O (3.3V Only)', 19))
    this.jp4.pins.push(new IOPin('IO20', 'Gen Purpose LVTTL I/O (3.3V Only)', 20))
    this.jp4.pins.push(new IOPin('IO21', 'Gen Purpose LVTTL I/O (3.3V Only)', 21))
    this.jp4.pins.push(new IOPin('IO22', 'Gen Purpose LVTTL I/O (3.3V Only)', 22))
    this.jp4.pins.push(new IOPin('IO23', 'Gen Purpose LVTTL I/O (3.3V Only)', 23))
    this.jp4.pins.push(new IOPin('IO24', 'Gen Purpose LVTTL I/O (3.3V Only)', 24))
    this.jp4.pins.push(new IOPin('IO25', 'Gen Purpose LVTTL I/O (3.3V Only)', 25))

    this.jp6.pins.push(new IOPin('VDD5', '+5 Volts Output'))
    this.jp6.pins.push(new IOPin('VDD12', '+12 Volts Output'))
    this.jp6.pins.push(new IOPin('VDD33', '+3.3 Volts Output'))
    this.jp6.pins.push(new IOPin('RESET#', 'Power up Reset (low true) output'))
    this.jp6.pins.push(new IOPin('IO26', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM0 out', 26))
    this.jp6.pins.push(new IOPin('IO27', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM1 out', 27))
    this.jp6.pins.push(new IOPin('IO28', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM2 out', 28))
    this.jp6.pins.push(new IOPin('GND', 'Digital Ground'))
    this.jp6.pins.push(new IOPin('GND', 'Digital Ground'))
    this.jp6.pins.push(new IOPin('IO29', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM3 out', 29))
    this.jp6.pins.push(new IOPin('IO30', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM4 out', 30))
    this.jp6.pins.push(new IOPin('IO31', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM5 out', 31))
    this.jp6.pins.push(new IOPin('IO32', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM6 out', 32))
    this.jp6.pins.push(new IOPin('IO33', 'Gen Purpose LVTTL I/O (3.3V Only) or PWM7 out', 33))
    this.jp6.pins.push(new IOPin('IO34', 'Gen Purpose LVTTL I/O (3.3V Only)', 34))
    this.jp6.pins.push(new IOPin('IO35', 'Gen Purpose LVTTL I/O (3.3V Only)', 35))


    this.jp5.pins.push(new IOPin('IO36', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Step 4 output) or (Encoder 4 Input Phase A)', 36))
    this.jp5.pins.push(new IOPin('IO37', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Dir 4 output)    or (Encoder 4 Input Phase B)', 37))
    this.jp5.pins.push(new IOPin('IO38', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Step 5 output) or (Encoder 5 Input Phase A)', 38))
    this.jp5.pins.push(new IOPin('IO39', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Dir 5 output)    or (Encoder 5 Input Phase B)', 39))
    this.jp5.pins.push(new IOPin('IO40', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Step 6 output) or (Encoder 6 Input Phase A)', 40))
    this.jp5.pins.push(new IOPin('IO41', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Dir 6 output)    or (Encoder 6 Input Phase B)', 41))
    this.jp5.pins.push(new IOPin('IO42', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Step 7 output) or (Encoder 7 Input Phase A)', 42))
    this.jp5.pins.push(new IOPin('IO43', 'Gen Purpose LVTTL I/O (5V Tolerant) or  (or Dir 7 output)    or (Encoder 7 Input Phase B)', 43))

  }

  getConnectorPins(even?: boolean) {
    //The modulus operation might seem backwards. 
    //But it is not the index but the pin number wich is index + 1 we are counting as even or not
    if (even === true) {
      return this.getConnector().pins.filter((item, index) => { return index % 2 != 0 })
    } else if (even === false) {
      return this.getConnector().pins.filter((item, index) => { return index % 2 == 0 })
    } else {
      return this.getConnector().pins
    }
  }


  getConnector() {
    let connector: Connector = null
    if (this.name === 'JP7') {
      connector = this.jp7
    } else if (this.name === 'JP4') {
      connector = this.jp4
    } else if (this.name === 'JP6') {
      connector = this.jp6
    } else if (this.name === 'JP5') {
      connector = this.jp5
    }


    if (connector != null) {
      for (let pin of connector.pins) {
        if (pin.number !== undefined) {
          pin.state = this.intStatus.bitsState[pin.number]
          pin.output = this.intStatus.bitsDirection[pin.number]
        }
      }

    }
    return connector
  }

}


@Component({
  selector: 'digital-io',
  directives: [ConnectorComponent],
  template: `
            <connector [name]="'JP7'"></connector>
            <connector [name]="'JP4'"></connector>
            <connector [name]="'JP6'"></connector>
            <connector [name]="'JP5'" [single]="true"></connector>
            `,

})

export class DigitalIOComponent {
  constructor() {


  }

}

