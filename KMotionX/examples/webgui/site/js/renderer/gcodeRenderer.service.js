(function() {
  angular.module('KMXRenderer')
    .factory('GCodeRenderer',GCodeRenderer);
  
  GCodeRenderer.$inject = ['$q','kmxThreeView', 'transformer'];
  
  function GCodeRenderer($q,kmxThreeView, transformer) {
    var currentObject = null;
    
    var instance = {
        clearScene: clearScene,
        renderGCode: renderGCode
    }; 
    
    return instance;
    
    
    function clearScene(){
      if (currentObject) {
        kmxThreeView.scene.remove(currentObject);
      }
    }
    
    function renderGCode(gcode){
      transformer.transformNamed('G-Code to Three', gcode).then(function (object) {
          clearScene();
          currentObject = object;
          kmxThreeView.scene.add(currentObject);
      },function (reason) {
          console.error(reason);
      });
    }
  }  
})();