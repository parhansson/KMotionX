import { Component, ViewChild, SkipSelf, Inject } from '@angular/core';
import { FileResource, Payload, FileServiceToken, IFileBackend } from '../resources'
import { AceEditorComponent, FileStoreToken, FileStore } from '../editor'
import { TransformingFileStore } from './transforming-file-store.service'
import { SocketService } from '../backend/socket.service';
import { BackendService } from '../backend/backend.service'

@Component({
  selector: 'gcode-editor',
  template: `
        <code-editor id="gcodeEditor" mode="gcode"></code-editor>
  `,
  viewProviders: [
    { provide: FileStoreToken, useClass: TransformingFileStore }
  ]
})
export class GCodeEditorComponent {
  @ViewChild(AceEditorComponent)
  editorComponent: AceEditorComponent;

  constructor( @Inject(FileStoreToken) private fileStore: TransformingFileStore,
    private socketService: SocketService,
    @Inject(FileServiceToken) private fileBackend: BackendService) {

  }
  ngAfterViewInit() {
    this.editorComponent.onFile(new FileResource('./gcode'));
    //this.editorComponent.onContentChange()
    this.socketService.gcodeFileSubject.subscribe(gcodeFile => {
      this.editorComponent.resource = gcodeFile;
      if (gcodeFile.file) {
        let subscription = this.fileBackend.loadFile(gcodeFile.canonical).subscribe(
          payload => {
            this.fileStore.payloadSubject.next(payload)
          },
          null,
          () => subscription.unsubscribe()
        );
      }

    })
  }

}