// ------------------------------------------------------------------
// DirectX.Capture
//
// History:
//	2003-Jan-24		BL		- created
//
// Future Improvements
// 	- Notify unusual events (disk full, quick logoff) using IMediaEventEx (see AMCap line 1624)
// 	- Capture stats: IAMDroppedFrames, IQualProp.get_AvgFrameRate
// 	- Show preview while cued: ICaptureGraphBuilder2.ControlStream lets you stop capture but continue preview
// 	- Pre-alloc space with ICaptureGraphBuilder2.AllocCapFile(), copy data using ICaptureGraphBuilder2.CopyCaptureFile()
// 	- Audio preview (why doesn't VirtualDub do audio preview?)
// 	- Hardware Deinterlacing: "Setting Deinterlace Preferences", or IVMRDeinterlaceControl9
// 	- Time limit (and TimeRemaining) -> AmCap manually handles the time limit
//  - Don't destory filter graph when changing compressors, just remove compressor and add new one
//  - Change VideoFormat (NTSC/PAL), I think you need to use IAMVideoDecoder (WDM only)
//
// Copyright (c) 2003 Brian Low
//
//	2006-Mar-15		HV		- modified
//
// New:
//  - audio file saving in Windows Media Audio (Wma) format
//  - audio rendering for TV-cards which have no wired audio
//    connection, but have an audio driver
//
//  2006-Apr-1      HV      - modified
// New:
//  - initialize audio source when there is audio but no selected source
//  - restore audio when audio gets lost for what ever reason
//  - video file saving in Windows Media Video (Wmv) format
//
//  2007-Feb-1      HV       - modified
// New:
//  - audio rendering for TV-cards which have no wired audio
//    audio connection and have no audio driver.
//  - TV frequency fine tuning
//
//  2007-Jun-1      HV       - modified
// New:
//  - improved audio rendering for TV-cards having no wired audio
//  - SampleGrabber functionality added using HeFrame event
//  - Added VMR9 option
//  - Added De-Interlace option (using Alpary Deinterlace Filter)
//  - DirectShowLib/DShowNET compatibility added (via conditional DSHOWNET)
//  - Visual Studio 2003/2005 compatibility added (via conditional VS2003)
//  - Improved exception handling upon loss of Audio or Video sources and
//    property page information
//
//   2009-Feb-27    HV       - modified
// New:
//  - Add color space (+ quick scan of available color spaces) selection
//    This color space implementation is partly based on Brian Low's
//    DirectX.Capture example.
//  - Add video standard selection (eg PAL_B)
//  - Use profile for Windows Media audio/video formats, profile should
//    be in the program directory.
//  - Add TV tuner initialization window to force user to provide input
//    on country dependent settings (without proper settings code may fail)
//  - Removed FM Radio, TV finetuning code to make code example easier to
//    for SampleGrabber.
//
//  2009-Mar-31    HV       - modified
// Fixed:
//  - menu Audio Compressor initialized if audio via video device
//
// Copyright (C) 2006, 2007, 2008, 2009 Hans Vosman
// ------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices; 
using System.Threading;
using System.Windows.Forms;

#if DSHOWNET
using DShowNET;
#else
using DirectShowLib;
#endif

																   

namespace DirectX.Capture
{

