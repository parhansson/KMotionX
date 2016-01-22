(function() {
  angular.module('KMXRenderer')
    .factory('GCodeRenderer',GCodeRenderer);
  
  GCodeRenderer.$inject = ['$q','kmxThreeView', 'transformer'];
  
  function GCodeRenderer($q,kmxThreeView, transformer) {
    var currentObject = null;
    var view = kmxThreeView.getInstance("main");
    var instance = {
        clearScene: clearScene,
        renderGCode: renderGCode
    }; 
    
    return instance;
    
    
    function clearScene(){
      if (currentObject) {
        view.modelObject.remove(currentObject);
        view.update();
      }
    }
    
    function renderGCode(gcode){
      transformer.transformNamed('G-Code to Three', gcode).then(function (object) {
          clearScene();
          currentObject = object;
          view.modelObject.add(currentObject);
          view.update();
      },function (reason) {
          console.error(reason);
      });
    }
  }  
})();