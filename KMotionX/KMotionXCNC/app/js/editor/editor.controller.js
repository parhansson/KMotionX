(function() {
  
  angular.module('CodeEditor')
  .controller('EditorController', EditorController);

  EditorController.$inject = ['$scope','kmxBackend','$timeout','NormalizePath'];

  function EditorController($scope, kmxBackend, $timeout, NormalizePath){
    //TODO use scope instead. Opens up for extending controller in a better way
    this.openFile = openFile; //not exposed in scope
    angular.extend($scope, 
      {
        fileSelected: fileSelected, //allows overrides for extending controllers 
        showFileModal: false,
        listFiles: listFiles, 
        selectFile: selectFile,
        save: save,
        editorContent: "",
        editorContentName: ""
      });
    
    function listFiles(){
      kmxBackend.listDir($scope.currentDir).then(function(response){
        $scope.files = response.data;
        $scope.showFileModal = true;
      });
    }
    function openFile(path) {
      kmxBackend.load(path).then(function(response){
        $scope.editorContent = response.data;
        $scope.editorContentName = path;
        
        //postpone calls to editor. cannot be made in this eventloop
        $timeout(function(){
          $scope.aceEditor.clearSelection();
          $scope.aceEditor.moveCursorTo(0, 0);
        }, 0);          
      });
      
    }
    function fileSelected(path){
      openFile(path);
    }
    function selectFile(file) {
      //send request for file. Let state update load the actual file.
      if(file.type == 8){
        var path = $scope.currentDir + file.name;
        $scope.fileSelected(path);
        $scope.showFileModal = false;
      } else if(file.type == 4){
        var newDir = $scope.currentDir + file.name + "/";
        $scope.currentDir = NormalizePath(newDir);
        if($scope.currentDir === ""){
          $scope.currentDir = "./";
        }
        $scope.listFiles($scope.currentDir);          
        
      }
    }
    
    function save(){
      kmxBackend.save($scope.editorContentName, $scope.editorContent);
    }
  }    
  

  
})();