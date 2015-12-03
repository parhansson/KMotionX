"""
msettings.py:

Classes for handling machine settings.

"""
import cPickle

class TypedSetting(object):
    def __init__(self, min_setting, max_setting, transfer_type=float):
        self.settings_map = {}      # All settings, indexed by number and also by pithy name
        self.vectors = {}           # Keyed by first vector entry (one entry for number and one for name), value is list of MS
        self.min_setting = min_setting
        self.max_setting = max_setting
        self.blocks = {}            # map block number or key to block
        self.block_map = {}         # map setting number to block
        self.settings_list = []     # generated complete list of settings from min to max-1.
        self.transfer_type = transfer_type  # Transfer type from controller (int or float: int is 32-bit int, float is IEEE 64-bit float)
    def jsonify(self, values_only=False):
        """Return as object which can be serialized by json.dumps().  This format is used to
        to communicate with web browsers etc. using JSON.  Since it doesn't support reference counting, 
        we need to simplify the structure.
        """
        return {'min':self.min_setting,
                'max':self.max_setting,
                's':{k:ms.jsonify(values_only) for k,ms in self.settings_map.items() if isinstance(k, str) and ms.implemented},
               }
    def get_vector(self, setting_num):
        """Return vector (list) of settings given an arbitrary setting in that vector.  If not vectorized,
        return None.
        """
        for k, v in self.vectors.items():
            if setting_num in [ms.number for ms in v]:
                return v
        return None
    def deepcopy(self):
        """Return deep copy.  Do this by pickle/unpickle using string, since it is an efficient
        way to preserve reference structure.
        """
        s = cPickle.dumps(self, cPickle.HIGHEST_PROTOCOL)
        return cPickle.loads(s)
    def update(self, other, tskey):
        """Update this object with other.  Each MS in other's settings_list is examined and the
        corresponding setting here is updated, except if this is implemented and other isn't, then no change.
        Completely new settings (but not blocks) can be added this way, but require set_typed_settings_dict() to
        be called with tsd set to a dict containing this (since creating new MS uses this dict).
        """
        oldmin = self.min_setting
        oldmax = self.max_setting
        prepend = max(0, oldmin - other.min_setting)
        append = max(0, other.max_setting - oldmax)
        self.min_setting = min(oldmin, other.min_setting)
        self.max_setting = max(oldmax, other.max_setting)
        if prepend:
            print "Prepending", prepend, "settings to", tskey
            plist = [MS(n, implemented=False, key=other.settings_map[n].key, setting_type=tskey) 
                     for n in range(other.min_setting, oldmin)]
        else: plist = []
        if append:
            print "Appending", append, "settings to", tskey
            alist = [MS(n, implemented=False, key=other.settings_map[n].key, setting_type=tskey) 
                     for n in range(oldmax, other.max_setting)]
        else: alist = []
        self.settings_list = plist + self.settings_list + alist
        for n, b in self.blocks.items():
            if not isinstance(n, int):
                continue
            b.update(other.blocks[n], self)
            for n in range(b.low, b.high):
                if not b.settings[n-b.low]:
                    b.settings[n-b.low] = self.settings_list[n - self.min_setting]
        for ms in other.settings_list:
            try:
                if not ms.implemented and self.settings_map[ms.number].implemented:
                    continue
                thisms = self.settings_map[ms.number]
                if thisms.update(ms):
                    print "re-key required:", thisms.key, "->", ms.key, "in", tskey
                    oldkey = thisms.key
                    newkey = ms.key
                    thisms.key = newkey
                    del self.settings_map[oldkey]
                    self.settings_map[newkey] = thisms
            except KeyError:
                pass
        self.vectors.clear()
        for k, v in other.vectors.items():
            self.vectors[k] = [self.settings_map[ms.number] for ms in v]
        return self
    def load(self, other, stream, calibration=False, read_only=False, source_stream=None, source_current=False, load_if_same=False):
        """Load values from other into this, with given stream id.
        Unimplemented values (in either) are ignored.
        Calibration values in this are ignored unless 'calibration' is True.
        Similar for read_only values.
        If the source is conflicting, also ignored.
        source_stream specifies a source stream, or defaults to requiring a non-conflicting value.
        source_current specifies whether to get the current value from source.
        load_if_same if True if perform the load even if same as current value.
        """
        for ms in self.settings_list:
            if not ms.implemented or ms.calibration and not calibration or ms.read_only and not read_only:
                continue
            try:
                mss = other.settings_map[ms.number]
                if not mss.implemented :
                    continue
                val = mss.get_current_value() if source_current else mss.get_new_value(source_stream)
                if not load_if_same and val == ms.get_current_value():
                    continue
                ms.override_value(stream, val, -1)
            except KeyError:
                pass    # Not in other
            except ValueError:
                pass    # Conflicting in other
        return self
    def commit(self):
        """Commit all changes
        """
        for ms in self.settings_list:
            ms.get_value()
        return self
    def is_overridden(self):
        """Return if any changes pending in this group.
        """
        return any([ms.is_overridden() for ms in self.settings_list])
        

