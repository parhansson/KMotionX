'use strict';
(function() {
  
  angular.module('KmotionXApp')
    .controller('SettingsController', SettingsController);
  
  SettingsController.$inject = ['$scope', 'settings', 'kmxBackend'];
  
  function SettingsController($scope, settings, kmxBackend){
    console.log("SettingsController");
    $scope.actionOptions = [
         //{action:-1,name:'Reserved', visible:[false,false,false,false,false,false],paramNames:['','','','','','']},
         {action:0,name:'None', visible:[false,false,false,false,false,false],paramNames:['','','','','','']},
         {action:1,name:'Set a bit high or low', visible:[true,true,false,false,false,false],paramNames:['Set bit','to','','','','']},
         {action:2,name:'Set two bits either high or low', visible:[true,true,true,true,false,false],paramNames:['Set bit','to',' and bit','to','','']},
         {action:3,name:'Output a value to a DAC', visible:[true,true,true,true,true,false],paramNames:['Set DAC: ','scale: ','offset: ','min: ','max: ','']},
         {action:4,name:'Run a KMotion User C program', visible:[true,true,false,false,false,true],paramNames:['Thread','VAR','','','','File']},
         {action:5,name:'Run a KMotion User C program wait til finished', visible:[true,true,false,false,false,true],paramNames:['Thread','VAR','','','','File']},
         {action:6,name:'Run a KMotion User C program wait til finished, resync positions', visible:[true,true,false,false,false,true],paramNames:['Thread','VAR','','','','File']},
         {action:7,name:'Run a Windows program wait til finished', visible:[false,false,false,false,false,true],paramNames:['','','','','','File']},
         {action:8,name:'Callback to the User Application', visible:[false,false,false,false,false,false],paramNames:['','','','','','']},
         {action:9,name:'Wait/Hold until a bit is high or low', visible:[true,true,false,false,false,false],paramNames:['Wait bit','till','','','','']}
       ];
    
    $scope.machine = settings.machine;
    
    $scope.saveMachine = function() {
      settings.save();
    };    
    
    $scope.showMachine = function(e) {
      console.log(angular.toJson(settings.machine,true));
    }
    
    $scope.$on('state-update', function(event, args){
      var state = args.state;
      if (state.machine != "" && state.machine != settings.fileName()) {
        settings.load(state.machine);
      }
    });
    
  }
  
})();

