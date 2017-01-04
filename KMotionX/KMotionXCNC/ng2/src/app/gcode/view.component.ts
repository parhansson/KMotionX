
import { Component, ElementRef, HostListener } from '@angular/core';

@Component({
  selector: 'three-viewer',
  // template: `
  //     <div (window:resize)="onResize2($event)">
  //     </div>    
  //   `
  template: ''
})
export class ThreeViewComponent {
  private renderer: THREE.WebGLRenderer
  private camera: THREE.PerspectiveCamera
  private controls: THREE.TrackballControls
  private scene: THREE.Scene
  private element: HTMLElement
  private mouseDown = false;
  private ticking = false;
  private orientation: Orientation
  private modelDetector: RaycastDetector
  private machineDetector: RaycastDetector
  private modelGroup = new THREE.Group()
  public auxiliaryGroup = new THREE.Group()
  private currentModelObject: THREE.Object3D = null


  set model(model: THREE.Object3D) {
    if (this.currentModelObject !== null) {
      this.modelGroup.remove(this.currentModelObject)
    }
    this.currentModelObject = model
    this.modelGroup.add(this.currentModelObject)
    this.requestTick()
  }

  constructor(private elRef: ElementRef) {
    //schedule setsize and render after element has been created
    setTimeout(function () {
      this.onResize(null);
      //render 
      this.requestTick();
    }.bind(this), 0);

    this.element = elRef.nativeElement
    //Directive Exports
    //this.viewer.updateFn = this.requestTick.bind(this);
    // Renderer
    this.renderer = new THREE.WebGLRenderer({ /*canvas: this.element.firstChild,*/ antialias: true, clearColor: 0x000000, alpha: true });

    this.element.appendChild(this.renderer.domElement);
    this.renderer.clear();

    //Scene
    this.scene = new THREE.Scene();

    // Camera...
    var fov = 45,
      aspect = this.width() / this.height(), //this will return NaN right now
      near = 1,
      far = 10000;
    this.camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
    //camera.rotationAutoUpdate = true;
    this.camera.position.x = 150;
    this.camera.position.y = 100;
    this.camera.position.z = 500;
    var center = new THREE.Vector3(150, 100, 0)
    this.camera.lookAt(center);
    //camera.up = new THREE.Vector3( 0, 0, 0 );
    this.scene.add(this.camera);
    this.scene.add(this.auxiliaryGroup);
    this.scene.add(this.modelGroup);

    this.controls = new THREE.TrackballControls(this.camera, this.element);
    //controls.noPan = false;
    this.controls.minDistance = 10;
    this.controls.maxDistance = 10000;
    this.controls.dynamicDampingFactor = 0.15;
    this.controls.target0 = center;
    this.controls.reset();


    this.orientation = new Orientation(this.element, this.camera);



    //Logic
    var cursor = new THREE.Mesh(new THREE.SphereGeometry(1), new THREE.MeshBasicMaterial({ color: 0xff0000 }));
    this.scene.add(cursor);
    this.modelDetector = new RaycastDetector(cursor, this.camera, this.modelGroup);
    this.machineDetector = new RaycastDetector(cursor, this.camera, this.auxiliaryGroup);
    // Lights...
    [[0, 0, 1, 0xFFFFCC],
    [0, 1, 0, 0xFFCCFF],
    [1, 0, 0, 0xCCFFFF],
    [0, 0, -1, 0xCCCCFF],
    [0, -1, 0, 0xCCFFCC],
    [-1, 0, 0, 0xFFCCCC]].forEach(function (position) {
      var light = new THREE.DirectionalLight(position[3]);
      light.position.set(position[0], position[1], position[2]).normalize();
      this.scene.add(light);
    }.bind(this));



    window.addEventListener('resize', this.onResize.bind(this));
    // this.element.addEventListener('click', function(event) {
    //   if (cursor.visible === true) {
    //     console.log(cursor.position);
    //   }
    // });

    this.controls.addEventListener('start', this.onControlsEvent.bind(this));
    this.controls.addEventListener('change', this.onControlsEvent.bind(this));
    this.controls.addEventListener('end', this.onControlsEvent.bind(this));

    /*
element.on( 'mouseenter', function(){
  //requestTick();
});
element.on( 'mouseleave', function(){
  //ticking = false;
});      
*/

  }

