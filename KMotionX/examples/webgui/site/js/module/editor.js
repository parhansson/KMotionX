(function() {
  var module = angular.module('CodeEditor', []);
  
  /*  
   * 
   * http://stackoverflow.com/questions/14324451/angular-service-vs-angular-factory
   * 
  dictt = JSON.parse(evt.data)
  angular.element(document.getElementById('gcc')).scope().$apply(function(scope){
      scope.filters = dictt.filters;
  });
  */
  module.factory('FileService', function($http,$timeout){     
    
    return {
      load: function (file,successCallback) { //currently not used
        $http.get(file).success(function(data, status, headers, config){
          successCallback(data);
        });
      },
      loadCodeEditorFile: function (fileName,codeEditor) {
        $http.get(fileName).success(function(data, status, headers, config){
          codeEditor.content = data;
          codeEditor.contentName = fileName;

          //postpone calls to editor. cannot be made in this eventloop
          $timeout(function(){
            codeEditor.editor.clearSelection();
            codeEditor.editor.moveCursorTo(0, 0);
          }, 1);
        });
      },      
      uploadFileToUrl: function(name, content, uploadUrl){
        //http://uncorkedstudios.com/blog/multipartformdata-file-upload-with-angularjs
        var fd = new FormData();
        var blob = new Blob([content], { type: 'plain/text', endings: 'transparent' });
        fd.append('file', blob, name);

        $http.post(uploadUrl, fd, {
            transformRequest: angular.identity,
            headers: {'Content-Type': undefined}
        })
        .success(function(response){
          //alert(response);
        })
        .error(function(){
          alert("Error saveing file");
        });
      },      
      newEditorFile: function(){
        return {
          editor: null,
          content: null,
          contentName: ''
        }
      }
    };
    
  });
  
  
  module.controller('CCodeEditorController', function($scope,FileService){
    $scope.codeEditor = FileService.newEditorFile();
    $scope.showFileModal = false;
    
    $scope.listFiles = function(){
      $scope.showFileModal = true;
      RPC.listDir.invoke($scope.currentDir, function(data){
        $scope.files = data;
        $scope.$apply();
      });
    }
    $scope.open = function (file) {
      //send request for file. Let state update load the actual file.
      if(file.type == 8){
        var path = $scope.currentDir + file.name;
        FileService.loadCodeEditorFile(path, $scope.codeEditor);
        $scope.showFileModal = false;
      } else if(file.type == 4){
        
      }
    }
    
    $scope.save = function(){
      FileService.uploadFileToUrl($scope.codeEditor.contentName, $scope.codeEditor.content, "http://localhost:8080/upload/bulle");
    }
  });
  module.controller('GCodeEditorController', function($scope,$rootScope,$timeout,FileService,NormalizePath){
    $scope.codeEditor = FileService.newEditorFile();
    $scope.showFileModal = false;
    
    $scope.listFiles = function(dir){
      $scope.showFileModal = true;
      RPC.listDir.invoke(dir, function(data){
        $scope.files = angular.copy(data);
        $scope.$apply();
      });
    }
    $scope.open = function (file) {
      //send request for file. Let state update load the actual file.
      if(file.type == 8){
        var path = $scope.currentDir + file.name;
        RPC.loadGCodeFile.invoke(path);
        $scope.showFileModal = false;
      } else if(file.type == 4){
        /*
        var newDir = $scope.currentDir + file.name + "/";
        $scope.currentDir = NormalizePath(newDir);
        $timeout(function(){
        $scope.listFiles($scope.currentDir);
          
          //alert(newDir +"   "+ $scope.currentDir);
        }, 5000);
        */
      }
    }
    
    $scope.save = function(){
      FileService.uploadFileToUrl($scope.codeEditor.contentName, $scope.codeEditor.content, "http://localhost:8080/upload/bulle");
    }
    
    
    $scope.$on('state-update', function(event, args){
      var state = args.state;
      
      $scope.simulating = state.simulate == 1;
      $scope.interpreting = state.interpreting == 1;
      
      if (state.file != "") {
        FileService.loadCodeEditorFile(state.file, $scope.codeEditor);
      }
    });
    
    $scope.simulate = function(){
      RPC.simulate.invoke();     
    }
    
    $scope.interpret = function(){
      RPC.interpret.invoke([2, $scope.codeEditor.contentName, 0, -1, 1]);
      /*
      var lastImported = localStorage.getItem('last-imported');
      var lastLoaded = localStorage.getItem('last-loaded');
      if (lastImported) {
        alert("Cannot interpret unsaved file.");
      } else {
        RPC.interpret.invoke([2, lastLoaded, 0, -1, 1]);
      }
      */
    }
    
    //This is great but not working very well on large(MB) files 
    $scope.$watch('codeEditor.content', function (newVal, oldVal) {
      if (!newVal) return;
      $rootScope.viewer3d.renderGCode(new GCode($scope.codeEditor.content));
    });

    $scope.transform = function(mime, data){
      if(mime == "image/svg+xml"){
        var gcode = new GCode(svg2gcode(data, {
          scale : 1,
          cutZ : 10,
          safeZ: 8,
          unit: "mm",
          dpi: 72
        }));
        
        return gcode.text;
      }
      return data;
    }
    
    $scope.selectLine = function (line) {
      //$timeout(function(){
        $scope.codeEditor.editor.moveCursorTo(line, 0);
      //}, 1);
    }
    
  });

  
  
  /*    
  It appears ace computes character size asynchronously and during that 
  calls to scrollToLine are getting wrong lineHeight. 
  as a workaround until this is fixed, call resize(true) before 
  scrollToLine to force synchronous resize 

  var editor = ace.edit('editor'); 
  editor.resize(true) 
  editor.scrollToLine(50, true, true, function () {});     
  */ 
  
  module.directive("ace",[function() {
    return {
        restrict : 'EA',
        require : '?ngModel',
        replace:true,
        template:"<div></div>",
        scope:{
          codeEditorObj: "=controllerEditor"
        },
        link : function(scope, elem, attrs, ngModel) {
            if(!ngModel) 
                return;

            //bind ace editor to editor attribute
            scope.$watch('aceEditor', function() {
                //not sure if this is correct even though it works
                //will $parent always be the controller scope here?
                if(attrs.editor)
                    scope.$eval('$parent.'+attrs.editor + ' = aceEditor');
            });


            var html = elem.html();
            var editor = ace.edit(elem[0]);
            
            //some default settings
            editor.setTheme(attrs.theme ? "ace/theme/"+attrs.theme : "ace/theme/chrome");
            editor.getSession().setMode(attrs.mode ? "ace/mode/"+attrs.mode : "ace/mode/javascript");
            attrs.$observe("mode",function(v){
                if(v){
                    editor.getSession().setMode("ace/mode/"+attrs.mode);
                }
            });

            editor.resize();
            editor.setValue(html);
            editor.clearSelection();
            editor.moveCursorTo(0, 0);

            scope.aceEditor = editor;
            //scope.$apply(function () {
            if(scope.codeEditorObj){
              scope.codeEditorObj.editor = editor;
            }

            //});
            //scope.$apply();
            
            //specify how UI should be updated
            ngModel.$render = function() {
                editor.setValue(ngModel.$viewValue);
            };

            function valid(editor) {
                return (Object.keys(editor.getSession()
                        .getAnnotations()).length == 0);
            }
            editor.on('change', function() {
                ngModel.$setViewValue(editor.getValue());
            });
        }
    };
    }]);
  //http://buildinternet.com/2013/08/drag-and-drop-file-upload-with-angularjs/
  //http://jsfiddle.net/lsiv568/fsfPe/10/
  module.directive('fileDropzone', function() {
    return {
      restrict: 'A',
      scope: {
        fileContent: '=',
        fileName: '=',
        transformFn: "&"
      },
      link: function(scope, element, attrs) {
        var checkSize, isTypeValid, validMimeTypes;
        
        var processDragOverOrEnter = function(event) {
          if (event != null) {
            event.preventDefault();
          }
          event.dataTransfer.effectAllowed = 'copy';
          return false;
        };
        validMimeTypes = attrs.fileDropzone;
        checkSize = function(size) {
          var _ref;
          if (((_ref = attrs.maxFileSize) === (void 0) || _ref === '') || (size / 1024) / 1024 < attrs.maxFileSize) {
            return true;
          } else {
            alert("File must be smaller than " + attrs.maxFileSize + " MB");
            return false;
          }
        };
        isTypeValid = function(type) {
          if ((validMimeTypes === (void 0) || validMimeTypes === '') || validMimeTypes.indexOf(type) > -1) {
            return true;
          } else {
            alert("Invalid file type.  File must be one of following types " + validMimeTypes);
            return false;
          }
        };
        element.bind('dragover', processDragOverOrEnter);
        element.bind('dragenter', processDragOverOrEnter);
        return element.bind('drop', function(event) {
          var file, name, reader, size, type;
          if (event != null) {
            event.preventDefault();
          }
          file = event.dataTransfer.files[0];
          name = file.name;
          type = file.type;
          size = file.size;
          
          reader = new FileReader();
          reader.onload = function(evt) {
            if (checkSize(size) && isTypeValid(type)) {
              
              var datat = scope.transformFn({
                mime: type,
                data: evt.target.result
              });
              
              //var data = evt.target.result;
              return scope.$apply(function() {
                scope.fileContent = datat;//evt.target.result;
                //scope.$root.$broadcast('drop-gcode-file');
                //scope.$emit('drop-gcode-file');
                if (angular.isString(scope.fileName)) {
                  return scope.fileName = name;
                }
              });
            }
          };

          reader.readAsBinaryString(file);
          return false;
        });
      }
    };
  });

  module.directive("modalShow", function ($parse) {
    return {
        restrict: "A",
        link: function (scope, element, attrs) {

            //Hide or show the modal
            scope.showModal = function (visible, elem) {
                if (!elem)
                    elem = element;

                if (visible)
                    $(elem).modal("show");                     
                else
                    $(elem).modal("hide");
            }

            //Watch for changes to the modal-visible attribute
            scope.$watch(attrs.modalShow, function (newValue, oldValue) {
                scope.showModal(newValue, attrs.$$element);
            });

            //Update the visible value when the dialog is closed through UI actions (Ok, cancel, etc.)
            $(element).bind("hide.bs.modal", function () {
                $parse(attrs.modalShow).assign(scope, false);
                if (!scope.$$phase && !scope.$root.$$phase)
                    scope.$apply();
            });
        }

    };
  });
  
  module.directive('fileModal', function() {
    return {
      restrict: 'E',
      templateUrl: 'js/partials/file-modal.html'
    };
  });
  
  
})();