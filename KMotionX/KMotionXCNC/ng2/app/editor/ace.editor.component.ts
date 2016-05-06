import {Component, Inject, Input, ViewChild} from '@angular/core';
import {AceDirective} from "../editor/ace.directive";
import {KMXUtil}    from '../util/KMXUtil'
import {ResourceComponent, FilePathComponent, FileDropZone}    from '../resources/resource.component'
import {FileResource, IFileObject} from '../backend/file'

export interface OnFileEventHandler {
  (file: IFileObject): void
}

@Component({
  selector: 'code-editor',
  directives: [AceDirective, ResourceComponent, FilePathComponent, FileDropZone],
  template: `
    <file-dialog #fd (selectedFile)="onFile($event)" [resource]="resource"></file-dialog>
    <div>
      <span class="glyphButtonBar">
          <span class="btn btn-primary glyphicon glyphicon-folder-open" title="Open" (click)="fd.open()"></span>
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

  @ViewChild(ResourceComponent)
  resourceComponent: ResourceComponent

  @Input() mode: string
  @Input() theme: string
  @Input() resource: FileResource
  onFileEventHandler: OnFileEventHandler
  dirty: boolean
  constructor() {

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
  }
  onSaveAs() {
    this.resourceComponent.saveAs(this.textContent)
  }

  onFile(file: IFileObject) {
    if (this.onFileEventHandler) {
      this.onFileEventHandler(file)
    } else {
      this.textContent = KMXUtil.ab2str(file.payload)
      this.dirty = false
    }
  }

}