using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;


namespace ExtendedLoggingKflop.Converters
{
    using Models;
    using System.Collections.ObjectModel;
    class YAxisKeyToAxisNameConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType,
               object parameter, System.Globalization.CultureInfo culture)
        {
            if (values.Length < 2) return null;

            ObservableCollection<AxisDefinitionModel> ad = values[0] as ObservableCollection<AxisDefinitionModel>;
            if (ad == null) return null;
            string axiskey = values[1] as string;
            if (axiskey == null) return null;
            AxisDefinitionModel axis = ad.SingleOrDefault(x => x.Key == axiskey);
            if (axis != null)
            {
                return axis.AxisTitle;
            }
            return null;
        }
        public object[] ConvertBack(object value, Type[] targetTypes,
               object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotSupportedException("Cannot convert back");
        }
    }
}
