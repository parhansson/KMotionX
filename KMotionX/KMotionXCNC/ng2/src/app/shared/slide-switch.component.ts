import { Component, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'slide-switch',
  templateUrl: './slide-switch.component.html',
  styleUrls: ['./slide-switch.component.css']
})
export class SlideSwitchComponent {
  @Input() state: boolean = false;
  @Output() switched: EventEmitter<boolean> = new EventEmitter<boolean>();

  public constructor() {
  }

  toggle() {
    this.switched.next(!this.state);
  }

}