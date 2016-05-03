import {Component,Inject} from 'angular2/core';
import {AceDirective} from "../editor/ace.directive";
import {ScreenComponent} from "../screen.component"
import {KMXUtil}    from '../util/KMXUtil'
import {ResourceComponent,FilePathComponent, FileResource, FileDropZone}    from '../resources/resource.component'
@Component({
    selector: 'ccode-screen',
    directives: [AceDirective,ResourceComponent,FilePathComponent, FileDropZone],
    templateUrl:'dist/app/ccode/ccode.screen.html'
})
export class CCodeScreenComponent extends ScreenComponent{ 
    editorContent:string
    resource:FileResource
    constructor(){
      super()
      this.resource = new FileResource("./settings/c-programs",'');
    }

  onOpenFile(event) {
    this.editorContent = KMXUtil.ab2str(event.payload)
  }
}