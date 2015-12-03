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
  
})();