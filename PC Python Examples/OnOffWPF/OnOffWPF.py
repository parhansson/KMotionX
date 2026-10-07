# IronPython is no longer supported by Microsoft for Visual Studio
# Open Source IronPython Version 3.4.1 from:
# https://ironpython.net/
# works with WPF.  However integrated debugging doesn't
# seem to be supported

import wpf,sys,clr,System,os.path

from System.Windows import Application, Window
from os.path import dirname

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))) + r"\KMotion\Release64")
clr.AddReference("KMotion_dotNet")
from KMotion_dotNet import KM_Controller

KM = KM_Controller()

class MyWindow(Window):
    def __init__(self):
        wpf.LoadComponent(self, 'OnOffWPF.xaml')
    
    def ONButton_Click(self, sender, e):
        KM.WriteLine("SetBit46")
       
    def OFFButton_Click(self, sender, e):
        KM.WriteLine("ClearBit46")

if __name__ == '__main__':
    Application().Run(MyWindow())
