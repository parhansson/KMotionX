using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Catel.Data;
using System.Runtime.Serialization;
using System.IO;
using ICSharpCode.AvalonEdit.Utils;
using ICSharpCode.AvalonEdit.Document;
using System.Collections.ObjectModel;
using Catel.Runtime.Serialization;
using System.ComponentModel;
using Jot.Configuration;

namespace ExtendedLoggingKflop.Models
{
    using Services;
    public class ThreadCProgram : INotifyPropertyChanged
    {

        private string _filepath;
        public string FilePath
        {
            get
            {
                return _filepath;
            }
            set
            {
                if (_filepath == value) return;
                _filepath = value;
                using (FileStream fs = new FileStream(_filepath,
                                   FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    using (StreamReader reader = FileReader.OpenStream(fs, Encoding.UTF8))
                    {
                        this._document = new TextDocument(reader.ReadToEnd());
                    }
                }
                NotifyPropertyChanged("FilePath");
            }
        }
        [ExcludeFromSerialization]
        public string FileName
        {
            get
            {
                string filename = System.IO.Path.GetFileName(FilePath);
                return filename;
            }
        }
        private TextDocument _document;
        [ExcludeFromSerialization]
        public TextDocument Document
        {
            get
            {
                return _document;
            }
            private set
            {
                _document = value;
            }
        }
        [ExcludeFromSerialization]
        public string Extension
        {
            get
            {
                return System.IO.Path.GetExtension(FilePath);
            }
        }
        private int? _threadID;
        public int? ThreadID
        {
            get
            {
                return _threadID;
            }
            set
            {
                if (_threadID != value)
                {
                    _threadID = value;
                    NotifyPropertyChanged("ThreadID");
                }
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                var e = new PropertyChangedEventArgs(propertyName);
                handler(this, e);
            }
        }

        public override string ToString()
        {
            return FileName;
        }
    }

    /// <summary>
    /// MainWindowModel model which fully supports serialization, property changed notifications,
    /// backwards compatibility and error checking.
    /// </summary>
#if !SILVERLIGHT
    [Serializable]
#endif
    public class MainWindowModel : SavableModelBase<MainWindowModel>
    {
        #region Fields
        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new object from scratch.
        /// </summary>
        public MainWindowModel()
        {
            ThreadCPrograms = new ObservableCollection<ThreadCProgram>();
            Plots = new ObservableCollection<Models.PlotModel>();
            //Services.Tracker.Configure(this).Apply();
        }

#if !SILVERLIGHT
        /// <summary>
        /// Initializes a new object based on <see cref="SerializationInfo"/>.
        /// </summary>
        /// <param name="info"><see cref="SerializationInfo"/> that contains the information.</param>
        /// <param name="context"><see cref="StreamingContext"/>.</param>
        protected MainWindowModel(SerializationInfo info, StreamingContext context)
            : base(info, context) { }
#endif
        #endregion

        #region Properties
        // TODO: Define your custom properties here using the modelprop code snippet
        public ObservableCollection<Models.PlotModel> Plots { get; set; }
        //[Trackable]
        public ObservableCollection<ThreadCProgram> ThreadCPrograms { get; set; }
        
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
