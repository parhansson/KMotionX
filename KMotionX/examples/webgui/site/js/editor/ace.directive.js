(function() {
  angular.module('CodeEditor')
    .directive("kmxAce", aceEditor); 
  
  /*    
  It appears ace computes character size asynchronously and during that 
  calls to scrollToLine are getting wrong lineHeight. 
  as a workaround until this is fixed, call resize(true) before 
  scrollToLine to force synchronous resize 

  var editor = ace.edit('editor'); 
  editor.resize(true) 
  editor.scrollToLine(50, true, true, function () {});     
  */ 
   
  function aceEditor() {
    return {
        restrict : 'EA',
        require : '?ngModel',
        replace:true,
        template:"<div></div>",
        link : link
    };
    
    function link(scope, elem, attrs, ngModel) {
      if(!ngModel) 
          return;
      var editor = createEditor(elem);
      scope.aceEditor = editor;
      /*
      //bind ace editor to editor attribute
      scope.$watch('aceEditor', function() {
          //not sure if this is correct even though it works
          //will $parent always be the controller scope here?
          if(attrs.editor)
              scope.$eval('$parent.'+attrs.editor + ' = aceEditor');
      });
      */



      //some default settings
      editor.setTheme(attrs.theme ? "ace/theme/"+attrs.theme : "ace/theme/chrome");
      editor.getSession().setMode(attrs.mode ? "ace/mode/"+attrs.mode : "ace/mode/javascript");
      attrs.$observe("mode",function(v){
        if(v){
          editor.getSession().setMode("ace/mode/"+attrs.mode);
        }
      });

      
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
    
    function createEditor(elem){

      var editor = ace.edit(elem[0]);
      editor.resize();
      editor.setValue(elem.html());
      editor.clearSelection();
      editor.moveCursorTo(0, 0);
      return editor;
    } 

    
  }
  
})();