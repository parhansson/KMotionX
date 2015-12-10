
# kmotion module init, for Windows.

# First, locate MenigCNC install path, since it contains KMotionServer.exe, compiler, &c.
import _winreg
_menigcnc_version = ''  # if version found, will be 'v2' etc.
_menigcnc_root = None   # set to {app} installation directory e.g. 'c:\MenigCNC'
with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\MDAPrecision\MenigCNC", 0, _winreg.KEY_READ) as key:
    nsubkeys, nvalues, _ = _winreg.QueryInfoKey(key)
    #print "key OK; nsubs=", nsubkeys, "nvalues=", nvalues
    for i in xrange(0, nsubkeys):
        skey_name = _winreg.EnumKey(key, i)
        #print "skey_name", skey_name
        if not skey_name.lower().startswith('v'):
            continue
        with _winreg.OpenKey(key, skey_name) as skey:
            try:
                print skey_name, ':', _winreg.QueryValueEx(skey, 'InstallPath') #[0]
                
                root = str(_winreg.QueryValueEx(skey, 'InstallPath')[0])
                if skey_name > _menigcnc_version:
                    _menigcnc_version = skey_name
                    _menigcnc_root = root
            except OSError as e:
                if e.errno == errno.ENOENT:
                    #print "No InstallPath in", skey
                    pass

if not _menigcnc_root:
    raise RuntimeError("MenigCNC installation root not found in Windows registry")

__all__ = ['_kmotion.pyd', 'kmotion.py', 'kmotion_extras.py']
from .kmotion import *

