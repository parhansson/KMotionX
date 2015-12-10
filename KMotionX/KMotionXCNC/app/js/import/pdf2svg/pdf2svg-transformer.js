'use strict';
(function() {
  angular.module('KMXImport').run(register);
  
  register.$inject = ['$q', 'transformer', 'transformerSettings'];
  
  function register($q, transformer, transformerSettings){
	  var descriptor = { 
      name: 'PDF',
      inputMime: ["application/postscript","application/pdf"],
      outputMime: "image/svg+xml",     
      execute: transcodePDF
    };
    
    transformer.register(descriptor);

    function transcodePDF(arrayBuffer, asText){
      var transformedDefer = $q.defer();
      var resultAsText = asText || false;
      var scale = 1.0;
      //this will use base64 encoded instead of bloburls for images
      //PDFJS.disableCreateObjectURL = true;
      //
      //PDFJS.disableFontFace = true;
      // Fetch the PDF document from the URL using promises
      //
      PDFJS.getDocument(arrayBuffer).then(function(pdf) {
        var numPages = pdf.numPages;
        // Using promise to fetch the page
        
        // For testing only.
        var MAX_NUM_PAGES = 50;
        var ii = Math.min(MAX_NUM_PAGES, numPages);
        var svgPages = [];
        var promise = Promise.resolve();
        for (var i = 1; i <= ii; i++) {
          var anchor = null;//createAnchor(i);
          if(transformerSettings.pdf.page!=i) continue;
          // Using promise to fetch and render the next page
          promise = promise.then(function (pageNum, anchor) {
            return pdf.getPage(pageNum).then(function (page) {
              var viewport = page.getViewport(scale);
              
              // var container = createContainer(pageNum,viewport.width,viewport.height);
              //  anchor.appendChild(container);
              
              return page.getOperatorList().then(function (opList) {
                var svgGfx = new PDFJS.SVGGraphics(page.commonObjs, page.objs);
                return svgGfx.getSVG(opList, viewport).then(function (svg) {
                  // container.appendChild(svg);
                  //console.info(svg.outerHTML);                  
                  if(resultAsText){
                    transformedDefer.resolve(svg.outerHTML);
                  } else {
                    transformedDefer.resolve(svg);
                  }
                  pdf.destroy(); //Destroy worker
                });
              });
            });
          }.bind(null, i , anchor));
        }
      });
      
      return transformedDefer.promise; 
    }
    function createContainer(pageNum,width,height){
      var container = document.createElement('div');
      container.id = 'pageContainer' + pageNum;
      container.className = 'pageContainer';
      container.style.width = width + 'px';
      container.style.height = height + 'px';
      return container;    
    }
    function createAnchor(pageNum){
      var anchor = document.createElement('a');
      anchor.setAttribute('name', 'page=' + pageNum);
      anchor.setAttribute('title', 'Page ' + pageNum);
      document.body.appendChild(anchor);
      return anchor;
    }
  }
  
})();