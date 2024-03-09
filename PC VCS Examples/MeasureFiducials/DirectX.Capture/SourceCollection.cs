// ------------------------------------------------------------------
// DirectX.Capture
//
// History:
//	2003-Jan-24		BL		- created
//
// Copyright (c) 2003 Brian Low
// ------------------------------------------------------------------

using System;
using System.Diagnostics; 
using System.Collections;
using System.Runtime.InteropServices;
#if DSHOWNET
using DShowNET;
#else
using DirectShowLib;
#endif

namespace DirectX.Capture
{
	/// <summary>
	///  A collection of sources (or physical connectors) on an 
	///  audio or video device. This is used by the <see cref="Capture"/>
	///  class to provide a list of available sources on the currently
	///  selected audio and video devices. This class cannot be created
	///  directly.  This class assumes there is only 1 video and 1 audio
	///  crossbar and all input pins route to a single output pin on each 
	///  crossbar.
	/// </summary>
	public class SourceCollection : CollectionBase, IDisposable
	{

		// ------------------ Constructors/Destructors -----------------------

		/// <summary> Initialize collection with no sources. </summary>
		internal SourceCollection()
		{
			InnerList.Capacity = 1;
		}

		/// <summary> Initialize collection with sources from graph. </summary>
		internal SourceCollection(ICaptureGraphBuilder2 graphBuilder, IBaseFilter deviceFilter, bool isVideoDevice)
		{
			addFromGraph( graphBuilder, deviceFilter, isVideoDevice );
		}

		/// <summary> Destructor. Release unmanaged resources. </summary>
		~SourceCollection()
		{
			Dispose();
		}



		// -------------------- Public Properties -----------------------

		/// <summary> Get the source at the specified index. </summary>
		public Source this[int index]
		{
			get { return( (Source) InnerList[index] ); }
		}

		/// <summary>
		///  Gets or sets the source/physical connector currently in use.
		///  This is marked internal so that the Capture class can control
		///  how and when the source is changed.
		/// </summary>
		internal Source CurrentSource
		{
			get 
			{
				// Loop through each source and find the first
				// enabled source.
				foreach ( Source s in InnerList )
				{
					if ( s.Enabled )
						return( s );
				}
				
				return ( null );
				
			}
			set
			{
				if ( value == null )
				{
					// Disable all sources
					foreach ( Source s in InnerList )
						s.Enabled = false;
				}
				else if ( value is CrossbarSource )
				{
					// Enable this source
					// (this will automatically disable all other sources)
					value.Enabled = true;
				}
				else
				{
					// Disable all sources
					// Enable selected source
					foreach ( Source s in InnerList )
						s.Enabled = false;
					value.Enabled = true;
				}
			}
		}



		// -------------------- Public methods -----------------------

		/// <summary> Empty the collection. </summary>
		public new void Clear()
		{
			for ( int c = 0; c < InnerList.Count; c++ )
				this[c].Dispose();
			InnerList.Clear();
		}
		
		/// <summary> Release unmanaged resources. </summary>
		public void Dispose()
		{
			Clear();
			InnerList.Capacity = 1;
		}



		// -------------------- Protected Methods -----------------------

//#if NEWCODE
		/// <summary> Populate the collection from a filter graph. </summary>
		public void addFromGraph(ICaptureGraphBuilder2 graphBuilder, IBaseFilter deviceFilter, bool isVideoDevice)
//#else
//		protected void addFromGraph ( ICaptureGraphBuilder2 graphBuilder, IBaseFilter deviceFilter, bool isVideoDevice )
//#endif
		{
			Trace.Assert( graphBuilder != null );

			ArrayList crossbars = findCrossbars( graphBuilder, deviceFilter );
			foreach ( IAMCrossbar crossbar in crossbars )
			{
				ArrayList sources = findCrossbarSources( graphBuilder, crossbar, isVideoDevice );
				InnerList.AddRange( sources );
			}

			if ( !isVideoDevice )
			{
				if ( InnerList.Count == 0 )
				{
					ArrayList sources = findAudioSources( graphBuilder, deviceFilter );
					InnerList.AddRange( sources );

				}
			}
		}

		/// <summary>
		///	 Retrieve a list of crossbar filters in the graph.
		///  Most hardware devices should have a maximum of 2 crossbars, 
		///  one for video and another for audio.
		/// </summary>
		protected ArrayList findCrossbars(ICaptureGraphBuilder2 graphBuilder, IBaseFilter deviceFilter)
		{
			ArrayList crossbars = new ArrayList();

			Guid category = FindDirection.UpstreamOnly;
			Guid type = new Guid();
			Guid riid = typeof(IAMCrossbar).GUID;
			int hr;

			object comObj = null;
			object comObjNext = null;

			// Find the first interface, look upstream from the selected device
#if DSHOWNET
			hr = graphBuilder.FindInterface( ref category, ref type, deviceFilter, ref riid, out comObj );
#else
			hr = graphBuilder.FindInterface( category, type, deviceFilter, riid, out comObj );
#endif
			while ( (hr == 0) && (comObj != null) )
			{
				// If found, add to the list
				if ( comObj is IAMCrossbar )
				{
					crossbars.Add( comObj as IAMCrossbar );

					// Find the second interface, look upstream from the next found crossbar
#if DSHOWNET
					hr = graphBuilder.FindInterface( ref category, ref type, comObj as IBaseFilter, ref riid, out comObjNext );
#else
					hr = graphBuilder.FindInterface( category, type, comObj as IBaseFilter, riid, out comObjNext );
#endif
					comObj = comObjNext;
				}
				else
					comObj = null;
			}

			return( crossbars );
		}

