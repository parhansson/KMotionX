(function() {
  angular.module('CodeEditor')
    .directive('fileDropzone', fileDropZone);

//http://buildinternet.com/2013/08/drag-and-drop-file-upload-with-angularjs/
  //http://jsfiddle.net/lsiv568/fsfPe/10/
  
  function fileDropZone() {
   
    return {
      restrict: 'A',
      scope: {
        fileContent: '=',
        fileName: '=',
        transformFn: "&"
      },
      link: link
    };
    
    function link(scope, element, attrs) {
      
      var validMimeTypes = attrs.fileDropzone;
      
      element.bind('dragover', processDragOverOrEnter);
      element.bind('dragenter', processDragOverOrEnter);
      
      return element.bind('drop', processDrop);
      
      
      
      function processDrop(event) {
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
            
            var datat;
            if(attrs.transformFn !== undefined){
              datat = scope.transformFn({
                mime: type,
                data: evt.target.result
              });                
            } else {
              datat = evt.target.result;
            }
            
            return scope.$apply(function() {
              scope.fileContent = datat;
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
      }
      
      function processDragOverOrEnter(event) {
        if (event != null) {
          event.preventDefault();
        }
        event.dataTransfer.effectAllowed = 'copy';
        return false;
      }
      function checkSize(size) {
        var _ref;
        if (((_ref = attrs.maxFileSize) === (void 0) || _ref === '') || (size / 1024) / 1024 < attrs.maxFileSize) {
          return true;
        } else {
          alert("File must be smaller than " + attrs.maxFileSize + " MB");
          return false;
        }
      }
      
      function isTypeValid(type) {
        if ((validMimeTypes === (void 0) || validMimeTypes === '') || validMimeTypes.indexOf(type) > -1) {
          return true;
        } else {
          alert("Invalid file type.  File must be one of following types " + validMimeTypes);
          return false;
        }
      }        
      
    }
  }

})();