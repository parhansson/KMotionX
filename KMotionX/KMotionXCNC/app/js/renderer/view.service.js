(function() {
  angular.module('KMXRenderer')
    .factory('kmxThreeView', kmxThreeView); 
    
  function kmxThreeView() {
      
      var service = {
          scene: null,
          camera: null,
          renderer: null,
          controls:null
      };
      
      return service;
    }
})();



