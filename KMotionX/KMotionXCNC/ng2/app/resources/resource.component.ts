import {Directive, Component, Input, Output, EventEmitter, ElementRef} from '@angular/core';
import {BackendService} from '../backend/backend.service'

export interface IFileObject {
  contentType: string
  payload: ArrayBuffer
}

@Directive({
  selector: "[file-dropzone]",
  host: {
    '(dragover)': 'processDragOverOrEnter($event)',
    '(dragenter)': 'processDragOverOrEnter($event)',
    '(drop)': 'processDrop($event)'
  }

})
export class FileDropZone {
  @Output() dropped = new EventEmitter<IFileObject>()

  /*
      var validMimeTypes = attrs.fileDropzone;
*/
  element: HTMLElement
  constructor(element: ElementRef) {
    this.element = element.nativeElement
  }

  processDrop(event) {
    var file, name, size, type;
    if (event != null) {
      event.preventDefault();
    }
    this.highlight(null)
    var ev = event.originalEvent || event; //if jquery is loaded before angular event is wrapped
    file = ev.dataTransfer.files[0];
    name = file.name;
    type = file.type;
    size = file.size;

    let reader = new FileReader();
    reader.onload = (evt) => {
      if (this.checkSize(size) && this.isTypeValid(type)) {
        console.log(typeof ((evt as any).target.result))
        this.dropped.emit({ contentType: type, payload: (evt as any).target.result })
      }
    };
    reader.readAsArrayBuffer(file)
    return false;
  }

  private highlight(color: string) {
    this.element.style.backgroundColor = color;
  }

  processDragOverOrEnter(event) {
    if (event != null) {
      event.preventDefault();
    }
    this.highlight('lightgray')
    var ev = event.originalEvent || event; //if jquery is loaded before angular event is wrapped
    ev.dataTransfer.effectAllowed = 'copy';
    return false;
  }
  private checkSize(size) {
    /*
    var _ref;
    if (((_ref = attrs.maxFileSize) === (void 0) || _ref === '') || (size / 1024) / 1024 < attrs.maxFileSize) {
      return true;
    } else {
      alert("File must be smaller than " + attrs.maxFileSize + " MB");
      return false;
    }
    */
    return true;
  }

  private isTypeValid(type) {
    /*
    if ((validMimeTypes === (void 0) || validMimeTypes === '') || validMimeTypes.indexOf(type) > -1) {
      return true;
    } else {
      alert("Invalid file type.  File must be one of following types " + validMimeTypes);
      return false;
    }
    */
    return true;
  }


}

export class FileResource {
  paths: string[]

  constructor(dir?: string, public file?: string) {
    this.dir = dir
  }
  set dir(dir: string) {
    this.paths = dir.split("/")
  }
  get dir() {
    return this.paths.join("/")
  }
  append(path: string) {
    this.paths.push(path)
  }

  get canonical() {
    return this.dir + "/" + this.file
  }
  set canonical(file) {
    let parts = file.split("/");
    this.file = parts.splice(parts.length - 1)[0]
    this.paths = parts
  }

  up(count: number) {
    this.paths = this.paths.splice(0, this.paths.length - count);
  }
  goto(index: number) {
    this.paths = this.paths.splice(0, index);
  }
}

@Component({
  selector: 'file-path',
  template: `
      <div>
        <span *ngFor="let part of resource?.paths; let index = index" (click)="openIndex(index)" >{{part}}/</span>
        <!--<span>{{resource.file}}</span>-->
      </div>
    `

})
export class FilePathComponent {
  @Output() changed = new EventEmitter()
  @Input() resource: FileResource
  constructor() {

  }
  openIndex(index: number) {
    this.resource.goto(index + 1);
    this.changed.emit(this.resource);
  }
}



@Component({
  selector: 'file-dialog',
  directives: [FilePathComponent, FileDropZone],
  template: `
  <div class="modal" [style.display]="modalDisplay">
      <div class="modal-dialog">
        <div class="modal-content">       
          <div class="modal-header">
            <a class="close" (click)="close()">&times;</a>
            <h4>Open GCode</h4>
          </div>
          <div class="modal-body" file-dropzone (dropped)="onDroppedFile($event)">
            <file-path [resource]="resource" (changed)="listDir()" ></file-path>
            <ul class="modal-file-list">
              <li class="button" (click)="selectFile(file)" *ngFor="let file of files">{{file.name}}</li>          
            </ul>
            <p>Open file by selecting or with drag and drop to editor from desktop</p>
          </div>
          <div class="modal-footer">
            <a class="btn" (click)="close()">Cancel</a>      
          </div>
        </div>
      </div>
      </div>
    `

})
export class ResourceComponent {
  @Input('resource') resource: FileResource
  @Output() selectedFile = new EventEmitter<IFileObject>()

  private files = []
  private showModal: boolean = false
  private modalDisplay: string = "none"

  constructor(private backendService: BackendService) {

  }


  selectFile(file: any) {
    if (file.type === 4) {
      if (file.name === "..") {
        this.resource.up(1)
      } else if (file.name === ".") {
        //Do nothing, same directory
      } else {
        this.resource.append(file.name)
      }
      this.listDir()
    } else {
      this.resource.file = file.name
      this.openFile()
    }
  }

  open() {
    this.showModal = true;
    this.modalDisplay = "block"
    this.listDir()
  }

  close() {
    this.showModal = false;
    this.modalDisplay = "none"
  }
  private onDroppedFile(data: IFileObject) {
    this.selectedFile.emit(data)
    this.close()
  }
  private openFile() {
    this.backendService.onOpenFile(this.resource.canonical).subscribe(
      data => this.selectedFile.emit(data),
      err => console.error(err),
      () => this.close()
    )
  }
  private listDir() {
    this.backendService.onListDir(this.resource.dir).subscribe(
      data => {
        let value = data.json()
        this.files = value.files
        this.resource.dir = value.dir
      },
      err => console.log(err)
    )

  }
}    