(function() {
  var app = angular.module('KMX-settings', []);
  
  app.factory('SettingsFactory', function(){
    
    var axesArr = function(){
      var axes = [] 
      var axisNames = ['X', 'Y', 'Z', 'A', 'B', 'C'];    
      for(var i = 0; i < 6; i++) {
        axes.push({name:axisNames[i]});
      }
      return axes;
    };
    var mcodes = ['','','','M3', 'M4', 'M5', 'M6', 'M7', 'M8', 'M9', 'S'];
    var mcodesExtended = ["M100", "M101", "M102", "M103", "M104", "M105", "M106", "M107", "M108", "M109", "M110", "M111", "M112", "M113", "M114", "M115", "M116", "M117", "M118", "M119"];
    
    var actionsArr = function(codes){
      var actions = [] 
      
      for(var i = 0; i < codes.length; i++) {
        actions.push({
          action:0,
          name:codes[i]
        });
        
      }   
      return actions;
    };
    var userActionsArr = function(){
      var actions = [] 
      for(var i = 0; i < 10; i++) {
        actions.push({
          action:0
        });
        
      }   
      return actions;
    };
    
    var data = {
        name: null,
        description: null,
        dimX: 0,
        dimY: 0,
        dimZ: 0,
        initProgram: null,
        initThread: 1,
        axes: axesArr(),
        tplanner: {},
        actions: actionsArr(mcodes),
        extendedActions: actionsArr(mcodesExtended),
        userActions: userActionsArr()
    };
             
    
    return {
      machine:data,
      load: function (file,callback) {
        return $.getJSON(file, function(jsonData) {
          data.name = jsonData.name;
          data.description = jsonData.description;
          data.dimX = jsonData.dimX;
          data.dimY = jsonData.dimY;
          data.dimZ = jsonData.dimZ;
          data.initProgram = jsonData.initProgram;
          data.initThread = jsonData.initThread;
          data.axes = jsonData.axes || axesArr();
          data.tplanner = jsonData.tplanner || {};
          data.actions = jsonData.actions || actionsArr(mcodes);
          data.extendedActions = jsonData.extendedActions || actionsArr(mcodesExtended);
          data.userActions = jsonData.userActions || userActionsArr();
          
          callback();
        });
        
      },
      save: function() {
        var file = "settings/" +data.name + ".cnf";
        var param = {};
        var copy = angular.copy(data);
        param[file] = copy;
        RPC.saveMachine.invoke(param);
        RPC.setMotionParams.invoke(copy);
      }    
    };
    
  });
  
  app.controller('UserButtonController', function($scope, SettingsFactory){
    $scope.machine = SettingsFactory.machine;
    $scope.onUserButton = function(action){
      if(action.action == 4){
        RPC.compileAndLoadCoff.invoke([action.file, action.dParam0]);
      }
    }
  });
  
  app.controller('SettingsController', function($scope, SettingsFactory){
    
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
    
    SettingsFactory.load('settings/Appa.cnf?s=1',function(){
      $scope.machine = SettingsFactory.machine;
      //apply scope when data is loaded
      $scope.$apply();
    });
    
    $scope.saveMachine = function() {
      SettingsFactory.save();
    };    
    
    $scope.showMachine = function(e) {
      console.log(angular.toJson(angular.element(e.srcElement).scope().machine,true));
    }
  });

  app.directive("settings", function() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/partials/machine-settings.html"
    };
  });
  app.directive("mcodeValue", function() {
    return {
      restrict: 'E',
      replace:true,
      templateUrl: "js/partials/mcode-value.html"
    };
  });
  
})();

