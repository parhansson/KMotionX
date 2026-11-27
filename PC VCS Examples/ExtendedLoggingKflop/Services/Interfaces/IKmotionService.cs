using KMotion_dotNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Services
{
    public interface IKmotionService
    {        
        KM_Controller KMController { get; }
    }
}
