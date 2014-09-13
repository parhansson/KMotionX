/*
Copyright © 2011 Brad Murry
All rights reserved.

BSD License:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



KM_CoordMotion_MotionParams.cs 
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace KMotion_dotNet
{

    public partial class KM_MotionParams
    {
        private IntPtr _InstanceHandle = new IntPtr(0L);

        #region Properties
        /// <summary>
        /// Max angle before Trajectory Planner locks the segment list and optimizes next entities
        /// </summary>
        public double BreakAngle
        {
            get
            {
                return Get_BreakAngle();
            }
            set
            {
                Set_BreakAngle(value);
            }
        }
        /// <summary>
        /// If the angle between consecutive linear segments is less than this value, they will be treated as the same segment
        /// </summary>
        public double CollinearTolerance
        {
            get
            {
                return Get_CollinearTol();
            }
            set
            {
                Set_CollinearTol(value);
            }
        }
        /// <summary>
        /// Maximum variance when smoothed cornering is applied
        /// </summary>
        public double CornerTolerance
        {
            get
            {
                return Get_CornerTol();
            }
            set
            {
                Set_CornerTol(value);
            }
        }
        /// <summary>
        /// Max anglular deviation for arc segments
        /// </summary>
        public double FacetAngle
        {
            get
            {
                return Get_FacetAngle();
            }
            set
            {
                Set_FacetAngle(value);
            }
        }
        /// <summary>
        /// How many seconds can be previewed for Trajectory Planning optimization
        /// </summary>
        public double TPLookahead
        {
            get
            {
                return Get_TPLookahead();
            }
            set
            {
                Set_TPLookahead(value);
            }
        }
        /// <summary>
        /// Radius Associated with Axis A
        /// </summary>
        public double RadiusA
        {
            get
            {
                return Get_RadiusA();
            }
            set
            {
                Set_RadiusA(value);
            }
        }

        /// <summary>
        /// Radius Associated with Axis B
        /// </summary>
        public double RadiusB
        {
            get
            {
                return Get_RadiusB();
            }
            set
            {
                Set_RadiusB(value);
            }
        }

        /// <summary>
        /// Radius Associated with Axis C
        /// </summary>
        public double RadiusC
        {
            get
            {
                return Get_RadiusC();
            }
            set
            {
                Set_RadiusC(value);
            }
        }

        
        
        /// <summary>
        /// Maximum Acceleration for the mapped "X" axis
        /// </summary>
        public double MaxAccelX
        {
            get
            {
                return Get_MaxAccelX();
            }
            set
            {
                Set_MaxAccelX(value);
            }
        }
        /// <summary>
        /// Maximum Acceleration for the mapped "Y" axis
        /// </summary>
        public double MaxAccelY
        {
            get
            {
                return Get_MaxAccelY();
            }
            set
            {
                Set_MaxAccelY(value);
            }
        }
        /// <summary>
        /// Maximum Acceleration for the mapped "Z" axis
        /// </summary>
        public double MaxAccelZ
        {
            get
            {
                return Get_MaxAccelZ();
            }
            set
            {
                Set_MaxAccelZ(value);
            }
        }
        /// <summary>
        /// Maximum Acceleration for the mapped "A" axis
        /// </summary>
        public double MaxAccelA
        {
            get
            {
                return Get_MaxAccelA();
            }
            set
            {
                Set_MaxAccelA(value);
            }
        }
        /// <summary>
        /// Maximum Acceleration for the mapped "B" axis
        /// </summary>
        public double MaxAccelB
        {
            get
            {
                return Get_MaxAccelB();
            }
            set
            {
                Set_MaxAccelB(value);
            }
        }
        /// <summary>
        /// Maximum Acceleration for the mapped "C" axis
        /// </summary>
        public double MaxAccelC
        {
            get
            {
                return Get_MaxAccelC();
            }
            set
            {
                Set_MaxAccelC(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "X" axis
        /// </summary>
        public double MaxVelX
        {
            get
            {
                return Get_MaxVelX();
            }
            set
            {
                Set_MaxVelX(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "Y" axis
        /// </summary>
        public double MaxVelY
        {
            get
            {
                return Get_MaxVelY();
            }
            set
            {
                Set_MaxVelY(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "Z" axis
        /// </summary>
        public double MaxVelZ
        {
            get
            {
                return Get_MaxVelZ();
            }
            set
            {
                Set_MaxVelZ(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "A" axis
        /// </summary>
        public double MaxVelA
        {
            get
            {
                return Get_MaxVelA();
            }
            set
            {
                Set_MaxVelA(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "B" axis
        /// </summary>
        public double MaxVelB
        {
            get
            {
                return Get_MaxVelB();
            }
            set
            {
                Set_MaxVelB(value);
            }
        }
        /// <summary>
        /// Maximum Velocity for the mapped "C" axis
        /// </summary>
        public double MaxVelC
        {
            get
            {
                return Get_MaxVelC();
            }
            set
            {
                Set_MaxVelC(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "X" axis
        /// </summary>
        public double CountsPerInchX
        {
            get
            {
                return Get_CountsPerInchX();
            }
            set
            {
                Set_CountsPerInchX(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "Y" axis
        /// </summary>
        public double CountsPerInchY
        {
            get
            {
                return Get_CountsPerInchY();
            }
            set
            {
                Set_CountsPerInchY(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "Z" axis
        /// </summary>
        public double CountsPerInchZ
        {
            get
            {
                return Get_CountsPerInchZ();
            }
            set
            {
                Set_CountsPerInchZ(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "A" axis
        /// </summary>
        public double CountsPerInchA
        {
            get
            {
                return Get_CountsPerInchA();
            }
            set
            {
                Set_CountsPerInchA(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "B" axis
        /// </summary>
        public double CountsPerInchB
        {
            get
            {
                return Get_CountsPerInchB();
            }
            set
            {
                Set_CountsPerInchB(value);
            }
        }
        /// <summary>
        /// Counts/Steps per Inch for the mapped "C" axis
        /// </summary>
        public double CountsPerInchC
        {
            get
            {
                return Get_CountsPerInchC();
            }
            set
            {
                Set_CountsPerInchC(value);
            }
        }
        /// <summary>
        /// Longest allowable segment length before dividing into smaller segments
        /// </summary>
        public double MaxLinearLength
        {
            get
            {
                return Get_MaxLinearLength();
            }
            set
            {
                Set_MaxLinearLength(value);
            }
        }
        /// <summary>
        /// Whether Axis A is in units of Degrees
        /// </summary>
        public bool DegreesA
        {
            get
            {
                return Get_DegreesA();
            }
            set
            {
                Set_DegreesA(value);
            }
        }
        /// <summary>
        /// Whether Axis B is in units of Degrees
        /// </summary>
        public bool DegreesB
        {
            get
            {
                return Get_DegreesB();
            }
            set
            {
                Set_DegreesB(value);
            }
        }
        /// <summary>
        /// Whether Axis C is in units of Degrees
        /// </summary>
        public bool DegreesC
        {
            get
            {
                return Get_DegreesC();
            }
            set
            {
                Set_DegreesC(value);
            }
        }
        #endregion

        /// <summary>
        /// Primary constructor for the KM_MotionParams object
        /// </summary>
        /// <param name="handle">Pointer to the associated CoordMotion object</param>
        public KM_MotionParams(IntPtr handle)
        {
            _InstanceHandle = handle;
        }

        #region Property Dispatchers
        /// <summary>
        /// Property getter Accessor for BreakAngle
        /// </summary>
        /// <returns>Current BreakAngle</returns>
        private double Get_BreakAngle()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_BreakAngle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_BreakAngle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_BreakAngle"));
            }
        }
        /// <summary>
        /// Property setter Accessor for BreakAngle
        /// </summary>
        /// <param name="value">BreakAngle to set</param>
        private void Set_BreakAngle(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_BreakAngle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_BreakAngle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_BreakAngle"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CollinearTol
        /// </summary>
        /// <returns>Current CollinearTol</returns>
        private double Get_CollinearTol()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CollinearTol"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CollinearTol"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CollinearTol"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CollinearTol
        /// </summary>
        /// <param name="value">CollinearTol to set</param>
        private void Set_CollinearTol(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CollinearTol"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CollinearTol"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CollinearTol"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CornerTol
        /// </summary>
        /// <returns>Current CornerTol</returns>
        private double Get_CornerTol()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CornerTol"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CornerTol"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CornerTol"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CornerTol
        /// </summary>
        /// <param name="value">CornerTol to set</param>
        private void Set_CornerTol(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CornerTol"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CornerTol"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CornerTol"));
            }
        }

        /// <summary>
        /// Property getter Accessor for FacetAngle
        /// </summary>
        /// <returns>Current FacetAngle</returns>
        private double Get_FacetAngle()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_FacetAngle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_FacetAngle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_FacetAngle"));
            }
        }

        /// <summary>
        /// Property setter Accessor for FacetAngle
        /// </summary>
        /// <param name="value">FacetAngle to set</param>
        private void Set_FacetAngle(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_FacetAngle"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_FacetAngle"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_FacetAngle"));
            }
        }

        /// <summary>
        /// Property getter Accessor for TPLookahead
        /// </summary>
        /// <returns>Current TPLookahead</returns>
        private double Get_TPLookahead()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_TPLookahead"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_TPLookahead"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_TPLookahead"));
            }
        }

        /// <summary>
        /// Property setter Accessor for TPLookahead
        /// </summary>
        /// <param name="value">TPLookahead to set</param>
        private void Set_TPLookahead(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_TPLookahead"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_TPLookahead"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_TPLookahead"));
            }
        }

        /// <summary>
        /// Property getter Accessor for RadiusA
        /// </summary>
        /// <returns>Current RadiusA</returns>
        private double Get_RadiusA()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_RadiusA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_RadiusA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_RadiusA"));
            }
        }

        /// <summary>
        /// Property setter Accessor for RadiusA
        /// </summary>
        /// <param name="value">RadiusA to set</param>
        private void Set_RadiusA(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_RadiusA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_RadiusA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_RadiusA"));
            }
        }


        /// <summary>
        /// Property getter Accessor for RadiusB
        /// </summary>
        /// <returns>Current RadiusB</returns>
        private double Get_RadiusB()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_RadiusB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_RadiusB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_RadiusB"));
            }
        }

        /// <summary>
        /// Property setter Accessor for RadiusB
        /// </summary>
        /// <param name="value">RadiusB to set</param>
        private void Set_RadiusB(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_RadiusB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_RadiusB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_RadiusB"));
            }
        }


        /// <summary>
        /// Property getter Accessor for RadiusC
        /// </summary>
        /// <returns>Current RadiusC</returns>
        private double Get_RadiusC()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_RadiusC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_RadiusC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_RadiusC"));
            }
        }

        /// <summary>
        /// Property setter Accessor for RadiusC
        /// </summary>
        /// <param name="value">RadiusC to set</param>
        private void Set_RadiusC(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_RadiusC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_RadiusC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_RadiusC"));
            }
        }



        /// <summary>
        /// Property getter Accessor for MaxAccelX
        /// </summary>
        /// <returns>Current MaxAccelX</returns>
        private double Get_MaxAccelX()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelX"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelX
        /// </summary>
        /// <param name="value">MaxAccelX to set</param>
        private void Set_MaxAccelX(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelX"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxAccelY
        /// </summary>
        /// <returns>Current MaxAccelY</returns>
        private double Get_MaxAccelY()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelY"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelY
        /// </summary>
        /// <param name="value">MaxAccelY to set</param>
        private void Set_MaxAccelY(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelY"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxAccelZ
        /// </summary>
        /// <returns>Current MaxAccelZ</returns>
        private double Get_MaxAccelZ()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelZ"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelZ
        /// </summary>
        /// <param name="value">MaxAccelZ to set</param>
        private void Set_MaxAccelZ(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelZ"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxAccelA
        /// </summary>
        /// <returns>Current MaxAccelA</returns>
        private double Get_MaxAccelA()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelA"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelA
        /// </summary>
        /// <param name="value">MaxAccelA to set</param>
        private void Set_MaxAccelA(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelA"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxAccelB
        /// </summary>
        /// <returns>Current MaxAccelB</returns>
        private double Get_MaxAccelB()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelB"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelB
        /// </summary>
        /// <param name="value">MaxAccelB to set</param>
        private void Set_MaxAccelB(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelB"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxAccelC
        /// </summary>
        /// <returns>Current MaxAccelC</returns>
        private double Get_MaxAccelC()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxAccelC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxAccelC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxAccelC"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxAccelC
        /// </summary>
        /// <param name="value">MaxAccelC to set</param>
        private void Set_MaxAccelC(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxAccelC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxAccelC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxAccelC"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelX
        /// </summary>
        /// <returns>Current MaxVelX</returns>
        private double Get_MaxVelX()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelX"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelX
        /// </summary>
        /// <param name="value">MaxVelX to set</param>
        private void Set_MaxVelX(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelX"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelY
        /// </summary>
        /// <returns>Current MaxVelY</returns>
        private double Get_MaxVelY()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelY"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelY
        /// </summary>
        /// <param name="value">MaxVelY to set</param>
        private void Set_MaxVelY(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelY"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelZ
        /// </summary>
        /// <returns>Current MaxVelZ</returns>
        private double Get_MaxVelZ()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelZ"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelZ
        /// </summary>
        /// <param name="value">MaxVelZ to set</param>
        private void Set_MaxVelZ(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelZ"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelA
        /// </summary>
        /// <returns>Current MaxVelA</returns>
        private double Get_MaxVelA()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelA"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelA
        /// </summary>
        /// <param name="value">MaxVelA to set</param>
        private void Set_MaxVelA(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelA"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelB
        /// </summary>
        /// <returns>Current MaxVelB</returns>
        private double Get_MaxVelB()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelB"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelB
        /// </summary>
        /// <param name="value">MaxVelB to set</param>
        private void Set_MaxVelB(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelB"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxVelC
        /// </summary>
        /// <returns>Current MaxVelC</returns>
        private double Get_MaxVelC()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxVelC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxVelC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxVelC"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxVelC
        /// </summary>
        /// <param name="value">MaxVelC to set</param>
        private void Set_MaxVelC(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxVelC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxVelC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxVelC"));
            }
        }


        /// <summary>
        /// Property getter Accessor for CountsPerInchX
        /// </summary>
        /// <returns>Current CountsPerInchX</returns>
        private double Get_CountsPerInchX()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchX"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchX
        /// </summary>
        /// <param name="value">CountsPerInchX to set</param>
        private void Set_CountsPerInchX(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "CountsPerInchX"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "CountsPerInchX"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "CountsPerInchX"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CountsPerInchY
        /// </summary>
        /// <returns>Current CountsPerInchY</returns>
        private double Get_CountsPerInchY()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchY"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchY
        /// </summary>
        /// <param name="value">CountsPerInchY to set</param>
        private void Set_CountsPerInchY(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CountsPerInchY"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CountsPerInchY"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CountsPerInchY"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CountsPerInchZ
        /// </summary>
        /// <returns>Current CountsPerInchZ</returns>
        private double Get_CountsPerInchZ()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchZ"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchZ
        /// </summary>
        /// <param name="value">CountsPerInchZ to set</param>
        private void Set_CountsPerInchZ(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CountsPerInchZ"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CountsPerInchZ"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CountsPerInchZ"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CountsPerInchA
        /// </summary>
        /// <returns>Current CountsPerInchA</returns>
        private double Get_CountsPerInchA()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchA"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchA
        /// </summary>
        /// <param name="value">CountsPerInchA to set</param>
        private void Set_CountsPerInchA(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CountsPerInchA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CountsPerInchA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CountsPerInchA"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CountsPerInchB
        /// </summary>
        /// <returns>Current CountsPerInchB</returns>
        private double Get_CountsPerInchB()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchB"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchB
        /// </summary>
        /// <param name="value">CountsPerInchB to set</param>
        private void Set_CountsPerInchB(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CountsPerInchB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CountsPerInchB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CountsPerInchB"));
            }
        }

        /// <summary>
        /// Property getter Accessor for CountsPerInchC
        /// </summary>
        /// <returns>Current CountsPerInchC</returns>
        private double Get_CountsPerInchC()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(_InstanceHandle); 
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_CountsPerInchC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_CountsPerInchC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_CountsPerInchC"));
            }
        }

        /// <summary>
        /// Property setter Accessor for CountsPerInchC
        /// </summary>
        /// <param name="value">CountsPerInchC to set</param>
        private void Set_CountsPerInchC(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_CountsPerInchC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_CountsPerInchC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_CountsPerInchC"));
            }
        }

        /// <summary>
        /// Property getter Accessor for MaxLinearLength
        /// </summary>
        /// <returns>Current MaxLinearLength</returns>
        private double Get_MaxLinearLength()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_MaxLinearLength"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_MaxLinearLength"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_MaxLinearLength"));
            }
        }

        /// <summary>
        /// Property setter Accessor for MaxLinearLength
        /// </summary>
        /// <param name="value">MaxLinearLength to set</param>
        private void Set_MaxLinearLength(double value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_MaxLinearLength"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_MaxLinearLength"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_MaxLinearLength"));
            }
        }



        /// <summary>
        /// Property getter Accessor for DegreesA (whether axis is in units of degrees)
        /// </summary>
        /// <returns>Current DegreesA</returns>
        private bool Get_DegreesA()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_DegreesA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_DegreesA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_DegreesA"));
            }
        }

        /// <summary>
        /// Property setter Accessor for DegreesA
        /// </summary>
        /// <param name="value">DegreesA to set (whether axis is in units of degrees)</param>
        private void Set_DegreesA(bool value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_DegreesA"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_DegreesA"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_DegreesA"));
            }
        }


        /// <summary>
        /// Property getter Accessor for DegreesB (whether axis is in units of degrees)
        /// </summary>
        /// <returns>Current DegreesB</returns>
        private bool Get_DegreesB()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_DegreesB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_DegreesB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_DegreesB"));
            }
        }

        /// <summary>
        /// Property setter Accessor for DegreesB
        /// </summary>
        /// <param name="value">DegreesB to set (whether axis is in units of degrees)</param>
        private void Set_DegreesB(bool value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_DegreesB"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_DegreesB"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_DegreesB"));
            }
        }

        /// <summary>
        /// Property getter Accessor for DegreesC (whether axis is in units of degrees)
        /// </summary>
        /// <returns>Current DegreesC</returns>
        private bool Get_DegreesC()
        {
            try
            {
                return KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Get_DegreesC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Get_DegreesC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Get_DegreesC"));
            }
        }

        /// <summary>
        /// Property setter Accessor for DegreesC
        /// </summary>
        /// <param name="value">DegreesC to set (whether axis is in units of degrees)</param>
        private void Set_DegreesC(bool value)
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(_InstanceHandle, value);
                SetTPParams();
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "Set_DegreesC"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "Set_DegreesC"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "Set_DegreesC"));
            }
        }

        /// <summary>
        /// Force the Trajectory Planner to update it's parameters
        /// </summary>
        public void SetTPParams()
        {
            try
            {
                KM_dotnet_Interop_CoordMotion_SetTPParams(_InstanceHandle);
            }
            catch (DllNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Dll Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                    this.ToString(), "SetTPParams"));
            }
            catch (EntryPointNotFoundException e)
            {
                throw new DMException(this, e, String.Format("Entry Point Not Found Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                   this.ToString(), "SetTPParams"));
            }
            catch (Exception e)
            {
                throw new DMException(this, e, String.Format("General Exception thrown :  Caller - [{0}] :: Member - [{1}]",
                  this.ToString(), "SetTPParams"));
            }
        }
        #endregion
    }
}
