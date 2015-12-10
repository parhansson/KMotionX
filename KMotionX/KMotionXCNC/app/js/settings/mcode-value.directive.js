(function() {
  angular.module('KmotionXApp')
  .directive("kmxMcodeValue", kmxMcodeValue);
  
  
  function kmxMcodeValue() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/settings/mcode-value.html"
    };
  }
  
})();
