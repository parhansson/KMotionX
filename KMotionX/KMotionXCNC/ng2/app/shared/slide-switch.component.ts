import {Component, Self, OnInit, Input,  Output, EventEmitter}  from '@angular/core';
//import {ControlValueAccessor, NgModel} from '@angular/common';
import { FORM_DIRECTIVES } from '@angular/forms'; 

@Component({
  moduleId: module.id,
  selector: 'slide-switch',
  template: `
  <label title="Simulation Mode" class="switch red" >
    <input type="checkbox" [ngModel]="state" (click)="toggle()">
    <i></i>
  </label>`,
  directives: [FORM_DIRECTIVES], 
  styleUrls: ['slide-switch.css']
})
export class SlideSwitchComponent  
{
  @Input() state:boolean = false;
  @Output() switched:EventEmitter<boolean> = new EventEmitter<boolean>();

  public constructor() {
  }

  toggle(){
    this.switched.next(!this.state);
  }

}