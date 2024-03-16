wchar_t * _rs274ngc_errors[] = {
/*   0 */ L"No error",
/*   1 */ L"No error",
/*   2 */ L"No error",
/*   3 */ L"No error",
/*   4 */ L"A file is already open", // rs274ngc_open
/*   5 */ L"All axes missing with either g52 or g92", // enhance_block
/*   6 */ L"All axes missing with motion code", // enhance_block
/*   7 */ L"Arc radius too small to reach end point", // arc_data_r
/*   8 */ L"Argument to acos out of range", // execute_unary
/*   9 */ L"Argument to asin out of range", // execute_unary
/*  10 */ L"Attempt to divide by zero", // execute_binary1
/*  11 */ L"Attempt to raise negative to non integer power", // execute_binary1
/*  12 */ L"Bad character used", // read_one_item
/*  13 */ L"Bad format unsigned integer", // read_integer_unsigned
/*  14 */ L"Bad number format", // read_real_number
/*  15 */ L"Bug bad g code modal group 0", // check_g_codes
/*  16 */ L"Bug code not g0 or g1", // convert_straight, convert_straight_comp1, convert_straight_comp2
/*  17 */ L"Bug code not g17 g18 or g19", // convert_set_plane
/*  18 */ L"Bug code not g20 or g21", // convert_length_units
/*  19 */ L"Bug code not g28 or g30", // convert_home
/*  20 */ L"Bug code not g2 or g3", // arc_data_comp_ijk, arc_data_ijk
/*  21 */ L"Bug code not g40 g41 or g42", // convert_cutter_compensation
/*  22 */ L"Bug code not g43 or g49", // convert_tool_length_offset
/*  23 */ L"Bug code not g4 g10 g28 g30 g53 or g92 series", // convert_modal_0
/*  24 */ L"Bug code not g61 g61 1 or g64", // convert_control_mode
/*  25 */ L"Bug code not g90 or g91", // convert_distance_mode
/*  26 */ L"Bug code not g93 or g94 or g95", // convert_feed_mode
/*  27 */ L"Bug code not g98 or g99", // convert_retract_mode
/*  28 */ L"Bug code not in g92 series", // convert_axis_offsets
/*  29 */ L"Bug code not in range g54 to g593", // convert_coordinate_system
/*  30 */ L"Bug code not m0 m1 m2 m30 m47 m60", // convert_stop
/*  31 */ L"Bug distance mode not g90 or g91", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  32 */ L"Bug function should not have been called", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx, read_a, read_b, read_c, read_comment, read_d, read_f, read_g, read_h, read_i, read_j, read_k, read_l, read_line_number, read_m, read_p, read_parameter, read_parameter_setting, read_q, read_r, read_real_expression, read_s, read_t, read_x, read_y, read_z
/*  33 */ L"Bug in tool radius comp", // arc_data_comp_r
/*  34 */ L"Bug plane not xy yz or xz", // convert_arc, convert_cycle
/*  35 */ L"Bug side not right or left", // convert_straight_comp1, convert_straight_comp2
/*  36 */ L"Bug unknown motion code", // convert_motion
/*  37 */ L"Bug unknown operation", // execute_binary1, execute_binary2, execute_unary
/*  38 */ L"Cannot change axis offsets with cutter radius comp", // convert_axis_offsets
/*  39 */ L"Cannot change units with cutter radius comp", // convert_length_units
/*  40 */ L"Cannot create backup file", // rs274ngc_save_parameters
/*  41 */ L"Cannot do g1 with zero feed rate", // convert_straight
/*  42 */ L"Cannot do zero repeats of cycle", // convert_cycle
/*  43 */ L"Cannot make arc with zero feed rate", // convert_arc
/*  44 */ L"Cannot move rotary axes during probing", // convert_probe
/*  45 */ L"Cannot open backup file", // rs274ngc_save_parameters
/*  46 */ L"Cannot open variable file", // rs274ngc_save_parameters
/*  47 */ L"Cannot probe in inverse time feed mode", // convert_probe
/*  48 */ L"Cannot probe with cutter radius comp on", // convert_probe
/*  49 */ L"Cannot probe with zero feed rate", // convert_probe
/*  50 */ L"Cannot put a b in canned cycle", // check_other_codes
/*  51 */ L"Cannot put a c in canned cycle", // check_other_codes
/*  52 */ L"Cannot put an a in canned cycle", // check_other_codes
/*  53 */ L"Cannot turn cutter radius comp on out of xy plane", // convert_cutter_compensation_on
/*  54 */ L"Cannot turn cutter radius comp on when on", // convert_cutter_compensation_on
/*  55 */ L"Cannot use a word", // read_a
/*  56 */ L"Cannot use axis values with g80", // enhance_block
/*  57 */ L"Cannot use axis values without a g code that uses them", // enhance_block
/*  58 */ L"Cannot use b word", // read_b
/*  59 */ L"Cannot use c word", // read_c
/*  60 */ L"Cannot use g28 or g30 with cutter radius comp", // convert_home
/*  61 */ L"Cannot use g53 incremental", // check_g_codes
/*  62 */ L"Cannot use g53 with cutter radius comp", // convert_straight
/*  63 */ L"Cannot use two g codes that both use axis values", // enhance_block
/*  64 */ L"Cannot use xz plane with cutter radius comp", // convert_set_plane
/*  65 */ L"Cannot use yz plane with cutter radius comp", // convert_set_plane
/*  66 */ L"Command too long", // read_text, rs274ngc_open
/*  67 */ L"Concave corner with cutter radius comp", // convert_arc_comp2, convert_straight_comp2
/*  68 */ L"Coordinate system index parameter 5220 out of range", // rs274ngc_init
/*  69 */ L"Current point same as end point of arc", // arc_data_r
/*  70 */ L"Cutter gouging with cutter radius comp", // convert_arc_comp1, convert_straight_comp1
/*  71 */ L"D word with no g41, g42, or g96", // check_other_codes
/*  72 */ L"Dwell time missing with g4", // check_g_codes
/*  73 */ L"Dwell time p word missing with g82", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  74 */ L"Dwell time p word missing with g86", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  75 */ L"Dwell time p word missing with g88", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  76 */ L"Dwell time p word missing with g89", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  77 */ L"Equal sign missing in parameter setting", // read_parameter_setting
/*  78 */ L"F word missing with inverse time arc move", // convert_arc
/*  79 */ L"F word missing with inverse time g1 move", // convert_straight
/*  80 */ L"File ended with no percent sign", // read_text, rs274ngc_open
/*  81 */ L"File ended with no percent sign or program end", // read_text
/*  82 */ L"File name too long", // rs274ngc_open
/*  83 */ L"File not open", // rs274ngc_read
/*  84 */ L"G code out of range", // read_g
/*  85 */ L"H word with no g43 or g43.4", // check_other_codes
/*  86 */ L"I word given for arc in yz plane", // convert_arc
/*  87 */ L"I word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  88 */ L"I word with no g2 or g3 or g87 to use it", // check_other_codes
/*  89 */ L"J word given for arc in xz plane", // convert_arc
/*  90 */ L"J word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  91 */ L"J word with no g2 or g3 or g87 to use it", // check_other_codes
/*  92 */ L"K word given for arc in xy plane", // convert_arc
/*  93 */ L"K word missing with g87", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/*  94 */ L"K word with no g2 or g3 or g83 or g87 to use it", // check_other_codes
/*  95 */ L"L word with no canned cycle or g10 or M98", // check_other_codes
/*  96 */ L"Left bracket missing after slash with atan", // read_atan
/*  97 */ L"Left bracket missing after unary operation name", // read_unary
/*  98 */ L"Line number greater than 99999", // read_line_number
/*  99 */ L"Line with g10 does not have l2", // check_g_codes
/* 100 */ L"M code greater than 119", // read_m
/* 101 */ L"Mixed radius ijk format for arc", // convert_arc
/* 102 */ L"Multiple a words on one line", // read_a
/* 103 */ L"Multiple b words on one line", // read_b
/* 104 */ L"Multiple c words on one line", // read_c
/* 105 */ L"Multiple d words on one line", // read_d
/* 106 */ L"Multiple f words on one line", // read_f
/* 107 */ L"Multiple h words on one line", // read_h
/* 108 */ L"Multiple i words on one line", // read_i
/* 109 */ L"Multiple j words on one line", // read_j
/* 110 */ L"Multiple k words on one line", // read_k
/* 111 */ L"Multiple l words on one line", // read_l
/* 112 */ L"Multiple p words on one line", // read_p
/* 113 */ L"Multiple q words on one line", // read_q
/* 114 */ L"Multiple r words on one line", // read_r
/* 115 */ L"Multiple s words on one line", // read_s
/* 116 */ L"Multiple t words on one line", // read_t
/* 117 */ L"Multiple x words on one line", // read_x
/* 118 */ L"Multiple y words on one line", // read_y
/* 119 */ L"Multiple z words on one line", // read_z
/* 120 */ L"Must use g0 or g1 with g53", // check_g_codes
/* 121 */ L"Negative argument to sqrt", // execute_unary
/* 122 */ L"Negative d word tool radius index used", // read_d
/* 123 */ L"Negative f word used", // read_f
/* 124 */ L"Negative g code used", // read_g
/* 125 */ L"Negative h word tool length offset index used", // read_h
/* 126 */ L"Negative l word used", // read_l
/* 127 */ L"Negative m code used", // read_m
/* 128 */ L"Negative q value used", // read_q
/* 129 */ L"Negative p word used", // read_p
/* 130 */ L"Negative spindle speed used", // read_s
/* 131 */ L"Negative tool id used", // read_t
/* 132 */ L"Nested comment found", // close_and_downcase
/* 133 */ L"No characters found in reading real value", // read_real_value
/* 134 */ L"No digits found where real number should be", // read_real_number
/* 135 */ L"Non integer value for integer", // read_integer_value
/* 136 */ L"Null missing after newline", // close_and_downcase
/* 137 */ L"Offset index missing", // convert_tool_length_offset
/* 138 */ L"P value not an integer with g10 l2 M98", // check_g_codes
/* 139 */ L"P value out of range with g10 l2", // check_g_codes
/* 140 */ L"P word with no g4 g10 g82 g83 g86 g88 g89 M98 M100-119", // check_other_codes
/* 141 */ L"Parameter file out of order", // rs274ngc_restore_parameters, rs274ngc_save_parameters
/* 142 */ L"Parameter number out of range", // read_parameter, read_parameter_setting, rs274ngc_restore_parameters, rs274ngc_save_parameters
/* 143 */ L"Q word missing with g83", // convert_cycle_xy, convert_cycle_yz, convert_cycle_zx
/* 144 */ L"Q word with no g83, g94, M98, or M100-119", // check_other_codes
/* 145 */ L"Queue is not empty after probing", // rs274ngc_read
/* 146 */ L"R clearance plane unspecified in cycle", // convert_cycle
/* 147 */ L"R i j k words all missing for arc", // convert_arc
/* 148 */ L"R less than x in cycle in yz plane", // convert_cycle_yz
/* 149 */ L"R less than y in cycle in xz plane", // convert_cycle_zx
/* 150 */ L"R less than z in cycle in xy plane", // convert_cycle_xy
/* 151 */ L"R word with no g code or m code that uses it", // check_other_codes
/* 152 */ L"Radius to end of arc differs from radius to start", // arc_data_comp_ijk, arc_data_ijk
/* 153 */ L"Radius too small to reach end point", // arc_data_comp_r
/* 154 */ L"Required parameter missing", // rs274ngc_restore_parameters
/* 155 */ L"Selected tool slot number too large", // convert_tool_select
/* 156 */ L"Slash missing after first atan argument", // read_atan
/* 157 */ L"Spindle not turning clockwise in g84", // convert_cycle_g84
/* 158 */ L"Spindle not turning in g86", // convert_cycle_g86
/* 159 */ L"Spindle not turning in g87", // convert_cycle_g87
/* 160 */ L"Spindle not turning in g88", // convert_cycle_g88
/* 161 */ L"Sscanf failed", // read_integer_unsigned, read_real_number
/* 162 */ L"Start point too close to probe point", // convert_probe
/* 163 */ L"Too many m codes on line", // check_m_codes
/* 164 */ L"Tool length offset index too big", // read_h
/* 165 */ L"Tool max too large", // rs274ngc_load_tool_table
/* 166 */ L"Tool radius index too big", // read_d
/* 167 */ L"Tool radius not less than arc radius with comp", // arc_data_comp_r, convert_arc_comp2
/* 168 */ L"Two g codes used from same modal group", // read_g
/* 169 */ L"Two m codes used from same modal group", // read_m
/* 170 */ L"Unable to open file", // convert_stop, rs274ngc_open, rs274ngc_restore_parameters
/* 171 */ L"Unclosed comment found", // close_and_downcase
/* 172 */ L"Unclosed expression", // read_operation
/* 173 */ L"Unknown g code used", // read_g
/* 174 */ L"Unknown m code used", // read_m
/* 175 */ L"Unknown operation", // read_operation
/* 176 */ L"Unknown operation name starting with a", // read_operation
/* 177 */ L"Unknown operation name starting with m", // read_operation
/* 178 */ L"Unknown operation name starting with o", // read_operation
/* 179 */ L"Unknown operation name starting with x", // read_operation
/* 180 */ L"Unknown word starting with a", // read_operation_unary
/* 181 */ L"Unknown word starting with c", // read_operation_unary
/* 182 */ L"Unknown word starting with e", // read_operation_unary
/* 183 */ L"Unknown word starting with f", // read_operation_unary
/* 184 */ L"Unknown word starting with l", // read_operation_unary
/* 185 */ L"Unknown word starting with r", // read_operation_unary
/* 186 */ L"Unknown word starting with s", // read_operation_unary
/* 187 */ L"Unknown word starting with t", // read_operation_unary
/* 188 */ L"Unknown word where unary operation could be", // read_operation_unary
/* 189 */ L"X and y words missing for arc in xy plane", // convert_arc
/* 190 */ L"X and z words missing for arc in xz plane", // convert_arc
/* 191 */ L"X value unspecified in yz plane canned cycle", // convert_cycle_yz
/* 192 */ L"X y and z words all missing with g38 2", // convert_probe
/* 193 */ L"Y and z words missing for arc in yz plane", // convert_arc
/* 194 */ L"Y value unspecified in xz plane canned cycle", // convert_cycle_zx
/* 195 */ L"Z value unspecified in xy plane canned cycle", // convert_cycle_xy
/* 196 */ L"Zero or negative argument to ln", // execute_unary
/* 197 */ L"Zero radius arc", // arc_data_ijk
/* 198 */ L"Stack Overflow", // Subroutine Stack
/* 199 */ L"Subroutine not found", // Subroutine Lable search
/* 200 */ L"Stack Underflow - Sub return before call", // Stack underflow
/* 201 */ L"Invalid Subroutine Label - only Oxxx and simple comment allowed", 
/* 202 */ L"Q value not an integer with M98",
/* 203 */ L"L value not an integer with M98",
/* 204 */ L"Bug code not g96 or g97",
/* 205 */ L"Cannot do g32 with zero feed rate", // convert_thread
/* 206 */ L"Cannot use g32 with cutter radius comp", // convert_thread
/* 207 */ L"Tool ID not found in Tool Table", // convert_tool_select
/* 208 */ L"Tool Slot not found in Tool Table", // convert_tool_select
/* 209 */ L"Cannot put a u in canned cycle", // check_other_codes
/* 210 */ L"Cannot put a v in canned cycle", // check_other_codes
/* 211 */ L"Multiple u words on one line", // read_u
/* 212 */ L"Multiple v words on one line", // read_v
/* 213 */ L"Invalid Q Value in G83 Canned Cycle", // convert_cycle_g83

L"The End"};


