import { Component, Inject, ViewChild, provide } from '@angular/core';
import { ScreenComponent } from "../screen.component"
import { AceEditorComponent, FileStoreToken, FileStore, DefaultFileStore } from '../editor'
import { FileResource } from '../resources'


@Component({
  selector: 'ccode-screen',
  directives: [AceEditorComponent],
  template: `
    <code-editor id="ccodeEditor" mode="c_cpp" >
      <buttons>TODO add compile, load and execute buttons</buttons>
    </code-editor>`,
  viewProviders: [
    provide(FileStoreToken, { useClass: DefaultFileStore })
  ]
})
export class CCodeScreenComponent extends ScreenComponent {
  @ViewChild(AceEditorComponent)
  editorComponent: AceEditorComponent;

  constructor(@Inject(FileStoreToken) private fileStore:FileStore) {
    super()
  }

  ngAfterViewInit() {
    this.editorComponent.onFile(new FileResource("./settings/c-programs")); 
  }
}