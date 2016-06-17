import { Component, Inject, Input, ViewChild} from '@angular/core';
import { AceDirective} from "./ace.directive";
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
    <file-dialog #fd (selectedFile)="onFile($event)" [loadOnSelect]="false" [resource]="resource"></file-dialog>
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
  private aceEditor: AceDirective

  @ViewChild(FileDialogComponent)
  resourceComponent: FileDialogComponent

  @Input() mode: string
  @Input() theme: string
  resource: FileResource
  onFileEventHandler: OnFileEventHandler
  dirty: boolean
  constructor(@Inject(FileServiceToken) private fileBackend: IFileBackend) {
    this.resource = new FileResource("");
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
    this.resourceComponent.save(this.textContent)
    //this.fileBackend.saveFile(this.resource.canonical, this.textContent)
  }
  onSaveAs() {
    this.resourceComponent.saveAs(this.textContent)
  }

  onFile(file: FileResource) {
    //TODO här ska jag använda en injectad service av olika slag
    if (this.onFileEventHandler) {
      this.onFileEventHandler(file)
    } else if(file.payload){
      
      this.textContent = file.payload.text()
      this.dirty = false
    }
  }

}