//IE required polyfills, in this exact order -->
//"node_modules/es6-shim/es6-shim.min.js"
//"node_modules/systemjs/dist/system-polyfills.js"
var root = "../../../"

importScripts(root + "node_modules/es6-shim/es6-shim.min.js");
importScripts(root + "node_modules/systemjs/dist/system.src.js");
importScripts(root + "vendor/mozilla/stringview.js");

//importScripts("angular2/web_worker/worker.js");
System.config({
  //baseURL: '../../dist',
  packages: {
   // "../../dist": {
   //   "./": {
        "../../../": {
      format: 'register',
      defaultExtension: 'js'
    }
  }
});
// System.import("../../../../vendor/mozilla/stringview.js").then(
//   function (module) {
//     console.log("Loaded " + workerModule);
//   },console.error.bind(console));
//System.import("app/backend/socket.worker.js");
//var workerModule = "socket.message.broker.js";
var workerModule = "socket.message.broker.js";
var workerThread = self;
System.import(workerModule).then(
  function (module) {
    console.log("Loaded " + workerModule);
    var socketWorker = new module.SocketMessageBroker(workerThread.postMessage.bind(workerThread));
    //self.addEventListener('message', onMessage, false);  
    self.onmessage = socketWorker.onMessage.bind(socketWorker);
    
  },
  function (error) {
    console.error("Failed:", error);
  }
  );
  
  