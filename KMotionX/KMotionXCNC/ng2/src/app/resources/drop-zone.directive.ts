import { Directive, Component, Input, Output, EventEmitter, ElementRef } from '@angular/core';
import { FileResource } from './file-resource'
import { Payload } from './payload'
@Directive({
  selector: '[file-dropzone]',
  host: {
    '(dragover)': 'processDragOverOrEnter($event)',
    '(dragenter)': 'processDragOverOrEnter($event)',
    '(dragleave)': 'processDragExit($event)',
    '(drop)': 'processDrop($event)'
  }

})
export class DropZoneDirective {
  @Output() dropped = new EventEmitter<Payload>()

  /*
      var validMimeTypes = attrs.fileDropzone;
*/
  element: HTMLElement
  constructor(element: ElementRef) {
    this.element = element.nativeElement
  }

  processDrop(event: DragEvent) {
    var file: File
    if (event != null) {
      event.preventDefault();
    }
    this.highlight(null)
    file = (event.dataTransfer as DataTransfer).files.item(0);
    if (this.checkSize(file.size) && this.isTypeValid(file.type)) {
      //TODO break this out to utility class and make it optional to actually load or emit  file as payload in FileResource 
      //let p: FilePropertyBag
      let reader = new FileReader();
      reader.addEventListener('load', (evt) => {
        let payload = new Payload((evt as any).target.result, file.type);
        payload.name = file.name;
        this.dropped.emit(payload);
      })
      reader.readAsArrayBuffer(file)

    }
    return false;
  }

  processDragExit(event: DragEvent) {
    if (event != null) {
      event.preventDefault();
    }
    this.highlight(null)
    event.dataTransfer.effectAllowed = 'none';
    return false
  }

  processDragOverOrEnter(event: DragEvent) {
    if (event != null) {
      event.preventDefault();
    }
    this.highlight('lightgray')
    event.dataTransfer.effectAllowed = 'copy';
    return false;
  }

  private highlight(color: string) {
    this.element.style.backgroundColor = color;
  }

  private checkSize(size) {
    /*
    var _ref;
    if (((_ref = attrs.maxFileSize) === (void 0) || _ref === '') || (size / 1024) / 1024 < attrs.maxFileSize) {
      return true;
    } else {
      alert('File must be smaller than ' + attrs.maxFileSize + ' MB');
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
      alert('Invalid file type.  File must be one of following types ' + validMimeTypes);
      return false;
    }
    */
    return true;
  }


}