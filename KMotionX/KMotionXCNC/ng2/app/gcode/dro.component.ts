import {Component} from 'angular2/core';
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';

@Component({
    selector:"kmx-dro",
    //directives: [AceDirective,LogComponent],
    template:`
      <div>
          <div *ngFor="#name of droAxes; #index = index" class="input-group">
              <span class="input-group-addon dro-axis">{{name}}</span>
              <span class="form-control input-lg text-right dro-display">{{dro[index] | number:'1.3-3'}}</span>
              <span class="btn input-group-addon">
                  <span (mousedown)="jog(index,-100)" (mouseup)="jog(index,0)"><span class="glyphicon glyphicon-minus"></span></span>
              </span>
              <span class="btn input-group-addon">
                  <span (mousedown)="jog(index,-100)" (mouseup)="jog(index,0)"><span class="glyphicon glyphicon-plus"></span></span>
              </span>
          </div>
          <div>Uptime:&nbsp;{{timeStamp | number:'1.1-3'}}</div>
      </div>    
    `
})
export class DroComponent { 
    droAxes = ['X','Y','Z'];
    dro = [0,0,0];
    timestamp = "";
    
    constructor(private backendService:BackendService, private socketService:SocketService){
      this.dro = socketService.data.dro;
      this.timestamp = socketService.data.timeStamp;
    }
    
    jog(axis:number, speed:number){
      this.backendService.jog(axis, speed);
    }
    
}