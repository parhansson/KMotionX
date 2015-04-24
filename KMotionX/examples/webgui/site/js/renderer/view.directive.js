(function() {
  angular.module('KMXRenderer')
  .directive('threeViewer', function($timeout) {
    return {
      restrict: 'A',
      scope: {
        threeViewer: '='
      },
      link: link
    };
    
    function link(scope, element, attrs) {
      var viewer = scope.threeViewer;
      var elem = $(element);

      // Renderer
      var renderer = new THREE.WebGLRenderer({antialias: true, clearColor:0x000000, alpha: true});
      renderer.setSize(elem.width(), elem.height());
      renderer.clear();
      
      //Scene
      var scene = new THREE.Scene(); 
      
      
      // Camera...
      var fov    = 45,
          aspect = elem.width() / elem.height(),
          near   = 1,
          far    = 10000;
      var camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
      //camera.rotationAutoUpdate = true;
      //camera.position.x = 0;
      //camera.position.y = 500;
      camera.position.z = 300;
      //camera.lookAt(scene.position);
      scene.add(camera);
      
      
      var controls = new THREE.TrackballControls(camera, element[0] );
      //controls.noPan = false;
      controls.dynamicDampingFactor = 0.15;

      //Directive Exports
      viewer.scene = scene;
      viewer.camera = camera;
      viewer.renderer = renderer;
      viewer.controls = controls;
      element.append(renderer.domElement);
      
      
    
      //need to add controls after element has been created
      $timeout(function(){
        //start render loop
        animate();    
      }, 0);
      
      //Logic
      var mouse = new THREE.Vector2();
      var mouse3d = new THREE.Vector3(0,0,0);
      var sgeometry = new THREE.SphereGeometry( 1 );
      var material = new THREE.MeshBasicMaterial( { color: 0xff0000 } );
      var sphereInter = new THREE.Mesh( sgeometry, material );
      sphereInter.visible = false;
      scene.add( sphereInter );
      
      var lineMaterial = new THREE.LineBasicMaterial({color: 0xff0000});
      var line;
      
      
      // Lights...
      [[0,0,1,  0xFFFFCC],
       [0,1,0,  0xFFCCFF],
       [1,0,0,  0xCCFFFF],
       [0,0,-1, 0xCCCCFF],
       [0,-1,0, 0xCCFFCC],
       [-1,0,0, 0xFFCCCC]].forEach(function(position) {
        var light = new THREE.DirectionalLight(position[3]);
        light.position.set(position[0], position[1], position[2]).normalize();
        scene.add(light);
      });
      
      var raycaster = new THREE.Raycaster();
      raycaster.linePrecision = 1;
      var currentIntersected;
      
      // Fix coordinates up if window is resized.
      $(window).on('resize', onResize);
      $(elem).on( 'mousemove', onElementMouseMove );
      //$(document).on( 'mousemove', onElementMouseMove );
      createOrigin();
      
      function createOrigin(){
        var lgeometry = new THREE.Geometry();
        var x = 100;
        var y = 100;
        var z= 100;
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(x, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, y, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(new THREE.Vector3(0, 0, z));
        
        line = new THREE.Line(lgeometry, lineMaterial);
        scene.add(line);   
      }
      
      function updateLine(){
        return;
        scene.remove(line);
        var lgeometry = new THREE.Geometry();
        lgeometry.vertices.push(new THREE.Vector3(0, 0, 0));
        lgeometry.vertices.push(mouse3d);
        line = new THREE.Line(lgeometry, lineMaterial);
        scene.add(line);    
        
      }
      
      function animate() {

        requestAnimationFrame( animate );// And repeat...
        controls.update();
        updateIntersections();
        renderer.render(scene, camera);
      }
      
      // Find mouse intersection objects!
      function updateIntersections() {
        
        // find intersections

        raycaster.setFromCamera( mouse, camera );
        

        var intersects = raycaster.intersectObjects( scene.children, true);
        
        if ( currentIntersected !== undefined ) {
          currentIntersected.material.linewidth = 1;
        }
        
        if ( intersects.length > 0 ) {

            //console.info("Type", intersects[ 0 ].object.type);
          
            currentIntersected = intersects[ 0 ].object;
            currentIntersected.material.linewidth = 5;

            sphereInter.visible = true;
            if(sphereInter !== currentIntersected){            
              sphereInter.position.copy( intersects[ 0 ].point );
            }

        } else {
            currentIntersected = undefined;

            sphereInter.visible = false;
        }      
        
        
      }
      
      function onResize(){
        var height = elem.height();
        var width = elem.width();
        renderer.setSize(width, height);
        
        camera.aspect = width / height;
        
        camera.updateProjectionMatrix();
        controls.screen.width = width; //window.innerWidth;
        controls.screen.height = height;// window.innerHeight;
        
      }

      function onElementMouseMove( event ) {
        var height = elem.height();
        var width = elem.width();
        event.preventDefault();

        // calculate mouse position in normalized device coordinates
        // (-1 to +1) for both components
        mouse3d.x = mouse.x = ( event.clientX / window.innerWidth ) * 2 - 1;
        mouse3d.y = mouse.y = - ( event.clientY / window.innerHeight ) * 2 + 1;
        
        
        mouse3d.z = 0.5; 
        mouse3d.unproject(camera);
        updateLine();
        //console.info("Mx",mouse.x,"width",width,"clientx",event.clientX);
        //console.info("My",mouse.y,"width",height,"clienty",event.clientY);
      }
      
    }
    
  });
   
  
})();



