import {Component} from 'angular2/core';
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';
import {KmxStatus} from '../backend/shared'

@Component({
    selector:"kmx-dro",
    template:`
      <div>
          <div *ngFor="#name of droAxes; #index = index" class="input-group">
              <span class="input-group-addon dro-axis">{{name}}</span>
              <span class="form-control input-lg text-right dro-display">{{intStatus.dro[index] | number:'1.3-3'}}</span>
              <span class="btn input-group-addon">
                  <span (mousedown)="jog(index,-100)" (mouseup)="jog(index,0)"><span class="glyphicon glyphicon-minus"></span></span>
              </span>
              <span class="btn input-group-addon">
                  <span (mousedown)="jog(index,-100)" (mouseup)="jog(index,0)"><span class="glyphicon glyphicon-plus"></span></span>
              </span>
          </div>
          <div>Uptime:&nbsp;{{intStatus.timeStamp}}</div>
      </div>    
    `
})
export class DroComponent { 
    droAxes = ['X','Y','Z'];
    intStatus:KmxStatus
    constructor(private backendService:BackendService, private socketService:SocketService){
      this.intStatus = socketService.data;
    }
    
    jog(axis:number, speed:number){
      this.backendService.jog(axis, speed);
    }
    
}