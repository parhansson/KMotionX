/*********************************************************************/
/*         Copyright (c) 2003-2016  DynoMotion Incorporated          */
/*********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices; // DsEvCode
using System.Threading;
using System.Windows.Forms;

namespace MeasureFiducials
{
    public struct COPYDATASTRUCT_RECEIVE
    {
        public IntPtr dwData;
        public int cbData;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string lpData;
    }

    class HostComm
    {
        struct COPYDATASTRUCT
        {
            public int dwData;
            public int cbData;
            public int lpData;
        }

        public bool TerminateAll = false;

        public const int WM_COPYDATA = 0x4A;
        public const int ID_ReloadGeoCorrection = 33016;

        const double MinZ = -0.5;

        [DllImport("User32.dll", SetLastError = true)]
        public static extern IntPtr FindWindow(String lpClassName, String lpWindowName);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool PostMessage(IntPtr hWnd, uint Msg, int wParam, int lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, ref COPYDATASTRUCT lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static extern int SendMessage(IntPtr hwnd, int wMsg, IntPtr wParam, ref COPYDATASTRUCT lParam);

        IntPtr KMotionCNCWindow, MainFormHandle;

        public String InterProcessString;
        public bool InterProcessStringRequested;
        public bool InterProcessStringReceived;

        double LastX = 0;
        double LastY = 0;
        double LastZ = 0;
        double Lasta = 0;
        double Lastb = 0;
        double Lastc = 0;

        bool FirstContact = true;

        Mutex GetPosMutex;


        public HostComm(IntPtr FormHandle)
        {
            MainFormHandle = FormHandle;
            KMotionCNCWindow = FindWindow("KMotionCNC", null);
            GetPosMutex = new Mutex();
        }


        public void JogStop()
        {
            SendString("0 0 0 0 0 0");

            int ID_DoJog = 33020;
            SendMessageService(ID_DoJog);
        }

        public void DoJog(double JoggingXSpeed, double JoggingYSpeed, double JoggingZSpeed, double JoggingCSpeed)
        {
            String s;

            s = JoggingXSpeed.ToString("F4"); s += ' ';
            s += JoggingYSpeed.ToString("F4"); s += ' ';
            s += JoggingZSpeed.ToString("F4"); s += ' ';
            s += "0"; s += ' ';
            s += "0"; s += ' ';
            s += JoggingCSpeed.ToString("F4");
            SendString(s);

            int ID_DoJog = 33020;
            SendMessageService(ID_DoJog);
        }

        double gxp, gyp, gzp, gcp;

        public void MoveXY(double xp, double yp)
        {
            gxp = xp;
            gyp = yp;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveXY);
            // Start the worker thread.
            workerThread.Start();
        }

        public void DoMoveXY()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(gxp, gyp, z, a, b, c);
            }
        }

        public void MoveXYRel(double xp, double yp)
        {
            gxp = xp;
            gyp = yp;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveXYRel);
            // Start the worker thread.
            workerThread.Start();
        }


        public void DoMoveXYRel()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(x + gxp, y + gyp, z, a, b, c);
            }
        }

        public void MoveXYZTRel(double xp, double yp, double zp, double Theta)
        {
            gxp = xp;
            gyp = yp;
            gzp = zp;
            gcp = Theta;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveXYZTRel);
            // Start the worker thread.
            workerThread.Start();
        }

        public void DoMoveXYZTRel()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(x + gxp, y + gyp, z + gzp, a, b, c + gcp);
            }
        }

        public void MoveXYZT(double xp, double yp, double zp, double Theta)
        {
            gxp = xp;
            gyp = yp;
            gzp = zp;
            gcp = Theta;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveXYZT);
            // Start the worker thread.
            workerThread.Start();
        }

        public void DoMoveXYZT()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(gxp, gyp, gzp, a, b, gcp);
            }
        }

        internal void MoveZRel(double zp)
        {
            gzp = zp;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveZRel);
            // Start the worker thread.
            workerThread.Start();
        }

        internal void DoMoveZRel()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(x, y, z + gzp, a, b, c);
            }
        }


        internal void MoveCRel(double theta)
        {
            gcp = theta;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveCRel);
            // Start the worker thread.
            workerThread.Start();
        }

        internal void DoMoveCRel()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                SendNumericStringMove(x, y, z, a, b, c + gcp);
            }
        }



        private void SendNumericStringMove(double x, double y, double z, double a, double b, double c)
        {
            String s;


            if (z < MinZ) z = MinZ;

            LastX = x;
            LastY = y;
            LastZ = z;
            Lasta = a;
            Lastb = b;
            Lastc = c;

            s = x.ToString("F4"); s += ' ';
            s += y.ToString("F4"); s += ' ';
            s += z.ToString("F4"); s += ' ';
            s += a.ToString("F4"); s += ' ';
            s += b.ToString("F4"); s += ' ';
            s += c.ToString("F4");
            SendString(s);

            int ID_DoMove = 33021;
            SendMessageService(ID_DoMove);
        }


        public void MoveExpXY(double xp, double yp)
        {
            gxp = xp;
            gyp = yp;

            // Create the thread object. This does not start the thread.
            Thread workerThread = new Thread(DoMoveExpXY);
            // Start the worker thread.
            workerThread.Start();
        }

        public void DoMoveExpXY()
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0, tau = 0.3;

            if (GetPositions(ref x, ref y, ref z, ref a, ref b, ref c))
            {
                String s;

                s = gxp.ToString("F4"); s += ' ';
                s += gyp.ToString("F4"); s += ' ';
                s += z.ToString("F4"); s += ' ';
                s += a.ToString("F4"); s += ' ';
                s += b.ToString("F4"); s += ' ';
                s += c.ToString("F4"); s += ' ';
                s += tau.ToString("F4");
                SendString(s);

                int ID_DoMoveExp = 33024;
                SendMessageService(ID_DoMoveExp);
            }
        }



        public bool GetPositions(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c)
        {
            return GetPositionsNoGeo(ref x, ref y, ref z, ref a, ref b, ref c, false);
        }



        public bool GetPositionsNoGeo(ref double x, ref double y, ref double z, ref double a, ref double b, ref double c, bool NoGeo)
        {
            bool success = false;
            int ID_GetPositions;

            GetPosMutex.WaitOne();
            // make sure we have sent something to establish Window handles
            if (FirstContact)
            {
                FirstContact = false;
                SendString("Hello");
            }

            if (!InterProcessStringRequested)
            {
                InterProcessStringReceived = false;
                InterProcessStringRequested = true;
                if (NoGeo)
                    ID_GetPositions = 33023;
                else
                    ID_GetPositions = 33022;
                SendMessageService(ID_GetPositions);
            }

            while (!InterProcessStringReceived)
            {
                if (TerminateAll) return true;
                Thread.Sleep(20);
            }

            if (InterProcessStringReceived)
            {
                string s = InterProcessString;

                int i0 = 0;
                int i1 = s.IndexOf(' ', i0 + 1);
                int i2 = s.IndexOf(' ', i1 + 1);
                int i3 = s.IndexOf(' ', i2 + 1);
                int i4 = s.IndexOf(' ', i3 + 1);
                int i5 = s.IndexOf(' ', i4 + 1);

                try
                {
                    x = Convert.ToDouble(s.Substring(i0, i1 - i0));
                    y = Convert.ToDouble(s.Substring(i1 + 1, i2 - i1));
                    z = Convert.ToDouble(s.Substring(i2 + 1, i3 - i2));
                    a = Convert.ToDouble(s.Substring(i3 + 1, i4 - i3));
                    b = Convert.ToDouble(s.Substring(i4 + 1, i5 - i4));
                    c = Convert.ToDouble(s.Substring(i5 + 1));
                }
                catch (Exception)
                {
                    x = y = z = a = b = c = 0;
                }


                success = true;
                InterProcessStringReceived = InterProcessStringRequested = false;
            }

            GetPosMutex.ReleaseMutex();
            return success;
        }


        internal bool GetPositionXY(ref double xp, ref double yp)
        {
            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            if (!GetPositions(ref x, ref y, ref z, ref a, ref b, ref c)) return false;

            xp = x;
            yp = y;
            return true;
        }



        public bool WaitLastMoveComplete(ref bool Done)
        {
            double TOL = 0.001;

            double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0;

            do
            {
                if (!GetPositions(ref x, ref y, ref z, ref a, ref b, ref c)) return false;
                Done = (Math.Abs(x - LastX) < TOL && Math.Abs(y - LastY) < TOL && Math.Abs(z - LastZ) < TOL &&
                         Math.Abs(a - Lasta) < TOL && Math.Abs(b - Lastb) < TOL && Math.Abs(c - Lastc) < TOL);
            } while (!Done);
            return true;
        }



        public void SendMessageService(int ID)
        {
            uint WM_COMMAND = 0x0111;
            PostMessage(KMotionCNCWindow, WM_COMMAND, ID, 0);
        }

        private int SendString(String s)
        {
            COPYDATASTRUCT cds;
            cds.dwData = 0;
            cds.lpData = (int)Marshal.StringToHGlobalUni(s);
            cds.cbData = s.Length * 2 + 1;

            int result = SendMessage(KMotionCNCWindow, (int)WM_COPYDATA, MainFormHandle, ref cds);
            Marshal.FreeHGlobal((IntPtr)cds.lpData);
            return result;
        }
    }
}

