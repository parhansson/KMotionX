using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExtendedLoggingKflop.Shared
{
    public class SelectableObject<T> : INotifyPropertyChanged
    {
        private bool _isSelected;
        public bool IsSelected {
            get
            {
                return _isSelected;
            }

            set
            {
                if (value != this._isSelected)
                {
                    this._isSelected = value;
                    NotifyPropertyChanged("IsSelected");
                }
            }
        }
        public T ObjectData { get; set; }
        public SelectableObject()
        {
            ObjectData = default(T);
            IsSelected = false;
        }
        public SelectableObject(T objectData)
        {
            ObjectData = objectData;
        }

        public SelectableObject(T objectData, bool isSelected)
        {
            IsSelected = isSelected;
            ObjectData = objectData;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }
    }
   
}
