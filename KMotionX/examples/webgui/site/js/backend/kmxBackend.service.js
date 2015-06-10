(function() {

  angular.module('KMXBackend')
    .factory('kmxBackend', kmxBackend);
  
  kmxBackend.$inject = ['$http'];
  
  function kmxBackend($http){     
    var service = {
        listDir: listDir,
        loadGlobalFile: loadGlobalFile,
        compileAndLoadCoff: compileAndLoadCoff,
        invokeAction: invokeAction,
        updateMotionParams: updateMotionParams,
        //interpret: interpret,        
        onSimulate: onSimulate,
        onFeedhold: onFeedhold,
        onAbort: onAbort,
        onHalt: onHalt,
        onCycleStart: onCycleStart,
        onStep: onStep,
        onReset: onReset,
        
        load: load,
        save: save
        
    };
    
    return service;
    
    
    function load(file) { 
      return $http({
        url: file,
        params: { 'nocache': new Date().getTime() }
      });
      
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
    
    
    
    function listDir(path) {
      var url = "/api/aux/listDir";
      var msg = {"params": path};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
      
    }
    function loadGlobalFile(type, file) {
      var url = "/api/aux/loadGlobalFile";
      var msg = {"params": [type, file]};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
      
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
    function onAbort() {
      onEvent('onAbort');
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
    function onEvent(eventName){
      
      var url = "/api/aux/"+eventName;
      var msg = {"ret":null};
      return $http.post(url, msg)
      .error(function(data, status, headers, config) {
        // called asynchronously if an error occurs
        // or server returns response with an error status.
        console.log(data);
      });
    }
    
    function compileAndLoadCoff(params) {
      var url = "/api/kmotion/CompileAndLoadCoff";
      var msg = {"params":params,"ret":null};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    }
    
    function updateMotionParams() {
      var url = "/api/interpreter/updateMotionParams";
      var msg = {"ret":null};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    }
    
    function invokeAction(action) {
      var url = "/api/interpreter/InvokeAction";
      var msg = {"params": [action]};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    }
    
    function interpret(BoardType, InFile, start, end, restart) {
      var url = "/api/interpreter/Interpret";
      var msg = {"params":[BoardType, InFile, start, end, restart],"ret":null};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    } 
    
    function doErrMsg(msg) { //currently not used
      var url = "/api/kmotion/DoErrMsg";
      return $http.post(url, {"params":msg})
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    }    
  }
  
    
})();

