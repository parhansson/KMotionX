namespace ExtendedLoggingKflop.ViewModels
{
    using Catel.MVVM;
    using System.Threading.Tasks;
    using Models;
    using Catel.Data;
    using Catel.Fody;
    using System.Collections;
    using System.Collections.Generic;
    public class SeriesDefinitionViewModel : ViewModelBase
    {       
        public SeriesDefinitionViewModel(IList<AxisDefinitionModel> axisDefinitions)
        {
            AvailableAxis = axisDefinitions;
            SeriesDefinitionModel = new SeriesDefinitionModel();
        }
        public SeriesDefinitionViewModel(IList<AxisDefinitionModel> axisDefinitions,SeriesDefinitionModel series)
        {
            AvailableAxis = axisDefinitions;
            SeriesDefinitionModel = series;
        }

        public override string Title { get { return "Series Configuration"; } }

        // TODO: Register models with the vmpropmodel codesnippet
        [Model]
        [Expose("SeriesTitle")]
        [Expose("YAxisKey")]
        [Expose("Color")]
        [Expose("ResultIndex1")]
        [Expose("ResultIndex2")]
        [Expose("ResultOperator")]
        public SeriesDefinitionModel SeriesDefinitionModel { get; set; }
        // TODO: Register view model properties with the vmprop or vmpropviewmodeltomodel codesnippets
        public IList<AxisDefinitionModel> AvailableAxis { get; set; }
        // TODO: Register commands with the vmcommand or vmcommandwithcanexecute codesnippets


        protected override async Task InitializeAsync()
        {
            await base.InitializeAsync();

            // TODO: subscribe to events here
        }

        protected override async Task CloseAsync()
        {
            // TODO: unsubscribe from events here

            await base.CloseAsync();
        }
    }
}
