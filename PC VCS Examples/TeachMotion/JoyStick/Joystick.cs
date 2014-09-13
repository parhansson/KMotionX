/*
 * name:	Sergey Arbuzov
 * date:	May 11, 2005
 * file:	Joystick.cs
 * version:	1
 * abstract:	this is the joystick control
 * 
 * */

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;


public delegate void Position(); 

public enum Compass
{
	none,n,ne,e,se,s,sw,w,nw
}

public class Joystick : System.Windows.Forms.Control
{
    private event Position doIt;	// gets called when joystick is moved
	private System.ComponentModel.Container components = null;
	private Point offset;
	private double diameter;
	private Pen rubberBand;
	private Compass compassPoint;
	private Point cursorLocation;
	private SolidBrush insideColor;
	private int radius;
	private double magnitude;
	private bool mouseDown;
	private bool mouseLock;
    private int tktk = 0;
	private bool FirstTime=true;

	public Joystick()
	{
		offset = new Point(0,0);	// location of the rubberBand from the center
		// This call is required by the Windows.Forms Form Designer.
		InitializeComponent();

		rubberBand = new Pen(Color.Red, 3);
		// width and color of the rubber band

		// double buffering on, to remove flicker
		this.SetStyle(ControlStyles.DoubleBuffer,true);
		this.SetStyle(ControlStyles.AllPaintingInWmPaint,true);
		this.SetStyle(ControlStyles.UserPaint,true);

		insideColor = new SolidBrush(Color.Thistle);
		mouseDown = false;
		mouseLock = false;
    }

	#region generated stuff
	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	protected override void Dispose( bool disposing )
	{
		if( disposing )
		{
			if( components != null )
				components.Dispose();
		}
		base.Dispose( disposing );
	}

