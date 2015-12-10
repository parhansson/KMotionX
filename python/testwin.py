#!/usr/bin/env python

# This startup script uses a DOS console for debugging.  Only required if
# exception occurs on start-up, before stdout/stderr can be redirected to log window.
import traceback
try:
    import kmotion
    from kmotion.kmotion_extras import *

    def poll(k):
        k.Poll()
        while True:
            m = k.get_message()
            if not m: break
            print m
            if m.is_fatal():
                # Kflop comms error
                print "Kflop disconnected!"
            

    k = KMotionX(0, with_console=True)
            
    poll(k)

    if True:
        print "FirmwareVersion:", k.FirmwareVersion()
        print k.WriteLineReadLine("version")
        print "Locations:", k.ListLocations()
        print "USBLocation:", k.USBLocation()
        print "CheckForReady:", k.CheckForReady()

    def flash(k):
        for i in range(10):
            k.WriteLine("setbit47")
            time.sleep(0.05)
            k.WriteLine("clearbit47")
            time.sleep(0.05)

    flash(k)

    poll(k)
except:
    traceback.print_exc(32)
    print
    print "[Hit enter to quit]"
    raw_input()

