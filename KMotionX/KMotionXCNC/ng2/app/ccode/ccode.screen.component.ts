import {Component,Inject} from 'angular2/core';
import {AceDirective} from "../editor/ace.directive";
import {ScreenComponent} from "../screen.component"
@Component({
    selector: 'ccode-screen',
    directives: [AceDirective],
    templateUrl:'dist/app/ccode/ccode.screen.html'
})
export class CCodeScreenComponent extends ScreenComponent{ 

    constructor(){
      super()
    }

}