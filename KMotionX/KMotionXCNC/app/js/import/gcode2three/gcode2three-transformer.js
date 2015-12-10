'use strict';
(function() {
  angular.module('KMXImport').run(register);
  
  register.$inject = ['$q', 'transformer', 'transformerSettings'];
  
  function register($q, transformer, transformerSettings){
    var descriptor = { 
      name: 'G-Code to Three',
      inputMime: ["application/x-gcode"],
      outputMime: "application/x-kmx-three",
      execute: transcodeGCode
    };
    
	  transformer.register(descriptor);
    
    
    function transcodeGCode(source){
        var disableWorker = document.URL.startsWith("file");
        var transformedDefer = $q.defer();
        if (angular.isObject(source)) {
          KMX.Transformers.gcode2three.transform(transformedDefer,source,disableWorker);     
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof source));
        }
        return transformedDefer.promise; 
      }
      
      
              
      
  }
  
})();