using Catel.Data;
using Catel.Runtime.Serialization;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace ExtendedLoggingKflop.Models
{
    public enum PlotAxisOperatorEnum
    {
        [Description("")]
        None,
        [Description("Add")]
        Add,
        [Description("Subtract")]
        Subtract,
        [Description("Multiple")]
        Multiple,
        [Description("Divide")]
        Divide
    }

    /// <summary>
    /// SeriesDefinitionModel model which fully supports serialization, property changed notifications,
    /// backwards compatibility and error checking.
    /// </summary>
#if !SILVERLIGHT
    [Serializable]
#endif
    [SerializerModifier(typeof(SeriesDefinitionModelSerializerModifier))]
    public class SeriesDefinitionModel : SavableModelBase<SeriesDefinitionModel>
    {
        #region Fields
        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new object from scratch.
        /// </summary>
        public SeriesDefinitionModel()
        {
            Color = Colors.Red;
        }

#if !SILVERLIGHT
        /// <summary>
        /// Initializes a new object based on <see cref="SerializationInfo"/>.
        /// </summary>
        /// <param name="info"><see cref="SerializationInfo"/> that contains the information.</param>
        /// <param name="context"><see cref="StreamingContext"/>.</param>
        protected SeriesDefinitionModel(SerializationInfo info, StreamingContext context)
            : base(info, context) { }
#endif
        #endregion

        #region Properties
        // TODO: Define your custom properties here using the modelprop code snippet
        /// <summary>
        /// Title of the series, displayed in the legend
        /// </summary>
        [Required]
        public string SeriesTitle { get; set; }
        /// <summary>
        /// Associate with a Y axis by this key.  OxyPlot associates series to axis
        /// via a key, this key is generated automatically in the empty 
        /// constructor.  For the most part can be ignored.
        /// </summary>
        [Required]
        public string YAxisKey { get; set; }
        /// <summary>
        /// Line Color, default Red.
        /// </summary>
        [Required]
        [IncludeInSerialization]
        public Color Color { get; set; }
        /// <summary>
        /// Index into the message results to get the data for this plot,
        /// This is IMPORTANT the index is 0 (Zero) based.  So if your
        /// printf looks like printf("%f,%f,%f,%f\n", data1,data2,data3,data4);
        /// data1 is index 0
        /// data2 is index 1
        /// etc.
        /// </summary>
        [Required]
        [SerializeUsingParseAndToString]
        public int ResultIndex1 { get; set; }

        /// <summary>
        /// Index into message results to get second data value for this plot,
        /// set to null to not use with ResultOperator.  In this case just
        /// ResultIndex1 will be plotted
        /// </summary>
        [IncludeInSerialization]
        public int? ResultIndex2 { get; set; }
        /// <summary>
        /// ResultOperator will be used with (ResultIndex1 ResultOperator ResultIndex2)
        /// example if ResultOperator is Subtract the plotted value will be
        /// (ResultIndex1 - ResultIndex2)
        /// </summary>
        [SerializeUsingParseAndToString]
        public PlotAxisOperatorEnum ResultOperator { get; set; }

        [ExcludeFromSerializationAttribute]
        public Object OxySeries { get; set; }

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

    public class SeriesDefinitionModelSerializerModifier : SerializerModifierBase<SeriesDefinitionModel>
    {
        public override void SerializeMember(ISerializationContext context, MemberValue memberValue)
        {
            if (string.Equals(memberValue.Name, "Color"))
            {
                var color = (Color)memberValue.Value;

                memberValue.Value = string.Format("{0}|{1}|{2}|{3}", color.A, color.R, color.G, color.B);
            }
            if (string.Equals(memberValue.Name, "ResultIndex2"))
            {
                var index = (int?)memberValue.Value;
                memberValue.Value = index.HasValue?index.Value.ToString():"null";
            }
        }

        public override void DeserializeMember(ISerializationContext context, MemberValue memberValue)
        {
            if (string.Equals(memberValue.Name, "Color"))
            {
                var stringValue = memberValue.Value as string;
                if (!string.IsNullOrWhiteSpace(stringValue))
                {
                    var splittedStringValue = stringValue.Split(new[] { '|' }, StringSplitOptions.None);
                    var a = (byte)int.Parse(splittedStringValue[0]);
                    var r = (byte)int.Parse(splittedStringValue[1]);
                    var g = (byte)int.Parse(splittedStringValue[2]);
                    var b = (byte)int.Parse(splittedStringValue[3]);

                    memberValue.Value = Color.FromArgb(a, r, g, b);
                }
            }
            if (string.Equals(memberValue.Name, "ResultIndex2"))
            {
                var stringValue = memberValue.Value as string;
                int i;
                int? nulli = null;
                if (int.TryParse(stringValue, out i))
                {
                    nulli = i;                    
                }                
                memberValue.Value = nulli;                
            }
        }
    }
}
