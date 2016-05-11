(function() {

  angular.module('KMXBackend')
    .factory('kmxBackend', kmxBackend);
  
  kmxBackend.$inject = ['$http'];
  
  function kmxBackend($http){     
    var service = {
        listDir: onListDir,
        loadGlobalFile: onLoadGlobalFile,
        onInvokeAction: onInvokeAction,
        onUpdateMotionParams: onUpdateMotionParams,   
        onSimulate: onSimulate,
        onFeedhold: onFeedhold,
        onEmergencyStop: onEmergencyStop,
        onHalt: onHalt,
        onCycleStart: onCycleStart,
        onStep: onStep,
        onReset: onReset,
        onDoErrorMessage: onDoErrorMessage,
        jog: jog,
        
        load: load,
        save: save
        
    };
    
    return service;
    
    
    function load(file) { 
      return onEvent('openFile',{"params": file});
    //       let url = "/api/kmx/" + 'openFile';
    // let data = { "params": path };
      // return $http({
      //   url: file,
      //   params: { 'nocache': new Date().getTime() }
      // });
      
    }

    function save(name, content){
      //http://uncorkedstudios.com/blog/multipartformdata-file-upload-with-angularjs
      var fd = new FormData();
      var blob = new Blob([content], { type: 'plain/text', endings: 'transparent' });
      fd.append('file', blob, name);

      $http.post("/upload", fd, {
          transformRequest: angular.identity,
          headers: {'Content-Type': undefined}
      })
      .success(function(response){
        //alert(response);
      })
      .error(function(){
        alert("Error saving file " + name);
      });
    }
   
    function onListDir(path) {
      return onEvent('listDir',{"params": path});
    }
    function onLoadGlobalFile(type, file) {
      if(type === 1){
        return onEvent('setGcodeFile', {"params":file});        
      } else if(type === 2){
        return onEvent('setMachineFile', {"params":file});
      }
    }
    function jog(axis, speed) {
      onEvent('jog',{"params": [axis, speed]});
    }
    function onFeedhold() {
      onEvent('onFeedhold');
    }
    function onSimulate() {
      onEvent('onSimulate');
    }
    function onHalt() {
      onEvent('onHalt');
    }
    function onEmergencyStop() {
      onEvent('onEmergencyStop');
    }
    function onCycleStart() {
      onEvent('onCycleStart');
    }
    function onStep() {
      onEvent('onStep');
    }
    function onReset() {
      onEvent('onReset');
    }
    function onUpdateMotionParams() {
      onEvent('onUpdateMotionParams');
    }
    function onInvokeAction(action) {
      onEvent('onInvokeAction', {"params": action});
    }
    function onDoErrorMessage(message) {
      onEvent('onDoErrorMessage', {"params": msg});
    }    
    
    function onEvent(eventName, msg){
      var url = "/api/kmx/"+eventName;
      var data = msg || {};
      return $http.post(url, data)
      .error(function(data, status, headers, config) {
        // called asynchronously if an error occurs
        // or server returns response with an error status.
        console.log(data);
      });
    }  
  }
  
    
})();

