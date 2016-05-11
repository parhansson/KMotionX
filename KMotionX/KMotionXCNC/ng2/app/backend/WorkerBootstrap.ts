
let root = "../../../"

let imports = [
  "node_modules/es6-shim/es6-shim.min.js",
  "node_modules/systemjs/dist/system.src.js",
  "../vendor/mozilla/stringview.js"
  //"node_modules/@angular/web_worker/worker.js"
]

let map = {
  //'app': 'app', // 'dist',
  'rxjs': root + 'node_modules/rxjs',
  //'@angular': 'node_modules/@angular',
  //'ng2-bootstrap':'node_modules/ng2-bootstrap',
  //'moment': 'node_modules/moment/moment.js', //needed by ng2-bootstrap datepicker
};

imports.forEach(function (script) {
  importScripts(root + script)
});

let packages = {
  'rxjs': { defaultExtension: 'js' },
}
packages[root] = { defaultExtension: 'js' } //format: 'register', 

System.config({
  //baseURL: '../../dist',
  map: map,
  packages: packages
});
//System.import("app/backend/socket.worker.js");
//var workerModule = "socket.message.broker.js";
let workerModule = "socket.message.broker.js";
let workerThread = self;
System.import(workerModule).then(
  function (module) {
    console.log("Loaded " + workerModule);
    let socketWorker = new module.SocketMessageBroker(workerThread.postMessage.bind(workerThread));
    //self.addEventListener('message', onMessage, false);  
    self.onmessage = socketWorker.onMessage.bind(socketWorker);

  },
  function (error) {
    console.error("Failed:", error)
  }
)