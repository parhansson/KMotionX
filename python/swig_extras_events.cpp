    // Event codes for GCodeInterpreter::Poll().
    // For efficiency in user program, put in every conceivable event...
    // Note that some of these are actually N events e.g. CHG_POS is the first of
    // 8 events, one each for X (axis 0), Y, Z...axis 7.
    
    // Code needs to define macros before including this file:
    //  GEV(enum_tag,fld,cbsel)
    //  GEVX(enum_tag, first_elem, array_size,fld,cbsel)
    //  GEVXX(enum_tag, array_size,fld,fldcast,cbsel)
    //  GEVXB(enum_tag, array_size, bitfield_size,fld)
    // cbsel should be one of cb_bool, cb_int, cb_float

#if defined(SWIG) || defined(GEV_GEN_ENUM)
    // SWIG needs to %import this as a full enum definition
#define GEV(tag,fld,cbsel) tag,
#define GEVX(tag, ff, n,fld,cbsel) tag, _LAST_##tag = tag+n-1-ff,
#define GEVXX(tag, n,fld,typ,cbsel) tag, _LAST_##tag = tag+n-1,
#define GEVXB(tag, n, bits,fld) tag, _LAST_##tag = tag+n-1,
#define GEV_BLOCK_KMOTION
#define GEV_BLOCK_INTERPETER
enum GEvtCode {
#endif  //SWIG
    
#ifdef GEV_BLOCK_KMOTION
    // First block corresponds to KMotion MAIN_STATUS fields
    GEVX(CHG_POS,0,N_CHANNELS,Position,cb_float)
    GEVX(CHG_DEST,0,N_CHANNELS,Dest,cb_float)
    GEVX(CHG_ADC,0,N_ADCS,ADC,cb_int)
    GEVX(CHG_DAC,0,N_DACS,DAC,cb_int)
    GEVX(CHG_PWM,0,N_PWMS,PWM,cb_int)
    GEVX(CHG_OUT_CHAN,0,N_CHANNELS,OutputChan0,cb_int)
    GEVXB(CHG_IN_MODE,N_CHANNELS,4,InputModes)
    GEVXB(CHG_OUT_MODE,N_CHANNELS,4,OutputModes)
    GEVXB(CHG_ENABLE,8,1,Enables)
    GEVXB(CHG_DONE,8,1,AxisDone)
    GEVXB(CHG_IO_DIR,64,1,BitsDirection[0])
    GEVXB(CHG_IO_STATE,64,1,BitsState[0])
    GEVXB(CHG_THREAD,8,1,ThreadActive)
    GEV(CHG_STOP,StopImmediateState,cb_int)
    GEVX(CHG_PCCOMM,0,1,PC_comm,cb_int)     // only looks for command var (persist[100]) changes
    GEVX(CHG_PCCOMMMFB,4,5,PC_comm,cb_int)  // looks for MPG feedback vars (persist[104]) changes
    GEVX(CHG_PCCOMMSFB,5,6,PC_comm,cb_int)  // looks for spindle speed/load feedback vars (persist[105]) changes
    GEVX(CHG_PCCOMMIO,6,7,PC_comm,cb_int)   // looks for I/O status vars (persist[106]) changes
    GEVX(CHG_PCCOMMST,7,8,PC_comm,cb_int)   // looks for general status vars (persist[107]) changes
    #undef GEV_BLOCK_KMOTION
#endif //GEV_BLOCK_KMOTION

#ifdef GEV_BLOCK_INTERPETER
    // This block is for GCode interpreter events (contents of the rs274ngc setup struct)
    GEVX(CHG_ACTIVE_G,0,RS274NGC_ACTIVE_G_CODES,active_g_codes,cb_int)
    GEVX(CHG_ACTIVE_M,0,RS274NGC_ACTIVE_M_CODES,active_m_codes,cb_int)
    GEVXX(CHG_G92_OFFSET,6,axis_offset_x,double,cb_float)
    GEVXX(CHG_ORIGIN_OFFSET,6,origin_offset_x,double,cb_float)
    GEVXX(CHG_CURRENT,6,current_x,double,cb_float)
    GEV(CHG_CONTROL_MODE,control_mode,cb_int)
    GEV(CHG_SPINDLE_MODE,spindle_mode,cb_int)
    GEV(CHG_CURRENT_SLOT,current_slot,cb_int)
    GEV(CHG_CUTTER_COMP_RADIUS,cutter_comp_radius,cb_float)
    GEV(CHG_CUTTER_COMP_SIDE,cutter_comp_side,cb_int)
    GEV(CHG_CANNED_CYCLE_VALUE,cycle_cc,cb_float)
    GEV(CHG_CANNED_CYCLE_I,cycle_i,cb_float)
    GEV(CHG_CANNED_CYCLE_J,cycle_j,cb_float)
    GEV(CHG_CANNED_CYCLE_K,cycle_k,cb_float)
    GEV(CHG_CANNED_CYCLE_L,cycle_l,cb_int)
    GEV(CHG_CANNED_CYCLE_P,cycle_p,cb_float)
    GEV(CHG_CANNED_CYCLE_Q,cycle_q,cb_float)
    GEV(CHG_CANNED_CYCLE_R,cycle_r,cb_float)
    GEV(CHG_DISTANCE_MODE,distance_mode,cb_int)
    GEV(CHG_FEED_MODE,feed_mode,cb_int)
    GEV(CHG_FEED_OVERRIDE,feed_override,cb_bool)
    GEV(CHG_FEED_RATE,feed_rate,cb_float)
    GEV(CHG_FLOOD,flood,cb_bool)
    GEV(CHG_MIST,mist,cb_bool)
    GEV(CHG_LENGTH_OFFSET_INDEX,length_offset_index,cb_int)
    GEV(CHG_LENGTH_UNITS,length_units,cb_int)        // G20/G21 change
    GEV(CHG_MOTION_MODE,motion_mode,cb_int)
    GEV(CHG_ORIGIN_INDEX,origin_index,cb_int)
    GEV(CHG_ACTIVE_PLANE,plane,cb_int)
    GEV(CHG_PROBE_FLAG,probe_flag,cb_bool)
    GEV(CHG_PROGRAM_X,program_x,cb_float)          // for cutter comp
    GEV(CHG_PROGRAM_Y,program_y,cb_float)
    GEV(CHG_RETRACT_MODE,retract_mode,cb_int)
    GEV(CHG_SELECTED_TOOL_SLOT,selected_tool_slot,cb_int)
    GEV(CHG_SEQUENCE_NUMBER,sequence_number,cb_int)
    GEV(CHG_SPINDLE_SPEED,speed,cb_float)
    GEV(CHG_SPEED_FEED_MODE,speed_feed_mode,cb_int)
    GEV(CHG_SPEED_OVERRIDE,speed_override,cb_bool)
    GEV(CHG_SPINDLE_TURNING,spindle_turning,cb_int)
    //GEV(CHG_TOOL_LENGTH_OFFSET,tool_length_offset,cb_float)
    GEVXX(CHG_TOOL_LENGTH_OFFSET,3,tool_length_offset,double,cb_float)   // was changed to XYZ offsets
    GEV(CHG_TOOL_TABLE_INDEX,tool_table_index,cb_int)
    GEV(CHG_TRAVERSE_RATE,traverse_rate,cb_float)
    GEV(CHG_BLOCK_DELETE,block_delete,cb_bool)
    GEV(CHG_CUTTER_RADIUS_COMPENSATION,cutter_radius_compensation,cb_int)
    GEV(CHG_CYCLE_Z,cycle_z,cb_float)
    GEV(CHG_CURRENT_LINE,current_line,cb_int)
    #undef GEV_BLOCK_INTERPETER
#endif //GEV_BLOCK_INTERPETER

#if defined(SWIG) || defined(GEV_GEN_ENUM)
    LAST_GEVTCODE    // Use as a base code for extensions in the application
};
#endif

#ifdef GEV
    #undef GEV
#endif
#ifdef GEVX
    #undef GEVX
#endif
#ifdef GEVXX
    #undef GEVXX
#endif
#ifdef GEVXB
    #undef GEVXB
#endif


