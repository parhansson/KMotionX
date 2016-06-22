import {Component, ViewChild, provide, SkipSelf, Inject} from '@angular/core';
import {FileResource, Payload, FileServiceToken, IFileBackend} from '../resources'
import { AceEditorComponent, FileStoreToken, FileStore } from '../editor'
import {TransformingFileStore} from './transforming-file-store.service'


@Component({
  selector: 'gcode-editor',
  directives: [
    AceEditorComponent
  ],
  template: `
        <code-editor id="gcodeEditor" mode="gcode"></code-editor>
  `,
  viewProviders: [
    provide(FileStoreToken, { useClass: TransformingFileStore })
  ]
})
export class GCodeEditorComponent{
  @ViewChild(AceEditorComponent)
  editorComponent: AceEditorComponent;

  constructor(@Inject(FileStoreToken) private fileStore: TransformingFileStore) {

  }
  ngAfterViewInit() {
   this.editorComponent.onFile(new FileResource("./gcode")); 
  }

}