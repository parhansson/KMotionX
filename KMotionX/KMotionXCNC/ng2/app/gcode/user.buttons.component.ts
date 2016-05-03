import {Component} from 'angular2/core';
import {SettingsService} from '../settings/settings.service';
import {BackendService} from '../backend/backend.service';

@Component({
  selector: "user-defined-buttons",
  template: `
    <span *ngFor="#action of machine.userActions; #index = index">
      <button  
        *ngIf="action.action > 0" 
        class='btn btn-primary btn-grid'
        (click)="onUserButton(index, action)">{{action.name || '[Unnamed]'}}
      </button>
    </span>   
    `
})
export class UserButtonsComponent {
  machine: any;

  constructor(settingsService: SettingsService, private backendService:BackendService) {
    this.machine = settingsService.machine;
  }

  onUserButton(index: number, action: any) {
    console.log(index, action);
    this.backendService.onInvokeAction(index+11);
  }

}