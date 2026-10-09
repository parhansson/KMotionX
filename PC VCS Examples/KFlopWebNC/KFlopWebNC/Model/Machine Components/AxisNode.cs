using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;

namespace KFlopWebNC.Model
{
    public class AxisNode
    {
        public KM_Axis Axis { get; set; }
        public double JogPercent { get; set; }
        public double JogVelocity { get; set; }

        public AxisNode(KM_Axis axis)
        {
            Axis = axis;
            JogPercent = 1;
            JogVelocity = 50;
        }
    }
}
