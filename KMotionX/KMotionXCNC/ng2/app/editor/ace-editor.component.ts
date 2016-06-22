import { Component, Inject, Input, ViewChild} from '@angular/core';
import { AceDirective} from "./ace.directive";
import { FileStoreToken, FileStore } from './file-store';
import { KMXUtil}    from '../util/KMXUtil'
import { DropZoneDirective,
  FileDialogComponent,
  FilePathComponent,
  FileResource,
  IFileBackend,
  FileServiceToken }    from '../resources'

export interface OnFileEventHandler {
  (file: FileResource): void
}

@Component({
  selector: 'code-editor',
  directives: [AceDirective, FileDialogComponent, FilePathComponent, DropZoneDirective],
  template: `
    <file-dialog #fd (selectedFile)="onFile($event)" [resource]="resource"></file-dialog>
    <div>
      <span class="glyphButtonBar">
          <span class="btn btn-primary glyphicon glyphicon-folder-open" title="Open" (click)="fd.show()"></span>
          <span class="btn btn-primary glyphicon glyphicon-floppy-save" title="Save" (click)="onSave()"></span>
          <span class="btn btn-primary glyphicon glyphicon-floppy-save" title="Save As" (click)="onSaveAs()"></span>
      </span>
      <ng-content select="buttons"></ng-content>
    </div>
    <div file-dropzone (dropped)="onFile($event)">
      <file-path [resource]="resource"></file-path>  
      <div class="codeEditor">
        <div aceEditor [mode]="mode" [theme]="theme" [text]="editorContent" (textChanged)="onContentChange($event)" ></div>
      </div>
    </div>
    `

})
export class AceEditorComponent {
  @ViewChild(AceDirective)
  private aceEditor: AceDirective;

  @ViewChild(FileDialogComponent)
  resourceComponent: FileDialogComponent;

  @Input() mode: string;
  @Input() theme: string;
  resource: FileResource;
  dirty: boolean;

  constructor( @Inject(FileStoreToken) private fileStore: FileStore) {
    this.resource = new FileResource("");

    this.fileStore.textSubject.subscribe(text => {
      this.textContent = text;
      this.dirty = false;
    },
      err => console.error(err)
    );
  }

  get textContent() {
    return this.aceEditor.text
  }
  set textContent(value: string) {
    this.aceEditor.text = value
    this.dirty = true
  }

  onContentChange(content: string) {
    console.log("onContentChange")
    this.dirty = true
  }

  onSave() {
    this.fileStore.store(this.resource.canonical, this.textContent)
  }

  onSaveAs() {
    //this.resourceComponent.saveAs(this.textContent)
  }

  onFile(file: FileResource) {
    this.resource = file;
    //Drop imported file
    //Selected in file dialog
    this.fileStore.load(this.resource);
  }

}