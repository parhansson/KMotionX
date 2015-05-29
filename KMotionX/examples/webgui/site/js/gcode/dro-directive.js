(function() {
  angular.module('KmotionXApp')
  .directive("kmxDro", kmxDRO);
  
  
  function kmxDRO() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/gcode/dro.html",
        scope: {},
        link: link,
        controller: droController
      };
      
      function link(scope, element, attrs) {
        
      }
      
      function droController($scope){
        $scope.$on('status-update', function statusUpdate(event, args){
          $scope.dro = args.status.dro;
          $scope.timeStamp = args.status.timeStamp;
          
          /*
          if (MainStatus.StopImmediateState>0 && up!=IDB_FeedHoldPause)
          {
              m_FeedHold.LoadBitmaps(IDB_FeedHoldPause,0,0);
              m_FeedHold.Invalidate(0);
          }
          else if (MainStatus.StopImmediateState==0 && up!=IDB_FeedHold)
          {
              m_FeedHold.LoadBitmaps(IDB_FeedHold,0,0);
              m_FeedHold.Invalidate(0);
          }
          */
        });
      }
  }
  
})();