# Master list of settings:
typed_settings = {
    'motion_planner' : TypedSetting(0, 100),     # Motion planner settings (used by host software only)
    'monitor_int' : TypedSetting(0, 64, transfer_type=int),        # Monitor data transfer from controller (flags and int fields)
    'monitor_float' : TypedSetting(0, 64, transfer_type=float),  # Monitor data transfer from controller (continuous values)
    'controller' : TypedSetting(4060, 4400),    # Standard controller settings (flashed on controller)
    'custom' : TypedSetting(4400, 4500),        # Custom controller settings (flashed on controller)
    } #typed_settings

def set_typed_settings_dict(d):
    global typed_settings
    typed_settings = d


class SettingsBlock(object):
    def __init__(self, block_number, setting_low, setting_high, key, macro_key=None, desc=None, setting_type='controller'):
        self.block_number = block_number
        self.low = setting_low
        self.high = setting_high
        self.max = self.low     # track max used
        self.key = key
        self.desc = desc or key
        self.macro_key = key.upper() if macro_key is None else macro_key
        ts = typed_settings[setting_type]
        if setting_low < ts.min_setting or setting_high > ts.max_setting or setting_low >= setting_high:
            raise ValueError("Creating invalid machine setting block number %d: low-high out of range for setting type %s" % (block_number, setting_type))
        bd = ts.blocks
        bm = ts.block_map
        if any([n in bm for n in xrange(setting_low, setting_high)]):
            raise ValueError("Creating invalid machine setting block number %d: overlaps with another block for setting type %s" % (block_number, setting_type))
        for n in xrange(setting_low, setting_high):
            bm[n] = self
        bd[block_number] = self
        bd[key] = self
        self.settings = [None]*(setting_high-setting_low)
    def add_setting(self, s):
        self.settings[s.number - self.low] = s
    def get_setting(self, hashvar):
        return self.settings[hashvar - self.low]
    def update(self, other, ts):
        """Update this block with other block.  Only allows expansion of block.
        Assumes same block number.  New settings are set to None; need to be filled in by caller.
        """
        prepend = max(0, self.low - other.low)
        append = max(0, other.high - self.high)
        if prepend:
            print "Prepending", prepend, "settings to block", self.key
        if append:
            print "Appending", append, "settings to block", self.key
        self.low -= prepend
        self.high += append
        self.settings = [None]*prepend + self.settings + [None]*append
        for n in xrange(self.low, self.high):
            ts.block_map[n] = self
        self.desc = other.desc
        self.macro_key = other.macro_key
        

class AllVal(object):
    def __init__(self, *validators):
        self.validators = validators
    def __call__(self, ms):
        return all([v(ms) for v in self.validators])
    def get_error_msg(self, ms):
        for v in self.validators:
            if not v(ms):
                return v.get_error_msg(ms)
        return ''
class AnyVal(object):
    def __init__(self, *validators):
        self.validators = validators
    def __call__(self, ms):
        return any([v(ms) for v in self.validators])
    def get_error_msg(self, ms):
        if self(ms): return ''
        return ' and '.join([v.get_error_msg(ms) for v in self.validators])
        
class RangeVal(object):
    def __init__(self, low, high):
        self.low = low
        self.high = high
    def __call__(self, ms):
        v = ms.get_new_value()
        return v >= self.low and v <= self.high
    def get_error_msg(self, ms):
        if self(ms): return ''
        return 'Value outside range [%g..%g]' % (self.low, self.high)
