using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Catel.Data;
using System.Runtime.Serialization;
using Anotar.Catel;
using Catel.Runtime.Serialization;
using System.ComponentModel;

namespace ExtendedLoggingKflop.Models
{
    /// <summary>
        /// PlotModel class is a Catel "Model" and holds the data that defines a 
        /// plot.  Note to be confused with OxyPlot.PlotModel.  
        /// </summary>
#if !SILVERLIGHT
    [Serializable]
#endif
    public class PlotModel : SavableModelBase<PlotModel>
    {
        #region Fields
        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new object from scratch.
        /// </summary>
        public PlotModel()
        {
            PlotConfiguration = new PlotConfigurationModel();
            SavePngPath = "";
            SaveCsvPath = "";
            PlotID = Guid.NewGuid().ToString();
        }

#if !SILVERLIGHT
        /// <summary>
        /// Initializes a new object based on <see cref="SerializationInfo"/>.
        /// </summary>
        /// <param name="info"><see cref="SerializationInfo"/> that contains the information.</param>
        /// <param name="context"><see cref="StreamingContext"/>.</param>
        protected PlotModel(SerializationInfo info, StreamingContext context)
            : base(info, context) { }
#endif
        #endregion

        #region Properties
        // TODO: Define your custom properties here using the modelprop code snippet
        public PlotConfigurationModel PlotConfiguration { get; set; }
        /// <summary>
        /// Used internally to identify this plot to it's data
        /// </summary>
        public string PlotID { get; set; }
        public string LogFormat { get; set; }
        [IncludeInSerialization]
        public int LogID { get; set; }
        public string SavePngPath { get; set; }
        public string SaveCsvPath { get; set; }
        [DefaultValue(100000)]
        public int MaxPlotPoints { get; set; }
        #endregion

        #region Methods
        private void OnLogIDChanged()
        {
            LogTo.Info("LogID: {0}", LogID);
        }
        /// <summary>
        /// Validates the field values of this object. Override this method to enable
        /// validation of field values.
        /// </summary>
        /// <param name="validationResults">The validation results, add additional results to this list.</param>
        protected override void ValidateFields(List<IFieldValidationResult> validationResults)
        {
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
