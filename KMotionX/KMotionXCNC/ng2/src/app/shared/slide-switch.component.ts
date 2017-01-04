import {Component, Self, OnInit, Input,  Output, EventEmitter}  from '@angular/core';

@Component({
  selector: 'slide-switch',
  template: `
  <label title="Simulation Mode" class="switch red" >
    <input type="checkbox" [ngModel]="state" (click)="toggle()">
    <i></i>
  </label>`,
  styleUrls: ['./slide-switch.component.css']
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