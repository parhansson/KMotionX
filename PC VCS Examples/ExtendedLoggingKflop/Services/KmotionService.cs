using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using KMotion_dotNet;
using Anotar.Catel;

namespace ExtendedLoggingKflop.Services
{
    public class KmotionService : IKmotionService
    {
        private KM_Controller _Controller = null;

        public KmotionService()
        {

        }
        ~KmotionService()
        {
            if (_Controller != null)
            {
                _Controller.Dispose();
            }
        }
        public KM_Controller KMController
        {
            get
            {
                if (_Controller == null)
                {
                    _Controller = new KM_Controller();
                    LogTo.Info("KMoition firmware version: {0}", _Controller.GetFirmwareVersion().ToString());
                }
                return _Controller;
            }            
        }
    }
}