	#region Component Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify 
	/// the contents of this method with the code editor.
	/// </summary>
	private void InitializeComponent()
	{
		// 
		// Joystick
		// 
		this.Cursor = System.Windows.Forms.Cursors.Cross;
		this.SizeChanged += new System.EventHandler(this.Joystick_SizeChanged);
		this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Joystick_MouseUp);
		this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Joystick_MouseMove);
		this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Joystick_MouseDown);
    }
	#endregion

	#endregion

	protected override void OnPaint(PaintEventArgs pe)
	{
		// Calling the base class OnPaint
		base.OnPaint(pe);
		//GraphicsState oldState = pe.Graphics.Save();
		//pe.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
		// this prevents jagged edges along the rubber band

		CompassMagnitude();

		Graphics g = pe.Graphics;
		GraphicsPath serge = new GraphicsPath();
		serge.AddEllipse(this.ClientRectangle);
		this.Region = new Region(serge);
		pe.Graphics.FillEllipse(insideColor,
			new Rectangle(0,0,this.Height, this.Width));
		pe.Graphics.DrawLine(rubberBand, offset.X, offset.Y,
			(float)this.Width/2, (float)this.Width/2);
		pe.Graphics.DrawEllipse(Pens.Red,this.Width/3,this.Height/3,this.Width/3,this.Height/3);
		pe.Graphics.DrawEllipse(Pens.Red,this.Width/6,this.Height/6,this.Width-this.Width/3,this.Height-this.Height/3);

		//pe.Graphics.Restore(oldState);
	}

	#region methods
	public void Subscribe(Position p)
	{
		doIt += p;
	}

	private void CompassMagnitude()
	{
        if (FirstTime)
        {
            FirstTime = false;
            offset.X = this.Width / 2;
            offset.Y = this.Height / 2;
        }
        
        // get the compass direction

		compassPoint = (offset.X > this.Width/2-this.Width/15 && offset.X < this.Width/2+this.Width/15 && offset.Y > this.Height/2)?
			Compass.s :(offset.Y == this.Width/2 && offset.X > this.Width/2)?//e
			Compass.e : (offset.Y == this.Height/2 && offset.X < this.Width/2)?//w
			Compass.w : (offset.X == this.Width/2 && offset.Y < this.Height/2)?//n
			Compass.n : (offset.X > this.Width/2 && offset.Y < this.Height/2)?//ne
			Compass.ne : (offset.X < this.Width/2 && offset.Y < this.Height/2)?//nw
			Compass.nw : (offset.X > this.Width/2 && offset.Y > this.Height/2)?//sw
			Compass.se : (offset.X < this.Width/2 && offset.Y > this.Height/2)?//se
			Compass.sw : Compass.none;

        cursorLocation.X =   ((int)((offset.X / (double)Width)  * 32.0 + 0.5)) - 16;
        cursorLocation.Y = -(((int)((offset.Y / (double)Height) * 32.0 + 0.5)) - 16);

		magnitude = DistanceFromCenter();
	}

	private double DistanceFromCenter()
	{
		return Math.Sqrt(Math.Pow(cursorLocation.X-0,2) + Math.Pow(cursorLocation.Y-0,2));
	}

	private void Joystick_SizeChanged(object sender, System.EventArgs e) 
	{
		this.Height = this.Width;
		this.Width = this.Height;
		diameter = this.Width;
		this.Refresh();
	}
	#endregion

	#region mouse events
	private void Joystick_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		if(mouseDown) 
		{
            // don't understand this but invalidating every move cause some type of
            // infinite loop and uses lots of CPU
            if (++tktk > 3) 
            {
                tktk = 0;
                offset.X = e.X;
                offset.Y = e.Y;
                //			this.Refresh();
                Invalidate();
                this.Invalidate(true);
                if (doIt != null)
                    doIt();
            }
		}
		else if( !mouseLock)
		{
            if (++tktk > 3)
            {
                tktk = 0;
                offset.X = radius;
                offset.Y = radius;
                Invalidate(true);
                //			this.Refresh();
                if (doIt != null)
                    doIt();
            }
		}
	}

	private void Joystick_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		if(e.Button == MouseButtons.Left)
		{
			mouseDown = true;
			mouseLock = false;
			offset.X = e.X;
			offset.Y = e.Y;
			this.Refresh();
			if(doIt != null)
				doIt();
		}
		else if(e.Button == MouseButtons.Right)
			mouseLock = true;
	}

	private void Joystick_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
	{
		mouseDown = false;
		if(!mouseLock) 
		{
            offset.X = this.Width / 2;
            offset.Y = this.Height / 2;
            this.Refresh();
			if(doIt != null)
				doIt();
		}
	}
	#endregion

	#region properties
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("diameter of the circle")
	]
	public double Diameter
	{
		get
		{ return diameter; }
		set
		{ 
			diameter = value;
			this.Width = (int)diameter;
			this.Height = (int)diameter;
			radius = Convert.ToInt32(diameter/2);
			this.Refresh();
		}
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("color of the rubber band")
	]
	public Color BandColor
	{
		get
		{ return rubberBand.Color; }
		set
		{
			rubberBand.Color = value; 
			this.Refresh();
		}
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("color of the rubber band")
	]
	public float BandWidth
	{
		get
		{ return rubberBand.Width; }
		set
		{
			rubberBand.Width = value; 
			this.Refresh();
		}
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("direction of the joystick")
	]
	public Compass CompassPoint
	{
		get
		{ return compassPoint; }
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("magnitude from the center")
	]
	public Point CursorLocation
	{
		get
		{       
            CompassMagnitude();
            return cursorLocation; 
        }
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("color of the joystick")
	]
	public Color InsideColor
	{
		get
		{ return insideColor.Color; }
		set
		{
			insideColor.Color = value;
			this.Refresh();
		}
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("magnitude of the crosshair from the center")
	]
	public double Magnitude
	{
		get
		{ return magnitude; }
	}
	[
	BrowsableAttribute(true),
	CategoryAttribute("Joystick"),
	DescriptionAttribute("end cap of the rubber band")
	]
	public LineCap EndCap
	{
		get
		{ return rubberBand.EndCap; }
		set
		{
			rubberBand.EndCap = value;
			this.Refresh(); 
		}
	}
	#endregion
}
