(function() {
  angular.module('KMXRenderer')
    .factory('kmxThreeView', kmxThreeView); 
    
  function kmxThreeView() {
      var instances = {};
      var service = {
          getInstance: getInstance
      };
      
      return service;
      
      function getInstance(name){
        if(instances[name] === undefined){
          instances[name] = new View(name);
        }
        return instances[name];
      }
      
      function View(name){
        var machineBounds = null;
        var machineBackground = null;
        this.name = name;
        this.machineObject = new THREE.Group();
        this.modelObject = new THREE.Group();
        this.updateFn = null; // set by viewer
        
        this.update = function (){
          if(this.updateFn){
            this.updateFn();
          }
        };
        this.updateSettings = function(settings){
          if(machineBounds != null){
            this.machineObject.remove(machineBounds);
          }
          if(machineBackground != null){
            this.machineObject.remove(machineBackground);
          }
          var x = settings.machine.dimX;
          var y = settings.machine.dimY;
          var z = settings.machine.dimZ;
          machineBounds = createMachineBounds(x,y,z);
          machineBackground = renderBackground(x,y,z);
          this.machineObject.add(machineBounds);
          this.machineObject.add(machineBackground);
          this.update();          
        }
        
        function createMachineBounds(x,y,z){
      
          var mesh = new THREE.Mesh(new THREE.BoxGeometry( x, y, z ));
          mesh.matrixWorld.makeTranslation(x/2,y/2,z/2);
          var edges = new THREE.EdgesHelper(mesh, 0x0000ff );
          edges.material.linewidth = 1;
          return edges;
          
        }
        
        function renderBackground(x,y,z){
            var texture = THREE.ImageUtils.loadTexture('images/bghoneym.jpg');
            
            // assuming you want the texture to repeat in both directions:
            //texture.wrapS = THREE.RepeatWrapping; 
            //texture.wrapT = THREE.RepeatWrapping;
            
            // how many times to repeat in each direction; the default is (1,1),
            //   which is probably why your example wasn't working
            //texture.repeat.set( 4, 4 ); 
    
            var material = new THREE.MeshBasicMaterial({
              map: texture, 
              side: THREE.DoubleSide, 
              transparent: true, 
              opacity: 0.3});
            var geometry = new THREE.PlaneGeometry(x, y);
            var mesh = new THREE.Mesh(geometry, material);
            //mesh.matrixWorld.makeTranslation(10,10,0);
            mesh.position.x = x/2;
            mesh.position.y = y/2;
            return mesh;   
          
        }        
      }
    }
})();



