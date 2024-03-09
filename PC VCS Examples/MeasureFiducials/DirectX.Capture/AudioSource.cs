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
//   instead of the DirectShowLib library.
// - Catch possible excetion if something goes wrong when using the mixer
//
// Copyright (C) 2007 Hans Vosman
// ------------------------------------------------------------------

using System;
using System.Runtime.InteropServices; 
#if DSHOWNET
using DShowNET;
#else
using DirectShowLib;
#endif

namespace DirectX.Capture
{
	/// <summary>
	///  Represents a physical connector or source on an 
	///  audio device. This class is used on filters that
	///  support the IAMAudioInputMixer interface such as 
	///  source cards.
	/// </summary>
	public class AudioSource : Source
	{

		// --------------------- Private/Internal properties -------------------------

		internal IPin		Pin;			// audio mixer interface (COM object)



		// -------------------- Constructors/Destructors ----------------------

		/// <summary> Constructor. This class cannot be created directly. </summary>
		internal AudioSource( IPin pin )
		{
			if ( (pin as IAMAudioInputMixer) == null )
				throw new NotSupportedException( "The input pin does not support the IAMAudioInputMixer interface" );
			this.Pin = pin;
			this.name = getName( pin );
		}



		// ----------------------- Public properties -------------------------

		/// <summary> Enable or disable this source. For audio sources it is 
		/// usually possible to enable several sources. When setting Enabled=true,
		/// set Enabled=false on all other audio sources. </summary>
		public override bool Enabled
		{
			get 
			{
				IAMAudioInputMixer mix = (IAMAudioInputMixer) Pin;
				bool e;
//#if NEWCODE
				try
				{
					mix.get_Enable( out e );
					return e;
				}
				catch
				{
					return false;
				}

//#else
//				mix.get_Enable( out e );
//				return( e );
//#endif
			}

			set
			{
				IAMAudioInputMixer mix = (IAMAudioInputMixer) Pin;
//#if NEWCODE
				try
				{
					mix.put_Enable(value);
				}
				catch
				{
					// Do nothing?
				}
//#else
//				mix.put_Enable( value );
//#endif
			}

		}


				
		// --------------------------- Private methods ----------------------------

		/// <summary> Retrieve the friendly name of a connectorType. </summary>
		private string getName( IPin pin )
		{
			string s = "Unknown pin";
			PinInfo pinInfo = new PinInfo();

			// Direction matches, so add pin name to listbox
			int hr = pin.QueryPinInfo( out pinInfo);
			if ( hr == 0 )
			{ 
				s = pinInfo.name + "";
			}
			else
				Marshal.ThrowExceptionForHR( hr );

			// The pininfo structure contains a reference to an IBaseFilter,
			// so you must release its reference to prevent resource a leak.
			if ( pinInfo.filter != null )
				Marshal.ReleaseComObject( pinInfo.filter  );  pinInfo.filter  = null;

			return( s );
		}

		// -------------------- IDisposable -----------------------

		/// <summary> Release unmanaged resources. </summary>
		public override void Dispose()
		{
			if ( Pin != null )
				Marshal.ReleaseComObject( Pin );
			Pin = null;
			base.Dispose();
		}	
	}
}
