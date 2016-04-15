import {Component, Inject} from 'angular2/core';
import {Alert} from 'ng2-bootstrap/ng2-bootstrap';
import {LogComponent} from "../log/log.component"
import {ScreenComponent} from "../screen.component"


@Component({
  selector: 'debug-screen',
  directives: [Alert, LogComponent],
  templateUrl: 'dist/app/debug/debug.screen.html'
})
export class DebugScreenComponent extends ScreenComponent{

  constructor() {
    super()
  }

}