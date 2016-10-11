
import { SocketMessageBroker } from './socket.message.broker'
let root = "../../../" //added to map. relative to this file
//let root = "/ng2/" //added to map. relative to this file

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


SystemJS.import(mainModule).then((module) =>{

    console.log("Loaded ",module);
    let socketWorker = new module['SocketMessageBroker'](this) as SocketMessageBroker;
  },
  (error) => {
    console.error("Failed:", error)
  }
)