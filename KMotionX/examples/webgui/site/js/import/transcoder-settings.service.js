(function() {
  angular.module('KMXImport')
    .factory('transcoderSettings', transcoderSettings); 
  
  //transcoderSettings.$inject = ['$q'];
  
  function transcoderSettings() {
    
    var service = {
        svg: {
            scale : 1,
            cutZ : 0, //20,
            safeZ: 0,//10,
            unit: "mm",
            dpi: 72,
            fractionalDigits: 2, 
            translateToOrigo: true,
            removeOutline: false,
            removeDuplicates: true,
            removeSingularites: true,
            initCode: 'M100 P200 Q100',
            feedRate: 250
            
        },
        pdf:{
          page: 1
        }
    };
    
    return service;
  }
  
  
})();