  @HostListener('mouseup', ['false'])
  @HostListener('mousedown', ['true'])
  onMouseButton(mouseDown: boolean) {
    this.mouseDown = mouseDown;
  }

  @HostListener('mousemove', ['$event'])
  onMouseMove(event) {
    event.preventDefault();
    var mouseVector = this.getMouseVector(event);
    var intersectionChanged = this.modelDetector.detect(mouseVector) || this.machineDetector.detect(mouseVector);

    if (intersectionChanged) {
      //object detected
      this.requestTick();
    }
    if (this.mouseDown) {
      //orbiting view with mouse or touch pad
      this.requestTick();
    }
  }

  requestTick() {
    //debounce multiple requests
    if (!this.ticking) {
      requestAnimationFrame(this.animate.bind(this));
    }
    this.ticking = true;
  }


  onControlsEvent(event: THREE.Event) {
    this.requestTick();
  }

  //Due to bug? in three scene does not dispatch events.
  //It is the added object that dispatches the even. TODO Bug report.
  //scene.addEventListener('added', onSceneEvent);
  //scene.addEventListener('removed', onSceneEvent);
  //function onSceneEvent(event){
  //  requestTick();
  //}

  width() {
    return this.element.offsetWidth;
  }
  height() {
    return this.element.offsetHeight;
  }

  animate() {
    // reset the tick so we can capture the next event
    this.ticking = false;
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
    this.orientation.render(this.controls);
    //console.log('animate');
  }

  onResize(event) {
    var height = this.height();//elem.height();
    var width = this.width();
    this.renderer.setSize(width, height);
    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();
    this.controls.screen.width = width; //window.innerWidth;
    this.controls.screen.height = height;// window.innerHeight;
    //this.requestTick()
  }

