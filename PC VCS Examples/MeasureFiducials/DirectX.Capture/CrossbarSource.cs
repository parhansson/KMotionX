// ------------------------------------------------------------------
// DirectX.Capture
//
// History:
//	2003-Jan-24		BL		- created
//
// Copyright (c) 2003 Brian Low
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
	///  audio/video device. This class is used on filters that
	///  support the IAMCrossbar interface such as TV Tuners.
	/// </summary>
	public class CrossbarSource : Source
	{
		// --------------------- Private/Internal properties -------------------------

		internal IAMCrossbar			Crossbar;				// crossbar filter (COM object)
		internal int					OutputPin;				// output pin number on the crossbar
		internal int					InputPin;				// input pin number on the crossbar
		internal int					RelatedInputPin = -1;	// usually the audio input pin for the same source
		internal CrossbarSource			RelatedInputSource;		// the Crossbar source associated with the RelatedInputPin
		internal PhysicalConnectorType	ConnectorType;			// type of the connector



		// ----------------------- Public properties -------------------------

		/// <summary> Enabled or disable this source. </summary>
		public override bool Enabled
		{
			get 
			{
				int i;
				if ( Crossbar.get_IsRoutedTo( OutputPin, out i ) == 0 )
					if ( InputPin == i )
						return( true );
				return( false );
			}

			set
			{
				if ( value )
				{
					// Enable this route
					int hr = this.Crossbar.Route( this.OutputPin, this.InputPin );
					if ( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

					// Enable the related pin as well
					if ( RelatedInputSource != null )
					{
						hr = this.Crossbar.Route( RelatedInputSource.OutputPin, RelatedInputSource.InputPin );  
						if ( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

					}
				}
				else
				{
					// Disable this route by routing the output
					// pin to input pin -1
					int hr = this.Crossbar.Route( this.OutputPin, -1 );
					if ( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

					// Disable the related pin as well
					if ( RelatedInputSource != null )
					{
						hr = this.Crossbar.Route( RelatedInputSource.OutputPin, -1 );  
						if ( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

					}
				}
			}
		}


		
		// -------------------- Constructors/Destructors ----------------------

		/// <summary> Constructor. This class cannot be created directly. </summary>
		internal CrossbarSource( IAMCrossbar crossbar, int outputPin, int inputPin, PhysicalConnectorType connectorType )
		{
			this.Crossbar = crossbar;
			this.OutputPin = outputPin;
			this.InputPin = inputPin;
			this.ConnectorType = connectorType;
			this.name = getName( connectorType );
		}

		/// <summary> Constructor. This class cannot be created directly. </summary>
		internal CrossbarSource( IAMCrossbar crossbar, int outputPin, int inputPin, int relatedInputPin, PhysicalConnectorType connectorType )
		{
			this.Crossbar = crossbar;
			this.OutputPin = outputPin;
			this.InputPin = inputPin;
			this.RelatedInputPin = relatedInputPin; 
			this.ConnectorType = connectorType;
			this.name = getName( connectorType );
		}

		// --------------------------- Private methods ----------------------------

		/// <summary> Retrieve the friendly name of a connectorType. </summary>
		private string getName( PhysicalConnectorType connectorType )
		{
			string name;
			switch( connectorType )
			{
				case PhysicalConnectorType.Video_Tuner:				name = "Video Tuner";			break;
				case PhysicalConnectorType.Video_Composite:			name = "Video Composite";		break;
				case PhysicalConnectorType.Video_SVideo:			name = "Video S-Video";			break;
				case PhysicalConnectorType.Video_RGB:				name = "Video RGB";				break;
				case PhysicalConnectorType.Video_YRYBY:				name = "Video YRYBY";			break;
				case PhysicalConnectorType.Video_SerialDigital:		name = "Video Serial Digital";	break;
				case PhysicalConnectorType.Video_ParallelDigital:	name = "Video Parallel Digital";break;
				case PhysicalConnectorType.Video_SCSI:				name = "Video SCSI";			break;
				case PhysicalConnectorType.Video_AUX:				name = "Video AUX";				break;
				case PhysicalConnectorType.Video_1394:				name = "Video Firewire";		break;
				case PhysicalConnectorType.Video_USB:				name = "Video USB";				break;
				case PhysicalConnectorType.Video_VideoDecoder:		name = "Video Decoder";			break;
				case PhysicalConnectorType.Video_VideoEncoder:		name = "Video Encoder";			break;
				case PhysicalConnectorType.Video_SCART:				name = "Video SCART";			break;

				case PhysicalConnectorType.Audio_Tuner:				name = "Audio Tuner";			break;
				case PhysicalConnectorType.Audio_Line:				name = "Audio Line In";			break;
				case PhysicalConnectorType.Audio_Mic:				name = "Audio Mic";				break;
				case PhysicalConnectorType.Audio_AESDigital:		name = "Audio AES Digital";		break;
				case PhysicalConnectorType.Audio_SPDIFDigital:		name = "Audio SPDIF Digital";	break;
				case PhysicalConnectorType.Audio_SCSI:				name = "Audio SCSI";			break;
				case PhysicalConnectorType.Audio_AUX:				name = "Audio AUX";				break;
				case PhysicalConnectorType.Audio_1394:				name = "Audio Firewire";		break;
				case PhysicalConnectorType.Audio_USB:				name = "Audio USB";				break;
				case PhysicalConnectorType.Audio_AudioDecoder:		name = "Audio Decoder";			break;

				default:											name = "Unknown Connector";		break;
			}
			return( name );
		}


		
		// -------------------- IDisposable -----------------------

		/// <summary> Release unmanaged resources. </summary>
		public override void Dispose()
		{
			if ( Crossbar != null )
				Marshal.ReleaseComObject( Crossbar );
			Crossbar = null;
			RelatedInputSource = null;
			base.Dispose();
		}	
	}
}
