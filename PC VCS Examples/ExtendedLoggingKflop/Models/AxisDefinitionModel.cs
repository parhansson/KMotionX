using Catel.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;
using Catel.MVVM;
using System.ComponentModel;
using Catel.Runtime.Serialization;

namespace ExtendedLoggingKflop.Models
{
    public enum PlotAxisPositionEnum
    {
        YAxisLeft,
        YAxisRight
    }
    
    /// <summary>
        /// AxisDefinitionModel model which fully supports serialization, property changed notifications,
        /// backwards compatibility and error checking.
        /// </summary>
#if !SILVERLIGHT
    [Serializable]
#endif
    public class AxisDefinitionModel : SavableModelBase<AxisDefinitionModel>
    {
        #region Fields
        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new object from scratch.
        /// </summary>
        public AxisDefinitionModel()
        {
            Key = Guid.NewGuid().ToString();
        }

#if !SILVERLIGHT
        /// <summary>
        /// Initializes a new object based on <see cref="SerializationInfo"/>.
        /// </summary>
        /// <param name="info"><see cref="SerializationInfo"/> that contains the information.</param>
        /// <param name="context"><see cref="StreamingContext"/>.</param>
        protected AxisDefinitionModel(SerializationInfo info, StreamingContext context)
            : base(info, context) { }
#endif
        #endregion

        #region Properties
        // TODO: Define your custom properties here using the modelprop code snippet
        // Using Catel.Fody so we declare properties just like regular properties
        // and Fody turns them into Catel properties        

        [DefaultValue(PlotAxisPositionEnum.YAxisRight)]
        public PlotAxisPositionEnum AxisPosition { get; set; }
        [Required]
        public string AxisTitle { get; set; }
        public string Key { get; set; }
        public string Unit { get; set; }
        [ExcludeFromSerializationAttribute]
        public Object OxyAxis { get; set; }


        #endregion

        #region Methods
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
