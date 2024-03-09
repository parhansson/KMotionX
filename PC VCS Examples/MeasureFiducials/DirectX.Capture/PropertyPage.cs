// ------------------------------------------------------------------
// DirectX.Capture
//
// History:
//	2003-Jan-24		BL		- created
//
// Copyright (c) 2003 Brian Low
//
//  2007-july-01    HV      - added modifications
// - Added DSHOWNET conditional for using the older DShowNET library
//   instead of the DirectShowLib library
//
// Copyright (C) 2007 Hans Vosman
// ------------------------------------------------------------------

using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;
#if DSHOWNET
using DShowNET;
#else
using DirectShowLib;
#endif

namespace DirectX.Capture
{
	/// <summary>
	///  A base class for representing property pages exposed by filters. 
	/// </summary>
	public class PropertyPage : IDisposable
	{

		// ---------------- Properties --------------------

		/// <summary> Name of property page. This name may not be unique </summary>
		public string Name;

		/// <summary> Does this property page support saving and loading the user's choices. </summary>
		public bool SupportsPersisting = false;

		/// <summary> 
		///  Get or set the state of the property page. This is used to save
		///  and restore the user's choices without redisplaying the property page. 
		/// </summary>
		/// <remarks>
		///  After showing this property page, read and store the value of 
		///  this property. At a later time, the user's choices can be 
		///  reloaded by setting this property with the value stored earlier. 
		///  Note that some property pages, after setting this property, 
		///  will not reflect the new state. However, the filter will use the
		///  new settings. 
		///  
		/// <para>
		///  When reading this property, copy the entire array at once then manipulate
		///  your local copy (e..g byte[] myState = propertyPage.State). When
		///  setting this property set the entire array at once (e.g. propertyPage = myState).
		/// </para>
		///  
		/// <para>
		///  Not all property pages support saving/loading state. Check the 
		///  <see cref="SupportsPersisting"/> property to determine if this 
		///  property page supports it.
		/// </para>
		/// </remarks>
		public virtual byte[] State
		{
			get { throw new NotSupportedException( "This property page does not support persisting state." ); }
			set { throw new NotSupportedException( "This property page does not support persisting state." ); }
		}



		// ---------------- Constructors --------------------

		/// <summary> Constructor </summary>
		public PropertyPage()
		{
		}



		// ---------------- Public Methods --------------------

		/// <summary> 
		///  Show the property page. Some property pages cannot be displayed 
		///  while previewing and/or capturing. This method will block until
		///  the property page is closed by the user.
		/// </summary>
		public virtual void Show(Control owner)
		{
			throw new NotSupportedException( "Not implemented. Use a derived class. " );
		}

		/// <summary> Release unmanaged resources </summary>
		public void Dispose()
		{
		}

	}
}
