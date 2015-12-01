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
    
    function transcodeSVG(src){
        var transformedDefer = $q.defer();
        var source;
        if (typeof src === 'string') {
          source = src;
        } else if (typeof src === 'object' && src.ownerSVGElement !== undefined) {
          source = src;
        } else if (angular.isArrayBuffer(src)) {
          source = ab2str(src);
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof src));
        }
        var igm = SVGReader.parse(source, {});
        
        
        transformedDefer.resolve(igm);
        return transformedDefer.promise; 
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