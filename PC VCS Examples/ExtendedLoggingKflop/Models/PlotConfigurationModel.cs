using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Catel.Data;
using System.Runtime.Serialization;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;

namespace ExtendedLoggingKflop.Models
{
    /// <summary>
        /// PlotConfigurationModel model which fully supports serialization, property changed notifications,
        /// backwards compatibility and error checking.
        /// </summary>
#if !SILVERLIGHT
    [Serializable]
#endif
    public class PlotConfigurationModel : SavableModelBase<PlotConfigurationModel>
    {
        #region Fields
        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new object from scratch.
        /// </summary>
        public PlotConfigurationModel()
        {
            Axes = new ObservableCollection<AxisDefinitionModel>();
            Series = new ObservableCollection<SeriesDefinitionModel>();
            PlotName = "New Plot " + DateTime.Now.ToString();
            SavePath = "";
        }

#if !SILVERLIGHT
        /// <summary>
        /// Initializes a new object based on <see cref="SerializationInfo"/>.
        /// </summary>
        /// <param name="info"><see cref="SerializationInfo"/> that contains the information.</param>
        /// <param name="context"><see cref="StreamingContext"/>.</param>
        protected PlotConfigurationModel(SerializationInfo info, StreamingContext context)
            : base(info, context) { }
#endif
        #endregion

        #region Properties
        public ObservableCollection<AxisDefinitionModel> Axes { get; set; }

        /// <summary>
            /// Gets or sets the property value.
            /// </summary>
        public ObservableCollection<SeriesDefinitionModel> Series
        {
            get { return GetValue<ObservableCollection<SeriesDefinitionModel>>(SeriesProperty); }
            set { SetValue(SeriesProperty, value); }
        }

        /// <summary>
        /// Register the name property so it is known in the class.
        /// </summary>
        public static readonly PropertyData SeriesProperty = RegisterProperty("Series", typeof(ObservableCollection<SeriesDefinitionModel>), null);
        //since we need to refer to this propertyName in the validation we can't use Catel Fody automatic property to dependency proptery and have to
        //declare in full above.
        //public ObservableCollection<SeriesDefinitionModel> Series { get; set; }

        [Required]
        public string PlotName { get; set; }
        public string Notes { get; set; }
        public string SavePath { get; set; }

        #endregion

        #region Methods
        /// <summary>
        /// Validates the field values of this object. Override this method to enable
        /// validation of field values.
        /// </summary>
        /// <param name="validationResults">The validation results, add additional results to this list.</param>
        protected override void ValidateFields(List<IFieldValidationResult> validationResults)
        {
            if (Series == null) return;
            foreach (var series in Series)
            {
//                AxisDefinitionModel axis = null;
                
                if (Axes==null || null == Axes.FirstOrDefault(x => x.Key == series.YAxisKey))
                {
                    //deleted axis that was assigned to this series, no longer valid.
                    validationResults.Add(FieldValidationResult.CreateError(SeriesProperty, "One of the series has an invalid Axis association."));
                }
            }
           
        }

        /// <summary>
        /// Validates the field values of this object. Override this method to enable
        /// validation of field values.
        /// </summary>
        /// <param name="validationResults">The validation results, add additional results to this list.</param>
        protected override void ValidateBusinessRules(List<IBusinessRuleValidationResult> validationResults)
        {
        }
        #endregion
    }
}
