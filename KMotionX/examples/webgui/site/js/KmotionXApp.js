
(function() {

  var app = angular.module('KmotionXApp', ['KMX-settings','CodeEditor','ThreeViewer','GCodeHandler','WebSocketHandler', 'OtherStuff']);

  
  app.run(function($rootScope, ViewerService) {
      $rootScope.viewer3d = ViewerService;
  });
  
  app.run(function(SocketService) {
    SocketService.init();
  });
  
  app.controller('KMotionXController', function($scope){
    
  });    


})();

