(function() {
  angular.module('KmotionXApp')
  .directive("kmxDro", kmxDRO);
  
  function kmxDRO() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/gcode/dro.html",
      link: link
    };
    function link(scope, elem, attrs){
      scope.droAxes = ['X','Y','Z']
    }
  }
  
})();
