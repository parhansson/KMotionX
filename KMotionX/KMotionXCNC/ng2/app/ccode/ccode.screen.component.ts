import {Component, Inject} from '@angular/core';
import {ScreenComponent} from "../screen.component"
import {AceEditorComponent} from '../editor/ace.editor.component'
import {FileResource} from '../backend/file'

@Component({
  selector: 'ccode-screen',
  directives: [AceEditorComponent],
  template: `
    <code-editor id="ccodeEditor" [resource]="resource" mode="c_cpp" >
      <buttons>TODO add compile, load and execute buttons</buttons>
    </code-editor>`
})
export class CCodeScreenComponent extends ScreenComponent {
  resource:FileResource
  
  constructor() {
    super()
    this.resource = new FileResource("./settings/c-programs", '');
  }
}