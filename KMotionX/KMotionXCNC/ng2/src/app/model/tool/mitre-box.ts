import { IGM, IgmObject } from '../igm'
import { GCodeVector } from '../vector'

interface MitreOptions {
  mitreTop: boolean
  mitreRight: boolean
  mitreBottom: boolean
  mitreLeft: boolean

}
export class MitreBox {

  private models: IgmObject[]

  private cut_width: number

  constructor(public width: number, public height: number, public depth: number, public materialThickness: number) {
  }

  generate() {
    /* Box dimensions (int 1 of a mm, and how many mitres to have, divided by two */
    let w = this.width
    let h = this.height
    let d = this.depth

    /* Thickness for the material (depth of the mitres) in 1 of a mm */
    let thick = this.materialThickness;

    /* How big the corner mitres are, in 1 of a mm 
      default is thickness of material times 4
    */
    let corner = thick * 4;


    /* how many mitres */
    let div_w = Math.floor(((w - 2 * corner) / thick) / 4)
    let div_h = Math.floor(((h - 2 * corner) / thick) / 4)
    let div_d = Math.floor(((d - 2 * corner) / thick) / 4)

    //let div_w = 6
    //let div_h = 3;
    //let div_d = 0;




    /* Frame around the sides panels */
    let frame = 5;
    this.models = [];
    /* Fudge Factor for kerf (width of cuts) - must be even as it is divided by two */
    this.cut_width = 0;
    /*******************************************************************/

    //Original code 
    //this.StartDoc(frame * 3 + w + d, frame * 3 + h + d);
    //this.MitrePanel(frame + w / 2, frame + h / 2, w, h, corner, div_w, div_h, thick, 0, 0);
    //this.MitrePanel(frame + w + frame + d / 2, frame + h / 2, d, h, corner, div_d, div_h, thick, 1, 0);
    //this.MitrePanel(frame + w / 2, frame + h + frame + d / 2, w, d, corner, div_w, div_d, thick, 1, 1);

    this.StartDoc(frame + w + d, frame + h + d);
    let lid = false;
    if (lid) {
      let options: MitreOptions = { mitreTop: true, mitreRight: true, mitreBottom: true, mitreLeft: true }
      this.MitrePanel(
        w / 2,
        h / 2,
        w, h, corner, div_w, div_h, thick, false, false, options);
      this.MitrePanel(
        w + frame + d / 2,
        h / 2,
        d, h, corner, div_d, div_h, thick, true, false, options);
      this.MitrePanel(
        w / 2,
        h + frame + d / 2,
        w, d, corner, div_w, div_d, thick, true, true, options);
    } else {

      let no_flat_edges: MitreOptions = { mitreTop: true, mitreRight: true, mitreBottom: true, mitreLeft: true }
      let flat_right: MitreOptions = { mitreTop: true, mitreRight: false, mitreBottom: true, mitreLeft: true }
      let flat_bottom: MitreOptions = { mitreTop: true, mitreRight: true, mitreBottom: false, mitreLeft: true }

      this.MitrePanel(
        w / 2,
        h / 2,
        w, h, corner, div_w, div_h, thick, false, false, no_flat_edges);
      this.MitrePanel(
        w + frame + d / 2,
        h / 2,
        d, h, corner, div_d, div_h, thick, true, false, flat_right);
      this.MitrePanel(
        w / 2,
        h + frame + d / 2,
        w, d, corner, div_w, div_d, thick, true, true, flat_bottom);
    }

    this.EndDoc();
    //console.log(this.svg);

    let igm = new IGM()
    for (let model of this.models) {
      igm.addToLayerObject('', model);
    }
    return igm;

  }

