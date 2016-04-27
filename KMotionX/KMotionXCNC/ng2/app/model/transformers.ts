import {igm2gcode} from './igm/igm2gcode'
import {IGM, GCodeSource} from './igm/igm'
import {ModelTransformer} from './transformer.service'
//import {Observable, Observer} from 'rxjs/rx';
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';
import {KMXUtil}    from '../util/KMXUtil'
import {Svg2Igm} from '../model/svg/svgreader';
import {ModelSettingsService} from '../model/model.settings.service';
import {Gcode2Three} from '../model/gcode/gcode2three';
import {PDF2SVG} from '../model/pdf/pdf2svg';



export class StaticTransformer {
  gcodeSourceObserver: Observer<GCodeSource>;
  gcodeSourceObservable = new Observable<GCodeSource>(observer => this.gcodeSourceObserver = observer)
  threeObserver: Observer<any>;
  threeObservable = new Observable<any>(observer => this.threeObserver = observer)
  
  constructor(private modelSettings: ModelSettingsService) {
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
    //Observable.fr
    var transformobserver: Observer<string | SVGElement>;
    var observable = new Observable<string | SVGElement>(observer => transformobserver = observer)
    observable.subscribe(
      data => this.parseSVG(data),
      err => console.error(err),
      () => console.log('testa')
    );

    new PDF2SVG(this.modelSettings).transcodePDF(transformobserver, data, false);
  }

  parseSVG(source: string | SVGElement | ArrayBuffer) {
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
    let igm = new Svg2Igm().transform(doc, null);
    this.parseGcode(new igm2gcode().transform(igm, {}))

    //return null;     
  }

  parseGcode(input: string | string[] | ArrayBuffer) {
    let gcode: GCodeSource
    if (input instanceof ArrayBuffer) {
      gcode = new GCodeSource(KMXUtil.ab2str(input));
    } else {
      gcode = new GCodeSource(input);
    }
    this.gcodeSourceObserver.next(gcode);
    this.createThree(gcode);
  }

  createThree(gcode: GCodeSource) {
    new Gcode2Three().transform(this.threeObserver, gcode, true);
  }
}

export class IGM2GCodeTransformer extends ModelTransformer {

  name: 'IGM to G-Code'
  inputMime: ["application/x-kmx-gcode"]
  outputMime: "application/x-gcode"

  //transformer.register(descriptor);

  execute(src) {
    return new Promise<string>((resolve, reject) => {

      if (src instanceof IGM) {
        //var gcode = new GCodeSource(new igm2gcode().transform(src, transformerSettings.svg));
        //resolve(gcode.text);
      } else {
        reject("Unsupported source: " + (typeof src));
      }
    })
  }
}