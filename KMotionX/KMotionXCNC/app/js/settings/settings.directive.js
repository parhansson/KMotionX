(function() {

  angular.module('KmotionXApp').directive("kmxSettings", kmxSettings);
  
  
  function kmxSettings() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/settings/machine-settings.html"
    };
  }
})();