  private MitrePanel(x, y, w, h, corner_size, div_x, div_y, thick, invertX: boolean, invertY: boolean, options: MitreOptions) {
    let a, b, i, d, half_cut;
    x = x - w / 2 + (invertX ? thick : 0);
    y = y - h / 2 + (invertY ? thick : 0);
    this.PolyStart();
    /////////////////////////////////
    // Top side
    /////////////////////////////////
    this.PolyPoint(x, y);
    x += corner_size - (invertX ? thick : 0);
    half_cut = (invertY ? -this.cut_width / 2 : this.cut_width / 2);
    d = (invertY ? -1 : 1);
    this.PolyPoint(x + half_cut, y);
    y += thick * d; d = -d;
    this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;


    // All but the center one
    a = (w - 2 * corner_size) / (2 * div_x + 1);
    // the center one
    b = w - 2 * corner_size - a * (2 * div_x);

    for (i = 0; i < div_x; i++) {
      x += a;
      this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      this.PolyPoint(x + half_cut, y);
      half_cut = -half_cut;
    }


    x += b;
    this.PolyPoint(x + half_cut, y);
    y += thick * d; d = -d;
    this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;

    for (i = 0; i < div_x; i++) {
      x += a;
      this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      this.PolyPoint(x + half_cut, y);
      half_cut = -half_cut;
    }


    x += corner_size - (invertX ? thick : 0);
    this.PolyPoint(x, y);

    /////////////////////////////////
    // Right Side
    /////////////////////////////////
    half_cut = (invertX ? -this.cut_width / 2 : this.cut_width / 2);
    y += corner_size - (invertY ? thick : 0);

    d = (invertX ? 1 : -1);
    this.PolyPoint(x, y + half_cut);
    x += thick * d; d = -d;
    if (options.mitreRight) this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    // All but the center one
    a = (h - 2 * corner_size) / (2 * div_y + 1);
    // the center one
    b = h - 2 * corner_size - a * (2 * div_y);

    for (i = 0; i < div_y; i++) {
      y += a;
      if (options.mitreRight) this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      if (options.mitreRight) this.PolyPoint(x, y + half_cut);
      half_cut = -half_cut;
    }


    y += b;
    if (options.mitreRight) this.PolyPoint(x, y + half_cut);
    x += thick * d; d = -d;
    if (options.mitreRight) this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    for (i = 0; i < div_y; i++) {
      y += a;
      if (options.mitreRight) this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      if (options.mitreRight) this.PolyPoint(x, y + half_cut);
      half_cut = -half_cut;
    }

    y += corner_size - (invertY ? thick : 0);
    this.PolyPoint(x, y);

    /////////////////////////////////////////////////////
    // bottom Side
    /////////////////////////////////////////////////////
    half_cut = (invertY ? this.cut_width / 2 : -this.cut_width / 2);
    x -= corner_size - (invertX ? thick : 0);

    d = (invertY ? 1 : -1);
    this.PolyPoint(x + half_cut, y);
    y += thick * d; d = -d;
    if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;

    // All but the center one
    a = (w - 2 * corner_size) / (2 * div_x + 1);
    // the center one
    b = w - 2 * corner_size - a * (2 * div_x);

    for (i = 0; i < div_x; i++) {
      x -= a;
      if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
      half_cut = -half_cut;
    }


    x -= b;
    if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
    y += thick * d; d = -d;
    if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;

    for (i = 0; i < div_x; i++) {
      x -= a;
      if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      if (options.mitreBottom) this.PolyPoint(x + half_cut, y);
      half_cut = -half_cut;
    }

    x -= corner_size - (invertX ? thick : 0);
    this.PolyPoint(x, y);

    /////////////////////////////////////////////////////
    // Left side
    /////////////////////////////////////////////////////
    half_cut = (invertX ? this.cut_width / 2 : -this.cut_width / 2);
    y -= corner_size - (invertY ? thick : 0);

    d = (invertX ? -1 : 1);
    this.PolyPoint(x, y + half_cut);
    x += thick * d; d = -d;
    this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    // All but the center one
    a = (h - 2 * corner_size) / (2 * div_y + 1);
    // the center one
    b = h - 2 * corner_size - a * (2 * div_y);

    for (i = 0; i < div_y; i++) {
      y -= a;
      this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      this.PolyPoint(x, y + half_cut);
      half_cut = -half_cut;
    }


    y -= b;
    this.PolyPoint(x, y + half_cut);
    x += thick * d; d = -d;
    this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    for (i = 0; i < div_y; i++) {
      y -= a;
      this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      this.PolyPoint(x, y + half_cut);
      half_cut = -half_cut;
    }

    y -= corner_size - (invertY ? thick : 0);
    this.PolyPoint(x, y);
    this.PolyEnd();
  }

  protected PolyStart() {
    this.models.push(new IgmObject());
  }
  private getLast() {
    return this.models[this.models.length - 1]
  }
  protected PolyPoint(x, y) {
    this.getLast().vectors.push(new GCodeVector(x, y))
  }

  protected PolyEnd() {

  }

  protected StartDoc(w, h) {


  }

  protected EndDoc() {

  }


}

