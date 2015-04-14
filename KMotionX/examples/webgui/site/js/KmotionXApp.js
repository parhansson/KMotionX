
(function() {

  angular.module('KmotionXApp', ['KMXLog','KMX-LaserPlugin','CodeEditor','KMXRenderer','KMXBackend', 'OtherStuff']);
  
  angular.module('KmotionXApp')
    .run(initApp);
  
  initApp.$inject =['SocketService'];
  
  function initApp(SocketService) {
    if (!Modernizr.webgl) {
      alert('Sorry, you need a WebGL capable browser to use this.\n\nGet the latest Chrome or FireFox.');
      return;
    }

    if (!Modernizr.localstorage) {
      alert("Man, your browser is ancient. I can't work with this. Please upgrade.");
      return;
    }
    //localStorage.removeItem("not-first-visit");
    // Show 'About' dialog for first time visits.
    if (!localStorage.getItem("not-first-visit")) {
      localStorage.setItem("not-first-visit", true);
      setTimeout(function(){
        $('#aboutModal').modal();
      }, 500);
    }
    
    
    SocketService.init();
  }
  
  angular.module('KmotionXApp')
    .controller('KMotionXController', KMotionXController);
  
  KMotionXController.$inject =['$scope', 'kmxThreeView', 'kmxBackend'];
  
  function KMotionXController($scope, kmxThreeView, kmxBackend){
    $scope.kmxThreeViewData = kmxThreeView;
    
    $scope.$on('state-update', function(event, args){
      var state = args.state;      
      $scope.simulating = state.simulate;
      //$scope.interpreting = state.interpreting == 1;
      //$scope.feedHold = state.feedHold == 1;
    });
    
    $scope.$watch('simulating', function(newValue, oldValue){
      if(newValue != oldValue){
        kmxBackend.simulate(newValue);        
      }
    });
    
  }


})();

