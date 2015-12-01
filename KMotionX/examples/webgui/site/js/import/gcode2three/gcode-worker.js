importScripts('gcode-parser.js');

var parser = new GCodeParser();
parser.codeHandler = {
    handle: function(cmd,line) {
      self.postMessage({cmd:cmd,line:line});
    }
}
parser.paramHandler = function(values, line){
  postMessage({params:values,line:line});
  
}
function onMessage(event){
  if(event.data.command == 'parse'){
    parser.parse(event.data.gcode);
    postMessage('done');
  }
  
}
self.addEventListener('message', onMessage, false);