  getMouseVector(event) {
    let height = this.height();
    let width = this.width();
    // calculate mouse position in normalized device coordinates
    // (-1 to +1) for both components
    let elementXPosition = (event.offsetX != null) ? event.offsetX : event.originalEvent.layerX;
    let elementYPosition = (event.offsetY != null) ? event.offsetY : event.originalEvent.layerY;
    let mouse = new THREE.Vector2();
    mouse.x = (elementXPosition / width) * 2 - 1;
    mouse.y = - (elementYPosition / height) * 2 + 1;
    //console.info('mouse.x mouse.y %',mouse.x ,mouse.y);
    //console.info('mouse.x mouse.y',elementXPosition ,elementYPosition);
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

// Find mouse intersection objects!
export class RaycastDetector {
  currentObject: any //THREE.Object3D;
  previousPoint = new THREE.Vector3(0, 0, 0);
  raycaster = new THREE.Raycaster();


  constructor(private markerObject: THREE.Mesh, private camera: THREE.Camera, private parentObject: THREE.Group) {
    this.raycaster.linePrecision = 0.5;
    markerObject.visible = false;
  }
  detect(mouseVector) {
    //save previous intersected object
    var prevIntersect = this.currentObject;
    this.previousPoint.copy(this.markerObject.position);

    //reset linewidth
    if (this.currentObject !== undefined) {
      this.currentObject.material.linewidth = 1;
    }

    // find intersections
    this.raycaster.setFromCamera(mouseVector, this.camera);
    var intersects = this.raycaster.intersectObjects(this.parentObject.children, true);

    if (intersects.length > 0) {
      this.markerObject.visible = true;

      this.currentObject = intersects[0].object;
      this.currentObject.material.linewidth = 5;

      //ensure intersected object is not marker sphere
      if (this.markerObject !== this.currentObject) {
        if (this.currentObject.userData.lineNo !== undefined) {
          console.log(this.currentObject.userData);
        }
        //console.info(intersects[ 0 ].point);
        this.markerObject.position.copy(intersects[0].point);
      }

    } else {
      this.markerObject.visible = false;
      this.currentObject = undefined;
    }

    if (prevIntersect !== this.currentObject) {
      //intersected object changed
      return true;
    }
    if (!this.previousPoint.equals(this.markerObject.position)) {
      //intersected position changed
      return true;
    }
    return false;
  }
}

export class Orientation {
  CANVAS_WIDTH = 400
  CANVAS_HEIGHT = 400
  CAM_DISTANCE = 200
  renderer: any
  scene: any
  camera: any

  constructor(private element, private syncCamera: THREE.Camera) {

    this.renderer = new THREE.WebGLRenderer({ antialias: false, clearColor: 0x000000, alpha: true });
    this.scene = new THREE.Scene();
    // camera
    this.camera = new THREE.PerspectiveCamera(50, this.CANVAS_WIDTH / this.CANVAS_HEIGHT, 1, 1000);
    this.camera.up = syncCamera.up; // important!

    // renderer
    this.renderer.setSize(this.CANVAS_WIDTH, this.CANVAS_HEIGHT);
    this.renderer.domElement.style.cssText =
      'width: 150px;height: 150px;' +
      //'background-color: transparent;'+ 
      'margin: 10px;' +
      'padding: 0px;' +
      'position: absolute;' +
      'right: 0px;' +
      'top: -20px;' +
      'z-index: 100;';
    //'border: 1px solid black; /* or none; */';
    element.appendChild(this.renderer.domElement);
    this.createCube(this.scene);
  }
  render(controls) {
    this.camera.position.copy(this.syncCamera.position);
    this.camera.position.sub(controls.target);
    this.camera.position.setLength(this.CAM_DISTANCE);
    this.camera.lookAt(this.scene.position);
    //console.log(this.camera.position)
    this.renderer.render(this.scene, this.camera);
  }

  createCube(scene) { // create an array with six textures for a cool cube
    new THREE.TextureLoader().load('/settings/textures/textures.png', onTexture);


    function onTexture(texture: THREE.Texture) {
      //var material = new THREE.MeshPhongMaterial( { map: THREE.ImageUtils.loadTexture('images/box-atlas.png') } );
      var material = new THREE.MeshBasicMaterial({
        map: texture
      });

      var size = 50;
      var geometry = new THREE.CubeGeometry(size, size, size, 1, 1, 1);
      geometry.faceVertexUvs[0] = [];
      var textPercent = 0.125;
      var LU = -textPercent;
      var RU = 0;
      var i = 0;
      for (var t = 0; t < 6; t++) {
        LU += textPercent; RU += textPercent;
        var pos = [new THREE.Vector2(LU, 1), new THREE.Vector2(LU, 0), new THREE.Vector2(RU, 0), new THREE.Vector2(RU, 1)];
        LU += textPercent; RU += textPercent;
        var neg = [new THREE.Vector2(LU, 1), new THREE.Vector2(LU, 0), new THREE.Vector2(RU, 0), new THREE.Vector2(RU, 1)];
        geometry.faceVertexUvs[0][i++] = [pos[0], pos[1], pos[3]];
        geometry.faceVertexUvs[0][i++] = [pos[1], pos[2], pos[3]];
        geometry.faceVertexUvs[0][i++] = [neg[0], neg[1], neg[3]];
        geometry.faceVertexUvs[0][i++] = [neg[1], neg[2], neg[3]];
      }
      var mesh = new THREE.Mesh(geometry, material);
      console.log(mesh.toJSON());
      scene.add(mesh);

    }

    // add axes
    var radius = 80;
    scene.add(new THREE.AxisHelper(radius));

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






