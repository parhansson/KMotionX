declare var KMX : KMX.KMX;

// Collapse KMX into kmx
import kmx = KMX;

declare module 'KMX' {
    export = KMX;
}


declare module KMX {

	interface KMX {
		Util : UtilStatic;
		Transformers : ITransformersStatic;
	}
    interface ITransformersStatic{
		igm2gcode : Transfomer;
		svg2igm : Transfomer;
		pdf2svg : Transfomer;
		gcode2three : Transfomer;
	
	}
	
	interface Transfomer {
		transform(igm:IGM, settings:any): void;
	}	
		
	interface UtilStatic {
		ab2str(buf: ArrayBuffer): string;
		str2ab(str: string): ArrayBuffer;
	}
	
	 
    interface IGM{
        new();
        layers:any; // sort by stroke color
        layerKeys:string[]; // layerKey is a mapping to add unnamed layers, layer will get a generated name
        textLayer:any; // textpaths
        unsupported:any;  // Unsupported nodes
        
        
        addToLayerObject():void;
        //applyModifications():Vec2[];
        getMaxBounds():void;
    }
	
} 