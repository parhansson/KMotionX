import {Component,Inject} from 'angular2/core';
import {AceDirective} from "../editor/ace.directive";
import {ScreenComponent} from "../screen.component"
import {KMXUtil}    from '../util/KMXUtil'
import {ResourceComponent}    from '../resources/resource.component'
@Component({
    selector: 'ccode-screen',
    directives: [AceDirective,ResourceComponent],
    templateUrl:'dist/app/ccode/ccode.screen.html'
})
export class CCodeScreenComponent extends ScreenComponent{ 
    editorContent:string
    constructor(){
      super()
    }

  onOpenFile(event) {
    //this.open(event.contentType, event.payload)
    this.editorContent = KMXUtil.ab2str(event.payload)
    
  }
}