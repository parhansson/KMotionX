# Python WPF and IronPython are no longer supported by Visual Studio
# so non-WPF methods are used.  pythonnet must be installed to allow
# clr to add .NET references in order to load the KMotion_dotNet reference.
#
# from Windows CMD window pythonnet might need to be installed
# note: python aliases py python or python3 might be used 
# but take care thy might be different Versions/installations
# without pythonnet installed
#
# download install python from https://www.python.org/downloads/
#
# py --version
# py -m pip --version
# py -m pip install --upgrade pip
# py -m pip install "pythonnet"

import sys,clr,os.path

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))) + r"\KMotion\Release64")
clr.AddReference("KMotion_dotNet")
from KMotion_dotNet import KM_Controller
KM = KM_Controller()

import tkinter as tk

def on_button1_click():
    KM.WriteLine("SetBit46")

def on_button2_click():
    KM.WriteLine("ClearBit46")

root = tk.Tk()
root.title("Dynomotion + Python")
root.geometry("300x200") # Set window size to 300x200 pixels

button1 = tk.Button(root, text="LED ON", font=("Helvetica", 14, "bold"), bg="green", command=on_button1_click, width=12, height=2)
button1.pack(padx=20, pady=20)

button2 = tk.Button(root, text="LED OFF", font=("Helvetica", 14, "bold"), bg="red", command=on_button2_click, width=12, height=2)
button2.pack(padx=20, pady=20)

root.mainloop()








# import wpf,sys,clr,System,os.path

# from System.Windows import Application, Window
# from os.path import dirname

# sys.path.append(dirname(dirname(dirname(dirname(os.path.abspath(__file__))))) + r"\KMotion\Release")
# clr.AddReference("KMotion_dotNet")
# from KMotion_dotNet import KM_Controller

# KM = KM_Controller()

# class MyWindow(Window):
#     def __init__(self):
#         wpf.LoadComponent(self, 'OnOffWPF.xaml')
    
#     def ONButton_Click(self, sender, e):
#         KM.WriteLine("SetBit46")
       
#     def OFFButton_Click(self, sender, e):
#         KM.WriteLine("ClearBit46")

# if __name__ == '__main__':
#    Application().Run(MyWindow())
