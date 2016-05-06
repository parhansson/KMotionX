import {Injectable} from '@angular/core';
import {Observable, Observer, Subject} from 'rxjs/Rx';
import {ModelTransformer} from './transformer/ModelTransformer'
import {Igm2GcodeTransformer} from './transformer/Igm2GcodeTransformer'
import {Gcode2ThreeTransformer} from './transformer/Gcode2ThreeTransformer';
import {Pdf2SvgTransformer} from './transformer/Pdf2SvgTransformer';
import {Svg2IgmTransformer} from './transformer/Svg2IgmTransformer';
import {IGM, GCodeSource} from './igm'
import {KMXUtil}    from '../util/KMXUtil'
import {ModelSettingsService} from './model.settings.service';
import {LogService} from "../log/log.service"


@Injectable()
export class StaticTransformer {
  gcodeSubject = new Subject<GCodeSource>()
  threeSubject = new Subject<THREE.Group>()
  svgSubject = new Subject<SVGElement>()
  igmSubject = new Subject<IGM>()
  pdf2svgTransformer: Pdf2SvgTransformer
  img2gcodeTransformer: Igm2GcodeTransformer
  svg2IgmTransformer: Svg2IgmTransformer

  constructor(private logService: LogService, private modelSettings: ModelSettingsService) {
    this.pdf2svgTransformer = new Pdf2SvgTransformer(modelSettings)
    this.img2gcodeTransformer = new Igm2GcodeTransformer(modelSettings.settings.svg)
    this.svg2IgmTransformer = new Svg2IgmTransformer()
    this.igmSubject.subscribe(
      igm => this.img2gcodeTransformer.execute(igm, this.gcodeSubject),
      err => console.error(err))
    this.svgSubject.subscribe(
      data => {
        //console.log(KMXUtil.svgToString(data))
        this.parseSVG(data)
      },
      err => console.error(err)
    );
    this.gcodeSubject.subscribe(
      gcode => new Gcode2ThreeTransformer(true).execute(gcode, this.threeSubject),
      err => console.error(err))
  }

  transform(contentType, payload: ArrayBuffer) {
    if (contentType === 'application/pdf') {
      this.parsePDF(payload)
    } else if (contentType === 'image/svg+xml') {
      this.parseSVG(payload)
    } else {
      this.parseGcode(payload)
    }
  }

  parsePDF(data: ArrayBuffer) {
    this.pdf2svgTransformer.execute(data, this.svgSubject);
  }

  asSVGElement(source: string | SVGElement | ArrayBuffer) {
    let doc: SVGElement = null

    if (source instanceof SVGElement) {
      doc = source
    } else {
      let svgstring: string = null
      if (typeof (source) === 'string') {
        svgstring = source as string
      } else if (source instanceof ArrayBuffer) {
        svgstring = KMXUtil.ab2str(source)
      } else {
        console.error("Unsupported source", source);
      }

      if ((window as any).DOMParser) {
        // clean off any preceding whitespace not sure if this is really needed
        //svgstring = svgstring.replace(/^[\n\r \t]/gm, '');
        doc = new DOMParser().parseFromString(svgstring, 'image/svg+xml').documentElement as any as SVGElement;
      } else {
        console.error("DOMParser not supported. Update your browser");
      }
    }
    if (doc.localName !== "svg") {
      this.logService.log("error", "Failed to parse SVG document: " + doc.textContent)
      doc = null
    }
    return doc
  }
  asGCodeSource(input: string | string[] | ArrayBuffer) {
    if (input instanceof ArrayBuffer) {
      return new GCodeSource(KMXUtil.ab2str(input));
    } else {
      return new GCodeSource(input);
    }
  }
  parseSVG(source: string | SVGElement | ArrayBuffer) {
    let doc: SVGElement = this.asSVGElement(source)
    this.svg2IgmTransformer.execute(doc, this.igmSubject)

  }

  parseGcode(input: string | string[] | ArrayBuffer) {
    let gcode = this.asGCodeSource(input)
    this.gcodeSubject.next(gcode);
  }


}

export class IGM2GCodeTransformer extends ModelTransformer<IGM, string> {

  name: 'IGM to G-Code'
  inputMime: ["application/x-kmx-gcode"]
  outputMime: "application/x-gcode"

  //transformer.register(descriptor);

  execute(source: IGM, targetObserver: Observer<string>) {

    if (source instanceof IGM) {
      //var gcode = new GCodeSource(new igm2gcode().transform(src, transformerSettings.svg));
      //resolve(gcode.text);
    } else {
      targetObserver.error("Unsupported source: " + (typeof source));
    }
    return null
  }
}