// ------------------------------------------------------------------
// DirectX.Capture
//
// History:
//	2009-Feb-27	HV		- created
//  - Added Brian's Low 'december 2003' code
//
// Copyright (C) 2009 Hans Vosman
// ------------------------------------------------------------------
using System;

namespace DirectX.Capture
{
	using System;

	[AttributeUsage(AttributeTargets.All)]
	public class LabelAttribute : Attribute
	{
		public readonly string Label;

		public LabelAttribute(string label)
		{
			Label = label;
		}

		public static string FromMember(object o)
		{
			return ((LabelAttribute)
				o.GetType().GetMember(o.ToString())[0].GetCustomAttributes(typeof(LabelAttribute), false)[0]).Label;
		}

		public static string FromType(object o)
		{
			return ((LabelAttribute)
				o.GetType().GetCustomAttributes(typeof(LabelAttribute), false)[0]).Label;
		}
	}

}
