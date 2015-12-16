(function() {
  angular.module('KmotionXApp')
   .factory('settings', settings);
  
  settings.$inject = ['$rootScope','$http','kmxBackend'];
  
  function settings($rootScope,$http,kmxBackend){
    var mcodes = ['M0','M1','M2','M3', 'M4', 'M5', 'M6', 'M7', 'M8', 'M9', 'S'];
    var mcodesSpecial = ['M30', 'Cycle Start', 'Halt', 'Stop', 'Feedhold', 'Resume', 'Prog Start', 'Prog Exit'];
    var mcodesExtended = ["M100", "M101", "M102", "M103", "M104", "M105", "M106", "M107", "M108", "M109", "M110", "M111", "M112", "M113", "M114", "M115", "M116", "M117", "M118", "M119"];
    
    var data = newMachine();
    
    var service =  {
      machine: data,
      load: load,
      save: save,
      fileName: fileName
    };
    return service;
    
    function newMachine(){
      return {
        name: null,
        description: null,
        dimX: 0,
        dimY: 0,
        dimZ: 0,
        initProgram: null,
        initThread: 1,
        axes: axesArr(),
        tplanner: {},
        actions: actionsArr(undefined, mcodes),
        specialActions: actionsArr(undefined, mcodesSpecial),
        extendedActions: actionsArr(undefined,mcodesExtended),
        userActions: userActionsArr()
      };
    }
    
    function load(file) {
      return $http.get(file +"?nocache=1").success(function(jsonData) {
        updateSettings(jsonData);
        $rootScope.$broadcast('settings-update');
      });
    }
      
    function updateSettings(jsonData) {
      var machineData = newMachine();
      machineData.name = jsonData.name;
      machineData.description = jsonData.description;
      machineData.dimX = jsonData.dimX;
      machineData.dimY = jsonData.dimY;
      machineData.dimZ = jsonData.dimZ;
      machineData.initProgram = jsonData.initProgram;
      machineData.initThread = jsonData.initThread;
      machineData.axes = jsonData.axes || axesArr();
      machineData.tplanner = jsonData.tplanner || {};
      machineData.actions = actionsArr(jsonData.actions,mcodes);
      machineData.specialActions = actionsArr(jsonData.specialActions, mcodesSpecial);
      machineData.extendedActions = actionsArr(jsonData.extendedActions, mcodesExtended);
      machineData.userActions = jsonData.userActions || userActionsArr();
      angular.copy(machineData, data);
    }
    
    function fileName(){
      return "settings/" +service.machine.name + ".cnf";
    }
    
    function save() {
      var file = fileName(); 
      kmxBackend.save(file, angular.toJson(service.machine,true));
      //TODO only update params if save succeeds
      kmxBackend.onUpdateMotionParams();
      $rootScope.$broadcast('settings-update');
    }    
    function axesArr(){
      var axes = [] 
      var axisNames = ['X', 'Y', 'Z', 'A', 'B', 'C'];    
      for(var i = 0; i < 6; i++) {
        axes.push({name:axisNames[i]});
      }
      return axes;
    }
    
    function actionsArr(existingActions, codes){

      var actions = existingActions || [] 
      
      for(var i = 0; i < codes.length; i++) {
        var codeName = codes[i];
        var found = false;
        for(var j = 0; j < actions.length; j++) {
          if(actions[j].name === codeName){
            found = true; 
            break;
          }
        }
        if(!found){
          actions.push({
            action:0,
            name:codeName
          });
        }
      }
      //Remove faulty named actions
      var idx = actions.length;
      while(idx--) {
        if(codes.indexOf(actions[idx].name) < 0){
          actions.splice(idx,1);
        };
      }
      
      actions.sort(function compare(a, b) {
        return codes.indexOf(a.name) - codes.indexOf(b.name);
      });   
      return actions;
    }
    
    function userActionsArr(){
      var actions = [] 
      for(var i = 0; i < 10; i++) {
        actions.push({
          action:0
        });
        
      }   
      return actions;
    }
    
  }
})();