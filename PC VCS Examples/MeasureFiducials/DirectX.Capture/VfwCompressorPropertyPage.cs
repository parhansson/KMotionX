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
using System.Windows.Forms;
#if DSHOWNET
using DShowNET;
#else
using DirectShowLib;
#endif

namespace DirectX.Capture
{
	/// <summary>
	///  The property page to configure a Video for Windows compliant
	///  compression codec. Most compressors support this property page
	///  rather than a DirectShow property page. Also, most compressors
	///  do not support the IAMVideoCompression interface so this
	///  property page is the only method to configure a compressor. 
	/// </summary>
	public class VfwCompressorPropertyPage : PropertyPage
	{

		// ---------------- Properties --------------------

		/// <summary> Video for Windows compression dialog interface </summary>
		protected IAMVfwCompressDialogs vfwCompressDialogs = null;

		/// <summary> 
		///  Get or set the state of the property page. This is used to save
		///  and restore the user's choices without redisplaying the property page.
		///  This property will be null if unable to retrieve the property page's
		///  state.
		/// </summary>
		/// <remarks>
		///  After showing this property page, read and store the value of 
		///  this property. At a later time, the user's choices can be 
		///  reloaded by setting this property with the value stored earlier. 
		///  Note that some property pages, after setting this property, 
		///  will not reflect the new state. However, the filter will use the
		///  new settings.
		/// </remarks>
		public override byte[] State
		{
			get 
			{
#if DSHOWNET 
				byte[] data = null;
				int size = 0;

				int hr = vfwCompressDialogs.GetState( null, ref size );
				if ( ( hr == 0 ) && ( size > 0 ) )
				{
					data = new byte[size];
					hr = vfwCompressDialogs.GetState( data, ref size );
					if ( hr != 0 ) data = null;
				}
				return( data );
#else
				IntPtr data = IntPtr.Zero;
				int size = 0;

				int hr = vfwCompressDialogs.GetState( IntPtr.Zero, ref size );
				if ( ( hr == 0 ) && ( size > 0 ) )
				{
					int sizeIntPtr = (size + 3)/ 4;
					data = Marshal.AllocCoTaskMem(sizeIntPtr);
                    hr = vfwCompressDialogs.GetState(data, ref size);
					if ( hr != 0 ) data = IntPtr.Zero;
				}
				return (byte[])Marshal.PtrToStructure(data, typeof(byte[]));
#endif
			}
			set 
			{
#if DSHOWNET  
				int hr = vfwCompressDialogs.SetState( value, value.Length );
				if ( hr != 0 ) Marshal.ThrowExceptionForHR( hr );
#else
				IntPtr data = IntPtr.Zero;
				Marshal.StructureToPtr(value, data, true);
				int hr = vfwCompressDialogs.SetState( data, value.Length );
				if ( hr != 0 ) Marshal.ThrowExceptionForHR( hr );
#endif
			}
		}


		// ---------------- Constructors --------------------

		/// <summary> Constructor </summary>
		public VfwCompressorPropertyPage(string name, IAMVfwCompressDialogs compressDialogs)
		{
			Name = name;
			SupportsPersisting = true;
			this.vfwCompressDialogs = compressDialogs;
		}



		// ---------------- Public Methods --------------------

		/// <summary> 
		///  Show the property page. Some property pages cannot be displayed 
		///  while previewing and/or capturing. 
		/// </summary>
		public override void Show(Control owner)
		{
			vfwCompressDialogs.ShowDialog( VfwCompressDialogs.Config, owner.Handle );
		}

	}
}
