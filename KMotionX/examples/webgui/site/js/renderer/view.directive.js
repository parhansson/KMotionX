'use strict';
(function() {
  
  angular.module('KMXRenderer')
  .directive('threeViewer', threeViewer);
  
  threeViewer.$inject = ['$timeout','$window'];
  
  function threeViewer($timeout,$window) {
    return {
      restrict: 'A',
      scope: {
        threeViewer: '='
      },
      link: link
    };
    
    function link(scope, element, attrs) {
      var viewer = scope.threeViewer;
      var windowElement = angular.element($window);
      var elem = element[0];//$(element);
      elem.width = function width(){
        return elem.offsetWidth;
      };
      elem.height = function width(){
        return elem.offsetHeight;
      };
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
      camera.position.x = 150;
      camera.position.y = 100;
      camera.position.z = 500;
      var center = new THREE.Vector3( 150, 100, 0 )
      camera.lookAt(center);
      //camera.up = new THREE.Vector3( 0, 0, 0 );
      scene.add(camera);
      
      
      var controls = new THREE.TrackballControls(camera, element[0] );
      //controls.noPan = false;
      controls.minDistance = 1;
      controls.maxDistance = 10000;
      controls.dynamicDampingFactor = 0.15;
      controls.target = center;
      
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
      windowElement.on('resize', onResize);
      element.on( 'mousemove', onElementMouseMove );

      
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
              //console.info(intersects[ 0 ].point);
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
        var elementXPosition = (event.offsetX != null) ? event.offsetX : event.originalEvent.layerX;
        var elementYPosition = (event.offsetY != null) ? event.offsetY : event.originalEvent.layerY;
        mouse3d.x = mouse.x = ( elementXPosition / width ) * 2 - 1;
        mouse3d.y = mouse.y = - ( elementYPosition / height ) * 2 + 1;
        //console.info("mouse.x mouse.y",mouse.x ,mouse.y);
        
        mouse3d.z = 0.5; 
        mouse3d.unproject(camera);
      }
      
    }
    
  }
  
   
  
})();



