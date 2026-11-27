using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KFlopWebNC.Model
{
    public class InterpreterData : SerializeBase<InterpreterData>
    {
        #region Fields
        private double _BreakAngle = 10.0;
        private double _LookAhead = 3.0;
        private double _CollinearTolerance = 0.0005;
        private double _CornerTolerance = 0.0005;
        private double _FacetAngle = 1.0;

        private int _XaxisIndex = 0;
        private double _XaxisCPI = 10000.0;
        private double _XaxisAccel = 16.0;
        private double _XaxisVel = 30.0;
        private double _XaxisJogVel = 30.0;

        private int _YaxisIndex = 1;
        private double _YaxisCPI = 10000.0;
        private double _YaxisAccel = 16.0;
        private double _YaxisVel = 30.0;
        private double _YaxisJogVel = 30.0;

        private int _ZaxisIndex = 2;
        private double _ZaxisCPI = 10000.0;
        private double _ZaxisAccel = 16.0;
        private double _ZaxisVel = 30.0;
        private double _ZaxisJogVel = 30.0;

        private int _AaxisIndex = 3;
        private double _AaxisCPI = 10000.0;
        private double _AaxisAccel = 16.0;
        private double _AaxisVel = 30.0;
        private double _AaxisJogVel = 30.0;

        private int _BaxisIndex = 4;
        private double _BaxisCPI = 10000.0;
        private double _BaxisAccel = 16.0;
        private double _BaxisVel = 30.0;
        private double _BaxisJogVel = 30.0;

        private int _CaxisIndex = 5;
        private double _CaxisCPI = 10000.0;
        private double _CaxisAccel = 16.0;
        private double _CaxisVel = 30.0;
        private double _CaxisJogVel = 30.0;

        private List<ToolData> _Tools = new List<ToolData>();
        private List<OffsetData> _Offsets = new List<OffsetData>();
        private List<CommandData> _Commands = new List<CommandData>();
        #endregion

        #region Properties
        public double BreakAngle
        {
            get
            {
                return _BreakAngle;
            }
            set
            {
                _BreakAngle = value;
            }
        }

        public double LookAhead
        {
            get
            {
                return _LookAhead;
            }
            set
            {
                _LookAhead = value;
            }
        }
        public double CollinearTolerance
        {
            get
            {
                return _CollinearTolerance;
            }
            set
            {
                _CollinearTolerance = value;
            }
        }
        public double CornerTolerance
        {
            get
            {
                return _CornerTolerance;
            }
            set
            {
                _CornerTolerance = value;
            }
        }
        public double FacetAngle
        {
            get
            {
                return _FacetAngle;
            }
            set
            {
                _FacetAngle = value;
            }
        }



        public int XaxisIndex
        {
            get
            {
                return _XaxisIndex;
            }
            set
            {
                _XaxisIndex = value;
            }
        }
        public double XaxisCPI
        {
            get
            {
                return _XaxisCPI;
            }
            set
            {
                _XaxisCPI = value;
            }
        }
        public double XaxisAccel
        {
            get
            {
                return _XaxisAccel;
            }
            set
            {
                _XaxisAccel = value;
            }
        }
        public double XaxisVel
        {
            get
            {
                return _XaxisVel;
            }
            set
            {
                _XaxisVel = value;
            }
        }
        public double XaxisJogVel
        {
            get
            {
                return _XaxisJogVel;
            }
            set
            {
                _XaxisJogVel = value;
            }
        }

        public int YaxisIndex
        {
            get
            {
                return _YaxisIndex;
            }
            set
            {
                _YaxisIndex = value;
            }
        }
        public double YaxisCPI
        {
            get
            {
                return _YaxisCPI;
            }
            set
            {
                _YaxisCPI = value;
            }
        }
        public double YaxisAccel
        {
            get
            {
                return _YaxisAccel;
            }
            set
            {
                _YaxisAccel = value;
            }
        }
        public double YaxisVel
        {
            get
            {
                return _YaxisVel;
            }
            set
            {
                _YaxisVel = value;
            }
        }
        public double YaxisJogVel
        {
            get
            {
                return _YaxisJogVel;
            }
            set
            {
                _YaxisJogVel = value;
            }
        }

        public int ZaxisIndex
        {
            get
            {
                return _ZaxisIndex;
            }
            set
            {
                _ZaxisIndex = value;
            }
        }
        public double ZaxisCPI
        {
            get
            {
                return _ZaxisCPI;
            }
            set
            {
                _ZaxisCPI = value;
            }
        }
        public double ZaxisAccel
        {
            get
            {
                return _ZaxisAccel;
            }
            set
            {
                _ZaxisAccel = value;
            }
        }
        public double ZaxisVel
        {
            get
            {
                return _ZaxisVel;
            }
            set
            {
                _ZaxisVel = value;
            }
        }
        public double ZaxisJogVel
        {
            get
            {
                return _ZaxisJogVel;
            }
            set
            {
                _ZaxisJogVel = value;
            }
        }

        public int AaxisIndex
        {
            get
            {
                return _AaxisIndex;
            }
            set
            {
                _AaxisIndex = value;
            }
        }
        public double AaxisCPI
        {
            get
            {
                return _AaxisCPI;
            }
            set
            {
                _AaxisCPI = value;
            }
        }
        public double AaxisAccel
        {
            get
            {
                return _AaxisAccel;
            }
            set
            {
                _AaxisAccel = value;
            }
        }
        public double AaxisVel
        {
            get
            {
                return _AaxisVel;
            }
            set
            {
                _AaxisVel = value;
            }
        }
        public double AaxisJogVel
        {
            get
            {
                return _AaxisJogVel;
            }
            set
            {
                _AaxisJogVel = value;
            }
        }

        public int BaxisIndex
        {
            get
            {
                return _BaxisIndex;
            }
            set
            {
                _BaxisIndex = value;
            }
        }
        public double BaxisCPI
        {
            get
            {
                return _BaxisCPI;
            }
            set
            {
                _BaxisCPI = value;
            }
        }
        public double BaxisAccel
        {
            get
            {
                return _BaxisAccel;
            }
            set
            {
                _BaxisAccel = value;
            }
        }
        public double BaxisVel
        {
            get
            {
                return _BaxisVel;
            }
            set
            {
                _BaxisVel = value;
            }
        }
        public double BaxisJogVel
        {
            get
            {
                return _BaxisJogVel;
            }
            set
            {
                _BaxisJogVel = value;
            }
        }

        public int CaxisIndex
        {
            get
            {
                return _CaxisIndex;
            }
            set
            {
                _CaxisIndex = value;
            }
        }
        public double CaxisCPI
        {
            get
            {
                return _CaxisCPI;
            }
            set
            {
                _CaxisCPI = value;
            }
        }
        public double CaxisAccel
        {
            get
            {
                return _CaxisAccel;
            }
            set
            {
                _CaxisAccel = value;
            }
        }
        public double CaxisVel
        {
            get
            {
                return _CaxisVel;
            }
            set
            {
                _CaxisVel = value;
            }
        }
        public double CaxisJogVel
        {
            get
            {
                return _CaxisJogVel;
            }
            set
            {
                _CaxisJogVel = value;
            }
        }



        public List<ToolData> Tools
        {
            get
            {
                return _Tools;
            }
            set
            {
                _Tools = value;
            }
        }

        public List<OffsetData> Offsets
        {
            get
            {
                return _Offsets;
            }
            set
            {
                _Offsets = value;
            }
        }

        public List<CommandData> Commands
        {
            get
            {
                return _Commands;
            }
            set
            {
                _Commands = value;
            }
        } 
        #endregion
    }
}
