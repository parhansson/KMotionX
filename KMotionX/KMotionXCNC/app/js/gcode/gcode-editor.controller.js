(function() {

  angular.module('CodeEditor')
  .controller('GCodeEditorController', GCodeEditorController);  
  
  GCodeEditorController.$inject = ['$scope','$controller','GCodeRenderer','kmxBackend','settings','kmxThreeView','SocketService']; 
  
  function GCodeEditorController($scope,$controller,GCodeRenderer,kmxBackend,settings,kmxThreeView,SocketService){

    //Initialize the super class (EditorController) and extend it.
    angular.extend(this, $controller('EditorController', {$scope: $scope}));
    var vm = this;
    //override fileSelected from EditorController
    $scope.fileSelected = function(path){
      //tells backend to that a new file is loaded. All clients will respond
      kmxBackend.loadGlobalFile(1, path);
    }
    var scrollInterval = null;
    var interpretLine = 0;
    vm.editorLine = -1;
    //… Additional extensions to create a mixin.
    $scope.$watch('kmx.intStatus.gcodeFile',function (newFile, oldFile){
      if (newFile && newFile != "" && newFile != $scope.editorContentName ) {
        vm.openFile(newFile)
      }
    
      //only load if different from loaded
      //TODO or if file has been updated, need a force flag or date
      /*
      var lastLoaded = localStorage.getItem('last-loaded');
      if (obj.data.file != "" && obj.data.file != lastLoaded) {
          loadGCodeFromPath(obj.data.file);
      }
      */
    });
    
    $scope.$watch('kmx.intStatus.currentLine',function statusUpdate(newLine, oldLine){
        if(interpretLine != newLine){
          interpretLine = newLine;
          vm.editorLine = interpretLine + 1;
          updateEditor();
        }       
    });
    
    
    function updateEditor(){
      //$scope.aceEditor.moveCursorTo(interpretLine, 0);
      //$scope.aceEditor.gotoLine(interpretLine);
      $scope.aceEditor.selection.moveCursorToPosition({row: interpretLine, column: 0});
      $scope.aceEditor.scrollToLine(interpretLine, true, true, function () {});
      //$scope.aceEditor.selection.selectLine();

    }
    vm.onFeedhold = function(){
      kmxBackend.onFeedhold();     
    }  
    vm.onEmergencyStop = function(){
      kmxBackend.onEmergencyStop();     
    }    
    vm.onStep = function(){
      kmxBackend.onStep();     
    }
    vm.onReset = function(){
      kmxBackend.onReset();     
    }
    vm.onCycleStart = function(){
      //set motion params should done on backend
      //call either onHalt or onCycle start there to force WYSIWYG if browser has faulty state
      //kmxBackend.onHalt();
      kmxBackend.onCycleStart();
    }
      /*
      var lastImported = localStorage.getItem('last-imported');
      var lastLoaded = localStorage.getItem('last-loaded');
      if (lastImported) {
        alert("Cannot interpret unsaved file.");
      } else {
        kmxBackend.interpret([2, lastLoaded, 0, -1, 1]);
      }
      */
    
    //TODO Det här borde kunna göras i direktivet. Registrera en function som ska köras
    //när editorn ändras. editor.on('change') finns i direktive. Då slipper man en watch
    //This is great but not working very well on large(MB) files 
    $scope.$watch('editorContent', function (newVal, oldVal) {
      if (!newVal) return;
       //TODO this is executed twice. either postpone rendering with a timeout or do as describe in comment above
       //I don't like the idea of watching an object this large anyway. 
      //view3d is inheritet from app controller
      GCodeRenderer.renderGCode(new GCode.Source($scope.editorContent));
    
    });
    
    $scope.$on('settings-update', function(event, args){
      var view = kmxThreeView.getInstance("main");
      view.updateSettings(settings);
    });
    
  }

  

  
})();