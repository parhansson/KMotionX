
let root = "../../../" //added to map. relative to this file

let imports = [
  "node_modules/core-js/client/shim.min.js",
  "node_modules/systemjs/dist/system.src.js",
  "../vendor/mozilla/stringview.js",
  "systemjs.config.js"
]


self['filterSystemConfig'] =  function(config){
    for(let key in config.map){
      config.map[key] = root + config.map[key]; 
    }
    config.packages[root] = { defaultExtension: 'js' }
  }

for(let script of imports){
  importScripts(root + script)
}

let mainModule = "socket.message.broker";


System.import(mainModule).then((module) =>{

    console.log("Loaded " + mainModule);
    let socketWorker = new module.SocketMessageBroker(this.postMessage.bind(this));
    //self.addEventListener('message', onMessage, false);  
    this.onmessage = socketWorker.onMessage.bind(socketWorker);

  },
  (error) => {
    console.error("Failed:", error)
  }
)