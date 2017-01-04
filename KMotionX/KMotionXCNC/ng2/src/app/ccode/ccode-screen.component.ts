import { Component } from '@angular/core';
import { CCodeEditorComponent } from './ccode-editor.component'

@Component({
  selector: 'ccode-screen',
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
export class CCodeScreenComponent {

  constructor() {
  }

}