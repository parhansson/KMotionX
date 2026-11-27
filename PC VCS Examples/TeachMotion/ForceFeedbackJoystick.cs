using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX.DirectInput;
using System.Collections;

namespace ForceFeedback
{
    public enum JOYSTICK_TYPE : int
    {
        NO_JOYSTICK = 0,
        NO_FORCE_FEEDBACK = 1,
        FORCE_FEEDBACK = 2
    }
    

    class ForceFeedbackJoystick
    {
        private Device device = null;
        private ArrayList effectList = new ArrayList();
        private Effect e;
        private EffectObject eo = null;
        private EffectInformation force_ei;
        public bool ForceFeedbackSupported = false;

        public JOYSTICK_TYPE Connect(System.Windows.Forms.Control Parent)
        {
            ForceFeedbackSupported = false;
            //create force feedback capable joystick device.
            try
            {
                foreach (DeviceInstance di in Manager.GetDevices(
                    DeviceClass.GameControl,
                    EnumDevicesFlags.AttachedOnly | EnumDevicesFlags.ForceFeeback))
                {
                    ForceFeedbackSupported = true;
                    device = new Device(di.InstanceGuid);
                    break;
                }
                if (device == null)
                {
                    //create force feedback capable joystick device.
                    foreach (DeviceInstance di in Manager.GetDevices(
                        DeviceClass.GameControl,
                        EnumDevicesFlags.AttachedOnly))
                    {
                        device = new Device(di.InstanceGuid);
                        break;
                    }
                    //Throw exception if joystick not found.
                    //throw new Exception("No joystick that supports forced feedback found.");
                    if (device == null)
                    {
                        return JOYSTICK_TYPE.NO_JOYSTICK;
                    }
                }
            }
            catch (System.TypeInitializationException)
            {
                // Managed Direct X doesn't seem to be present or working
                return JOYSTICK_TYPE.NO_JOYSTICK;
            }
            //set cooperative level.
            device.SetCooperativeLevel(
                Parent,
                CooperativeLevelFlags.Exclusive | CooperativeLevelFlags.Background);

            if (ForceFeedbackSupported)
            {
                // Since we will be playing force feedback effects, we should disable the
                // auto-centering spring.
                device.Properties.AutoCenter = false;
            }

            //Set axis mode absolute.
            device.Properties.AxisModeAbsolute = true;

            //Acquire joystick for capturing.
            device.Acquire();


            //Configure axes
            int[] axis = null;
            foreach (DeviceObjectInstance doi in device.Objects)
            {
                //Set axes ranges.
                if ((doi.ObjectId & (int)DeviceObjectTypeFlags.Axis) != 0)
                {
                    device.Properties.SetRange(
                    ParameterHow.ById,
                    doi.ObjectId,
                    new InputRange(-5000, 5000));
                }

                int[] temp;

                // Get info about first two FF axii on the device
                if ((doi.Flags & (int)ObjectInstanceFlags.Actuator) != 0)
                {
                    if (axis != null)
                    {
                        temp = new int[axis.Length + 1];
                        axis.CopyTo(temp, 0);
                        axis = temp;
                    }
                    else
                    {
                        axis = new int[1];
                    }

                    // Store the offset of each axis.
                    axis[axis.Length - 1] = doi.Offset;
                    if (axis.Length == 2)
                    {
                        break;
                    }
                }
            }


            //See if joystick supports ConstantForce and set it.

            foreach (EffectInformation ei in device.GetEffects(EffectType.All))
            {
                //If the joystick supports ConstantForce, then apply it.

                if (DInputHelper.GetTypeCode(ei.EffectType)
                    == (int)EffectType.ConstantForce)
                {
                    // Fill in some generic values for the effect.
                    e = new Effect();

                    e.SetDirection(new int[axis.Length]);
                    e.SetAxes(new int[axis.Length]);
                    e.ConditionStruct = new Condition[axis.Length];

                    e.EffectType = EffectType.ConstantForce;
                    
                    e.Constant = new ConstantForce();
                    e.Constant.Magnitude = 10000; 
                    
                    e.Duration = (int)DI.Infinite;
                    e.Gain = 10000;
                    e.SamplePeriod = 0;
                    e.TriggerButton = (int)Microsoft.DirectX.DirectInput.Button.NoTrigger;
                    e.TriggerRepeatInterval = (int)DI.Infinite;
                    e.Flags = EffectFlags.ObjectOffsets | EffectFlags.Cartesian;
                    e.SetAxes(axis);

                    var dd = e.GetDirection();
                    dd[0] = -100;
                    dd[1] = 100;
                    e.SetDirection(dd);


                    // Create the effect, using the passed in guid.
                    eo = new EffectObject(ei.EffectGuid, e, device);

                    eo.Start(1, EffectStartFlags.NoDownload);

                    force_ei = ei;
                    break;
                }
            }

            if (eo == null)
            {
                return JOYSTICK_TYPE.NO_FORCE_FEEDBACK;
            }
            return JOYSTICK_TYPE.FORCE_FEEDBACK;
        }

        public void ApplyForce(double x, double y)
        {
            if (ForceFeedbackSupported)
            {
                var dd = e.GetDirection();

                dd[0] = (int)(x * 100);
                dd[1] = (int)(y * 100);
                e.SetDirection(dd);

                e.Constant.Magnitude = (int)(Math.Sqrt(x * x + y * y) * 10000.0);

                eo.SetParameters(e, EffectParameterFlags.Direction);
            }
        }

        public void PollXY(ref double X, ref double Y, ref byte[] buttons)
        {
            if (device != null)
            {
                X = device.CurrentJoystickState.X/5000.0;
                Y = device.CurrentJoystickState.Y/5000.0;
                buttons = device.CurrentJoystickState.GetButtons();
            } 
        }
    }
}
