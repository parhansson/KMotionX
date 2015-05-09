import time
try:
    import kmotion
    from kmotion_extras import *
except:
    print "Failed to import kmotion module"
    print "Did you forget to..."
    print "export PYTHONPATH=/home/steve/KMotionX/bin"
    print "export KMOTION_BIN=/home/steve/KMotionX/bin"
    raise

def poll(k):
    k.Poll()
    while True:
        m = k.get_message()
        if not m: break
        print m
        if m.is_fatal():
            # Kflop comms error
            print "Kflop disconnected!"
        

# Select TCP or unix domain socket...
#k = KMotionX(0, "localhost", with_console=True)
k = KMotionX(0, with_console=True)
        
poll(k)

if True:
    print "FirmwareVersion:", k.FirmwareVersion()
    print k.WriteLineReadLine("version")
    print "Locations:", k.ListLocations()
    print "USBLocation:", k.USBLocation()
    print "CheckForReady:", k.CheckForReady()

def flash(k):
    for i in range(3):
        k.WriteLine("setbit47")
        time.sleep(0.05)
        k.WriteLine("clearbit47")
        time.sleep(0.05)

flash(k)

poll(k)

