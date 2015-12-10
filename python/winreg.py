#!/usr/bin/env python

# This startup script uses a DOS console for debugging.  Only required if
# exception occurs on start-up, before stdout/stderr can be redirected to log window.
import traceback
try:
    import errno, _winreg

    #arch_keys = {0:"Default", _winreg.KEY_WOW64_32KEY:"64:32", _winreg.KEY_WOW64_64KEY:"64:64"}
    arch_keys = {0:"Default"}

    for arch_key in arch_keys.keys():
        print "arch_key:", arch_key
        #for node in [r"SOFTWARE\wow6432node\MDAPrecision\MenigCNC\v1", r"SOFTWARE\MDAPrecision\MenigCNC\v1"]:
        for node in [r"SOFTWARE\MDAPrecision\MenigCNC"]:
            print "node:", node
            try:
                with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, node, 0, _winreg.KEY_READ | arch_key) as key:
                    nsubkeys, nvalues, _ = _winreg.QueryInfoKey(key)
                    print "key OK; nsubs=", nsubkeys, "nvalues=", nvalues
                    if nvalues:
                        try:
                            print "InstallPath value..."
                            print _winreg.QueryValueEx(key, 'InstallPath') #[0]
                        except WindowsError:
                            print "InstallPath not located here"
                        
                    for i in xrange(0, nsubkeys):
                        skey_name = _winreg.EnumKey(key, i)
                        with _winreg.OpenKey(key, skey_name) as skey:
                            try:
                                print skey_name, ':'
                                print _winreg.QueryValueEx(skey, 'InstallPath') #[0]
                            except OSError as e:
                                if e.errno == errno.ENOENT:
                                    # DisplayName doesn't exist in this skey
                                    print "No InstallPath in", arch_keys[arch_key]
                                    pass
            except WindowsError:
                print "fluffed"

except:
    traceback.print_exc(32)
    
print
print "[Hit enter to quit]"
raw_input()

