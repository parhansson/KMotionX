'use strict';
(function() {
  angular.module('KMXImport')
    .factory('transformer', transformer); 
  
  transformer.$inject = ['$q','transformerSettings'];
  
  function transformer($q,transformerSettings) {
      
      var transformers = [];
      var service = {
          transcode: transcode,
          transformNamed: transformNamed,
          register: register
      };
      
      return service;
      
      function register(transformer){
        transformers.push(transformer);
      }
      
      function matchType(mime){
        for(var i = 0; i < transformers.length;i++){
          if(transformers[i].inputMime.indexOf(mime) > -1){
            return transformers[i];
          }
        }
        return null;
      }
      function matchName(name){
        for(var i = 0; i < transformers.length;i++){
          if(transformers[i].name === name){
            return transformers[i];
          }
        }
        return null;
      }      
      function transformNamed(transformerName, source){
        
        var transformer = matchName(transformerName);
        if(transformer !== null){
          return transformer.execute(source);
        }
        var transformedDefer = $q.defer();
        transformedDefer.reject('Named transformer "' + transformerName + '" not found');
        return transformedDefer.promise;
      }      
      function transcode(mime, source){
        
        var transformer = matchType(mime);
        if(transformer !== null){
          var resultPromise = transformer.execute(source);
          if(transformer.outputMime !== "application/x-gcode"){
            return resultPromise.then(
              function(result){
                return transcode(transformer.outputMime,result);
              });
          }
          return resultPromise;
        }

        var transformedDefer = $q.defer();
        if (typeof source === 'string') {
          //gcode text do not transform
          transformedDefer.resolve(source);
        } else if (angular.isArrayBuffer(source)) {
          //gcode file do not transform
          transformedDefer.resolve(KMX.Util.ab2str(source));
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof source));
        }
        
        return transformedDefer.promise;
      }
  }
  
})();



