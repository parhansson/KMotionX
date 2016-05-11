import {Component, Inject, ViewChild} from '@angular/core';
import {ScreenComponent} from "../screen.component"
import {AceEditorComponent} from '../editor/ace.editor.component'
import {FileResource} from '../resources/FileResource'

@Component({
  selector: 'ccode-screen',
  directives: [AceEditorComponent],
  template: `
    <code-editor id="ccodeEditor" mode="c_cpp" >
      <buttons>TODO add compile, load and execute buttons</buttons>
    </code-editor>`
})
export class CCodeScreenComponent extends ScreenComponent {
  @ViewChild(AceEditorComponent)
  editorComponent: AceEditorComponent;

  constructor() {
    super()
  }
  
  ngAfterViewInit() {
    this.editorComponent.resourceComponent.loadOnSelect = true
    this.editorComponent.resource.dir = "./settings/c-programs"
  }
}