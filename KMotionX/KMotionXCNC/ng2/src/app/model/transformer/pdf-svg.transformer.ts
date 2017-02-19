import { Observer } from 'rxjs/Rx';
import { ModelSettingsService, ModelSettings } from '../model.settings.service';
import { ModelTransformer } from './model.transformer';


export class Pdf2SvgTransformer extends ModelTransformer<ArrayBuffer, SVGElement> {
  transformerSettings: ModelSettings

  constructor(private modelSettingsService: ModelSettingsService) {
    super()
    this.transformerSettings = modelSettingsService.settings;
  }

  execute(source: ArrayBuffer, targetObserver: Observer<SVGElement>) {
    var scale = 1.0;
    //this will use base64 encoded instead of bloburls for images
    //PDFJS.disableCreateObjectURL = true;
    //
    PDFJS.disableFontFace = true;
    PDFJS.workerSrc='assets/pdf.worker.js'
    // Fetch the PDF document from the URL using promises
    //
    let transformerSettings = this.transformerSettings

    PDFJS.getDocument(source).then(function (pdf) {
      let numPages = pdf.numPages;
      // Using promise to fetch the page

      // For testing only.
      let MAX_NUM_PAGES = 50;
      let ii = Math.min(MAX_NUM_PAGES, numPages);
      let svgPages = [];
      let promise: Promise<any> = Promise.resolve();
      for (var i = 1; i <= ii; i++) {
        let anchor = null;//createAnchor(i);
        if (transformerSettings.pdf.page != i) continue;
        // Using promise to fetch and render the next page
        promise = promise.then(function (pageNum, anchor) {
          return pdf.getPage(pageNum).then(page => {
            let viewport = page.getViewport(scale);

            // var container = createContainer(pageNum,viewport.width,viewport.height);
            //  anchor.appendChild(container);

            return page.getOperatorList().then(opList => {
              let svgGfx = <PDFJSExtra.SVGGraphics>new PDFJS.SVGGraphics(page.commonObjs, page.objs);
              return svgGfx.getSVG(opList, viewport).then(svg => {
                targetObserver.next(svg)
                //svgObserver.complete()
                pdf.destroy(); //Destroy worker
              });
            });
          });
        }.bind(null, i, anchor));
      }
    });
  }
  createContainer(pageNum, width, height) {
    var container = document.createElement('div');
    container.id = 'pageContainer' + pageNum;
    container.className = 'pageContainer';
    container.style.width = width + 'px';
    container.style.height = height + 'px';
    return container;
  }
  createAnchor(pageNum) {
    var anchor = document.createElement('a');
    anchor.setAttribute('name', 'page=' + pageNum);
    anchor.setAttribute('title', 'Page ' + pageNum);
    document.body.appendChild(anchor);
    return anchor;
  }

}