class IntRangeVal(object):
    def __init__(self, low, high):
        self.low = low
        self.high = high
    def __call__(self, ms):
        v = ms.get_new_value()
        if ms.value_type(int(v)) != v:
            return False
        return v >= self.low and v <= self.high
    def get_error_msg(self, ms):
        if self(ms): return ''
        return 'Value outside range [%d..%d] or not an integer' % (self.low, self.high)
class BitfieldVal(object):
    def __init__(self, fields):
        self.mask = 0
        for m in [b.bitmask for b in fields if b.implemented]:
            self.mask |= m
    def __call__(self, ms):
        v = ms.get_new_value()
        if ms.value_type(int(v)) != v:
            return False
        return not (int(v) & ~self.mask)        
    def get_error_msg(self, ms):
        if self(ms): return ''
        return 'Not a valid bitfield value'

class MS(object):
    def __init__(self, number, desc=None, fields=None, units=None, default=None, implemented=True, read_only=False, \
                key=None, setting_type='controller', calibration=False, ctlact=None, validator=None, \
                doc=None, vec_component=None, vec_type=None, vec_component_name=None):
        """Machine setting:
        number - setting number (int)
        desc - short description (str)
        fields - None or a list of MSBF objects
        units - units (if not a bitfield)
        default - default value
        implemented - False if not implemented or reserved
        read_only - True if controller/target does not allow setting this value
        key - pithy name for this setting
        setting_type - main key in typed_settings (global) dict.
        calibration - whether this setting should be treated specially as an individual machine
                      calibration value.  Basically, these settings are protected from casual
                      updates.
        ctlact - controller action required to actually use this setting.  None if the setting is
                      always read when required (most cases), 'init' if only at controller init
                      time, etc.
        validator - callable taking this as a parameter: returns True if new value (or current value
                      if there are no pending overrides) is acceptable.  Call self.is_valid() to
                      invoke this.  Default is to allow any value.  Validator classes above should
                      be used (handy for pickling).
        doc - full documentation (may use Pango markup)
        vec_component - None if not a vector, or integer indexing the component (0,1,...)
        vec_type - if vector, is string indicating the vector type (currently 'element', 'probe' or 'component')
        vec_component_name - if vector, string indicating component name e.g. 'X','Y','Secondary' etc.
        """
        if setting_type is not None:
            ts = typed_settings[setting_type]
            sm = ts.settings_map
            if number < ts.min_setting or number >= ts.max_setting:
                raise ValueError("Creating invalid machine setting number %d" % number)
            if number in sm:
                raise ValueError("Machine setting number %d already defined" % number)
            self.value_type = ts.transfer_type 
        else:
            self.value_type = float
        self.number = number
        self.desc = desc or key or "#%d" % number
        self.fields = fields
        if fields:
            units = "bitfield"
        self.units = units
        self.default = default
        self.implemented=implemented
        self.read_only = read_only
        self.calibration = calibration
        self.ctlact = ctlact
        self.validator = validator
        self.doc = doc or ''
        self.vec_component = vec_component
        self.vec_type = vec_type
        self.vec_component_name = vec_component_name
        self.set_value(default or 0., True)
        self.clear_overrides()
        if key is None:
            print "Warning: setting number", number, "had no short name assigned."
            key = "MS%d" % number
        self.key = key
        if setting_type is not None:
            sm[number] = self
            if key in sm:
                raise ValueError("Machine setting name %s already defined" % key)
            sm[key] = self
            try:
                self.block = ts.block_map[number] # find the block we are in
                self.block.add_setting(self)
            except KeyError:
                if implemented:
                    raise ValueError("Machine setting number %d cannot find block" % number)
    def update(self, other):
        """Update attributes other than number, key or dynamic values, from another MS.
        Exception: if this is currently unimplemented, and other is, the current value is set.
        In this case, returns True to indicate caller should re-key this one.
        """
        self.desc = other.desc
        self.fields = other.fields
        self.units = other.units
        self.default = other.default
        self.implemented = other.implemented
        self.read_only = other.read_only
        self.calibration = other.calibration
        self.ctlact = other.ctlact
        self.validator = other.validator
        self.doc = other.doc
        self.vec_component = other.vec_component
        self.vec_type = other.vec_type
        self.vec_component_name = other.vec_component_name
        if not self.implemented and other.implemented:
            self.set_value(other.get_current_value())
        return self.key != other.key
    def jsonify(self, values_only=False):
        d = {'n':self.number, 'cv':self.get_current_value(),}
        if self.implemented and not values_only:
            d['d'] = self.desc
            d['u'] = self.units
            d['ro'] = self.read_only
            d['c'] = self.calibration
            d['D'] = self.doc
            if self.vec_component is not None:
                d['vc'] = self.vec_component
                d['vt'] = self.vec_type
                d['vn'] = self.vec_component_name
            if self.fields:
                d['f'] = [v.jsonify() for v in self.fields if v.implemented]
        return {'type':'MS', 'MS':d}
    def is_bitfield(self):
        return self.fields is not None
    def set_value(self, val, quiet=False):
        self.value = self.value_type(val)
        #if not quiet:
        #    print "-set", self.key, "=", val
    def is_overridden(self):
        """Return whether get_new_value() would be different than get_current_value().
        """
        try:
            return self.get_new_value() != self.get_current_value()
        except ValueError:
            return True
    def is_valid(self):
        if self.validator is None:
            try:
                self.get_new_value()
                return True
            except ValueError:
                return False
        try:
            return self.validator(self)
        except ValueError:
            return False
    def get_error_msg(self):
        if self.validator is None:
            try:
                self.get_new_value()
                return ''
            except ValueError:
                return 'More than one update, with conflicting values'
        try:
            return self.validator.get_error_msg(self)
        except ValueError:
            return 'More than one update, with conflicting values'
    def has_stream(self, stream):
        return stream in self.ovalue
    def has_conflict(self):
        try:
            self.get_new_value()
            return False
        except ValueError:
            return True
    def get_new_value(self, stream=None, verbose=False):
        """Get value, using overrides (if any) and also check for conflicts.
        Overrides are associated with one or more 'streams'.  If more than one stream,
        then they must all agree on the new value, otherwise raises ValueError.
        If stream is not None, then just get that stream's value without conflict check -
        if the stream does not exist, return the current value instead.
        """
        if stream and stream not in self.ovalue:
            return self.value
        oval = None
        for k, v in ([(stream, self.ovalue[stream])] if stream else self.ovalue.items()):
            if self.is_bitfield():
                m = self.omask[k]
                if oval is None:
                    oval = v
                    mask = m
                    strm = k
                else:
                    cmask = m & mask
                    if (cmask & oval) != (cmask & v):
                        if verbose:
                            print "Conflict: cmask=0x%X oval=0x%X v=0x%X m=0x%X mask=0x%X" % (cmask, oval, v, m, mask)
                        raise ValueError("Conflicting bitfield setting for %d/%s: %s and %s" % (self.number, self.key, strm, k))
                    oval |= m & v
                    mask |= m
            else:
                if oval is None:
                    oval = v
                    strm = k
                else:
                    if oval != v:
                        raise ValueError("Conflicting setting for %d/%s: %s and %s" % (self.number, self.key, strm, k))
        if oval is None:
            return self.value
        if self.is_bitfield():
            return self.value_type(oval & mask | int(self.value) & ~mask)
        return self.value_type(oval)
    def get_current_value(self):
        """Get value without overrides.
        """
        return self.value
    def get_value(self):
        """Get new value and commit.
        Commit means to replace current value with override, and clear overrides.
        Will raise ValueError if unresolved conflict.
        """
        rc = self.get_new_value()
        self.set_value(rc)
        self.clear_overrides()
        return rc
    def clear_overrides(self, stream=None):
        """Discard any pending overrides (roll back).
        If stream is None, delete all streams.  Otherwise, just the named stream.
        """
        if stream:
            try:
                del self.ovalue[stream]
                del self.omask[stream]
            except KeyError:
                pass
        else:
            self.ovalue = {}
            self.omask = {}
    def override_value(self, stream, val, mask=None):
        # stream is an arbitrary key for the override set (usually a variant string)
        if self.is_bitfield():
            if self.omask.get(stream, None) is not None:
                self.omask[stream] |= mask
                self.ovalue[stream] = self.ovalue[stream] & ~mask | int(val) & mask
            else:
                self.omask[stream] = mask
                self.ovalue[stream] = int(val) & mask
            #print "-over", self.key, "=", val, mask
        else:
            self.ovalue[stream] = self.value_type(val)
            #print "-over", self.key, "=", val
    def get_short_desc(self):
        return self.key + ': ' + self.desc.replace('(','[').replace(')',']')
    def value_str(self, current=False, stream=None):
        # Return current value as string - either modified value (default) or current.
        if self.is_bitfield():
            return "0x%08X" % (int(self.get_current_value() if current else self.get_new_value(stream)), )
        if self.units == "int":
            return "%d" % int(self.get_current_value() if current else self.get_new_value(stream))
        return self.as_gfloat(current, stream)
    def value_bool(self, msbf, current=False, stream=None):
        # Return whether field msbf is true - either modified value (default) or current.
        if not self.is_bitfield():
            return False
        v = int(self.get_current_value() if current else self.get_new_value(stream))
        return bool(v & msbf.bitmask)
    def __str__(self):
        bf = '\n'.join([str(f) for f in self.fields]) if self.fields else ''
        dsc = self.get_short_desc()
        return "MS %4d: %s" % (self.number, '\n'.join([dsc, bf]))
    def as_listing(self):
        # Format as string for application listing
        if self.implemented:
            ok = self.is_valid()
            return "#%4d: %16s %-16s [%s%s%s]" % (self.number, self.value_str(), self.units, '' if ok else 'Bad! ', 'CAL ' if self.calibration else '', self.key)
        return "#%4d: [reserved]" % (self.number, )
    def as_gfloat(self, current=False, stream=None):
        v = self.get_current_value() if current else self.get_new_value(stream)
        if v != 0. and abs(v) < 0.0001:
            s = "%.16f" % v
            while s.endswith('0'): s = s[:-1]
        elif abs(v) > 9.9999999e15:
            s = "%.0f" % v
        else:
            s = "%.16g" % v
        return s
    def as_gcode(self):
        # Must avoid exponential notation, g-code interpreter chokes.
        # This commits any pending overrides
        self.get_value()
        return "#%d=%s (%s)" % (self.number, self.as_gfloat(), self.get_short_desc())
    def as_c_header(self):
        # This commits any pending overrides
        self.get_value()
        return "/* %d %s */ %.16g" % (self.number, self.key, self.get_value(),)
        
