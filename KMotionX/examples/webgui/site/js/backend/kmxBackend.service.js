(function() {

  angular.module('KMXBackend')
    .factory('kmxBackend', kmxBackend);
  
  kmxBackend.$inject = ['$http'];
  
  function kmxBackend($http){     
    var service = {
        listDir: listDir,
        loadGlobalFile: loadGlobalFile,
        saveMachine: saveMachine,
        feedHold: feedHold,
        compileAndLoadCoff: compileAndLoadCoff,
        invokeAction: invokeAction,
        setMotionParams: setMotionParams,
        simulate: simulate,
        interpret: interpret,
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
  //TODO use upload service instead
    function saveMachine(data) { 
      var url = "/api/aux/saveMachine";
      var msg = {"params":file,"ret":null};
      return $http.post(url, msg)
        .error(function(data, status, headers, config) {
          // called asynchronously if an error occurs
          // or server returns response with an error status.
          console.log(data);
        });
    }
    
    function feedHold() {
      var url = "/api/kmotion/feedHold";
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
    
    function setMotionParams(file) {
      var url = "/api/interpreter/setMotionParams";
      var msg = {"params":file,"ret":null};
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

    
    function simulate(enable) {
      var url = "/api/interpreter/simulate";
      var msg = {"params": enable};
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

