importScripts('gcode-parser.js');

var parser = new GCodeParser(codeHandler,paramHandler);

function codeHandler(cmdObj) {
  self.postMessage(cmdObj);
}
function paramHandler(paramObj){
  postMessage(paramObj); 
}
function onMessage(event){
  if(event.data.command == 'parse'){
    parser.parse(event.data.gcode);
    postMessage('done');
  }
  
}
self.addEventListener('message', onMessage, false);