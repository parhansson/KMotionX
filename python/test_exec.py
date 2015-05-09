"""Test compilation and execution of C programs on Kflop
"""

import time
try:
    import kmotion
    from kmotion_extras import *
except:
    print "Failed to import kmotion module"
    print "Did you forget to..."
    print "export PYTHONPATH=<KMmotionX install path>/bin"
    print "export KMOTION_BIN=<KMmotionX install path>/bin"
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

def idle_func(kthread):
    global k
    poll(k)

# Select TCP or unix domain socket...
k = KMotionX(0, "192.168.7.2", with_console=True)
#k = KMotionX(0, with_console=True)
        
poll(k)

print k.WriteLineReadLine("version")

cfile = "./test.c"
kthread = 3   # Run in kflop thread 3
k.run_c(cfile, kthread)
k.wait_c(kthread, idle_func)

poll(k)

print "done!"

