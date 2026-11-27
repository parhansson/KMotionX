using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OxyPlot;
using Catel.Data;

namespace ExtendedLoggingKflop.Services
{
    using OxyPlot.Axes;
    using Shared;
    using System.Collections.Specialized;
    public class AvailablePlotsService : IAvailablePlotsService
    {
        public AvailablePlotsService()
        {            
            AvailablePlots = new ObservableCollection<PlotModel>();
            //m_plotWatcher = new ChangeNotificationWrapper(AvailablePlots);
            //m_plotWatcher.CollectionChanged += OnAvailablePlotsChanged;
        }
        public bool SyncedXAxis { get; set; }
        public bool SyncedYAxis { get; set; }
        public ObservableCollection<PlotModel> AvailablePlots { get; private set; }
        public bool isInternalChange { get; set; }
        public void FinishedWithPlot(PlotModel plot)
        {
            throw new NotImplementedException();
        }
       
        public void OnAxisChanged(object sender, AxisChangedEventArgs e)
        {
            if (isInternalChange)
            {
                return;
            }
            LinearAxis axis = sender as LinearAxis;
            if (axis == null) return;

            OnAxisChanged(axis);
        }
        private void OnAxisChanged(LinearAxis axis)
        {
           
            foreach (OxyPlot.PlotModel plot in AvailablePlots)
            {
                if (plot == axis.PlotModel) continue;
                isInternalChange = true;
                if (plot.Axes != null && plot.Axes.Count > 0)
                {
                    //hmm limited to one axis per position with this code...
                    var plotaxis = plot.Axes.FirstOrDefault(x => x.Position == axis.Position);
                    if (plotaxis != null  
                        && (
                        (SyncedXAxis && axis.Position == AxisPosition.Bottom)
                        ||
                        (SyncedYAxis && axis.Position != AxisPosition.Bottom)))//only sync x axis for now
                    {
                        plotaxis.Zoom(axis.ActualMinimum, axis.ActualMaximum);
                    }
                    
                    plot.InvalidatePlot(false);
                }
                isInternalChange = false;
            }
        }
    

    }
}