		/// <summary>
		///  Populate the internal InnerList with sources/physical connectors
		///  found on the crossbars. Each instance of this class is limited
		///  to video only or audio only sources ( specified by the isVideoDevice
		///  parameter on the constructor) so we check each source before adding
		///  it to the list.
		/// </summary>
		protected ArrayList findCrossbarSources(ICaptureGraphBuilder2 graphBuilder, IAMCrossbar crossbar, bool isVideoDevice)
		{
			ArrayList sources = new ArrayList();
			int hr;
			int numOutPins;
			int numInPins;
			hr = crossbar.get_PinCounts( out numOutPins, out numInPins );
			if ( hr < 0 )
				Marshal.ThrowExceptionForHR( hr );

			// We loop through every combination of output and input pin
			// to see which combinations match.

			// Loop through output pins
			for ( int cOut = 0; cOut < numOutPins; cOut++ )
			{
				// Loop through input pins
				for ( int cIn = 0; cIn < numInPins; cIn++ )
				{
					// Can this combination be routed?
					hr = crossbar.CanRoute( cOut, cIn );
					if ( hr == 0 )
					{
						// Yes, this can be routed
						int relatedInputPin;
						PhysicalConnectorType connectorType;
						hr = crossbar.get_CrossbarPinInfo( true, cIn, out relatedInputPin, out connectorType );
						if ( hr < 0 )
							Marshal.ThrowExceptionForHR( hr );

						// Add it to the list
						CrossbarSource source = new CrossbarSource( crossbar, cOut, cIn, relatedInputPin, connectorType );
						sources.Add( source );
					}
				}
			}

			// Some silly drivers (*cough* Nvidia *cough*) add crossbars
			// with no real choices. Every input can only be routed to
			// one output. Loop through every Source and see if there
			// at least one other Source with the same output pin.
			int refIndex = 0;
			while ( refIndex < sources.Count )
			{
				bool found = false;
				CrossbarSource refSource = (CrossbarSource) sources[refIndex];
				for ( int c = 0; c < sources.Count; c++ )
				{
					CrossbarSource s = (CrossbarSource) sources[c];
					if ( ( refSource.OutputPin == s.OutputPin ) && ( refIndex != c ) )
					{
						found = true;
						break;
					}
				}
				if ( found )
					refIndex++;
				else
					sources.RemoveAt( refIndex );
			}

			// Some of the video input pins have related audio pins 
			// that should be connected at the same time. We noted the pin number
			// in the CrossbarSource.RelatedInputPin. Now that we have all
			// the sources, lookup the CrossbarSource object associated with
			// that pin
			foreach ( CrossbarSource source in sources )
			{
				if ( source.RelatedInputPin != -1 )
				{
					foreach( CrossbarSource related in sources )
					{
						if ( source.RelatedInputPin == related.InputPin )
							source.RelatedInputSource = related;
					}
				}
			}

			// Remove any sources that are not of the correct type
			for (int c=0; c<sources.Count; c++)
			{
				if ( ((CrossbarSource)sources[c]).ConnectorType < PhysicalConnectorType.Audio_Tuner )
				{
					if ( !isVideoDevice )
					{
						sources.RemoveAt( c );
						c--;
					}
				}
				else
				{
					if ( isVideoDevice )
					{
						sources.RemoveAt( c );
						c--;
					}
				}
			}
    	    return( sources );
		}

		protected ArrayList findAudioSources(ICaptureGraphBuilder2 graphBuilder, IBaseFilter deviceFilter)
		{
			ArrayList sources = new ArrayList();
			IAMAudioInputMixer audioInputMixer = deviceFilter as IAMAudioInputMixer;
			if ( audioInputMixer != null )
			{
				// Get a pin enumerator off the filter
				IEnumPins pinEnum;
				int hr = deviceFilter.EnumPins( out pinEnum );
				pinEnum.Reset();
				if( (hr == 0) && (pinEnum != null) )
				{
					// Loop through each pin
					IPin[] pins = new IPin[1];
#if VS2003 || DSHOWNET
					int f;
#else
                    IntPtr f = IntPtr.Zero;
#endif
					do
					{
						// Get the next pin
#if VS2003 || DSHOWNET
						hr = pinEnum.Next( 1, pins, out f );
#else
                        hr = pinEnum.Next(1, pins, f);
#endif
						if( (hr == 0) && (pins[0] != null) )
						{
							// Is this an input pin?
							PinDirection dir = PinDirection.Output;
							hr = pins[0].QueryDirection( out dir );
							if( (hr == 0) && (dir == (PinDirection.Input)) )
							{
								// Add the input pin to the sources list
								AudioSource source = new AudioSource( pins[0] ); 
								sources.Add( source );
							}
							pins[0] = null;
						}
					}
					while( hr == 0 );

					Marshal.ReleaseComObject( pinEnum ); pinEnum = null;
				}
			}

			// If there is only one source, don't return it
			// because there is nothing for the user to choose.
			// (Hopefully that single source is already enabled).
			if ( sources.Count == 1 )
				sources.Clear();

			return( sources );
		}

	}
}
