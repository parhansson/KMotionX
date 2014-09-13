/*	ModBusMaster.c is a library for the Dynomotion KFlop board.
	It complies with ModBus protocol, customized for exchanging 
	information between Industrial controllers and the KFlop board.
	
	To get a copy of the GNU General Public License see <http://www.gnu.org/licenses/>.
*/

#ifndef ModBusMaster_h
#define ModBusMaster_h

#define N_MB_REGISTERS 	(0x40)

#define PERSIST_MBREG_BLOCK_ADR		82  // persist.UserData array index to pointer-to-ModbusRWRegister

#define MBReg ((unsigned short *)persist.UserData[PERSIST_MBREG_BLOCK_ADR])

#endif
		