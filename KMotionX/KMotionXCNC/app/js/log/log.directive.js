
(function() {
  angular.module('KMXLog',[]);
  
  angular.module('KMXLog')
    .directive('kmxLog', kmxLog);
  
  kmxLog.$inject = ['kmxLogger'];
  
  function kmxLog(kmxLogger) {
    var directive = {
      restrict: 'E',
      template: '<button class="btn btn-primary btn_wide" ng-click="clearLog()">{{::title}} (Clear)</button><div id="{{::consoleId}}" class="status_message status_small" title="{{::title}}"></div>',
      scope: {},
      link: link
    };
    return directive;
    
    
    function link(scope, element, attrs) {
      kmxLogger.registerConsole(attrs.consoleid,element[0].childNodes[1], attrs.color);
      scope.consoleId = attrs.consoleid;
      scope.title = attrs.title;
      scope.clearLog = clear;
      
      function clear(){
        kmxLogger.clear(attrs.consoleid);
      }
    }
  }
  
  
})();