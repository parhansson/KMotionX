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
      //Directive Exports
      viewer.updateFn = requestTick;
      
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
      scene.add(viewer.machineObject);
      scene.add(viewer.modelObject);
      
      
      var controls = new THREE.TrackballControls(camera, element[0] );
      //controls.noPan = false;
      controls.minDistance = 10;
      controls.maxDistance = 10000;
      controls.dynamicDampingFactor = 0.15;
      controls.target = center;
      
      element.append(renderer.domElement);

      var o = new Orientation(element, camera);      
      
      //need to add controls after element has been created
      $timeout(function(){
        //render 
        requestTick();    
      }, 0);
      
      //Logic
      var cursor = new THREE.Mesh(new THREE.SphereGeometry( 1 ), new THREE.MeshBasicMaterial( { color: 0xff0000 } ) );
      scene.add( cursor );
      var modelDetector = new RaycastDetector(cursor, camera, viewer.modelObject);
      var machineDetector = new RaycastDetector(cursor, camera, viewer.machineObject);
      
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
     
      var mouseDown = false;
      var ticking = false;
      //Not really necessary just trying debounce function
      //var debouncedResize = KMX.Util.debounce(onResize,50); 
      // Fix coordinates up if window is resized.
      windowElement.on('resize', onResize);
      element.on( 'mousemove', onMouseMove);
      element.on( 'mousedown', function(event){
        mouseDown = true;
      });
      element.on( 'mouseup', function(event){
        mouseDown = false;
      });
      element.on( 'click', function(event){
        if(cursor.visible === true){
          console.log(cursor.position);
        }
      });
            
      function onMouseMove(event){
        event.preventDefault();
        var mouseVector = getMouseVector(event);
        var intersectionChanged = modelDetector.detect(mouseVector) || machineDetector.detect(mouseVector);
        
        if(intersectionChanged){
          //object detected
          requestTick();
        }
        if(mouseDown){
          //orbiting view with mouse or touch pad
          requestTick();
        }
      }
      /*
      element.on( 'mouseenter', function(){
        //requestTick();
      });
      element.on( 'mouseleave', function(){
        //ticking = false;
      });      
      */
      function requestTick() {
        //debounce multiple requests
        if(!ticking) {
          requestAnimationFrame(animate);
        }
        ticking = true;
      }
      controls.addEventListener('start', onControlsEvent);
      controls.addEventListener('change', onControlsEvent);
      controls.addEventListener('end', onControlsEvent);
      function onControlsEvent(event){
        requestTick();
      }
            
      //Due to bug? in three scene does not dispatch events.
      //It is the added object that dispatches the even. TODO Bug report.
      //scene.addEventListener('added', onSceneEvent);
      //scene.addEventListener('removed', onSceneEvent);
      //function onSceneEvent(event){
      //  requestTick();
      //}
      
      function animate() {
        // reset the tick so we can capture the next event
        ticking = false;
        controls.update();                 
        renderer.render(scene, camera);
        o.render(controls);
        //console.log("animate");
      }

      function onResize(event){
        var height = element.height();//elem.height();
        var width = elem.width();
        renderer.setSize(width, height);
        
        camera.aspect = width / height;
        
        camera.updateProjectionMatrix();
        controls.screen.width = width; //window.innerWidth;
        controls.screen.height = height;// window.innerHeight;
        
      }

      function getMouseVector(event) {
        var height = elem.height();
        var width = elem.width();
        // calculate mouse position in normalized device coordinates
        // (-1 to +1) for both components
        var elementXPosition = (event.offsetX != null) ? event.offsetX : event.originalEvent.layerX;
        var elementYPosition = (event.offsetY != null) ? event.offsetY : event.originalEvent.layerY;
        var mouse = new THREE.Vector2();
        mouse.x = ( elementXPosition / width ) * 2 - 1;
        mouse.y = - ( elementYPosition / height ) * 2 + 1;
        //console.info("mouse.x mouse.y %",mouse.x ,mouse.y);
        //console.info("mouse.x mouse.y",elementXPosition ,elementYPosition);
        /*
        var mouse3d = new THREE.Vector3(0,0,0);
        mouse3d.x = mouse.x;
        mouse3d.y = mouse.y;
        mouse3d.z = 0.5; 
        mouse3d.unproject(camera);
        */
        return mouse;
      }
      
    }
    
  }

  // Find mouse intersection objects!
  function RaycastDetector(markerObject, camera, parentObject){
    this.detect = detect;
    var currentObject;
    var previousPoint = new THREE.Vector3(0,0,0);
    var raycaster = new THREE.Raycaster();
    raycaster.linePrecision = 0.5;
    markerObject.visible = false;
    
    function detect(mouseVector){
      //save previous intersected object
      var prevIntersect = currentObject;
      previousPoint.copy(markerObject.position);
      
      //reset linewidth
      if ( currentObject !== undefined ) {
        currentObject.material.linewidth = 1;
      }
      
      // find intersections
      raycaster.setFromCamera( mouseVector, camera );
      var intersects = raycaster.intersectObjects( parentObject.children, true);
      
      if ( intersects.length > 0 ) {
          markerObject.visible = true;
        
          currentObject = intersects[ 0 ].object;
          currentObject.material.linewidth = 5;

          //ensure intersected object is not marker sphere
          if(markerObject !== currentObject){    
            if(currentObject.userData.lineNo !== undefined){
              console.log(currentObject.userData);
            }        
            //console.info(intersects[ 0 ].point);
            markerObject.position.copy( intersects[ 0 ].point );
          }

      } else {          
          markerObject.visible = false;
          currentObject = undefined;
      }      
      
      if(prevIntersect !== currentObject){
        //intersected object changed
        return true;
      }
      if(!previousPoint.equals(markerObject.position)){
        //intersected position changed
        return true;
      }
      return false;          
    }
  }


  function Orientation(element, syncCamera){
    var CANVAS_WIDTH = 400,
      CANVAS_HEIGHT = 400,
      CAM_DISTANCE = 200;
      
    this.render = renderInternal;
    this.renderer = new THREE.WebGLRenderer({antialias: false, clearColor:0x000000, alpha: true});
    this.scene = new THREE.Scene();
    // camera
    this.camera = new THREE.PerspectiveCamera( 50, CANVAS_WIDTH / CANVAS_HEIGHT, 1, 1000 );
    this.camera.up = syncCamera.up; // important!
      
    // renderer
    this.renderer.setSize( CANVAS_WIDTH, CANVAS_HEIGHT );
    this.renderer.domElement.style.cssText =
      'width: 150px;height: 150px;'+
      //'background-color: transparent;'+ 
      'margin: 10px;'+
      'padding: 0px;'+
      'position: absolute;'+
      'right: 0px;'+
      'top: -20px;'+
      'z-index: 100;';
      //'border: 1px solid black; /* or none; */';
      element.append(this.renderer.domElement);
      createCube(this.scene);
      
      
    function renderInternal(controls){
      this.camera.position.copy( syncCamera.position );
      this.camera.position.sub( controls.target );
      this.camera.position.setLength( CAM_DISTANCE );
      this.camera.lookAt( this.scene.position );  
      this.renderer.render(this.scene, this.camera);
    }
    
    function createCube(scene){ // create an array with six textures for a cool cube
      var loader = new THREE.TextureLoader();
      loader.load(
        '/settings/textures/textures.png',
        onTexture,
        function ( xhr ) { console.log( (xhr.loaded / xhr.total * 100) + '% loaded' );},
        function ( xhr ) { console.log( 'An error happened' );}
      );   
      
      function onTexture(texture){
          //var material = new THREE.MeshPhongMaterial( { map: THREE.ImageUtils.loadTexture('images/box-atlas.png') } );
          var material = new THREE.MeshBasicMaterial( {
            map: texture
          } );  
          
        var size = 50;
        var geometry = new THREE.CubeGeometry( size, size, size, 1, 1, 1 );
        geometry.faceVertexUvs[0] = [];
        var textPercent = 0.125;
        var LU = -textPercent;
        var RU = 0;
        var i = 0;
        for(var t = 0; t < 6; t++){
          LU += textPercent;RU += textPercent;
          var pos = [new THREE.Vector2(LU, 1), new THREE.Vector2(LU, 0), new THREE.Vector2(RU, 0), new THREE.Vector2(RU, 1)];
          LU += textPercent;RU += textPercent;
          var neg = [new THREE.Vector2(LU, 1), new THREE.Vector2(LU, 0), new THREE.Vector2(RU, 0), new THREE.Vector2(RU, 1)];
          geometry.faceVertexUvs[0][i++] = [ pos[0], pos[1], pos[3] ];
          geometry.faceVertexUvs[0][i++] = [ pos[1], pos[2], pos[3] ]; 
          geometry.faceVertexUvs[0][i++] = [ neg[0], neg[1], neg[3] ];
          geometry.faceVertexUvs[0][i++] = [ neg[1], neg[2], neg[3] ];            
        }
        var mesh = new THREE.Mesh(geometry,  material);
        console.log(mesh.toJSON());
        scene.add( mesh );
        
      }
      
      // add axes
      var radius = 80; 
      scene.add(new THREE.AxisHelper( radius ));
      
      /* 
      var segments = 32; 
      var material = new THREE.LineBasicMaterial({
          opacity: 0.6,
          transparent: true,
          linewidth: 1,
          color: 0x0000ff
        });
        
      var radians = THREE.Math.degToRad( 90 ); //90 * Math.PI / 180   
      var segments = 64,
      c1 = new THREE.CircleGeometry( radius, segments ),
      c2 = new THREE.CircleGeometry( radius, segments ),
      c3 = new THREE.CircleGeometry( radius, segments );
      c2.rotateY(radians);
      c3.rotateX(radians);
      // Remove center vertex
      c1.vertices.shift();
      c2.vertices.shift();
      c3.vertices.shift();
      scene.add( new THREE.Line( c1, material ) );
      scene.add( new THREE.Line( c2, material ) );
      scene.add( new THREE.Line( c3, material ) );
      */
    }
  }  
   
  
})();



