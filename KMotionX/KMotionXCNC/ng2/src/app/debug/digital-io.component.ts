import { Component, Injectable, Pipe, PipeTransform, Input, Attribute } from '@angular/core';
import { SocketService } from '../backend/socket.service'
import { KFlop, KmxStatus } from '../hal/kflop'
import { Connector, IOPin } from '../hal'




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
  //pipes: [ConnectorPipe],
  template: `
            {{connector.name}}
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
  @Input() single: boolean
  kflop = KFlop.getInstance()
  connector: Connector

  constructor(private socketService: SocketService, @Attribute('name') name: string) {
    this.intStatus = this.socketService.data
    this.connector = this.kflop.getConnector(name)
  }

  getConnectorPins(even?: boolean) {
    //The modulus operation might seem backwards. 
    //But it is not the index but the pin number wich is index + 1 we are counting as even or not
    this.kflop.update(this.connector, this.intStatus)
    if (even === true) {
      return this.connector.pins.filter((item, index) => { return index % 2 != 0 })
    } else if (even === false) {
      return this.connector.pins.filter((item, index) => { return index % 2 == 0 })
    } else {
      return this.connector.pins
    }
  }

}


@Component({
  selector: 'digital-io',
  template: `
            <connector name="JP7"></connector>
            <connector name="JP4"></connector>
            <connector name="JP6"></connector>
            <connector name="JP5" [single]="true"></connector>
            `,

})
export class DigitalIOComponent {
  constructor() { }
}

