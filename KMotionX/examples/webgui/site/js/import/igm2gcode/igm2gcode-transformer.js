'use strict';
(function() {
  angular.module('KMXImport').run(register);
  
  register.$inject = ['$q', 'transformer', 'transformerSettings'];
  
  function register($q, transformer, transformerSettings){
    var descriptor = { 
      name: 'IGM to G-Code',
      inputMime: ["application/x-kmx-gcode"],
      outputMime: "application/x-gcode",
      execute: transcodeIGM
    };
    
	  transformer.register(descriptor);
    
    function transcodeIGM(src){
        var transformedDefer = $q.defer();
        if (angular.isJSON(src)) {
          var gcode = new GCode.Source(igm2gcode(src, transformerSettings.svg));          
          transformedDefer.resolve(gcode.text);
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof src));
        }
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