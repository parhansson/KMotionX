(function() {
  angular.module('KmotionXApp')
    .controller('UserButtonController', UserButtonController);
  
  UserButtonController.$inject = ['$scope', 'settings','kmxBackend'];
  
  function UserButtonController($scope, settings,kmxBackend){
    $scope.machine = settings.machine;
    $scope.onUserButton = function(action){
      if(action.action == 4){
        kmxBackend.compileAndLoadCoff([action.file, action.dParam0]);
      }
    }
  }
})();