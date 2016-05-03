import {Component, Inject} from '@angular/core';
import {LogComponent} from "../log/log.component"
import {ScreenComponent} from "../screen.component"
import {DigitalIOComponent} from "./digital.io.component"


@Component({
  selector: 'debug-screen',
  directives: [LogComponent,DigitalIOComponent],
  templateUrl: 'dist/app/debug/debug.screen.html'
})
export class DebugScreenComponent extends ScreenComponent{

  constructor() {
    super()
  }

}