//transcodes svg to intermediate gcode model 
'use strict';
(function() {
  angular.module('KMXImport').run(register);
  
  register.$inject = ['$q', 'transformer', 'transformerSettings'];
  
  function register($q, transformer, transformerSettings){
    var descriptor = { 
      name: 'SVG to IGM',
      inputMime: ["image/svg+xml"],
      outputMime: "application/x-kmx-gcode",
      execute: transcodeSVG
    };
    
	  transformer.register(descriptor);
    
    function transcodeSVG(source){
        var transformedDefer = $q.defer();
        var svgRootElement;
        if (typeof source === 'string') {
          svgRootElement = parseXML(source);
        } else if (typeof source === 'object' && source.ownerSVGElement !== undefined) {
          svgRootElement = source;
        } else if (angular.isArrayBuffer(source)) {
          svgRootElement = parseXML(ab2str(source));
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof source));
        }
            // parse xml        
        if(svgRootElement == null){
          transformedDefer.reject("SVG element not present");
        }
        var igm = SVGReader.parse(svgRootElement, {});
        
        
        transformedDefer.resolve(igm);
        
        return transformedDefer.promise; 
      }
      
      function parseXML(source){
          if (window.DOMParser) {
            var parser = new DOMParser();
            // clean off any preceding whitespace
            //not sure if this is really needed
            var svgstring = source.replace(/^[\n\r \t]/gm, '');
            return parser.parseFromString(svgstring, 'image/svg+xml').documentElement;
          } else {
            console.error("DOMParser not supported. Update your browser");
          } 
          return null;     
      }
  }
  
  function ab2str(buf) {
    var arr = new Uint8Array(buf)
    var str = "";
    for(var i=0,l=arr.length; i<l; i++)
        str += String.fromCharCode(arr[i]);
    return str;
    //Call stack too deep on certain browsers
    //return String.fromCharCode.apply(null, new Uint8Array(buf)); //Uint16Array
  }
  function str2ab(str) {
    var buf = new ArrayBuffer(str.length*2); // 2 bytes for each char
    var bufView = new Uint16Array(buf);
    for (var i=0, strLen=str.length; i < strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return buf;
  }
  
})();