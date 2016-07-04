import { Component, Inject, ViewChild, provide } from '@angular/core';
import {TAB_DIRECTIVES} from 'ng2-bootstrap/ng2-bootstrap';
import { ScreenComponent } from "../screen.component"
import {CCodeEditorComponent} from './ccode-editor.component'

@Component({
  selector: 'ccode-screen',
  directives: [TAB_DIRECTIVES,CCodeEditorComponent],
  template: `
    <tabset>
      <tab heading="File1">
        <ccode-editor></ccode-editor>
      </tab>
      <tab heading="File2">
        <ccode-editor></ccode-editor>
      </tab>  
    </tabset>
    `
})
export class CCodeScreenComponent extends ScreenComponent {

  constructor() {
    super()
  }

}