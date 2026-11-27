using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KFlopWebNC.DOM_Helpers
{
    public static class Elements
    {
        public static string IsConnected
        {
            get
            {
                return "connectedvalue";
            }
        }
        public static string VersionDisplay
        {
            get
            {
                return "kflopversionvalue";
            }
        }
 

        #region Axis Dest Values
        public static string XDest
        {
            get
            {
                return "xdest";
            }
        }
        public static string YDest
        {
            get
            {
                return "ydest";
            }
        }
        public static string ZDest
        {
            get
            {
                return "zdest";
            }
        }

        public static string ADest
        {
            get
            {
                return "adest";
            }
        }
        public static string BDest
        {
            get
            {
                return "bdest";
            }
        }
        public static string CDest
        {
            get
            {
                return "cdest";
            }
        } 
        #endregion


        #region Axis Pos Values
        public static string XPos
        {
            get
            {
                return "xpos";
            }
        }
        public static string YPos
        {
            get
            {
                return "ypos";
            }
        }
        public static string ZPos
        {
            get
            {
                return "zpos";
            }
        }

        public static string APos
        {
            get
            {
                return "apos";
            }
        }
        public static string BPos
        {
            get
            {
                return "bpos";
            }
        }
        public static string CPos
        {
            get
            {
                return "cpos";
            }
        } 
        #endregion

        #region Axis Enable
        public static string XEnable
        {
            get
            {
                return "xenable";
            }
        }
        public static string YEnable
        {
            get
            {
                return "yenable";
            }
        }
        public static string ZEnable
        {
            get
            {
                return "zenable";
            }
        }

        public static string AEnable
        {
            get
            {
                return "aenable";
            }
        }
        public static string BEnable
        {
            get
            {
                return "benable";
            }
        }
        public static string CEnable
        {
            get
            {
                return "cenable";
            }
        } 
        #endregion

        #region Axis Zero
        public static string XZero
        {
            get
            {
                return "xzero";
            }
        }
        public static string YZero
        {
            get
            {
                return "yzero";
            }
        }
        public static string ZZero
        {
            get
            {
                return "zzero";
            }
        }

        public static string AZero
        {
            get
            {
                return "azero";
            }
        }
        public static string BZero
        {
            get
            {
                return "bzero";
            }
        }
        public static string CZero
        {
            get
            {
                return "czero";
            }
        }
        #endregion


        #region Axis Jog Negative
        public static string XJogNeg
        {
            get
            {
                return "xjogneg";
            }
        }
        public static string YJogNeg
        {
            get
            {
                return "yjogneg";
            }
        }
        public static string ZJogNeg
        {
            get
            {
                return "zjogneg";
            }
        }

        public static string AJogNeg
        {
            get
            {
                return "ajogneg";
            }
        }
        public static string BJogNeg
        {
            get
            {
                return "bjogneg";
            }
        }
        public static string CJogNeg
        {
            get
            {
                return "cjogneg";
            }
        }
        #endregion

        #region Axis Jog Negative
        public static string XJogPos
        {
            get
            {
                return "xjogpos";
            }
        }
        public static string YJogPos
        {
            get
            {
                return "yjogpos";
            }
        }
        public static string ZJogPos
        {
            get
            {
                return "zjogpos";
            }
        }

        public static string AJogPos
        {
            get
            {
                return "ajogpos";
            }
        }
        public static string BJogPos
        {
            get
            {
                return "bjogpos";
            }
        }
        public static string CJogPos
        {
            get
            {
                return "cjogpos";
            }
        }
        #endregion
 
        #region Axis Jog Slider
        public static string XJogSlider
        {
            get
            {
                return "xjogslider";
            }
        }
        public static string YJogSlider
        {
            get
            {
                return "yjogslider";
            }
        }
        public static string ZJogSlider
        {
            get
            {
                return "zjogslider";
            }
        }

        public static string AJogSlider
        {
            get
            {
                return "ajogslider";
            }
        }
        public static string BJogSlider
        {
            get
            {
                return "bjogslider";
            }
        }
        public static string CJogSlider
        {
            get
            {
                return "cjogslider";
            }
        }



        public static string ValueXJogPercent
        {
            get
            {
                return "xjogpercentvalue";
            }
        }
        public static string ValueYJogPercent
        {
            get
            {
                return "yjogpercentvalue";
            }
        }
        public static string ValueZJogPercent
        {
            get
            {
                return "zjogpercentvalue";
            }
        }

        public static string ValueAJogPercent
        {
            get
            {
                return "ajogpercentvalue";
            }
        }
        public static string ValueBJogPercent
        {
            get
            {
                return "bjogpercentvalue";
            }
        }
        public static string ValueCJogPercent
        {
            get
            {
                return "cjogpercentvalue";
            }
        }
        #endregion


        #region G Code
        public static string GcodeDisplay
        {
            get
            {
                return "gcodedisplay";
            }
        }
        public static string GCodeProgramName
        {
            get
            {
                return "gcodeprogramname";
            }
        }
        public static string LoadGCode
        {
            get
            {
                return "loadgcode";
            }
        }
        public static string RewindGCode
        {
            get
            {
                return "rewindgcode";
            }
        }
        public static string RunFromLine
        {
            get
            {
                return "runfromline";
            }
        }
        public static string GCodeLineToRun
        {
            get
            {
                return "gcodelinetorun";
            }
        }
        public static string Simulate
        {
            get
            {
                return "simulate";
            }
        }
        public static string BlockDelete
        {
            get
            {
                return "blockdelete";
            }
        } 
        #endregion
 
        #region MDI
        public static string MDICommand
        {
            get
            {
                return "mdicommand";
            }
        }
        public static string ValueFRO
        {
            get
            {
                return "frovalue";
            }
        }


        public static string SliderFRO
        {
            get
            {
                return "froslider";
            }
        } 
        #endregion

        #region User Buttons
        public static string UserButton1
        {
            get
            {
                return "userbutton1";
            }
        }
        public static string UserButton2
        {
            get
            {
                return "userbutton2";
            }
        }
        public static string UserButton3
        {
            get
            {
                return "userbutton3";
            }
        }
        public static string UserButton4
        {
            get
            {
                return "userbutton4";
            }
        }
        public static string UserButton5
        {
            get
            {
                return "userbutton5";
            }
        }
        public static string UserButton6
        {
            get
            {
                return "userbutton6";
            }
        }
        public static string UserButton7
        {
            get
            {
                return "userbutton7";
            }
        }
        public static string UserButton8
        {
            get
            {
                return "userbutton8";
            }
        }
        public static string UserButton9
        {
            get
            {
                return "userbutton9";
            }
        }
        public static string UserButton10
        {
            get
            {
                return "userbutton10";
            }
        } 
        #endregion
 
        #region Main Commands
        public static string CycleStart
        {
            get
            {
                return "cyclestart";
            }
        }
        public static string CyclePause
        {
            get
            {
                return "cyclepause";
            }
        }
        public static string Stop
        {
            get
            {
                return "stop";
            }
        } 
        #endregion
    }
}
