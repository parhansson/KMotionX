(function() {
  var module = angular.module('ThreeViewer', ['GCodeHandler']);

  module.factory('ViewerService', function(GCodeRenderer) {
    
    var viewData = {
        scene: null,
        camera: null,
        renderer: null,
        controls:null,
        object:null
    };
    
    return {
        view:viewData,
        renderGCode: function(gcode){
          if (viewData.object) {
            viewData.scene.remove(viewData.object);
          }
          GCodeRenderer.createObject(gcode).then(function (object) {
            //alert("I promised I would show up");
             viewData.object = object;
             viewData.scene.add(viewData.object);
          });
        }
    };
  }); 
  
  module.directive('threeViewer', function($timeout) {
    return {
      restrict: 'A',
      scope: {
        threeViewer: '='
      },
      link: function(scope, element, attrs) {
          
          var view = createViewer(element,$timeout);;
          element.append(view.renderer.domElement);
          scope.threeViewer.scene = view.scene;
      }
    };
  });
  
  function createViewer(element,$timeout) {
    var elem = $(element);
  //Copyright (c) 2012 Joe Walnes
    // Renderer
    var renderer = new THREE.WebGLRenderer({clearColor:0x000000, alpha: true});
    renderer.setSize(elem.width(), elem.height());
    renderer.clear();

    // Scene
    var scene = new THREE.Scene(); 

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

    // Camera...
    var fov    = 45,
        aspect = elem.width() / elem.height(),
        near   = 1,
        far    = 10000,
        camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
    //camera.rotationAutoUpdate = true;
    //camera.position.x = 0;
    //camera.position.y = 500;
    camera.position.z = 300;
    //camera.lookAt(scene.position);
    scene.add(camera);

    var viewer = {
        scene: scene,
        camera: camera,
        renderer: renderer,
        controls:null
        };
    
    // Action!
    function render() {
      viewer.controls.update();
      viewer.renderer.render(scene, camera);
      
      requestAnimationFrame(render); // And repeat...
    }
    
    function onResize(){
      var height = elem.height();
      var width = elem.width();
      renderer.setSize(width, height);
      
      camera.aspect = width / height;
      
      viewer.camera.updateProjectionMatrix();
      viewer.controls.screen.width = width; //window.innerWidth;
      viewer.controls.screen.height = height;// window.innerHeight;
      
    }
    // Fix coordinates up if window is resized.
    $(window).on('resize', onResize);

    //need to add controls after element has been created
    $timeout(function(){
      viewer.controls = new THREE.TrackballControls(camera, element[0] );
      //controls.noPan = false;
      viewer.controls.dynamicDampingFactor = 0.15;
  
      //start render loop
      render();
  
    }, 1);

    return viewer;
  }  
  
})();