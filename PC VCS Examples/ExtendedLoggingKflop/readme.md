## Extended Logging for Kflop ##
### What is it? ###
Extended logging allows plotting of Kflop data as it's generated on the board.
![Example plot](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/ExLogExamplePlot.png)
### Why? ###
Kmotion already has built in plotting, however it only allows at most 3.5 seconds of logging.  For most (all?) axis tuning this is enough.  However while trying to tune an axis that is both very slow (speed is in inch per hour) and with low resolution encoders (40 counts/mm)  I needed more than 3.5 seconds of logging to see exactly how the axis was performing.  Kmotion and the kflop already have tools to accomplish the job of logging for extended amounts of time, however it is an iterative approach.  Where data is generated on the kflop, then saved to the PC, then you need to plot in another program.  I wrote a little program to combine those steps, then it grew into Extended Logging for Kflop.
### How it works ###
In either a free thread or in your forever loop you send messages from the Kflop board to the PC with the **printf** statement.  The data is sent comma (',') delimited.  An example log statement looks like:
### **printf("@0,%g,%g,%g,%g\n",time,param1,param2,param3);** ###
Note that each log statement begins with the "**@**" sign followed by a single Hex digit (to allow up to 16 unique plot statements), then the delimited data to use in plotting.  Also not that each plot statement must include time (or some other counting variable) as the first parameter.  This variable is used as the X axis for all plots (current limitation).  To help with getting the printf statement correct the program will generate this for you after you define your plot.  

On the PC side the program monitors the console for these messages and delivers them to the plot(s) that have been configured to listen for them.  Non plot messages are printed to the console so you can still use printf.

### Getting Started ###

 

 - Either download the binary or source.  See downloading below.
 - Start the program:
 ![MainScreen](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/ExLogMainScreenEmpty.png)
 - If you are connected to the Kflop you should see the a green dot next to the word Status on the main toolbar.
 - Add a C program, press the Add button, go to the KMotionVersion\C Programs folder and select LogExampleSine.c (if you downloaded the source as zip these files will be in SampleCPrograms under source).  Then select it in the list.  Right click the thread number to change it to something other than 1. 
 ![enter image description here](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/ExLogMainScreenSineExample.png)
 - This is a simple program which loops 10000 times calculating sine values and logging them to the PC with printf.  It does this every 8 timeslices.  Note the format of the printf statement.  All log statements start with **@** then the log id (**0** in this case), then the log parameters.  First log parameter is always time.
## printf("@0,%f,%f,%f,%f\n",tcurr,y1,y2,y3); ##
  
 - Press the download button to compile and download to the kflop.
 - Press the new plot button ![plot icon](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/graph.png)
![empty plot](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/emptyplot.png) 
 - Press the Configure button.
 ![empty configuration](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/emptyconfigiure.png)
 
 - Use this screen to define your axis and series.  For now press the load button to use a previously saved configuration.
 - Press the Load button, and go to the KMotionVersion\Kmotion\Data directory and select the cpoVsTime.kfp (\PlotConfigTemplates in source)
 ![command position output vs time configuration](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/cpoVsTimeConfig.png)
 - Note the ResultIndex column, this configures where in the comma delimited array of the printf statement to look for the data.  The index starts at zero which is the "@0", 1 is would be time, and 2 the first parameter, 3 the second... etc.  Press OK.  The printf statement will be generated for you.
![configured plot](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/configuredplot.png)
 - At this point you can either load previous saved data (including data saved in Kmotion) or log new data.  To load previous data press the load button and select your comma delimited data (columns must match the format shown in the "Log Format" text box).  To start a new logging session press start.  Do that now.
 - With the plot configured and logging started, go back to the main screen and start the LogExampleSine.c program on the kflop.  You should see the plot update as data is sent to the PC, along with some status messages in the console.
![sinewave plot](https://gitlab.com/Greg9504/ExtendedLoggingKflop/raw/master/web/plotsine.png)  
 - Press the Stop button on the plot, you can now save the data, and export to image.  
 - The Zero X button will shift the data in the x axis to start at zero.  
 - It's possible to have multiple plots logging at the same time, using either the same printf (same Log ID) or with different printf statements (mutiple axis etc...)
 ## Downloading ##
### Gitclone ###
If you download the source using gitclone, clone the repository in \KMotion434a\PC VC Examples, then open the BuildExamples.sln and add the project file. 
### Zip file ###
There is a link above that will allow you to download a zip file with all the source.  Unzip in \KMotion434a\PC VC Examples  then open the BuildExamples.sln and add the project file.
### Binary ###
https://dl.dropboxusercontent.com/u/32233852/ExtendedLoggingRedist_0.0.0.3_For_KMotion434a.zip
Backup your KMotion434a directory.  Unzip the file in \KMotion434a directory, this will place the binaries in \KMotion434a\KMotion\Release, to run the program start the ExtendedLoggingKflop.exe

> Written with [StackEdit](https://stackedit.io/).