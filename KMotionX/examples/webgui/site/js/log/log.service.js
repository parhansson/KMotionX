(function() {
  angular.module('KMXLog')
    .factory('kmxLogger', kmxLogger); 
    
  function kmxLogger() {
      var consoles = {};
      var service = {
          registerConsole: registerConsole,
          log: log,
          clear: clear,
          logFragment: logFragment
      };
      
      return service;
      
      function registerConsole(id,nodeElement, color){
        consoles[id] = {
            nodeElement: nodeElement,
            color: color || 'black'
        };
      }
      function clear(id){
        $(consoles[id].nodeElement).empty(); 
      }
      
      function logFragment(id, fragment){
        var con = consoles[id];
        if(con === undefined){
          console.log("Console %s is undefined",id);
        } else {
          var node = con.nodeElement; 
          node.appendChild(fragment);
          node.scrollTop = node.scrollHeight;
        }
      }
      
      function log(id, message){
        var con = consoles[id];
        if(con === undefined){
          console.log("Console %s: %s",id, message);
        } else {
          logFragment(id,createLogPost(message,con.color));
        }
      }
      
      function createLogPost(text, color) {
        var fragment=document.createDocumentFragment();
        var div = document.createElement('div');
        var sp = document.createElement('span');
        sp.style.color = color;
        sp.appendChild(document.createTextNode(text));
        div.appendChild(sp);
        fragment.appendChild(div);
        return fragment;

      }
    }
})();



