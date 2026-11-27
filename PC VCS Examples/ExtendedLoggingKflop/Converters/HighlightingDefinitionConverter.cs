using System;
using System.Globalization;
using System.Windows.Data;
using ICSharpCode.AvalonEdit.Highlighting;
namespace ExtendedLoggingKflop.Converters
{
    public class HighlightingDefinitionConverter : IValueConverter
    {
        private static readonly HighlightingDefinitionTypeConverter Converter = new HighlightingDefinitionTypeConverter();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            //IHighlightingDefinition hl = HighlightingManager.Instance.GetDefinition("‌​C++");
            //hl = HighlightingManager.Instance.HighlightingDefinitions[8];
            string s = value as string;
            var c = Converter.ConvertFrom("C++");
            return c;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return Converter.ConvertToString(value);
        }
    }
}
