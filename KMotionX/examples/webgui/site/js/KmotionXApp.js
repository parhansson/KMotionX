
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
    var globalState = {};
    $scope.globalState = globalState;
    $scope.$on('state-update', function(event, args){
      //globalState =args.state;
      angular.copy(args.state, globalState);
      $scope.simulating = globalState.simulate;
      //$scope.interpreting = state.interpreting == 1;
      //$scope.feedHold = state.feedHold == 1;
    });
    var status = {}
    $scope.status = status;
    $scope.$on('status-update', function(event, args){
      angular.copy(args.status, status);
      var xDROElement = $("#xDRO");
      var yDROElement = $("#yDRO");
      var zDROElement = $("#zDRO");
      //TODO divide with machine settings
      xDROElement.text((25.4*status.dest[0]/2540).toFixed(3));
      yDROElement.text((25.4*status.dest[1]/2540).toFixed(3));
      zDROElement.text((25.4*status.dest[2]/2540).toFixed(3));
      //$scope.interpreting = state.interpreting == 1;
      //$scope.feedHold = state.feedHold == 1;
      
      //(1000/2540)*25,4 dro;
      
      //console.info(status.threadActive);
    });
    
    
    $scope.$watch('simulating', function(newValue, oldValue){
      if(newValue != oldValue){
        kmxBackend.simulate(newValue);        
      }
    });
    
    
    
  }


})();

