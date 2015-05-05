(function() {
  angular.module('KmotionXApp')
  .directive("kmxDro", kmxDRO);
  
  
  function kmxDRO() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/gcode/dro.html",
      /*
        scope: {
          droRaw: '='
        },
        */
        link: link
      };
      
      function link(scope, element, attrs) {
        //(1000/2540)*25,4 dro;
        
      }
  }
  
})();
