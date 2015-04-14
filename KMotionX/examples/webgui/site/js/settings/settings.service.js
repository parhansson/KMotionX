(function() {
  angular.module('KmotionXApp')
   .factory('settings', settings);
  
  settings.$inject = ['$http','kmxBackend'];
  
  function settings($http,kmxBackend){
    var mcodes = ['','','','M3', 'M4', 'M5', 'M6', 'M7', 'M8', 'M9', 'S'];
    var mcodesExtended = ["M100", "M101", "M102", "M103", "M104", "M105", "M106", "M107", "M108", "M109", "M110", "M111", "M112", "M113", "M114", "M115", "M116", "M117", "M118", "M119"];
    
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

    var service =  {
      machine:data,
      load: load,
      save: save,
      fileName: fileName
    };
    return service;
    
    function load(file) {
      return $http.get(file).success(function(jsonData) {
        updateSettings(jsonData);
      });
    }
      
    function updateSettings(jsonData) {
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
    }
    
    function fileName(){
      return "settings/" +data.name + ".cnf";
    }
    
    function save() {
      var file = fileName(); 
      var param = {};
      var copy = angular.copy(data);
      param[file] = copy;
      kmxBackend.save(file, angular.toJson(data,true));
    }    
    function axesArr(){
      var axes = [] 
      var axisNames = ['X', 'Y', 'Z', 'A', 'B', 'C'];    
      for(var i = 0; i < 6; i++) {
        axes.push({name:axisNames[i]});
      }
      return axes;
    }
    
    function actionsArr(codes){
      var actions = [] 
      
      for(var i = 0; i < codes.length; i++) {
        actions.push({
          action:0,
          name:codes[i]
        });
        
      }   
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