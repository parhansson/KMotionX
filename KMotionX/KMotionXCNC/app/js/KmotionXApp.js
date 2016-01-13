
(function() {

  angular.module('KmotionXApp', ['KMXImport','KMXLog','KMX-LaserPlugin','CodeEditor','KMXRenderer','KMXBackend', 'OtherStuff']);
  
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
  
  KMotionXController.$inject =['$scope', 'kmxThreeView', 'kmxBackend', 'SocketService'];
  
  function KMotionXController($scope, kmxThreeView, kmxBackend, SocketService){
    var vm = this;
    vm.intStatus = SocketService.data;
    vm.kmxThreeViewData = kmxThreeView;
    vm.onSimulate = function(){
      kmxBackend.onSimulate();
    }
    vm.jog = function(axis, speed){
      kmxBackend.jog(axis, speed);
    }
  }


})();

