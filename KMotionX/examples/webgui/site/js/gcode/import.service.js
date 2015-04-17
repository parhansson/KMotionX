(function() {
  angular.module('CodeEditor')
    .factory('transcoder', transcoder); 
  
  transcoder.$inject = ['$q'];
  
  function transcoder($q) {
      
      var service = {
          transcode: transcode
      };
      
      return service;
      
      function transcode(mime, source){
        
        if(mime == "image/svg+xml"){
          return transcodeSVG(source);
        } else if(mime == "application/postscript" || mime == "application/pdf"){
          return transcodePDF(source).then(
              function(result){
                return transcodeSVG(result)
              });
        }
        
        var transformedDefer = $q.defer();
        if (typeof source === 'string') {
          transformedDefer.resolve(source);
        } else if (isArrayBuffer(source)) {
          transformedDefer.resolve(ab2str(source));
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof src));
        }
        
        return transformedDefer.promise;
      }
      
      function transcodeSVG(src){
        var transformedDefer = $q.defer();
        var source;
        if (typeof src === 'string') {
          source = src;
        } else if (typeof src === 'object' && src.ownerSVGElement !== undefined) {
          source = src;
        } else if (isArrayBuffer(src)) {
          source = ab2str(src);
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof src));
        }
        
        
        var gcode = new GCode.Source(svg2gcode(source, {
          scale : 1,
          cutZ : 10,
          safeZ: 8,
          unit: "mm",
          dpi: 72
        }));
        
        transformedDefer.resolve(gcode.text);
        return transformedDefer.promise; 
      }
      function transcodePDF(arrayBuffer, asText){
        var transformedDefer = $q.defer();
        var resultAsText = asText || false;
        var scale = 1.0;
        
        //
        // Fetch the PDF document from the URL using promises
        //
        PDFJS.getDocument(arrayBuffer).then(function(pdf) {
          var numPages = pdf.numPages;
          // Using promise to fetch the page
          
          // For testing only.
          var MAX_NUM_PAGES = 50;
          var ii = Math.min(MAX_NUM_PAGES, numPages);
          
          var promise = Promise.resolve();
          for (var i = 1; i <= ii; i++) {
            var anchor = document.createElement('a');
            anchor.setAttribute('name', 'page=' + i);
            anchor.setAttribute('title', 'Page ' + i);
            document.body.appendChild(anchor);
            
            // Using promise to fetch and render the next page
            promise = promise.then(function (pageNum, anchor) {
              return pdf.getPage(pageNum).then(function (page) {
                var viewport = page.getViewport(scale);
                
                var container = document.createElement('div');
                container.id = 'pageContainer' + pageNum;
                container.className = 'pageContainer';
                container.style.width = viewport.width + 'px';
                container.style.height = viewport.height + 'px';
                anchor.appendChild(container);
                
                return page.getOperatorList().then(function (opList) {
                  var svgGfx = new PDFJS.SVGGraphics(page.commonObjs, page.objs);
                  return svgGfx.getSVG(opList, viewport).then(function (svg) {
                    container.appendChild(svg);
                    if(resultAsText){
                      transformedDefer.resolve(svg.outerHTML);
                    } else {
                      transformedDefer.resolve(svg);
                    }
                    //console.info(container.innerHTML);
                  });
                });
              });
            }.bind(null, i, anchor));
          }
        });
        
        return transformedDefer.promise; 
      }
      
      
      function ab2str(buf) {
        return String.fromCharCode.apply(null, new Uint8Array(buf)); //Uint16Array
      }
      function str2ab(str) {
        var buf = new ArrayBuffer(str.length*2); // 2 bytes for each char
        var bufView = new Uint16Array(buf);
        for (var i=0, strLen=str.length; i < strLen; i++) {
          bufView[i] = str.charCodeAt(i);
        }
        return buf;
      }
      
      function isArrayBuffer(v) {
        return typeof v === 'object' && v !== null && v.byteLength !== undefined;
      }
  }
  
  
})();



