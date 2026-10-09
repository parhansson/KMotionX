using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Services
{
    using OxyPlot.Axes;
    using Shared;
    public interface IAvailablePlotsService
    {
        bool isInternalChange { get; set; }
        ObservableCollection<OxyPlot.PlotModel> AvailablePlots { get; }
        
        bool SyncedXAxis { get; set; }

        bool SyncedYAxis { get; set; }


        void OnAxisChanged(object sender, AxisChangedEventArgs e);

    }
}