class MSBF(object):
    """Machine setting bitfield.
    A list of these is passed as the 'fields' parameter of MS().
    """
    def __init__(self, bitmask, desc=None, doc=None, implemented=True):
        self.bitmask = bitmask
        self.desc = desc or "0x%X" % bitmask
        self.implemented = implemented
        self.doc = doc or ''
    def __str__(self):
        return "  MSBF 0x%04X: %s" % (self.bitmask, self.desc if self.implemented else "(not implemented)")
    def jsonify(self):
        return {'m':self.bitmask, 'd':self.desc, 'D':self.doc}
        

def ms_vector(base_number, dimension, desc=None, default=None, units=None, stride=1, fields=None, read_only=False, key=None, \
                setting_type='controller', calibration=False, ctlact=None, validator=None, doc=None, implemented=True, key_base=None, underscore=True):
    ts = typed_settings[setting_type]
    vm = ts.vectors
    # This is a bit hackish, but currently all 2-d vectors are for prim/sec probe, all 9-d for matrix, and the rest for XYZABC.
    if key_base is None:
        if dimension==2:
            thing=['Primary', 'Secondary']
            thing2="probe"
        elif dimension==9:  # 3x3 matrix
            thing=['00','01','02','10','11','12','20','21','22']
            thing2="element"
        else:
            thing = "XYZABCUV"
            thing2="component"
    else:
        thing2 = "element"
        thing = [str(n) for n in range(key_base, 16+key_base)]
    usc = '_' if underscore else ''
    arry = [MS(base_number+n*stride, desc=desc, units=units if units else "um" if n < 3 else "mdeg", \
                default=default[n] if default else None, fields=fields, read_only=read_only, key=key + usc + thing[n] if key else None, \
                setting_type=setting_type, calibration=calibration, ctlact=ctlact, validator=validator, doc=doc, \
                vec_component=n, vec_type=thing2, vec_component_name=thing[n], implemented=implemented) \
            for n in range(dimension)]
    vm[base_number] = arry
    if key is not None:
        vm[key] = arry


