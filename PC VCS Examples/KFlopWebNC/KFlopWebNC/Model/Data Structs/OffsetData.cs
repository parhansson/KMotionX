using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KFlopWebNC.Model
{
    [Serializable()]
    public class OffsetData
    {

        #region Fields
        private int _Index = -1;
        private string _Name = "";

        private double _X = 0.0;
        private double _Y = 0.0;
        private double _Z = 0.0;
        private double _A = 0.0;
        private double _B = 0.0;
        private double _C = 0.0; 
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

        public double X
        {
            get
            {
                return _X;
            }
            set
            {
                _X = value;
            }
        }
        public double Y
        {
            get
            {
                return _Y;
            }
            set
            {
                _Y = value;
            }
        }
        public double Z
        {
            get
            {
                return _Z;
            }
            set
            {
                _Z = value;
            }
        }
        public double A
        {
            get
            {
                return _A;
            }
            set
            {
                _A = value;
            }
        }
        public double B
        {
            get
            {
                return _B;
            }
            set
            {
                _B = value;
            }
        }
        public double C
        {
            get
            {
                return _C;
            }
            set
            {
                _C = value;
            }
        } 
        #endregion
    }
}
