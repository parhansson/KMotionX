using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using KMotion_dotNet;

namespace KFlopWebNC.Model
{ 
    [Serializable()]
    public class CommandData
    {
        #region Fields
        private MCODE_TYPE _Type = MCODE_TYPE.M_Action_None;
        private int _Index = -1;
        private string _Name = "";

        private int _Bit1 = 0;
        private int _Bit2 = 0;
        private bool _State1 = false;
        private bool _State2 = false;

        private int _DacScale = -1;
        private int _DacOffset = -1;
        private int _DacMin = -1;
        private int _DacMax = -1;

        private int _Thread = -1;
        private int _VAR = -1;
        private string _FileName = ""; 
        #endregion
 
        #region Properties
        public MCODE_TYPE Type
        {
            get
            {
                return _Type;
            }
            set
            {
                _Type = value;
            }
        }
        public int Index
        {
            get
            {
                return _Index;
            }
            set
            {
                _Index = value;
            }
        }
        public string Name
        {
            get
            {
                return _Name;
            }
            set
            {
                _Name = value;
            }
        }

        public int Bit1
        {
            get
            {
                return _Bit1;
            }
            set
            {
                _Bit1 = value;
            }
        }
        public int Bit2
        {
            get
            {
                return _Bit2;
            }
            set
            {
                _Bit2 = value;
            }
        }

        public bool State1
        {
            get
            {
                return _State1;
            }
            set
            {
                _State1 = value;
            }
        }
        public bool State2
        {
            get
            {
                return _State2;
            }
            set
            {
                _State2 = value;
            }
        }



        public int DacScale
        {
            get
            {
                return _DacScale;
            }
            set
            {
                _DacScale = value;
            }
        }
        public int DacOffset
        {
            get
            {
                return _DacOffset;
            }
            set
            {
                _DacOffset = value;
            }
        }
        public int DacMin
        {
            get
            {
                return _DacMin;
            }
            set
            {
                _DacMin = value;
            }
        }
        public int DacMax
        {
            get
            {
                return _DacMax;
            }
            set
            {
                _DacMax = value;
            }
        }


        public int Thread
        {
            get
            {
                return _Thread;
            }
            set
            {
                _Thread = value;
            }
        }
        public int VAR
        {
            get
            {
                return _VAR;
            }
            set
            {
                _VAR = value;
            }
        }

        public string FileName
        {
            get
            {
                return _FileName;
            }
            set
            {
                _FileName = value;
            }
        }  
        #endregion
    }
}
