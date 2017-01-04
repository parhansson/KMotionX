import { Component } from '@angular/core';
import { SettingsService, Machine } from '../settings/settings.service';
import { BackendService } from '../backend/backend.service';

@Component({
  selector: 'user-defined-buttons',
  template: `
    <span *ngFor="let action of machine.userActions; let index = index">
      <button  
        *ngIf="action.action > 0" 
        class='btn btn-primary btn-grid'
        (click)="onUserButton(index, action)">{{action.name || '[Unnamed]'}}
      </button>
    </span>   
    `
})
export class UserButtonsComponent {
  machine: Machine;

  constructor(settingsService: SettingsService, private backendService: BackendService) {
    settingsService.subject.subscribe(machine => this.machine = machine);
  }

  onUserButton(index: number, action: any) {
    console.log(index, action);
    this.backendService.onInvokeAction(index + 11);
  }

}