import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';
import {ModelSettingsService, ModelSettings} from '../model.settings.service';

declare var PDFJS: any

export class PDF2SVG {
  transformerSettings: ModelSettings

  constructor(private modelSettingsService: ModelSettingsService) {
    this.transformerSettings = modelSettingsService.settings;
  }

  transcodePDF(transformedDefer: Observer<string | SVGElement>, arrayBuffer, asText: boolean) {
    var resultAsText = asText || false;
    var scale = 1.0;
    //this will use base64 encoded instead of bloburls for images
    //PDFJS.disableCreateObjectURL = true;
    //
    //PDFJS.disableFontFace = true;
    // Fetch the PDF document from the URL using promises
    //
    let transformerSettings = this.transformerSettings

    PDFJS.getDocument(arrayBuffer).then(function (pdf) {
      let numPages = pdf.numPages;
      // Using promise to fetch the page

      // For testing only.
      let MAX_NUM_PAGES = 50;
      let ii = Math.min(MAX_NUM_PAGES, numPages);
      let svgPages = [];
      let promise = Promise.resolve();
      for (var i = 1; i <= ii; i++) {
        let anchor = null;//createAnchor(i);
        if (transformerSettings.pdf.page != i) continue;
        // Using promise to fetch and render the next page
        promise = promise.then(function (pageNum, anchor) {
          return pdf.getPage(pageNum).then(function (page) {
            let viewport = page.getViewport(scale);

            // var container = createContainer(pageNum,viewport.width,viewport.height);
            //  anchor.appendChild(container);

            return page.getOperatorList().then(function (opList) {
              let svgGfx = new PDFJS.SVGGraphics(page.commonObjs, page.objs);
              return svgGfx.getSVG(opList, viewport).then(function (svg) {
                // container.appendChild(svg);                  
                if (resultAsText) {
                  // need to add namespace declarations for this to be a valid xml document
                  transformedDefer.next(svg.outerHTML.replace('<svg:svg ', '<svg:svg xmlns:svg="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" '))
                } else {
                  //transformedDefer.resolve(svg);
                  transformedDefer.next(svg)

                }
                transformedDefer.complete()
                pdf.destroy(); //Destroy worker
              });
            });
          });
        }.bind(null, i, anchor));
      }
    });

    return transformedDefer;
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