	/// <summary>
	///  Use the Capture class to capture audio and video to AVI files.
	/// </summary>
	/// <remarks>
	///  This is the core class of the Capture Class Library. The following 
	///  sections introduce the Capture class and how to use this library.
	///  
	/// <br/><br/>
	/// <para><b>Basic Usage</b></para>
	/// 
	/// <para>
	///  The Capture class only requires a video device and/or audio device
	///  to begin capturing. The <see cref="Filters"/> class provides
	///  lists of the installed video and audio devices. 
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  // Remember to add a reference to DirectX.Capture.dll
	///  using DirectX.Capture
	///  ...
	///  Capture capture = new Capture( Filters.VideoInputDevices[0], 
	///                                 Filters.AudioInputDevices[0] );
	///  capture.Start();
	///  ...
	///  capture.Stop();
	/// </div></code>
	/// <para>
	///  This will capture video and audio using the first video and audio devices
	///  installed on the system. To capture video only, pass a null as the second
	///  parameter of the constructor.
	/// </para>
	/// <para> 
	///  The class is initialized to a valid temporary file in the Windows temp
	///  folder. To capture to a different file, set the 
	///  <see cref="Capture.Filename"/> property before you begin
	///  capturing. Remember to add DirectX.Capture.dll to 
	///  your project references.
	/// </para>
	///
	/// <br/>
	/// <para><b>Setting Common Properties</b></para>
	/// 
	/// <para>
	///  The example below shows how to change video and audio settings. 
	///  Properties such as <see cref="Capture.FrameRate"/> and 
	///  <see cref="AudioSampleSize"/> allow you to programmatically adjust
	///  the capture. Use <see cref="Capture.VideoCaps"/> and 
	///  <see cref="Capture.AudioCaps"/> to determine valid values for these
	///  properties.
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  Capture capture = new Capture( Filters.VideoInputDevices[0], 
	///                                 Filters.AudioInputDevices[1] );
	///  capture.VideoCompressor = Filters.VideoCompressors[0];
	///  capture.AudioCompressor = Filters.AudioCompressors[0];
	///  capture.FrameRate = 29.997;
	///  capture.FrameSize = new Size( 640, 480 );
	///  capture.AudioSamplingRate = 44100;
	///  capture.AudioSampleSize = 16;
	///  capture.Filename = "C:\MyVideo.avi";
	///  capture.Start();
	///  ...
	///  capture.Stop();
	/// </div></code>
	/// <para>
	///  The example above also shows the use of video and audio compressors. In most 
	///  cases you will want to use compressors. Uncompressed video can easily
	///  consume over a 1GB of disk space per minute. Whenever possible, set 
	///  the <see cref="Capture.VideoCompressor"/> and <see cref="Capture.AudioCompressor"/>
	///  properties as early as possible. Changing them requires the internal filter
	///  graph to be rebuilt which often causes most of the other properties to
	///  be reset to default values.
	/// </para>
	///
	/// <br/>
	/// <para><b>Listing Devices</b></para>
	/// 
	/// <para>
	///  Use the <see cref="Filters.VideoInputDevices"/> collection to list
	///  video capture devices installed on the system.
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  foreach ( Filter f in Filters.VideoInputDevices )
	///  {
	///		Debug.WriteLine( f.Name );
	///  }
	/// </div></code>
	/// The <see cref="Filters"/> class also provides collections for audio 
	/// capture devices, video compressors and audio compressors.
	///
	/// <br/>
	/// <para><b>Preview</b></para>
	/// 
	/// <para>
	///  Video preview is controled with the <see cref="Capture.PreviewWindow"/>
	///  property. Setting this property to a visible control will immediately 
	///  begin preview. Set to null to stop the preview. 
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  // Enable preview
	///  capture.PreviewWindow = myPanel;
	///  // Disable preview
	///  capture.PreviewWindow = null;
	/// </div></code>
	/// <para>
	///  The control used must have a window handle (HWND), good controls to 
	///  use are the Panel or the form itself.
	/// </para>
	/// <para>
	///  Retrieving or changing video/audio settings such as FrameRate, 
	///  FrameSize, AudioSamplingRate, and AudioSampleSize will cause
	///  the preview window to flash. This is beacuse the preview must be
	///  temporarily stopped. Disable the preview if you need to access
	///  several properties at the same time.
	/// </para>
	/// 
	/// <br/>
	/// <para><b>Property Pages</b></para>
	///  
	/// <para>
	///  Property pages exposed by the devices and compressors are
	///  available through the <see cref="Capture.PropertyPages"/> 
	///  collection.
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  // Display the first property page
	///  capture.PropertyPages[0].Show();
	/// </div></code>
	/// <para>
	///  The property pages will often expose more settings than
	///  the Capture class does directly. Some examples are brightness, 
	///  color space, audio balance and bass boost. The disadvantage
	///  to using the property pages is the user's choices cannot be
	///  saved and later restored. The exception to this is the video
	///  and audio compressor property pages. Most compressors support
	///  the saving and restoring state, see the 
	///  <see cref="PropertyPage.State"/> property for more information.
	/// </para>
	/// <para>
	///  Changes made in the property page will be reflected 
	///  immediately in the Capture class properties (e.g. Capture.FrameSize).
	///  However, the reverse is not always true. A change made directly to
	///  FrameSize, for example, may not be reflected in the associated
	///  property page. Fortunately, the filter will use requested FrameSize
	///  even though the property page shows otherwise.
	/// </para>
	/// 
	/// <br/>
	/// <para><b>Saving and Restoring Settings</b></para>
	///  
	/// <para>
	///  To save the user's choice of devices and compressors, 
	///  save <see cref="Filter.MonikerString"/> and user it later
	///  to recreate the Filter object.
	/// </para>
	/// <para>
	///  To save a user's choices from a property page use
	///  <see cref="PropertyPage.State"/>. However, only the audio
	///  and video compressor property pages support this.
	/// </para>
	/// <para>
	///  The last items to save are the video and audio settings such
	///  as FrameSize and AudioSamplingRate. When restoring, remember
	///  to restore these properties after setting the video and audio
	///  compressors.
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  // Disable preview
	///  capture.PreviewWindow = null;
	///  
	///  // Save settings
	///  string videoDevice = capture.VideoDevice.MonikerString;
	///  string audioDevice = capture.AudioDevice.MonikerString;
	///  string videoCompressor = capture.VideoCompressor.MonikerString;
	///  string audioCompressor = capture.AudioCompressor.MonikerString;
	///  double frameRate = capture.FrameRate;
	///  Size frameSize = capture.FrameSize;
	///  short audioChannels = capture.AudioChannels;
	///  short audioSampleSize = capture.AudioSampleSize;
	///  int audioSamplingRate = capture.AudioSamplingRate;
	///  ArrayList pages = new ArrayList();
	///  foreach ( PropertyPage p in capture.PropertyPages )
	///  {
	///		if ( p.SupportsPersisting )
	///			pages.Add( p.State );
	///	}
	///			
	///  
	///  // Restore settings
	///  Capture capture = new Capture( new Filter( videoDevice), 
	///				new Filter( audioDevice) );
	///  capture.VideoCompressor = new Filter( videoCompressor );
	///  capture.AudioCompressor = new Filter( audioCompressor );
	///  capture.FrameRate = frameRate;
	///  capture.FrameSize = frameSize;
	///  capture.AudioChannels = audioChannels;
	///  capture.AudioSampleSize = audioSampleSize;
	///  capture.AudioSamplingRate = audioSamplingRate;
	///  foreach ( PropertyPage p in capture.PropertyPages )
	///  {
	///		if ( p.SupportsPersisting )
	///		{
	///			p.State = (byte[]) pages[0]
	///			pages.RemoveAt( 0 );
	///		}
	///	 }
	///  // Enable preview
	///  capture.PreviewWindow = myPanel;
	/// </div></code>
	///  
	/// <br/>
	/// <para><b>TV Tuner</b></para>
	/// 
	/// <para>
	///  To access the TV Tuner, use the <see cref="Capture.Tuner"/> property.
	///  If the device does not have a TV tuner, this property will be null.
	///  See <see cref="DirectX.Capture.Tuner.Channel"/>, 
	///  <see cref="DirectX.Capture.Tuner.InputType"/> and 
	///  <see cref="DirectX.Capture.Tuner.SignalPresent"/> 
	///  for more information.
	/// </para>
	/// <code><div style="background-color:whitesmoke;">
	///  // Change to channel 5
	///  capture.Tuner.Channel = 5;
	/// </div></code>
	/// 
	/// <br/>
	/// <para><b>Troubleshooting</b></para>
	/// 
	/// <para>
	///  This class library uses COM Interop to access the full
	///  capabilities of DirectShow, so if there is another
	///  application that can successfully use a hardware device
	///  then it should be possible to modify this class library
	///  to use the device.
	/// </para>
	/// <para>
	///  Try the <b>AMCap</b> sample from the DirectX SDK 
	///  (DX9\Samples\C++\DirectShow\Bin\AMCap.exe) or 
	///  <b>Virtual VCR</b> from http://www.DigTV.ws 
	/// </para>
	/// 
	/// <br/>
	/// <para><b>Credits</b></para>
	/// 
	/// <para>
	///  This class library would not be possible without the
	///  DirectShowLib project by NETMaster: 
	///  http://www.codeproject.com/useritems/directshownet.asp
	/// </para>
	/// <para>
	///  Documentation is generated by nDoc available at
	///  http://ndoc.sourceforge.net
	/// </para>
	/// 
	/// <br/>
	/// <para><b>Feedback</b></para>
	/// 
	///  Feel free to send comments and questions to me at
	///  mportobello@hotmail.com. If the the topic may be of interest
	///  to others, post your question on the www.codeproject.com
	///  page for DirectX.Capture.
	/// </remarks>
	public class Capture : ISampleGrabberCB
    {

		// ------------------ Private Enumerations --------------------

		/// <summary> Possible states of the interal filter graph </summary>
		protected enum GraphState
		{
			Null,			// No filter graph at all
			Created,		// Filter graph created with device filters added
			Rendered,		// Filter complete built, ready to run (possibly previewing)
			Capturing		// Filter is capturing
        }

		/// <summary>
		/// Recording file mode type enumerations
		/// </summary>
		public enum RecFileModeType
		{
			/// <summary> Avi video (+audio) </summary>
			Avi,
			/// <summary> Asf video (+audio), Wmv </summary>
			Wmv,
			/// <summary> Asf audio, Wma </summary>
			Wma,
		}

        // ------------------ Public Properties --------------------

		/// <summary> Is the class currently capturing. Read-only. </summary>
		public bool Capturing { get { return( graphState==GraphState.Capturing ); } }

		/// <summary> Has the class been cued to begin capturing. Read-only. </summary>
		public bool Cued { get { return( isCaptureRendered && graphState==GraphState.Rendered ); } }

		/// <summary> Is the class currently stopped. Read-only. </summary>
		public bool Stopped { get { return( graphState!=GraphState.Capturing ); } }

		/// <summary> 
		///  Name of file to capture to. Initially set to
		///  a valid temporary file.
		/// </summary>		
		/// <remarks>
		///  If the file does not exist, it will be created. If it does 
		///  exist, it will be overwritten. An overwritten file will 
		///  not be shortened if the captured data is smaller than the 
		///  original file. The file will be valid, it will just contain 
		///  extra, unused, data after the audio/video data. 
		/// 
		/// <para>
		///  A future version of this class will provide a method to copy 
		///  only the valid audio/video data to a new file. </para>
		/// 
		/// <para>
		///  This property cannot be changed while capturing or cued. </para>
		/// </remarks> 
		
		public string Filename 
		{ 
			get { return( filename ); } 
			set 
			{ 
				assertStopped();
				if ( Cued )
					throw new InvalidOperationException( "The Filename cannot be changed once cued. Use Stop() before changing the filename." );
				filename = value; 
				if ( fileWriterFilter != null )
				{
					string s;
					AMMediaType mt = new AMMediaType(); 
					int hr = fileWriterFilter.GetCurFile( out s, mt );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
					if ( mt.formatSize > 0 )
						Marshal.FreeCoTaskMem( mt.formatPtr ); 
					hr = fileWriterFilter.SetFileName( filename, mt );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}
			} 
		}

		/// <summary>
		/// Recording file modes
		/// </summary>
		public RecFileModeType RecFileMode
		{
			get { return(recFileMode); }
			set
			{
				if(this.graphState == GraphState.Capturing)
				{
					// Value may not be changed now
					return;
				}

				recFileMode = value;

				// Clear previous Asf information
				if(asfFormat != null)
				{
					asfFormat.Clear();
				}

				switch(recFileMode)
				{
					case RecFileModeType.Wma:
						if(asfFormat == null)
						{
							asfFormat = new AsfFormat(AsfFormat.AsfFormatSelection.AudioOnly);
						} 
						else
						{
							asfFormat.GetProfileFormatInfo(AsfFormat.AsfFormatSelection.AudioOnly);
						}
						break;
					case RecFileModeType.Wmv:
						if(asfFormat == null)
						{
							asfFormat = new AsfFormat(AsfFormat.AsfFormatSelection.Video);
						} 
						else
						{
							asfFormat.GetProfileFormatInfo(AsfFormat.AsfFormatSelection.Video);
						}
						break;
					case RecFileModeType.Avi:
						break;
					default:
						// Unsupported file format
						return;
				}

				// Change filename extension
				this.filename = Path.ChangeExtension(this.filename, RecFileMode.ToString().ToLower());
			}
		}

		/// <summary>
		/// Access to AsfFormat interface
		/// </summary>
		public AsfFormat AsfFormat
		{
			get { return asfFormat; }
		}
        
        /// <summary>
		///  The control that will host the preview window. 
		/// </summary>
		/// <remarks>
		///  Setting this property will begin video preview
		///  immediately. Set this property after setting all
		///  other properties to avoid unnecessary changes
		///  to the internal filter graph (some properties like
		///  FrameSize require the internal filter graph to be 
		///  stopped and disconnected before the property
		///  can be retrieved or set).
		///  
		/// <para>
		///  To stop video preview, set this property to null. </para>
		/// </remarks>
		public Control PreviewWindow
		{
			get { return( previewWindow ); }
			set
			{
				assertStopped();
				derenderGraph();
				previewWindow = value;
				wantPreviewRendered = ( ( previewWindow != null ) && ( videoDevice != null ) );
				renderGraph();
				startPreviewIfNeeded();
			}
		}


		/// <summary>
		///  The capabilities of the video device.
		/// </summary>
		/// <remarks>
		///  It may be required to cue the capture (see <see cref="Cue"/>) 
		///  before all capabilities are correctly reported. If you 
		///  have such a device, the developer would be interested to
		///  hear from you.
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		/// </remarks>
		public VideoCapabilities VideoCaps 
		{ 
			get 
			{ 
				if ( videoCaps == null )
				{
					if ( videoStreamConfig != null )
					{
						try 
						{
							videoCaps = new VideoCapabilities( videoStreamConfig ); 
						}
						catch ( Exception ex ) { Debug.WriteLine( "VideoCaps: unable to create videoCaps." + ex.ToString() ); }
					}
				}
				return( videoCaps ); 
			}
		}

		/// <summary>
		///  The capabilities of the audio device.
		/// </summary>
		/// <remarks>
		///  It may be required to cue the capture (see <see cref="Cue"/>) 
		///  before all capabilities are correctly reported. If you 
		///  have such a device, the developer would be interested to
		///  hear from you.
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		/// </remarks>
		public AudioCapabilities AudioCaps 
		{ 
			get 
			{ 
				if ( audioCaps == null )
				{
					if ( audioStreamConfig != null )
					{
						try 
						{ 
							audioCaps = new AudioCapabilities( audioStreamConfig ); 
						}
						catch ( Exception ex ) { Debug.WriteLine( "AudioCaps: unable to create audioCaps." + ex.ToString() ); }
					}
				}
				return( audioCaps ); 
			} 
		}

		/// <summary> 
		///  The video capture device filter. Read-only. To use a different 
		///  device, dispose of the current Capture instance and create a new 
		///  instance with the desired device. 
		/// </summary>
		public Filter VideoDevice { get { return( videoDevice ); } }

		/// <summary> 
		///  The audio capture device filter. Read-only. To use a different 
		///  device, dispose of the current Capture instance and create a new 
		///  instance with the desired device. 
		/// </summary>
		public Filter AudioDevice { get { return( audioDevice ); } }

		/// <summary> 
		///  The video compression filter. When this property is changed 
		///  the internal filter graph is rebuilt. This means that some properties
		///  will be reset. Set this property as early as possible to avoid losing 
		///  changes. This property cannot be changed while capturing.
		/// </summary>
		public Filter VideoCompressor { 
			get { return( videoCompressor ); } 
			set 
			{ 
				assertStopped();
				destroyGraph();
				videoCompressor = value;
				renderGraph();
                startPreviewIfNeeded();
			}
		}

		/// <summary> 
		///  The audio compression filter. 
		/// </summary>
		/// <remarks>
		///  When this property is changed 
		///  the internal filter graph is rebuilt. This means that some properties
		///  will be reset. Set this property as early as possible to avoid losing 
		///  changes. This property cannot be changed while capturing.
		/// </remarks>
		public Filter AudioCompressor 
		{ 
			get { return( audioCompressor ); } 
			set 
			{ 
				assertStopped();
				destroyGraph();
				audioCompressor = value;
				renderGraph();
				startPreviewIfNeeded();
			}
		}

		/// <summary> 
		///  The current video source. Use Capture.VideoSources to 
		///  list available sources. Set to null to disable all 
		///  sources (mute).
		/// </summary>
		public Source VideoSource 
		{ 
			get { return( VideoSources.CurrentSource ); } 
			set { VideoSources.CurrentSource = value; } 
		}

		/// <summary> 
		///  The current audio source. Use Capture.AudioSources to 
		///  list available sources. Set to null to disable all 
		///  sources (mute).
		/// </summary>
		public Source AudioSource 
		{
			get { return( AudioSources.CurrentSource ); } 
			set { AudioSources.CurrentSource = value; }
		}

		/// <summary> 
		///  Collection of available video sources/physical connectors 
		///  on the current video device. 
		/// </summary>
		/// <remarks>
		///  In most cases, if the device has only one source, 
		///  this collection will be empty. 
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		/// </remarks>
		public SourceCollection VideoSources
		{
			get 
			{ 
				if ( videoSources == null )
				{
					try
					{
						if ( videoDevice != null )
							videoSources = new SourceCollection( captureGraphBuilder, videoDeviceFilter, true );
						else
							videoSources = new SourceCollection();
					}
					catch ( Exception ex ) { Debug.WriteLine( "VideoSources: unable to create VideoSources." + ex.ToString() ); }
				}
				return ( videoSources );
			}
            set
            {
                if (value == null)
                {
                    this.videoSources = null;
                    this.PropertyPages = null;
                }
            }
		}


		/// <summary> 
		///  Collection of available audio sources/physical connectors 
		///  on the current audio device. 
		/// </summary>
		/// <remarks>
		///  In most cases, if the device has only one source, 
		///  this collection will be empty. For audio
		///  there are 2 different methods for enumerating audio sources
		///  an audio crossbar (usually TV tuners?) or an audio mixer 
		///  (usually sound cards?). This class will first look for an 
		///  audio crossbar. If no sources or only one source is available
		///  on the crossbar, this class will then look for an audio mixer.
		///  This class does not support both methods.
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		///  </remarks>
		public SourceCollection AudioSources
		{
			get 
			{ 
				if ( audioSources == null )
				{
					try
					{
                        if (audioDevice != null)
                        {
                            audioSources = new SourceCollection(captureGraphBuilder, audioDeviceFilter, false);
							// Try to get a good reason to add additional
							// sources.

							// If there is an audioDevice, but there are no
							// sources found yet, than it might be possible
							// that this might be a video device with audio
							// capture via PCI bus. Than the crossbar related
							// to the video device must be checked.
							if((this.AudioViaPci)&&(videoDevice != null)&&(audioSources.Count == 0))
							{
								// Maybe there should be a check whether the
								// Audio device and the video device belongs
								// together. Too many checks however, might
								// limit functionality or make the code
								// complex. For now, keep it simple ...
								audioSources.addFromGraph( captureGraphBuilder, videoDeviceFilter, false );
							}
                        }
                        else
                        {
                            audioSources = new SourceCollection();
							// Try to get a good reason to add additional
							// sources.

							// If there is an audioDevice, but there are no
							// sources found yet, than it might be possible
							// that this might be a video device with audio
							// capture via PCI bus. Than the crossbar related
							// to the video device must be checked.
							if((this.AudioViaPci)&&(videoDevice != null)/*&&(audioSources.Count == 0)*/)
							{
								// Maybe there should be a check whether the
								// Audio device and the video device belongs
								// together. Too many checks however, might
								// limit functionality or make the code
								// complex. For now, keep it simple ...
								audioSources.addFromGraph( captureGraphBuilder, videoDeviceFilter, false );
							}
                        }
					}
					catch ( Exception ex ) { Debug.WriteLine( "AudioSources: unable to create AudioSources." + ex.ToString() ); }
				}
				return ( audioSources );
			}
            set
            {
                if (value == null)
                {
                    this.audioSources = null;
                    this.PropertyPages = null;
                }
            }
		}

		/// <summary>
		///  Available property pages. 
		/// </summary>
		/// <remarks>
		///  These are property pages exposed by the DirectShow filters. 
		///  These property pages allow users modify settings on the 
		///  filters directly. 
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		/// </remarks>
		public PropertyPageCollection PropertyPages 
		{
			get
			{
				if ( propertyPages == null )
				{
					try 
					{
						if( (this.AudioViaPci)&&
							(this.audioDeviceFilter == null)&&(this.videoDeviceFilter != null) )
						{
							propertyPages = new PropertyPageCollection( 
								captureGraphBuilder, 
								videoDeviceFilter, videoDeviceFilter, 
								videoCompressorFilter, audioCompressorFilter, 
								VideoSources, AudioSources );
						}
						else
						{
                        propertyPages = new PropertyPageCollection( 
							captureGraphBuilder, 
							videoDeviceFilter, audioDeviceFilter, 
							videoCompressorFilter, audioCompressorFilter, 
							VideoSources, AudioSources );
						}
                    }
					catch ( Exception ex ) { Debug.WriteLine( "PropertyPages: unable to get property pages." + ex.ToString() ); }

				}
				return( propertyPages );
			}
            set
            {
                if (value == null)
                {
                    // Reload any property pages exposed by filters
                    if (propertyPages != null)
                    {
                        propertyPages.Dispose();
                        propertyPages = null;
                    }
                }
            }
		}

		/// <summary>
		///  The TV Tuner or null if the current video device 
		///  does not have a TV Tuner.
		/// </summary>
		public Tuner Tuner { get { return( tuner ); } }
	
		/// <summary>
		///  Gets and sets the frame rate used to capture video.
		/// </summary>
		/// <remarks>
		///  Common frame rates: 24 fps for film, 25 for PAL, 29.997
		///  for NTSC. Not all NTSC capture cards can capture at 
		///  exactly 29.997 fps. Not all frame rates are supported. 
		///  When changing the frame rate, the closest supported 
		///  frame rate will be used. 
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		///  
		/// <para>
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public double FrameRate
		{
			get
			{
				long avgTimePerFrame = (long) getStreamConfigSetting( videoStreamConfig, "AvgTimePerFrame" );
				return( (double) 10000000 / avgTimePerFrame );
			}
			set
			{
				long avgTimePerFrame = (long) ( 10000000 / value );
				setStreamConfigSetting( videoStreamConfig, "AvgTimePerFrame", avgTimePerFrame );
			}
		}

		/// <summary>
		///  Gets and sets the frame size used to capture video.
		/// </summary>
		/// <remarks>
		///  To change the frame size, assign a new Size object 
		///  to this property <code>capture.Size = new Size( w, h );</code>
		///  rather than modifying the size in place 
		///  (capture.Size.Width = w;). Not all frame
		///  rates are supported.
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		/// 
		/// <para> 
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public Size FrameSize
		{
			get
			{
				BitmapInfoHeader bmiHeader;
				bmiHeader = (BitmapInfoHeader) getStreamConfigSetting( videoStreamConfig, "BmiHeader" );
				Size size = new Size( bmiHeader.Width, bmiHeader.Height );
				return( size );
			}
			set
			{
				BitmapInfoHeader bmiHeader;
				bmiHeader = (BitmapInfoHeader) getStreamConfigSetting( videoStreamConfig, "BmiHeader" );
				bmiHeader.Width = value.Width;
				bmiHeader.Height = value.Height;
				setStreamConfigSetting( videoStreamConfig, "BmiHeader", bmiHeader );
//#if NEWCODE
				this.videoCaps = null;
//#endif
			}		
		}

		/// <summary>
		///  Get or set the number of channels in the waveform-audio data. 
		/// </summary>
		/// <remarks>
		///  Monaural data uses one channel and stereo data uses two channels. 
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		///  
		/// <para>
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public short AudioChannels
		{
			get
			{
				short audioChannels = (short) getStreamConfigSetting( audioStreamConfig, "nChannels" );
				return( audioChannels );
			}
			set
			{
				setStreamConfigSetting( audioStreamConfig, "nChannels", value );
			}
		}

		/// <summary>
		///  Get or set the number of audio samples taken per second.
		/// </summary>
		/// <remarks>
		///  Common sampling rates are 8.0 kHz, 11.025 kHz, 22.05 kHz, and 
		///  44.1 kHz. Not all sampling rates are supported.
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		///  
		/// <para>
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public int AudioSamplingRate
		{
			get
			{
				int samplingRate = (int) getStreamConfigSetting( audioStreamConfig, "nSamplesPerSec" );
				return( samplingRate );
			}
			set
			{
				setStreamConfigSetting( audioStreamConfig, "nSamplesPerSec", value );
			}
		}

		/// <summary>
		///  Get or set the number of bits recorded per sample. 
		/// </summary>
		/// <remarks>
		///  Common sample sizes are 8 bit and 16 bit. Not all
		///  samples sizes are supported.
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		///  
		/// <para>
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public short AudioSampleSize
		{
			get
			{
				short sampleSize = (short) getStreamConfigSetting( audioStreamConfig, "wBitsPerSample" );
				return( sampleSize );
			}
			set
			{
				setStreamConfigSetting( audioStreamConfig, "wBitsPerSample", value );
			}
		}


		// --------------------- Events ----------------------

		/// <summary> Fired when a capture is completed (manually or automatically). </summary>
		public event EventHandler					CaptureComplete;



		// ------------- Protected/private Properties --------------

		protected GraphState		graphState = GraphState.Null;		// State of the internal filter graph
		protected bool				isPreviewRendered = false;			// When graphState==Rendered, have we rendered the preview stream?
		protected bool				isCaptureRendered = false;			// When graphState==Rendered, have we rendered the capture stream?
		protected bool				wantPreviewRendered = false;		// Do we need the preview stream rendered (VideoDevice and PreviewWindow != null)
		protected bool				wantCaptureRendered = false;		// Do we need the capture stream rendered

#if DSHOWNET
		protected int				rotCookie = 0;						// Cookie into the Running Object Table
#else
        /// <summary>
        /// Special variable for debugging purposes
        /// Cookie into the Running Object Table
        /// </summary>
        protected DsROTEntry rotCookie = null;
#endif
        protected Filter			videoDevice = null;					// Property Backer: Video capture device filter
		protected Filter			audioDevice = null;					// Property Backer: Audio capture device filter
		protected Filter			videoCompressor = null;				// Property Backer: Video compression filter
		protected Filter			audioCompressor = null;				// Property Backer: Audio compression filter
		protected string			filename = "";						// Property Backer: Name of file to capture to
		protected Control			previewWindow = null;				// Property Backer: Owner control for preview
		protected VideoCapabilities	videoCaps = null;					// Property Backer: capabilities of video device
		protected AudioCapabilities	audioCaps = null;					// Property Backer: capabilities of audio device
		protected SourceCollection	videoSources = null;				// Property Backer: list of physical video sources
		protected SourceCollection	audioSources = null;				// Property Backer: list of physical audio sources
		protected PropertyPageCollection propertyPages = null;			// Property Backer: list of property pages exposed by filters
		protected Tuner				tuner = null;						// Property Backer: TV Tuner
		protected IGraphBuilder		graphBuilder;						// DShow Filter: Graph builder 
		protected IMediaControl		mediaControl;						// DShow Filter: Start/Stop the filter graph -> copy of graphBuilder
		protected IVideoWindow		videoWindow;						// DShow Filter: Control preview window -> copy of graphBuilder
		protected ICaptureGraphBuilder2		captureGraphBuilder = null;	// DShow Filter: building graphs for capturing video
		protected IAMStreamConfig	videoStreamConfig = null;			// DShow Filter: configure frame rate, size
		protected IAMStreamConfig	audioStreamConfig = null;			// DShow Filter: configure sample rate, sample size
		protected IBaseFilter		videoDeviceFilter = null;			// DShow Filter: selected video device
		protected IBaseFilter		videoCompressorFilter = null;		// DShow Filter: selected video compressor
		protected IBaseFilter		audioDeviceFilter = null;			// DShow Filter: selected audio device
		protected IBaseFilter		audioCompressorFilter = null;		// DShow Filter: selected audio compressor
		protected IBaseFilter		muxFilter = null;					// DShow Filter: multiplexor (combine video and audio streams)
		protected IFileSinkFilter	fileWriterFilter = null;			// DShow Filter: file writer

		/// <summary> Recording file mode (e.g. Windows Media Audio) </summary>		 
		protected RecFileModeType recFileMode = RecFileModeType.Wmv;

		// Option for selection audio rendering via the pci bus of the TV card
		// For wired audio connections the value must be false!
		// For TV-cards, like the Hauppauge PVR150, the value must be true!
		// This TV-card does not have a wired audio connection. However, this
		// option will work only if the TV-card driver has an audio device!
		private bool AudioViaPci = true;

		/// <summary>
		/// Check if there is an Audio Device
		/// </summary>
		public bool AudioAvailable
		{
			get
			{
				return ( ( (this.AudioViaPci)&&(this.VideoDevice != null) )||
				         (this.AudioDevice != null) );
			}
		}

		// Initialize AsfFormat class
		private AsfFormat asfFormat = new AsfFormat(AsfFormat.AsfFormatSelection.Video);

        // ------------- Constructors/Destructors --------------

		/// <summary> 
		///  Create a new Capture object. 
		///  videoDevice and audioDevice can be null if you do not 
		///  wish to capture both audio and video. However at least
		///  one must be a valid device. Use the <see cref="Filters"/> 
		///  class to list available devices.
		///  </summary>
		public Capture(Filter videoDevice, Filter audioDevice, bool audioViaPci)
        {
			if ( videoDevice == null && audioDevice == null )
				throw new ArgumentException( "The videoDevice and/or the audioDevice parameter must be set to a valid Filter.\n" );
			this.videoDevice = videoDevice;
			this.audioDevice = audioDevice;
			this.Filename = getTempFilename();
			this.AudioViaPci = audioViaPci;
            createGraph();
		}

		/// <summary> Destructor. Dispose of resources. </summary>
		~Capture()
		{
			Dispose();
		}


		// --------------------- Public Methods -----------------------

		/// <summary>
		///  Prepare for capturing. Use this method when capturing 
		///  must begin as quickly as possible. 
		/// </summary>
		/// <remarks>
		///  This will create/overwrite a zero byte file with 
		///  the name set in the Filename property. 
		///  
		/// <para>
		///  This will disable preview. Preview will resume
		///  once capture begins. This problem can be fixed
		///  if someone is willing to make the change. </para>
		///  
		/// <para>
		///  This method is optional. If Cue() is not called, 
		///  Start() will call it before capturing. This method
		///  cannot be called while capturing. </para>
		/// </remarks>
		public void Cue()
		{
			assertStopped();

			// We want the capture stream rendered
			wantCaptureRendered = true;

			// Re-render the graph (if necessary)
			renderGraph();

			// Pause the graph
			int hr = mediaControl.Pause();
			if ( hr != 0 ) Marshal.ThrowExceptionForHR( hr ); 
		}

		/// <summary> Begin capturing. </summary>
		public void Start()
		{
			assertStopped();

			// We want the capture stream rendered
			wantCaptureRendered = true;

			// Re-render the graph (if necessary)
			renderGraph();

			// Start the filter graph: begin capturing
			int hr = mediaControl.Run();
			if ( hr != 0 ) Marshal.ThrowExceptionForHR( hr ); 

			// Update the state
			graphState = GraphState.Capturing;
		}

		/// <summary> 
		///  Stop the current capture capture. If there is no
		///  current capture, this method will succeed.
		/// </summary>
		public void Stop()
		{
			wantCaptureRendered = false;

			// Stop the graph if it is running
			// If we have a preview running we should only stop the
			// capture stream. However, if we have a preview stream
			// we need to re-render the graph anyways because we 
			// need to get rid of the capture stream. To re-render
			// we need to stop the entire graph
			if ( mediaControl != null )
			{
				mediaControl.Stop();
			}

			// Update the state
			if ( graphState == GraphState.Capturing )
			{
				graphState = GraphState.Rendered;
				if ( CaptureComplete != null )
					CaptureComplete( this, null );
			}

			// So we destroy the capture stream IF 
			// we need a preview stream. If we don't
			// this will leave the graph as it is.
			try { renderGraph(); } catch {}
			try { startPreviewIfNeeded(); } catch {}
		}

		/// <summary> 
		///  Calls Stop, releases all references. If a capture is in progress
		///  it will be stopped, but the CaptureComplete event will NOT fire.
		/// </summary>
		public void Dispose()
		{
			wantPreviewRendered = false;
			wantCaptureRendered = false;
			CaptureComplete = null;

			try { destroyGraph(); } catch {}

			if ( videoSources != null )
				videoSources.Dispose(); videoSources = null;
			if ( audioSources != null )
				audioSources.Dispose(); audioSources = null;

		}



		// --------------------- Private Methods -----------------------
		
		/// <summary> 
		///  Create a new filter graph and add filters (devices, compressors, 
		///  misc), but leave the filters unconnected. Call renderGraph()
		///  to connect the filters.
		/// </summary>
		protected void createGraph()
		{
			Guid					cat;
			Guid					med;
			int						hr;

			// Ensure required properties are set
			if ( videoDevice == null && audioDevice == null )
				throw new ArgumentException( "The video and/or audio device have not been set. Please set one or both to valid capture devices.\n" );

			// Skip if we are already created
			if ( (int)graphState < (int)GraphState.Created )
			{
				// Garbage collect, ensure that previous filters are released
				GC.Collect();

				// Make a new filter graph
#if DSHOWNET
                // Make a new filter graph
                graphBuilder = (IGraphBuilder)Activator.CreateInstance(Type.GetTypeFromCLSID(Clsid.FilterGraph, true));

                // Get the Capture Graph Builder
                Guid clsid = Clsid.CaptureGraphBuilder2;
                Guid riid = typeof(ICaptureGraphBuilder2).GUID;
                captureGraphBuilder = (ICaptureGraphBuilder2)DsBugWO.CreateDsInstance(ref clsid, ref riid);
#else
				FilterGraph graph = new FilterGraph();
				graphBuilder = (IGraphBuilder)graph;

				// Get the Capture Graph Builder
				captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
#endif

                // Link the CaptureGraphBuilder to the filter graph
                hr = captureGraphBuilder.SetFiltergraph(graphBuilder);
                if (hr < 0) Marshal.ThrowExceptionForHR(hr);

                // Add the graph to the Running Object Table so it can be
                // viewed with GraphEdit
#if DEBUG
#if DSHOWNET
				DsROT.AddGraphToRot(graphBuilder, out rotCookie);
#else
                rotCookie = new DsROTEntry(graphBuilder);
#endif
#endif

                // Get the video device and add it to the filter graph
				if ( VideoDevice != null )
				{
					videoDeviceFilter = (IBaseFilter) Marshal.BindToMoniker( VideoDevice.MonikerString );
					hr = graphBuilder.AddFilter( videoDeviceFilter, "Video Capture Device" );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

				// Get the audio device and add it to the filter graph
				if ( AudioDevice != null )
				{
					audioDeviceFilter = (IBaseFilter) Marshal.BindToMoniker( AudioDevice.MonikerString );
					hr = graphBuilder.AddFilter( audioDeviceFilter, "Audio Capture Device" );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

                // Get the video compressor and add it to the filter graph
				if ( VideoCompressor != null )
				{
					videoCompressorFilter = (IBaseFilter) Marshal.BindToMoniker( VideoCompressor.MonikerString ); 
					hr = graphBuilder.AddFilter( videoCompressorFilter, "Video Compressor" );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

				// Get the audio compressor and add it to the filter graph
				if ( AudioCompressor != null )
				{
					audioCompressorFilter = (IBaseFilter) Marshal.BindToMoniker( AudioCompressor.MonikerString ); 
					hr = graphBuilder.AddFilter( audioCompressorFilter, "Audio Compressor" );
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

				// Retrieve the stream control interface for the video device
				// FindInterface will also add any required filters
				// (WDM devices in particular may need additional
				// upstream filters to function).

				// Try looking for an interleaved media type
				object o;
				cat = PinCategory.Capture;
				med = MediaType.Interleaved;
				Guid iid = typeof(IAMStreamConfig).GUID;
#if DSHOWNET
                hr = captureGraphBuilder.FindInterface(
                    ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
				hr = captureGraphBuilder.FindInterface(
					DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif

				if ( hr != 0 )
				{
					// If not found, try looking for a video media type
					med = MediaType.Video;
#if DSHOWNET
                    hr = captureGraphBuilder.FindInterface(
                        ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
					hr = captureGraphBuilder.FindInterface(
						DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif
				
					if ( hr != 0 )
						o = null;
				}
				videoStreamConfig = o as IAMStreamConfig;

// #if NEWCODE
				// Start of new Brian's Low code
				// Retrieve the stream control interface for the video device
				// FindInterface will also add any required filters
				// (WDM devices in particular may need additional
				// upstream filters to function).

				// Try looking for an interleaved media type
				o = null;
				cat = PinCategory.Preview;
				med = MediaType.Interleaved;
				iid = typeof(IAMStreamConfig).GUID;
#if DSHOWNET
				hr = captureGraphBuilder.FindInterface(
					ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
				hr = captureGraphBuilder.FindInterface(
					DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif

				if ( hr != 0 )
				{
					// If not found, try looking for a video media type
					med = MediaType.Video;
#if DSHOWNET
					hr = captureGraphBuilder.FindInterface(
						ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
					hr = captureGraphBuilder.FindInterface(
						DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif
				
					if ( hr != 0 )
						o = null;
				}
				this.previewStreamConfig = o as IAMStreamConfig;
				// End of new Brian's Low code

				if( (this.videoStreamConfig != null)||
					(this.previewStreamConfig != null) )
				{
					this.dxUtils = new DxUtils();
					bool result = this.dxUtils.InitDxUtils(this.videoDeviceFilter);

					if((!result)&&(!this.dxUtils.FindMediaData(this.videoStreamConfig)))
					{
						this.dxUtils.Dispose();
						this.dxUtils = null;
					}
				}
// #endif
				// Retrieve the stream control interface for the audio device
				o = null;
				cat = PinCategory.Capture;
				med = MediaType.Audio ;
				iid = typeof(IAMStreamConfig).GUID;
				if( (this.AudioViaPci)&&
					(audioDeviceFilter == null)&&(videoDeviceFilter != null) )
				{
                    hr = captureGraphBuilder.FindInterface(
#if DSHOWNET
						ref cat, ref med, videoDeviceFilter, ref iid, out o );
#else
                        DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o);
#endif
				}
				else
				{
#if DSHOWNET
                    hr = captureGraphBuilder.FindInterface(
                        ref cat, ref med, audioDeviceFilter, ref iid, out o);
#else
                    hr = captureGraphBuilder.FindInterface(
	    				DsGuid.FromGuid(cat), DsGuid.FromGuid(med), audioDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif
				}

				if (hr != 0)
					o = null;
				audioStreamConfig = o as IAMStreamConfig;

				// Retreive the media control interface (for starting/stopping graph)
				mediaControl = (IMediaControl) graphBuilder;

				// Reload any video crossbars
				if ( videoSources != null ) videoSources.Dispose(); videoSources = null;

				// Reload any audio crossbars
				if ( audioSources != null ) audioSources.Dispose(); audioSources = null;
				
				// Reload any property pages exposed by filters
                this.PropertyPages = null;

				// Reload capabilities of video device
				videoCaps = null;
				previewCaps = null;

				// Reload capabilities of video device
				audioCaps = null;

				// Retrieve TV Tuner if available
				o = null;
				cat = PinCategory.Capture;
				med = MediaType.Interleaved; 
				iid = typeof(IAMTVTuner).GUID;
#if DSHOWNET
                hr = captureGraphBuilder.FindInterface(
                    ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else

				hr = captureGraphBuilder.FindInterface( 
					DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif
				if ( hr != 0 )
				{
					med = MediaType.Video ;
#if DSHOWNET
                    hr = captureGraphBuilder.FindInterface(
                        ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
					hr = captureGraphBuilder.FindInterface( 
						DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, DsGuid.FromGuid(iid), out o );
#endif
					if ( hr != 0 )
						o = null;
				}
				IAMTVTuner t = o as IAMTVTuner;
				if ( t != null )
				{
					tuner = new Tuner(t);
					// Do not forget to set proper country code (Netherlands is 31)
				}

				// No check on TV Audio needed, it will show up in the
				// PropertyPages when it is available
				// Code for finding the TV audio interface
				o = null;
				cat = PinCategory.Capture;
				med = MediaType.Interleaved;
				iid = typeof(IAMTVAudio).GUID;
				hr = captureGraphBuilder.FindInterface(
#if DSHOWNET
					ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
                    cat, med, videoDeviceFilter, iid, out o);
#endif
				if ( hr != 0 )
				{
					med = MediaType.Video;
#if DSHOWNET
					hr = captureGraphBuilder.FindInterface(
						ref cat, ref med, videoDeviceFilter, ref iid, out o);
#else
				hr = captureGraphBuilder.FindInterface(
					cat, med, videoDeviceFilter, iid, out o);
#endif
					if ( hr != 0 )
					{
						o = null;
					}
				}

				if((o != null)&&(tuner != null))
				{
					IAMTVAudio a = o as IAMTVAudio;
					TvAudio = a;
#if DEBUG
					Debug.WriteLine("FindInterface tuner.TvAudio");
#endif // DEBUG
				}

				/*
							// ----------- VMR 9 -------------------
							//## check out samples\inc\vmrutil.h :: RenderFileToVMR9

							IBaseFilter vmr = null;
							if ( ( VideoDevice != null ) && ( previewWindow != null ) )
							{
								vmr = (IBaseFilter) Activator.CreateInstance( Type.GetTypeFromCLSID( Clsid.VideoMixingRenderer9, true ) ); 
								hr = graphBuilder.AddFilter( vmr, "VMR" );
								if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

								IVMRFilterConfig9 vmrFilterConfig = (IVMRFilterConfig9) vmr;
								hr = vmrFilterConfig.SetRenderingMode( VMRMode9.Windowless );
								if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

								IVMRWindowlessControl9 vmrWindowsless = (IVMRWindowlessControl9) vmr;	
								hr = vmrWindowsless.SetVideoClippingWindow( previewWindow.Handle );
								if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
							}
							//------------------------------------------- 

							// ---------- SmartTee ---------------------

							IBaseFilter smartTeeFilter = (IBaseFilter) Activator.CreateInstance( Type.GetTypeFromCLSID( Clsid.SmartTee, true ) ); 
							hr = graphBuilder.AddFilter( smartTeeFilter, "Video Smart Tee" );
							if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

							// Video -> SmartTee
							cat = PinCategory.Capture;
							med = MediaType.Video;
							hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, null, smartTeeFilter ); 
							if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

							// smarttee -> mux
							cat = PinCategory.Capture;
							med = MediaType.Video;
							hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), smartTeeFilter, null, muxFilter ); 
							if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

							// smarttee -> vmr
							cat = PinCategory.Preview;
							med = MediaType.Video;
							hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), smartTeeFilter, null, vmr ); 
							if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

							// -------------------------------------
				*/		
				// Update the state now that we are done
				graphState = GraphState.Created;
			}
		}

		/// <summary>
		///  Connects the filters of a previously created graph 
		///  (created by createGraph()). Once rendered the graph
		///  is ready to be used. This method may also destroy
		///  streams if we have streams we no longer want.
		/// </summary>
		protected void renderGraph()
		{
			Guid					cat;
			Guid					med;
			int						hr;
			bool					didSomething = false;
#if DSHOWNET
			const int WS_CHILD			= 0x40000000;	
			const int WS_CLIPCHILDREN	= 0x02000000;
			const int WS_CLIPSIBLINGS	= 0x04000000;
#endif

			assertStopped();

			// Ensure required properties set
			if ( filename == null )
				throw new ArgumentException( "The Filename property has not been set to a file.\n" );

			// Stop the graph
			if ( mediaControl != null )
				mediaControl.Stop();

			// Create the graph if needed (group should already be created)
			createGraph();

			// Derender the graph if we have a capture or preview stream
			// that we no longer want. We can't derender the capture and 
			// preview streams seperately. 
			// Notice the second case will leave a capture stream intact
			// even if we no longer want it. This allows the user that is
			// not using the preview to Stop() and Start() without
			// rerendering the graph.
			if ( !wantPreviewRendered && isPreviewRendered )
				derenderGraph();
			if ( !wantCaptureRendered && isCaptureRendered )
				if ( wantPreviewRendered )
					derenderGraph();


			// Render capture stream (only if necessary)
			if ( wantCaptureRendered && !isCaptureRendered )
			{
				// Render the file writer portion of graph (mux -> file)
				// Record captured audio/video in Avi, Wmv or Wma format
				Guid mediaSubType; // Media sub type
				bool captureAudio = true;
				bool captureVideo = true;
				IBaseFilter videoCompressorfilter = null;

				// Set media sub type and video compressor filter if needed
				if(RecFileMode == RecFileModeType.Avi)
				{
					mediaSubType = MediaSubType.Avi;
					// For Avi file saving a video compressor must be used
					// If one is selected, that one will be used.
					videoCompressorfilter = videoCompressorFilter;
				}
				else
				{
					mediaSubType = MediaSubType.Asf;
				}

				// Intialize the Avi or Asf file writer
#if DSHOWNET
                hr = captureGraphBuilder.SetOutputFileName(ref mediaSubType, Filename, out muxFilter, out fileWriterFilter);
#else
                hr = captureGraphBuilder.SetOutputFileName(mediaSubType, Filename, out muxFilter, out fileWriterFilter);
#endif
				if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

				// For Wma (and Wmv) a suitable profile must be selected. This
				// can be done via a property window, however the muxFilter is
				// just created. if needed, the property windows should show up
				// right now!
				// Another solution is to configure the Asf file writer, the
				// use interface must ensure the proper format has been
				// selected.
				if((RecFileMode == RecFileModeType.Wma)||
					(RecFileMode == RecFileModeType.Wmv))
				{
					if(this.AsfFormat != null)
					{
						this.AsfFormat.UpdateAsfAVFormat(this.muxFilter);
						this.AsfFormat.GetCurrentAsfAVInfo(out captureAudio, out captureVideo);
					}
				}

				// Render video (video -> mux) if needed or possible
				if((VideoDevice != null)&&(captureVideo))
                {
					// Try interleaved first, because if the device supports it,
					// it's the only way to get audio as well as video
					cat = PinCategory.Capture;
					med = MediaType.Interleaved;
#if DSHOWNET
                    hr = captureGraphBuilder.RenderStream(ref cat, ref med, videoDeviceFilter, videoCompressorfilter, muxFilter);
#else
					hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, videoCompressorFilter, muxFilter ); 
#endif
					if( hr < 0 ) 
					{
						med = MediaType.Video;
#if DSHOWNET
                        hr = captureGraphBuilder.RenderStream(ref cat, ref med, videoDeviceFilter, videoCompressorfilter, muxFilter);
#else
						hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, videoCompressorFilter, muxFilter ); 
#endif
						if ( hr == -2147220969 ) throw new DeviceInUseException( "Video device", hr );
						if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
					}
				}

				// Render audio (audio -> mux) if possible
				if((audioDeviceFilter != null)&&(captureAudio))
				{
					// If this Asf file format than please keep in mind that
					// certain Wmv formats do not have an audio stream, so
					// when using this code, please ensure you use a format
					// which supports audio!
					cat = PinCategory.Capture;
					med = MediaType.Audio;
#if DSHOWNET
					hr = captureGraphBuilder.RenderStream( ref cat, ref med, audioDeviceFilter, audioCompressorFilter, muxFilter ); 
#else
					hr = captureGraphBuilder.RenderStream( DsGuid.FromGuid(cat), DsGuid.FromGuid(med), audioDeviceFilter, audioCompressorFilter, muxFilter );
#endif
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}
				else
					if( (this.AudioViaPci)&&(captureAudio)&&
					(audioDeviceFilter == null)&&(videoDeviceFilter != null) )
				{
					cat = PinCategory.Capture;
					med = MediaType.Audio;
#if DSHOWNET
					hr = captureGraphBuilder.RenderStream( ref cat, ref med, videoDeviceFilter, audioCompressorFilter, muxFilter );
#else
                    hr = captureGraphBuilder.RenderStream(cat, med, videoDeviceFilter, audioCompressorFilter, muxFilter);
#endif
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

				isCaptureRendered = true;
				didSomething = true;
			}

			// Render preview stream (only if necessary)
			if ( wantPreviewRendered && !isPreviewRendered )
			{
				// Render preview (video -> renderer)
				this.InitVideoRenderer();
				this.AddDeInterlaceFilter();

				// When capture pin is used, preview works immediately,
				// however this conflicts with file saving.
				// An alternative is to use VMR9
                cat = PinCategory.Preview;
				med = MediaType.Video;
// #if NEWCODE
				if(this.InitSampleGrabber())
				{
					Debug.WriteLine("SampleGrabber added to graph.");

#if DSHOWNET
					hr = captureGraphBuilder.RenderStream(ref cat, ref med, videoDeviceFilter, this.baseGrabFlt, this.videoRendererFilter); 
#else
					hr = captureGraphBuilder.RenderStream(DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, this.baseGrabFlt, this.videoRendererFilter); 
#endif
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}
				else
// #endif NEWCODE
				{
#if DSHOWNET
					hr = captureGraphBuilder.RenderStream(ref cat, ref med, videoDeviceFilter, null, this.videoRendererFilter);
#else
					hr = captureGraphBuilder.RenderStream(DsGuid.FromGuid(cat), DsGuid.FromGuid(med), videoDeviceFilter, null, this.videoRendererFilter); 
#endif
					if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );
				}

				// Special option to enable rendering audio via PCI bus
				if((this.AudioViaPci)&&(audioDeviceFilter != null))
				{
					cat = PinCategory.Preview;
					med = MediaType.Audio;
#if DSHOWNET
					hr = captureGraphBuilder.RenderStream( ref cat, ref med, audioDeviceFilter, null, null ); 
#else
                    hr = captureGraphBuilder.RenderStream(DsGuid.FromGuid(cat), DsGuid.FromGuid(med), audioDeviceFilter, null, null);
#endif
					if( hr < 0 )
					{
						Marshal.ThrowExceptionForHR( hr );
					}
				}
				else
					if( (this.AudioViaPci)&&
					(this.audioDeviceFilter == null)&&(this.videoDeviceFilter != null) )
				{
					cat = PinCategory.Preview;
					med = MediaType.Audio;
#if DSHOWNET
					hr = captureGraphBuilder.RenderStream( ref cat, ref med, videoDeviceFilter, null, null ); 
#else
                    hr = captureGraphBuilder.RenderStream(cat, med, videoDeviceFilter, null, null);
#endif
					if( hr < 0 )
					{
						Marshal.ThrowExceptionForHR( hr );
					}
				}

				// Get the IVideoWindow interface
				videoWindow = (IVideoWindow) graphBuilder;

				// Set the video window to be a child of the main window
				hr = videoWindow.put_Owner( previewWindow.Handle );
				if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

				// Set video window style
#if DSHOWNET
				hr = videoWindow.put_WindowStyle( WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
#else
			    hr = videoWindow.put_WindowStyle(WindowStyle.Child | WindowStyle.ClipChildren | WindowStyle.ClipSiblings);
#endif
				if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

				// Position video window in client rect of owner window
				previewWindow.Resize += new EventHandler( onPreviewWindowResize );
				onPreviewWindowResize( this, null );

				// Make the video window visible, now that it is properly positioned
#if DSHOWNET
				hr = videoWindow.put_Visible( DsHlp.OATRUE );
#else
				hr = videoWindow.put_Visible( OABool.True );
#endif
				if( hr < 0 ) Marshal.ThrowExceptionForHR( hr );

				isPreviewRendered = true;
				didSomething = true;
// #if NEWCODE
				SetMediaSampleGrabber();
// #endif NEWCODE
			}

			if ( didSomething )
				graphState = GraphState.Rendered;
		}

		/// <summary>
		///  Setup and start the preview window if the user has
		///  requested it (by setting PreviewWindow).
		/// </summary>
		protected void startPreviewIfNeeded()
		{
			// Render preview 
			if ( wantPreviewRendered && isPreviewRendered && !isCaptureRendered )
			{
				// Run the graph (ignore errors)
				// We can run the entire graph becuase the capture
				// stream should not be rendered (and that is enforced
				// in the if statement above)
#if DEBUG
				try
				{
					int hr = mediaControl.Run();
					if(hr != 0)
					{
						Debug.WriteLine("MediaControl.Run() returns: " + hr.ToString());
					}
				}
				catch
				{
					Debug.WriteLine("MediaControl.Run()in StartPreviewIfNeeded cause fatal exception ...");
				}
#else
                mediaControl.Run();
#endif
            }
		}

		/// <summary>
		///  Disconnect and remove all filters except the device
		///  and compressor filters. This is the opposite of
		///  renderGraph(). Soem properties such as FrameRate
		///  can only be set when the device output pins are not
		///  connected. 
		/// </summary>
		protected void derenderGraph()
		{
			// Stop the graph if it is running (ignore errors)
			if ( mediaControl != null )
				mediaControl.Stop();

			// Free the preview window (ignore errors)
			if ( videoWindow != null )
			{
#if DSHOWNET
                videoWindow.put_Visible(DsHlp.OAFALSE);
#else
                videoWindow.put_Visible(OABool.False);
#endif
				videoWindow.put_Owner( IntPtr.Zero );
				videoWindow = null;
			}

			// Remove the Resize event handler
			if ( PreviewWindow != null )
				previewWindow.Resize -= new EventHandler( onPreviewWindowResize );

			if ( (int)graphState >= (int)GraphState.Rendered )
			{
				// Update the state
				graphState = GraphState.Created;
				isCaptureRendered = false;
				isPreviewRendered = false;

				// Disconnect all filters downstream of the 
				// video and audio devices. If we have a compressor
				// then disconnect it, but don't remove it
				if (videoDeviceFilter != null)
				{
					try
					{
						removeDownstream(videoDeviceFilter, (videoCompressor == null));
					}
					catch
					{
						Debug.WriteLine("Error removeDownstream videoDeviceFilter");
					}
				}
				if (audioDeviceFilter != null)
				{
					try
					{
						removeDownstream(audioDeviceFilter, (audioCompressor == null));
					}
					catch
					{
						Debug.WriteLine("Error removeDownstream audioDeviceFilter");
					}
				}

				// These filters should have been removed by the
				// calls above. (Is there anyway to check?)
				muxFilter = null;
				fileWriterFilter = null;
				this.videoRendererFilter = null;
				this.deInterlaceFilter = null;
            }
		}

		/// <summary>
		///  Removes all filters downstream from a filter from the graph.
		///  This is called only by derenderGraph() to remove everything
		///  from the graph except the devices and compressors. The parameter
		///  "removeFirstFilter" is used to keep a compressor (that should
		///  be immediately downstream of the device) if one is begin used.
		/// </summary>
		protected void removeDownstream( IBaseFilter filter, bool removeFirstFilter )
		{
			// Get a pin enumerator off the filter
			IEnumPins pinEnum;
			int hr = filter.EnumPins( out pinEnum );
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
						// Get the pin it is connected to
						IPin pinTo = null;
						pins[0].ConnectedTo( out pinTo );
						if ( pinTo != null )
						{
							// Is this an input pin?
							PinInfo info = new PinInfo();
							hr = pinTo.QueryPinInfo( out info );
							if( (hr == 0) && (info.dir == (PinDirection.Input)) )
							{
								// Recurse down this branch
								removeDownstream( info.filter, true );

								// Disconnect 
								graphBuilder.Disconnect( pinTo );
								graphBuilder.Disconnect( pins[0] );

								// Remove this filter
								// but don't remove the video or audio compressors
								if ( ( info.filter != videoCompressorFilter ) &&
									 ( info.filter != audioCompressorFilter ) )
									graphBuilder.RemoveFilter( info.filter );
							}
							Marshal.ReleaseComObject( info.filter );
							Marshal.ReleaseComObject( pinTo );
						}
						Marshal.ReleaseComObject( pins[0] );
					}
				}
				while( hr == 0 );

				Marshal.ReleaseComObject( pinEnum ); pinEnum = null;
			}
		}

		/// <summary>
		///  Completely tear down a filter graph and 
		///  release all associated resources.
		/// </summary>
		protected void destroyGraph()
		{
			// Derender the graph (This will stop the graph
			// and release preview window. It also destroys
			// half of the graph which is unnecessary but
			// harmless here.) (ignore errors)
			try{ derenderGraph(); } catch {}

			// Update the state after derender because it
			// depends on correct status. But we also want to
			// update the state as early as possible in case
			// of error.
			graphState = GraphState.Null;
			isCaptureRendered = false;
			isPreviewRendered = false;

            // Remove graph from the ROT
#if DSHOWNET
			if ( rotCookie != 0 )
			{
				DsROT.RemoveGraphFromRot( ref rotCookie );
				rotCookie = 0;
			}
#else
            if (rotCookie != null)
            {
                rotCookie.Dispose();
                rotCookie = null;
            }
#endif

            // Remove filters from the graph
			// This should be unnecessary but the Nvidia WDM
			// video driver cannot be used by this application 
			// again unless we remove it. Ideally, we should
			// simply enumerate all the filters in the graph
			// and remove them. (ignore errors)
			if ( muxFilter != null )
				graphBuilder.RemoveFilter( muxFilter );
			if ( videoCompressorFilter != null )
				graphBuilder.RemoveFilter( videoCompressorFilter  );
			if ( audioCompressorFilter != null )
				graphBuilder.RemoveFilter( audioCompressorFilter  );
			if ( videoDeviceFilter != null )
				graphBuilder.RemoveFilter( videoDeviceFilter );
			if ( audioDeviceFilter != null )
				graphBuilder.RemoveFilter( audioDeviceFilter );

			if(this.videoRendererFilter != null)
			{
				this.graphBuilder.RemoveFilter(this.videoRendererFilter);
			}

            // Clean up properties
			if ( videoSources != null )
				videoSources.Dispose(); videoSources = null;
			if ( audioSources != null )
				audioSources.Dispose(); audioSources = null;
            this.PropertyPages = null; // Disposal done within PropertyPages
			if ( tuner != null )
				tuner.Dispose(); tuner = null;

// #if NEWCODE
			if(this.tvAudio != null)
			{
				Marshal.ReleaseComObject(this.tvAudio); tvAudio = null;
			}

			if(this.dxUtils != null)
			{
				this.dxUtils.Dispose();
				this.dxUtils = null;
			}
// #endif

			// Cleanup
			if ( graphBuilder != null )
				Marshal.ReleaseComObject( graphBuilder );  graphBuilder = null;
			if ( captureGraphBuilder != null )
				Marshal.ReleaseComObject( captureGraphBuilder ); captureGraphBuilder = null;
			if ( muxFilter != null )
				Marshal.ReleaseComObject( muxFilter ); muxFilter = null;
			if ( fileWriterFilter != null )
				Marshal.ReleaseComObject( fileWriterFilter ); fileWriterFilter = null;
			if ( videoDeviceFilter != null )
				Marshal.ReleaseComObject( videoDeviceFilter ); videoDeviceFilter = null;
			if ( audioDeviceFilter != null )
				Marshal.ReleaseComObject( audioDeviceFilter ); audioDeviceFilter = null;
			if ( videoCompressorFilter != null )
				Marshal.ReleaseComObject( videoCompressorFilter ); videoCompressorFilter = null;
			if ( audioCompressorFilter != null )
				Marshal.ReleaseComObject( audioCompressorFilter ); audioCompressorFilter = null;
// #if NEWCODE
			this.DisposeSampleGrabber();
// #endif

			if(this.videoRendererFilter != null)
			{
				Marshal.ReleaseComObject(this.videoRendererFilter); this.videoRendererFilter = null;
			}

            // These are copies of graphBuilder
			mediaControl = null;
			videoWindow = null;

			// For unmanaged objects we haven't released explicitly
			GC.Collect();
		}

		/// <summary> Resize the preview when the PreviewWindow is resized </summary>
		protected void onPreviewWindowResize(object sender, EventArgs e)
		{
			if ( videoWindow != null )
			{
				// Position video window in client rect of owner window
				Rectangle rc = previewWindow.ClientRectangle;
				videoWindow.SetWindowPosition( 0, 0, rc.Right, rc.Bottom );
			}
		}
	
		/// <summary> 
		///  Get a valid temporary filename (with path). We aren't using 
		///  Path.GetTempFileName() because it creates a 0-byte file 
		/// </summary>
		protected string getTempFilename()
		{
			string s;
			try
			{
				int count = 0;
				int i;
				Random r = new Random();
				string tempPath = Path.GetTempPath();
				do
				{
					i = r.Next();
					s = Path.Combine( tempPath, i.ToString("X") + ".avi" );
					count++;
					if ( count > 100 ) throw new InvalidOperationException( "Unable to find temporary file." );
				} while ( File.Exists( s ) );
			}
			catch { s = "c:\temp.avi"; }
			return( s );
		}

		/// <summary>
		///  Retrieves the value of one member of the IAMStreamConfig format block.
		///  Helper function for several properties that expose
		///  video/audio settings from IAMStreamConfig.GetFormat().
		///  IAMStreamConfig.GetFormat() returns a AMMediaType struct.
		///  AMMediaType.formatPtr points to a format block structure.
		///  This format block structure may be one of several 
		///  types, the type being determined by AMMediaType.formatType.
		/// </summary>
		protected object getStreamConfigSetting( IAMStreamConfig streamConfig, string fieldName)
		{
			if ( streamConfig == null )
				throw new NotSupportedException();
			assertStopped();

			derenderGraph();

			object returnValue = null;
#if DSHOWNET
			IntPtr pmt = IntPtr.Zero;
#endif
			AMMediaType mediaType = new AMMediaType();

			try 
			{
				// Get the current format info
#if DSHOWNET
                int hr = streamConfig.GetFormat(out pmt);
#else
				int hr = streamConfig.GetFormat(out mediaType);
#endif
				if ( hr != 0 )
					Marshal.ThrowExceptionForHR( hr );

#if DSHOWNET
				Marshal.PtrToStructure( pmt, mediaType );
#endif

				// The formatPtr member points to different structures
				// dependingon the formatType
				object formatStruct;
				if ( mediaType.formatType == FormatType.WaveEx )
					formatStruct = new WaveFormatEx();
				else if ( mediaType.formatType == FormatType.VideoInfo )
					formatStruct = new VideoInfoHeader();
				else if ( mediaType.formatType == FormatType.VideoInfo2 )
					formatStruct = new VideoInfoHeader2();
				else
					throw new NotSupportedException( "This device does not support a recognized format block." );

				// Retrieve the nested structure
				Marshal.PtrToStructure( mediaType.formatPtr, formatStruct );

				// Find the required field
				Type structType = formatStruct.GetType();
				FieldInfo fieldInfo = structType.GetField( fieldName );
				if ( fieldInfo == null )
					throw new NotSupportedException( "Unable to find the member '" + fieldName + "' in the format block." );

				// Extract the field's current value
				returnValue = fieldInfo.GetValue( formatStruct ); 
						
			}
			finally
			{
				DsUtils.FreeAMMediaType( mediaType );
#if DSHOWNET
				Marshal.FreeCoTaskMem( pmt );
#endif
			}
			renderGraph();
			startPreviewIfNeeded();

			return( returnValue );
		}

		/// <summary>
		///  Set the value of one member of the IAMStreamConfig format block.
		///  Helper function for several properties that expose
		///  video/audio settings from IAMStreamConfig.GetFormat().
		///  IAMStreamConfig.GetFormat() returns a AMMediaType struct.
		///  AMMediaType.formatPtr points to a format block structure.
		///  This format block structure may be one of several 
		///  types, the type being determined by AMMediaType.formatType.
		/// </summary>
		protected object setStreamConfigSetting( IAMStreamConfig streamConfig, string fieldName, object newValue)
		{
			if ( streamConfig == null )
				throw new NotSupportedException();
			assertStopped();
			derenderGraph();

			object returnValue = null;
#if DSHOWNET
            IntPtr pmt = IntPtr.Zero;
#endif
            AMMediaType mediaType = new AMMediaType();

			try 
			{
				// Get the current format info
#if DSHOWNET
                int hr = streamConfig.GetFormat(out pmt);
#else
				int hr = streamConfig.GetFormat(out mediaType);
#endif
				if ( hr != 0 )
					Marshal.ThrowExceptionForHR( hr );

#if DSHOWNET
                Marshal.PtrToStructure(pmt, mediaType);
#endif

				// The formatPtr member points to different structures
				// dependingon the formatType
				object formatStruct;
				if ( mediaType.formatType == FormatType.WaveEx )
					formatStruct = new WaveFormatEx();
				else if ( mediaType.formatType == FormatType.VideoInfo )
					formatStruct = new VideoInfoHeader();
				else if ( mediaType.formatType == FormatType.VideoInfo2 )
					formatStruct = new VideoInfoHeader2();
				else
					throw new NotSupportedException( "This device does not support a recognized format block." );

				// Retrieve the nested structure
				Marshal.PtrToStructure( mediaType.formatPtr, formatStruct );

				// Find the required field
				Type structType = formatStruct.GetType();
				FieldInfo fieldInfo = structType.GetField( fieldName );
				if ( fieldInfo == null )
					throw new NotSupportedException( "Unable to find the member '" + fieldName + "' in the format block." );

				// Update the value of the field
				fieldInfo.SetValue( formatStruct, newValue );

				// Update fields that may depend on specific values of other attributes
				if (mediaType.formatType == FormatType.WaveEx)
				{
					WaveFormatEx waveFmt = formatStruct as WaveFormatEx;
					waveFmt.nBlockAlign = (short)(waveFmt.nChannels * waveFmt.wBitsPerSample / 8);
					waveFmt.nAvgBytesPerSec = waveFmt.nBlockAlign * waveFmt.nSamplesPerSec;
				}

                // PtrToStructure copies the data so we need to copy it back
				Marshal.StructureToPtr( formatStruct, mediaType.formatPtr, false ); 

				// Save the changes
				hr = streamConfig.SetFormat( mediaType );
				if ( hr != 0 )
					Marshal.ThrowExceptionForHR( hr );
			}
			finally
			{
				DsUtils.FreeAMMediaType( mediaType );
#if DSHOWNET
                Marshal.FreeCoTaskMem(pmt);
#endif
            }
			renderGraph();
			startPreviewIfNeeded();

			return( returnValue );
		}

		/// <summary>
		///  Assert that the class is in a Stopped state.
		/// </summary>
		protected void assertStopped()
		{
			if ( !Stopped )
				throw new InvalidOperationException( "This operation not allowed while Capturing. Please Stop the current capture." ); 
		}

// #if NEWCODE
#if DSHOWNET
		/// <summary>
		/// CLSID_VideoRenderer
		/// </summary>
		[ComImport, Guid("70e102b0-5556-11ce-97c0-00aa0055595a")]
			public class VideoRenderer
		{
		}
#endif

		/// <summary>
		/// Use VMR9 flag, if false use the video renderer instead
		/// </summary>
		private bool useVMR9 = false;

		private IBaseFilter videoRendererFilter = null;

		/// <summary>
		/// Check if VMR9 should be used
		/// </summary>
		public bool UseVMR9
		{
			get { return this.useVMR9; }
			set	{ this.useVMR9 = value; }
		}

		private bool InitVideoRenderer()
		{
			if(this.videoRendererFilter != null)
			{
				this.graphBuilder.RemoveFilter(this.videoRendererFilter);
				Marshal.ReleaseComObject(this.videoRendererFilter);
				this.videoRendererFilter = null;
			}

			if(this.useVMR9)
			{
				this.videoRendererFilter = (IBaseFilter)new VideoMixingRenderer9();
			}
			else
			{
				this.videoRendererFilter = (IBaseFilter)new VideoRenderer();
			}

			if(this.videoRendererFilter != null)
			{
				this.graphBuilder.AddFilter(this.videoRendererFilter, "Video Renderer");
			}
			return false;
		}

		/// <summary>
		/// IAMTVAudio property
		/// </summary>
		protected IAMTVAudio tvAudio = null;

		/// <summary>
		/// Access to TV audio property
		/// </summary>
		public IAMTVAudio TvAudio
		{
			get { return tvAudio; }
			set
			{
				tvAudio = value;
			}
		}

#if DSHOWNET
		/// <summary>
		/// From AMTVAudioEventType
		/// </summary>
		[Flags]
			public enum AMTVAudioEventType
		{
			/// <summary> TV Audio changed event </summary>
			Changed = 0x0001
		}

		/// <summary>
		/// IAMTVAudio interface
		/// </summary>
		[Guid("83EC1C30-23D1-11d1-99E6-00A0C9560266"),
			InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
			public interface IAMTVAudio
		{
			/// <summary> </summary>
			[PreserveSig]
			int GetHardwareSupportedTVAudioModes([Out] out TVAudioMode plModes);

			/// <summary> </summary>
			[PreserveSig]
			int GetAvailableTVAudioModes([Out] out TVAudioMode plModes);

			/// <summary> </summary>
			[PreserveSig]
			int get_TVAudioMode([Out] out TVAudioMode plMode);

			/// <summary> </summary>
			[PreserveSig]
			int put_TVAudioMode([In] TVAudioMode lMode);

			/// <summary> </summary>
			[PreserveSig]
			int RegisterNotificationCallBack(
				[In] IAMTunerNotification pNotify,
				[In] AMTVAudioEventType lEvents
				);

			/// <summary> </summary>
			[PreserveSig]
			int UnRegisterNotificationCallBack([In] IAMTunerNotification pNotify);
		}

		/// <summary>
		/// From TVAudioMode
		/// </summary>
		[Flags]
			public enum TVAudioMode
		{
			/// <summary> Mono </summary>
			Mono = 0x0001,
			/// <summary> Stereo </summary>
			Stereo = 0x0002,
			/// <summary> Language A </summary>
			LangA = 0x0010,
			/// <summary> Language B </summary>
			LangB = 0x0020,
			/// <summary> Language C </summary>
			LangC = 0x0040,
		}
#endif

		private Filter deInterlace = null;
		private IBaseFilter deInterlaceFilter = null;

		/// <summary>
		/// Store De-Interlace filter reference, is used upon adding
		/// de-interlace filter to graph
		/// </summary>
		public Filter DeInterlace
		{
			get { return this.deInterlace; }
			set
			{
				if(value == null)
				{
					if (this.deInterlaceFilter != null)
					{
						this.graphBuilder.RemoveFilter(this.deInterlaceFilter);
						Marshal.ReleaseComObject(this.deInterlaceFilter);
						this.deInterlaceFilter = null;
					}

					if (this.deInterlace != null)
					{
						try
						{
							//Marshal.ReleaseComObject(this.deInterlace);
						}
						catch
						{
							this.DeInterlace = null;
						};
					}
				}
				this.deInterlace = value;
			}
		}

		private bool AddDeInterlaceFilter()
		{
			if(this.DeInterlace != null)
			{
				if(this.deInterlaceFilter != null)
				{
					this.graphBuilder.RemoveFilter(this.deInterlaceFilter);
					Marshal.ReleaseComObject(this.deInterlaceFilter);
					this.deInterlaceFilter = null;
				}
				this.deInterlaceFilter = (IBaseFilter) Marshal.BindToMoniker(this.DeInterlace.MonikerString);
				int hr = this.graphBuilder.AddFilter(this.deInterlaceFilter, "De-Interlace filter");
				if(hr >= 0)
				{
					return true;
				}
			}
			return false;
		}

#if DSHOWNET
		/// <summary>
		/// CLSID_SampleGrabber
		/// </summary>
		[ComImport, Guid("C1F400A0-3F08-11d3-9F0B-006008039E37")]
			public class SampleGrabber
		{
		}
#endif
		/// <summary>
		/// SampleGrabber flag, if false do not insert SampleGrabber in graph
		/// </summary>
		private bool allowSampleGrabber = false;

		/// <summary> grabber filter interface. </summary>
		private IBaseFilter baseGrabFlt = null;

		/// <summary>
		/// Sample Grabber interface
		/// </summary>
		protected ISampleGrabber sampGrabber = null;

		/// <summary>
		/// Check if usage SampleGrabber is allowed
		/// </summary>
		public bool AllowSampleGrabber
		{
			get { return allowSampleGrabber; }
			set { allowSampleGrabber = value; }
		}

		int ISampleGrabberCB.SampleCB( double SampleTime, IMediaSample pSample )
		{
			Trace.Write ("Sample");
			return 0;
		}

		/// <summary>
		/// Disable grabbing next frame
		/// </summary>
		public void DisableEvent()
		{
			if(this.sampGrabber != null)
			{
				this.sampGrabber.SetCallback(null, 0);
			}
		}

		/// <summary> Interface frame event </summary>
		public delegate void HeFrame(System.Drawing.Bitmap BM);
		private delegate void CaptureDone();
		/// <summary> Frame event </summary>
		public event HeFrame FrameEvent2;
		private	byte[] savedArray;
		private	int	bufferedSize;
		private bool frameCaptured = true;

		int ISampleGrabberCB.BufferCB(double SampleTime, IntPtr pBuffer, int BufferLen )
		{
			if(frameCaptured)
			{
				return 0;
			}

            this.bufferedSize = BufferLen;
			
			int stride = this.SnapShotWidth * 3;

			Marshal.Copy( pBuffer, this.savedArray, 0, BufferLen );

			GCHandle handle = GCHandle.Alloc( this.savedArray, GCHandleType.Pinned );
			int scan0 = (int) handle.AddrOfPinnedObject();
			scan0 += (this.SnapShotHeight - 1) * stride;
			Bitmap b = new Bitmap(this.SnapShotWidth, this.SnapShotHeight, -stride, System.Drawing.Imaging.PixelFormat.Format24bppRgb, (IntPtr) scan0 );
			handle.Free();

			// Transfer bitmap upon firing event
			this.FrameEvent2(b);
			return 0;
		}

		/// <summary> Allocate memory space and set SetCallBack </summary>
		public void GrapImg()
		{
			Trace.Write ("IMG");

			if( this.savedArray == null )
			{
				int size = this.snapShotImageSize;
				if( (size < 1000) || (size > 16000000) )
					return;
				this.savedArray = new byte[ size + 64000 ];
			}
			this.sampGrabber.SetCallback( this, 1 );
			this.frameCaptured = false;
		}

		private bool InitSampleGrabber()
		{
			if (this.VideoDevice == null)
			{
				// nothing to do
				return false;
			}

			if (!this.allowSampleGrabber)
			{
				return false;
			}

			this.DisposeSampleGrabber();

			int hr  = 0;

			// Get SampleGrabber if needed
			if(this.sampGrabber == null)
			{
				this.sampGrabber = new SampleGrabber() as ISampleGrabber;
			}

			if(this.sampGrabber == null)
			{
				return false;
			}

#if DSHOWNET
			this.baseGrabFlt	= (IBaseFilter)this.sampGrabber;
#else
            this.baseGrabFlt = sampGrabber as IBaseFilter;
#endif

			if(this.baseGrabFlt == null)
			{
				Marshal.ReleaseComObject(this.sampGrabber);
				this.sampGrabber = null;
			}

			AMMediaType media = new AMMediaType();

			media.majorType	= MediaType.Video;
			media.subType	= MediaSubType.RGB24;
			media.formatPtr = IntPtr.Zero;
			hr = sampGrabber.SetMediaType(media);
			if(hr < 0)
			{
				Marshal.ThrowExceptionForHR(hr);
			}

			hr = graphBuilder.AddFilter(baseGrabFlt, "SampleGrabber");
			if(hr < 0)
			{
				Marshal.ThrowExceptionForHR(hr);
			}

			hr = sampGrabber.SetBufferSamples(false);
			if( hr == 0 )
			{
				hr = sampGrabber.SetOneShot(false);
			}
			if( hr == 0 )
			{
				hr = sampGrabber.SetCallback(null, 0);
			}
			if( hr < 0 )
			{
				Marshal.ThrowExceptionForHR(hr);
			}

			return true;
		}

		private void SetMediaSampleGrabber()
		{
			this.snapShotValid = false;
			if((this.baseGrabFlt != null)&&(this.AllowSampleGrabber))
			{
				AMMediaType media = new AMMediaType();
				VideoInfoHeader videoInfoHeader;
				int hr;

				hr = sampGrabber.GetConnectedMediaType(media);
				if (hr < 0)
				{
					Marshal.ThrowExceptionForHR(hr);
				}
				if ((media.formatType != FormatType.VideoInfo) || (media.formatPtr == IntPtr.Zero))
				{
					throw new NotSupportedException("Unknown Grabber Media Format");
				}

				videoInfoHeader = (VideoInfoHeader)Marshal.PtrToStructure(media.formatPtr, typeof(VideoInfoHeader));
				this.snapShotWidth = videoInfoHeader.BmiHeader.Width;
				this.snapShotHeight = videoInfoHeader.BmiHeader.Height;
				this.snapShotImageSize = videoInfoHeader.BmiHeader.ImageSize;
				Marshal.FreeCoTaskMem(media.formatPtr);
				media.formatPtr = IntPtr.Zero;
				this.snapShotValid = true;
			}

			if (!this.snapShotValid)
			{
				this.snapShotWidth = 0;
				this.snapShotHeight = 0;
				this.snapShotImageSize = 0;
			}
		}

		private int snapShotWidth = 0;
		private int snapShotHeight = 0;
		private int snapShotImageSize = 0;
		private bool snapShotValid = false;

		/// <summary>
		/// Dispose Sample Grabber specific data
		/// </summary>
		public void DisposeSampleGrabber()
		{
			if(this.baseGrabFlt != null)
			{
				try
				{
					this.graphBuilder.RemoveFilter(this.baseGrabFlt);
				}
				catch
				{
				}
				Marshal.ReleaseComObject(this.baseGrabFlt);
				this.baseGrabFlt = null;
			}

			if(this.sampGrabber != null)
			{
				Marshal.ReleaseComObject(this.sampGrabber);
				this.sampGrabber = null;
			}
			this.savedArray =  null;
		}

		/// <summary>
		/// SampleGrabber video width
		/// </summary>
		public int SnapShotWidth
		{
			get { return snapShotWidth; }
		}

		/// <summary>
		/// SampleGrabber video height
		/// </summary>
		public int SnapShotHeight
		{
			get { return snapShotHeight; }
		}

		/// <summary>
		/// Show property page of object 
		/// </summary>
		/// <param name="filter"></param>
		/// <param name="o"></param>
		/// <returns></returns>
		public bool ShowPropertyPage(int filter, Control o)
		{
			ISpecifyPropertyPages specifyPropertyPages =  null;
			String propertyPageName = "";
			switch(filter)
			{
				case 1:
					if(this.videoRendererFilter != null)
					{
						specifyPropertyPages = this.videoRendererFilter as ISpecifyPropertyPages;
						propertyPageName = "Video Renderer";
					}
					break;
				case 2:
					if(this.deInterlaceFilter != null)
					{
						specifyPropertyPages = this.deInterlaceFilter as ISpecifyPropertyPages;
						propertyPageName = "De-Interlacer";
					}
					break;
				default:
					break;
			}
			DirectShowPropertyPage PropertyPage = new DirectShowPropertyPage(propertyPageName, specifyPropertyPages);
			if(PropertyPage != null)
			{
				PropertyPage.Show(o);
				PropertyPage.Dispose();
				return true;
			}
			return false;
		}
//#endif
// Start of Brian's Low new code

		/// <summary>
		/// IAMStreamConfig interface of preview pin. It is not really
		/// common that the preview has such interface, but if it has
		/// such interface it can be used "independent" from the capture
		/// pin interface.
		/// </summary>
		protected IAMStreamConfig	previewStreamConfig = null;	

		/// <summary>
		/// Property Backer: preview capabilities of video device
		/// </summary>
		protected VideoCapabilities	previewCaps = null;

		/// <summary>
		/// Interface to DirectShow utilities for controlling video
		/// </summary>
		public DxUtils dxUtils = null;

		/// <summary>
		///  Gets and sets the frame size used to preview video.
		/// </summary>
		/// <remarks>
		///  To change the frame size, assign a new Size object 
		///  to this property <code>capture.Size = new Size( w, h );</code>
		///  rather than modifying the size in place 
		///  (capture.Size.Width = w;). Not all frame
		///  rates are supported.
		///  
		/// <para>
		///  Not all devices support getting/setting this property.
		///  If this property is not supported, accessing it will
		///  throw and exception. </para>
		/// 
		///  <para>
		///  The frame size used to capture video can be different
		///  than the frame size used in preview.</para>
		///
		/// <para> 
		///  This property cannot be changed while capturing. Changing 
		///  this property while preview is enabled will cause some 
		///  fickering while the internal filter graph is partially
		///  rebuilt. Changing this property while cued will cancel the
		///  cue. Call Cue() again to re-cue the capture. </para>
		/// </remarks>
		public Size PreviewFrameSize
		{
			get
			{
				BitmapInfoHeader bmiHeader;
				bmiHeader = (BitmapInfoHeader) getStreamConfigSetting( previewStreamConfig, "BmiHeader" );
				Size size = new Size( bmiHeader.Width, bmiHeader.Height );
				return( size );
			}
			set
			{
				BitmapInfoHeader bmiHeader;
				bmiHeader = (BitmapInfoHeader) getStreamConfigSetting( previewStreamConfig, "BmiHeader" );
				bmiHeader.Width = value.Width;
				bmiHeader.Height = value.Height;
				setStreamConfigSetting( previewStreamConfig, "BmiHeader", bmiHeader );
			}		
		}

		/// <summary>
		///  Gets and sets the color space (and pixel format) used for capture.
		/// </summary>
		/// <remarks>
		///  Retrieving this propert will temporarily stop the preview. 
		///  This property cannot be changed while capturing or cued. 
		/// </remarks> 
		public DxUtils.ColorSpaceEnum ColorSpace
		{
			get { return getMediaSubType( videoStreamConfig ); }
			set { setMediaSubType( videoStreamConfig, value ); }
		}

		/// <summary>
		///  Gets and sets the color space (and pixel format) used for preview.
		/// </summary>
		/// <remarks>
		///  Retrieving this propert will temporarily stop the preview. 
		///  This property cannot be changed while capturing or cued. 
		/// </remarks> 
		public DxUtils.ColorSpaceEnum PreviewColorSpace
		{
			get { return getMediaSubType( previewStreamConfig ); }
			set { setMediaSubType( previewStreamConfig, value ); }
		}

		private DxUtils.ColorSpaceEnum getMediaSubType( IAMStreamConfig streamConfig )
		{
			if(this.dxUtils == null)
			{
				return DxUtils.ColorSpaceEnum.RGB24;
			}

			// Derender the graph. For some drivers these settings
			// cannot be read while the graph is built
			if ( streamConfig == null )
			{
				throw new NotSupportedException();
			}
			assertStopped();
			derenderGraph();

			DxUtils.ColorSpaceEnum retval = dxUtils.getMediaSubType(streamConfig);

			renderGraph();
			startPreviewIfNeeded();

			return retval;
		}

		private void setMediaSubType( IAMStreamConfig streamConfig, DxUtils.ColorSpaceEnum newValue )
		{
			if(this.dxUtils == null)
			{
				return;
			}

			if(videoStreamConfig == null)
			{
				throw new NotSupportedException();
			}
			assertStopped();
			derenderGraph();

			dxUtils.setMediaSubType(streamConfig, newValue);

			renderGraph();
			startPreviewIfNeeded();
		}

		/// <summary>
		///  The preview capabilities of the video device.
		/// </summary>
		/// <remarks>
		///  It may be required to cue the capture (see <see cref="Cue"/>) 
		///  before all capabilities are correctly reported. If you 
		///  have such a device, the developer would be interested to
		///  hear from you.
		/// 
		/// <para>
		///  The information contained in this property is retrieved and
		///  cached the first time this property is accessed. Future
		///  calls to this property use the cached results. This was done 
		///  for performance. </para>
		///  
		/// <para>
		///  However, this means <b>you may get different results depending 
		///  on when you access this property first</b>. If you are experiencing 
		///  problems, try accessing the property immediately after creating 
		///  the Capture class or immediately after setting the video and 
		///  audio compressors. Also, inform the developer. </para>
		/// </remarks>
		public VideoCapabilities PreviewCaps 
		{ 
			get 
			{ 
				if ( previewCaps == null )
				{
					if ( previewStreamConfig != null )
					{
						try 
						{ 
							previewCaps = new VideoCapabilities( previewStreamConfig ); 
						}
						catch ( Exception ex ) { Debug.WriteLine( "VideoCaps: unable to create previewCaps." + ex.ToString() ); }
					}
				}
				return( previewCaps ); 
			} 
		}
// End of Brian's Low new code
	}
}

