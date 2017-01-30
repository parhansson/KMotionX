import { Component, Inject, Input, Output } from '@angular/core';
import { SVGBox } from './svg-box'
@Component({
  selector: 'box-creator',
  templateUrl: './box-creator.component.html'
})
export class BoxCreatorComponent {
  box = {
    width: 70,
    height: 70,
    depth: 30,
    materialThickness: 5
  }

  constructor() {

  }

  render() {
    new SVGBox(this.box.width, this.box.height, this.box.depth, this.box.materialThickness);
  }
}