namespace ExtendedLoggingKflop.ViewModels
{
    using Catel.MVVM;
    using System.Threading.Tasks;
    using Models;
    using Catel.Data;
    using Catel.Fody;
    public class AxisDefinitionViewModel : ViewModelBase
    {
        public AxisDefinitionViewModel()
        {
            AxisDefinitionModel = new AxisDefinitionModel();
        }
        public AxisDefinitionViewModel(AxisDefinitionModel model)
        {
            AxisDefinitionModel = model;
        }

        public override string Title { get { return "Axis Configuration"; } }

        // TODO: Register models with the vmpropmodel codesnippet
        // TODO: Register view model properties with the vmprop or vmpropviewmodeltomodel codesnippets
        // TODO: Register commands with the vmcommand or vmcommandwithcanexecute codesnippets

        /// <summary>
        /// Gets or sets the property value.
        /// </summary>
        [Model]
        [Expose("AxisPosition")]
        [Expose("AxisTitle")]
        [Expose("Key")]
        [Expose("Unit")]
        public AxisDefinitionModel AxisDefinitionModel
        {
            get { return GetValue<AxisDefinitionModel>(AxisDefinitionModelProperty); }
            private set { SetValue(AxisDefinitionModelProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData AxisDefinitionModelProperty = RegisterProperty("AxisDefinitionModel", typeof(AxisDefinitionModel));
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
