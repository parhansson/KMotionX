import {Component, Inject} from '@angular/core';
import {ScreenComponent} from '../screen.component'

@Component({
  //moduleId: module.id,
  selector: 'debug-screen',
  templateUrl: './debug-screen.html'
})
export class DebugScreenComponent extends ScreenComponent{

  constructor() {
    super()
  }

}