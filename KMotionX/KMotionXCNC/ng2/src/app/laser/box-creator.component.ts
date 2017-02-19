import { Component, Inject, Input, Output, ViewChild, ElementRef } from '@angular/core';
import { StaticTransformer } from '../model/transformers'
import { IGM } from '../model/IGM'
import { MitreBox } from '../model/tool/mitre-box'

@Component({
  selector: 'box-creator',
  templateUrl: './box-creator.component.html'
})
export class BoxCreatorComponent {
  box: MitreBox

  @ViewChild('preview')
  private previewContainer: ElementRef

  constructor(private staticTransformer: StaticTransformer) {
    this.box = new MitreBox(70, 70, 30, 3);
  }

  render() {
    let svg = this.toSVG(this.box.generate());
    //Open in new window
    //let blob = new Blob([svg], { type: 'image/svg+xml' });
    //window.open(window.URL.createObjectURL(blob));
    let doc = new DOMParser().parseFromString(svg, 'image/svg+xml').documentElement as any as SVGElement;
    let node = this.previewContainer.nativeElement as Element
    while (node.firstChild) {
      node.removeChild(node.firstChild);
    }
    node.appendChild(doc);

    //TODO only content of currently loaded gcode file is changed not the file name
    this.staticTransformer.transform('image/svg+xml', svg)
  }

  toSVG(model: IGM) {
    let svg = ''

    const res = 1;
    const paths = model.alllayers
    model.setBounds(paths);
    const bounds = model.getMaxBounds(paths)
    const w = bounds.x2
    const h = bounds.y2
    const dpi = 72 //output DPI
    const dpiScale = dpi / 25.4 // assuming input model in mm not in inches
    svg += '<?xml version="1.0" standalone="no"?>\r\n';
    svg += '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\r\n';
    svg += '<svg width="' + w / res + 'mm" height="' + h / res + 'mm" viewBox="0 0 ' + w * dpiScale + ' ' + h * dpiScale + '" xmlns="http://www.w3.org/2000/svg" version="1.1">\r\n';


    for (let part of model.alllayers) {
      part.scale(dpiScale)
      svg += ('<polyline points="')
      let points = []
      for (let vec of part.vectors) {
        points.push(vec.x + ',' + vec.y)
      }
      svg += points.join(' ')

      svg += ('" fill="none" stroke="black" stroke-width="0.2" />\r\n');
    }

    svg += ('</svg>\r\n');
    return svg
  }
}