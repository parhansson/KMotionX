(function() {

  angular.module('CodeEditor')
  .controller('GCodeEditorController', GCodeEditorController);  
  
  GCodeEditorController.$inject = ['$scope','$controller','GCodeRenderer','kmxBackend','settings']; 
  
  function GCodeEditorController($scope,$controller,GCodeRenderer,kmxBackend,settings){

    //Initialize the super class (EditorController) and extend it.
    angular.extend(this, $controller('EditorController', {$scope: $scope}));
    var vm = this;
    //override fileSelected from EditorController
    $scope.fileSelected = function(path){
      //will trigger a state-update event
      kmxBackend.loadGlobalFile(1, path);
    }

    //… Additional extensions to create a mixin.
    $scope.$on('state-update', function(event, args){
      var state = args.state;
      
      $scope.interpreting = state.interpreting == 1;
      $scope.feedHold = state.feedHold == 1;
      $scope.$apply();
      if (state.file != "" && state.file != $scope.editorContentName ) {
        vm.openFile(state.file)
      }
    });
    
    vm.feedHold = function(){
      kmxBackend.feedHold();     
    }
    vm.initMachine = function(){
      kmxBackend.compileAndLoadCoff([settings.machine.initProgram, settings.machine.initThread]);
      //TODO needs  to Execute program as well
      //Execute<thread>
    }
    
    
    vm.interpret = function(){
      //set motion params should done on backend
      kmxBackend.setMotionParams(settings.machine);
      var BoardType = 2; //KLFOP
      kmxBackend.interpret(BoardType, $scope.editorContentName, 0, -1, true);
      
      /*
      var lastImported = localStorage.getItem('last-imported');
      var lastLoaded = localStorage.getItem('last-loaded');
      if (lastImported) {
        alert("Cannot interpret unsaved file.");
      } else {
        kmxBackend.interpret([2, lastLoaded, 0, -1, 1]);
      }
      */
    }
    
    //TODO Det här borde kunna göras i direktivet. Registrera en function som ska köras
    //när editorn ändras. editor.on('change') finns i direktive. Då slipper man en watch
    //This is great but not working very well on large(MB) files 
    $scope.$watch('editorContent', function (newVal, oldVal) {
      if (!newVal) return;
      //view3d is inheritet from app controller
      GCodeRenderer.renderGCode(new GCode.Source($scope.editorContent));
    });


    
    $scope.selectLine = function (line) {
      $scope.aceEditor.moveCursorTo(line, 0);
    }
    
  }

  

  
})();