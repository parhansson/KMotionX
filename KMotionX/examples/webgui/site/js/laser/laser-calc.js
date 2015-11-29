(function() {
  var app = angular.module('KMX-LaserPlugin', []);
  
  LaserCalculatorController.$inject = ['$scope'];
  
  app.controller('LaserCalculatorController', LaserCalculatorController);
  function LaserCalculatorController($scope){
    
    
    
    $scope.calc = {
        pulseWidth:2,
        ppi:1200,
        feedRate:250,
        powerSetting:100,
        power:40
    }
    
    var result = {
        ppmm:0,
        pps:0,
        pulsePeriod:0,
        dutyCycle:0,
        energyPerPulse:0,
        energyDensity:0,
        powerToMaterial:0
    }
    //$scope.calc = calc
    $scope.result = result;
    
    $scope.$watch('calc', function(newValue, oldValue){ 
      result.ppmm = newValue.ppi/25.4;
      result.pps = result.ppmm*newValue.feedRate / 60;
      result.pulsePeriod=1/result.pps*1000;
      result.dutyCycle=newValue.pulseWidth/result.pulsePeriod*100;
      result.energyPerPulse=newValue.power*newValue.pulseWidth/1000*newValue.powerSetting/100;
      result.energyDensity=result.ppmm*result.energyPerPulse;
      result.powerToMaterial=result.pps*result.energyPerPulse;
    }, true);
    
    
  }
  

  app.directive("laserPlugin", function() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/laser/laser-calc.html"
    };
  });
  
})();

