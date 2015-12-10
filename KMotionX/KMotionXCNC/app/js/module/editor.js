'use strict';
(function() {
  var module = angular.module('CodeEditor');
  
  /*  
   * 
   * http://stackoverflow.com/questions/14324451/angular-service-vs-angular-factory
   * 
  dictt = JSON.parse(evt.data)
  angular.element(document.getElementById('gcc')).scope().$apply(function(scope){
      scope.filters = dictt.filters;
  });
  */
 

  module.directive("modalShow", modalShow);
  
  modalShow.$inject = ['$parse'];
  
  function modalShow($parse) {
    return {
        restrict: "A",
        link: function (scope, element, attrs) {

            //Hide or show the modal
            scope.showModal = function (visible, elem) {
                if (!elem){
                    elem = element;
                }

                if (visible){
                    elem.modal("show");                     
                } else {
                    elem.modal("hide");
                }
            }

            //Watch for changes to the modal-visible attribute
            scope.$watch(attrs.modalShow, function (newValue, oldValue) {
                scope.showModal(newValue, attrs.$$element);
            });
            
            //Update the visible value when the dialog is closed through UI actions (Ok, cancel, etc.)
            element.on("hide.bs.modal", function () {
                $parse(attrs.modalShow).assign(scope, false);
                //if (!scope.$$phase && !scope.$root.$$phase)
                //    scope.$apply();
            });

        }

    };
  }
  
  module.directive('fileModal', function fileModal() {
    return {
      restrict: 'E',
      templateUrl: 'js/partials/file-modal.html'
    };
  });
  
  
})();
