using Jot;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Services
{
    static class Services
    {
        //the Jot tracking tool doesn't support Catel style services (where the service is defined in an Interface)
        //so for simplicity this static instance is declared.
        public static StateTracker Tracker = new StateTracker(Environment.SpecialFolder.ApplicationData);
    }
}
