

//declare var module: { id: string, exports: any };

declare class DxfParser{
  constructor(stream?:any)
  parseSync(fileText:string)
}

declare module 'three-trackballcontrols' { }

// Merge PDFJS typings
declare namespace PDFJSExtra {
  export interface PDFPageOperatorList {
    argsArray: Object[],
    fnArray: Object[],
    lastChunk: boolean
  }
  export class SVGGraphics {
    constructor(
      objects: PDFObjects,
      objects1: PDFObjects,
      forceDataSchema?: boolean)

    getSVG(opList: PDFJSExtra.PDFPageOperatorList, viewport: PDFPageViewport):PDFPromise<SVGElement>;
  }
}
interface PDFObjects {
  objs: Object
}
interface PDFPageProxy {
  getOperatorList(): PDFPromise<PDFJSExtra.PDFPageOperatorList>
  commonObjs: PDFObjects
  objs: PDFObjects
}

 interface PDFJSStatic {
  SVGGraphics(
    objects: PDFObjects,
    objects1: PDFObjects,
    forceDataSchema?: boolean): void
 }
