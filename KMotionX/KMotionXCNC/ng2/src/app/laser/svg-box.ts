
/*********************************************************
 * 
 * Generate an SVG file for a box.
 *
 * Original Author Mike Field <hamster@snap.net.nz>
 * http://hamsterworks.co.nz/mediawiki/index.php/Svg_box
 *
 ********************************************************/

export class SVGBox {

  private svg: string
  private cut_width: number
  private polyFirst: boolean

  constructor(width: number, height: number, depth: number, thickness: number) {
    /* Box dimensions (int 1/10ths of a mm, and how many mitres to have, divided by two */
    let w = width * 10
    let div_w = 6
    let h = height * 10
    let div_h = 3;
    let d = depth * 10
    let div_d = 0;

    /* How big the corner mitres are, in 1/10ths of a mm */
    let corner = 100;
    /* Thickness for the material (depth of the mitres) in 1/10ths of a mm */
    let thick = thickness * 10;

    /* Frame around the sides panels */
    let frame = 50;
    this.svg = '';
    /* Fudge Factor for kerf (width of cuts) - must be even as it is divided by two */
    this.cut_width = 2;
    /*******************************************************************/
    /* For getting the correct space characters in the polyline object */
    this.polyFirst;
    /*******************************************************************/
    this.StartDoc(frame * 3 + w + d, frame * 3 + h + d);


    this.MitrePanel(frame + w / 2, frame + h / 2, w, h, corner, div_w, div_h, thick, 0, 0);
    this.MitrePanel(frame + w + frame + d / 2, frame + h / 2, d, h, corner, div_d, div_h, thick, 1, 0);
    this.MitrePanel(frame + w / 2, frame + h + frame + d / 2, w, d, corner, div_w, div_d, thick, 1, 1);

    this.EndDoc();
    //console.log(this.svg);

    window.URL = window.URL;// || window.webkitURL;

    var blob = new Blob([this.svg], { type: 'image/svg+xml' });

    window.open(window.URL.createObjectURL(blob));

  }

  MitrePanel(x, y, w, h, corner_size, div_x, div_y, thick, invertX, invertY) {
    var a, b, i, d, half_cut;
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
    this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    // All but the center one
    a = (h - 2 * corner_size) / (2 * div_y + 1);
    // the center one
    b = h - 2 * corner_size - a * (2 * div_y);

    for (i = 0; i < div_y; i++) {
      y += a;
      this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      this.PolyPoint(x, y + half_cut);
      half_cut = -half_cut;
    }


    y += b;
    this.PolyPoint(x, y + half_cut);
    x += thick * d; d = -d;
    this.PolyPoint(x, y + half_cut);
    half_cut = -half_cut;

    for (i = 0; i < div_y; i++) {
      y += a;
      this.PolyPoint(x, y + half_cut);
      x += thick * d; d = -d;
      this.PolyPoint(x, y + half_cut);
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
    this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;

    // All but the center one
    a = (w - 2 * corner_size) / (2 * div_x + 1);
    // the center one
    b = w - 2 * corner_size - a * (2 * div_x);

    for (i = 0; i < div_x; i++) {
      x -= a;
      this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      this.PolyPoint(x + half_cut, y);
      half_cut = -half_cut;
    }


    x -= b;
    this.PolyPoint(x + half_cut, y);
    y += thick * d; d = -d;
    this.PolyPoint(x + half_cut, y);
    half_cut = -half_cut;

    for (i = 0; i < div_x; i++) {
      x -= a;
      this.PolyPoint(x + half_cut, y);
      y += thick * d; d = -d;
      this.PolyPoint(x + half_cut, y);
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

  PolyStart() {
    this.svg += ('<polyline points="');
    this.polyFirst = true;
  }

  PolyPoint(x, y) {
    if (!this.polyFirst)
      this.svg += ' ';
    this.polyFirst = false;
    this.svg += x;// ("%i,%i",x,y);
    this.svg += ',';
    this.svg += y;
  }

  PolyEnd() {
    this.svg += ('" fill="none" stroke="black" stroke-width="3" />\r\n');
  }
  StartDoc(w, h) {
    var res = 100;
    //w = (w+(99))/100*100;
    //w = (w+(res-1))/res*res;
    //h = (h+(res-1))/res*res;

    this.svg += '<?xml version="1.0" standalone="no"?>\r\n';
    this.svg += '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\r\n';
    //this.svg += ("<svg width=\"%icm\" height=\"%icm\" viewBox=\"0 0 %i %i\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\r\n", w/100, h/100, w,h);
    //this.svg += "<svg width=\"" + w/100 + "cm\" height=\"" + h/100 + "cm\" viewBox=\"0 0 "+ w +" "+ h +"\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\r\n";
    this.svg += '<svg width="' + w / res + 'cm" height="' + h / res + 'cm" viewBox="0 0 ' + w + ' ' + h + '" xmlns="http://www.w3.org/2000/svg" version="1.1">\r\n';

  }

  EndDoc() {
    this.svg += ('</svg>\r\n');
  }


}

