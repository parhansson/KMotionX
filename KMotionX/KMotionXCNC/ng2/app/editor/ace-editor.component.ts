import { Component, Inject, Input, ViewChild} from '@angular/core';
import { AceDirective} from "./ace.directive";
import { FileStoreToken, FileStore } from './file-store';
import { KMXUtil}    from '../util/KMXUtil'
import { DropZoneDirective,
  FileDialogComponent,
  FilePathComponent,
  FileResource,
  IFileBackend,
  FileServiceToken,
  Payload }    from '../resources'

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
      <span *ngIf="dirty">*</span><file-path [resource]="resource"></file-path>  
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

  onContentChange(change: AceAjax.EditorChangeEvent) {
    console.log("onContentChange", change);
    this.dirty = true;
  }

  onSave() {
    this.fileStore.store(this.resource.canonical, this.textContent)
    this.dirty = false;
  }

  onSaveAs() {
    //this.resourceComponent.saveAs(this.textContent)
  }

  onFile(file: FileResource | Payload) {
    if(file instanceof FileResource){
      this.resource = file;
    } else {
      //Use imported name
      this.resource.canonical = file.name;
    }
    //Selected in file dialog or drop imported file
    //load() should be responsible for returning file resource.
    //then imported files can be saved and get a real name
    this.fileStore.load(file);

  }

}