import {Component, Injectable, Pipe, PipeTransform,Input} from '@angular/core';
import {NgClass} from '@angular/common';
import {SocketService} from '../backend/socket.service'
import {KmxStatus,IOPin} from '../backend/shared'

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
            {{name}}
            <div class="well">
              <div class="row">
                <div class="iopin outer_circle" *ngFor="let pin of getConnectorPins(true)">
                  <div class="circle" [attr.style]="pin.state?'color:green':'color:red'">
                  <span class="glyphicon" [ngClass]="{'glyphicon-resize-full': pin.output, 'glyphicon-resize-small': !pin.output}" ></span>
                  </div>
                </div>
              </div>
              <div class="row">
                <div class="iopin outer_circle" *ngFor="let pin of getConnectorPins(false)">
                  <div class="circle" [attr.style]="pin.state?'color:green':'color:red'">
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


.outer_circle {
    height:20px;
    line-height:20px;
    width:20px;
    border-radius:50%;
    background-color:#000000;
    position:relative;
    margin:0 auto;
    }
.outer_circle:after {
  
  position:clear
}
.outer_circle p {
    text-align:center;
    vertical-align:middle;
    color:#ffffff;
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

.circle p {
    font-size:10px;
    vertical-align:middle;
    color:#000000;
    }            
            `]
})
export class ConnectorComponent {
  intStatus: KmxStatus
  @Input() name:string
  constructor(private socketService: SocketService) {
    this.intStatus = this.socketService.data

  }
  
  getConnectorPins(even:boolean){
    if(even){
      return this.intStatus.getConnector(this.name).pins.filter((item,index)=>{return index % 2 == 0})
    } else {
      return this.intStatus.getConnector(this.name).pins.filter((item,index)=>{return index % 2 != 0})
    }
  }

}


@Component({
  selector: 'digital-io',
  directives: [ConnectorComponent],
  template: `
            <connector [name]="'JP7'"></connector>
            <connector [name]="'JP4'"></connector>
            `,

})

export class DigitalIOComponent {
  constructor() {


  }

}

