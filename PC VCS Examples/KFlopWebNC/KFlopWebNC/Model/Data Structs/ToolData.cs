using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KFlopWebNC.Model
{
    [Serializable()]
    public class ToolData
    {
        #region Fields
        private int _Index = -1;
        private string _ToolName = "";
        private int _SlotNumber = -1;
        private int _ID = -1;
        private double _LengthOffset = 0.0;
        private double _DiameterOffset = 0.0;
        private double _XOffset = 0.0;
        private double _YOffset = 0.0;
        #endregion

        #region Properties
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
        public string ToolName
        {
            get
            {
                return _ToolName;
            }
            set
            {
                _ToolName = value;
            }
        }
        public int SlotNumber
        {
            get
            {
                return _SlotNumber;
            }
            set
            {
                _SlotNumber = value;
            }
        }
        public int ID
        {
            get
            {
                return _ID;
            }
            set
            {
                _ID = value;
            }
        }
        public double LengthOffset
        {
            get
            {
                return _LengthOffset;
            }
            set
            {
                _LengthOffset = value;
            }
        }
        public double DiameterOffset
        {
            get
            {
                return _DiameterOffset;
            }
            set
            {
                _DiameterOffset = value;
            }
        }
        public double XOffset
        {
            get
            {
                return _XOffset;
            }
            set
            {
                _XOffset = value;
            }
        }
        public double YOffset
        {
            get
            {
                return _YOffset;
            }
            set
            {
                _YOffset = value;
            }
        }
        #endregion
    }
}
