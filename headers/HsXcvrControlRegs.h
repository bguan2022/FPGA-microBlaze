//--HsXcvrControlRegs.h-------------------------------------------------------------------------------------------------------------------------
// XcvrControls component register map
// Revision History:		Aug 2008:	Creation, Donald Dansereau
//------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __HSXCVR_CONTROL_REGS_H_
#define __HSXCVR_CONTROL_REGS_H_

#include <xio.h>

//--Xcvr Control Registers------------------------------------------------------------------------------------------------------------------
// XPAR_XCVRCONTROL_S_AXI_BASEADDR
#define XCVR_CTRL_RESET_STATUS_CTRL_REG				(0x0)
#define XCVR_CTRL_RESET_START						(1 << 0) // Self-clearing bit
#define XCVR_CTRL_LPMDFERESET_START					(1 << 1) // Self-clearing bit
#define XCVR_CTRL_RESET_DONE                        (1 << 2)
#define XCVR_CTRL_EYESCANRESET_START				(1 << 3) // Self-clearing bit
#define XCVR_CTRL_PLL_LOCK							(1 << 4)
#define XCVR_CTRL_CDR_LOCK	                        (1 << 5)
#define XCVR_CTRL_RXLPMEN_MASK						(1 << XCVR_CTRL_RXLPMEN_SHIFT)
#define XCVR_CTRL_RXLPMEN_SHIFT						(6)

// LPM/DFE
#define XCVR_CTRL_LPM_KL_STATE_REG					(0x1)
#define XCVR_CTRL_LPM_KL_STATE_MASK					(0xFFFF << XCVR_CTRL_LPM_KL_STATE_SHIFT)
#define XCVR_CTRL_LPM_KL_STATE_SHIFT				(0)

#define XCVR_CTRL_LPM_KH_STATE_REG					(0x2)
#define XCVR_CTRL_LPM_KH_STATE_MASK					(0xFFFF << XCVR_CTRL_LPM_KH_STATE_SHIFT)
#define XCVR_CTRL_LPM_KH_STATE_SHIFT				(0)

#define XCVR_CTRL_LPM_OS_STATE_REG					(0x3)
#define XCVR_CTRL_LPM_OS_STATE_MASK					(0xFFFF << XCVR_CTRL_LPM_OS_STATE_SHIFT)
#define XCVR_CTRL_LPM_OS_STATE_SHIFT				(0)

#define XCVR_CTRL_LPM_GC_STATE_REG					(0x4)
#define XCVR_CTRL_LPM_GC_STATE_MASK					(0xFFFF << XCVR_CTRL_LPM_GC_STATE_SHIFT)
#define XCVR_CTRL_LPM_GC_STATE_SHIFT				(0)

// Pre cursor, post cursor, diff
#define XCVR_CTRL_TX_PRE_CURSOR_REG0				(0x7)
#define XCVR_CTRL_TX_PRE_CURSOR_REG1				(0x8)
#define XCVR_CTRL_TX_PRE_CURSOR_REG2				(0x9)
#define XCVR_CTRL_TX_PRE_CURSOR_REG3				(0xA)
#define XCVR_CTRL_TX_PRE_CURSOR_REG4				(0xB)
#define XCVR_CTRL_TX_PRE_CURSOR_REG5				(0xC)
#define XCVR_CTRL_TX_PRE_CURSOR_REG6				(0xD)
#define XCVR_CTRL_TX_PRE_CURSOR_REG7				(0xE)
#define XCVR_CTRL_TX_POST_CURSOR_REG0				(0xF)
#define XCVR_CTRL_TX_POST_CURSOR_REG1				(0x10)
#define XCVR_CTRL_TX_POST_CURSOR_REG2				(0x11)
#define XCVR_CTRL_TX_POST_CURSOR_REG3				(0x12)
#define XCVR_CTRL_TX_POST_CURSOR_REG4				(0x13)
#define XCVR_CTRL_TX_POST_CURSOR_REG5				(0x14)
#define XCVR_CTRL_TX_POST_CURSOR_REG6				(0x15)
#define XCVR_CTRL_TX_POST_CURSOR_REG7				(0x16)

#define XCVR_CTRL_DMONITOR_OUT_REG0					(0x26)
#define XCVR_CTRL_DMONITOR_OUT_REG1					(0x27)
#define XCVR_CTRL_DMONITOR_OUT_REG2					(0x28)
#define XCVR_CTRL_DMONITOR_OUT_REG3					(0x29)
#define XCVR_CTRL_DMONITOR_OUT_REG4					(0x2A)
#define XCVR_CTRL_DMONITOR_OUT_REG5					(0x2B)
#define XCVR_CTRL_DMONITOR_OUT_REG6					(0x2C)
#define XCVR_CTRL_DMONITOR_OUT_REG7					(0x2D)
#define	XCVR_CTRL_DMONITOR_KH_SHIFT					(2)
#define	XCVR_CTRL_DMONITOR_KH_MASK					(0x1F << XCVR_CTRL_DMONITOR_KH_SHIFT)
#define	XCVR_CTRL_DMONITOR_KL_SHIFT					(2)
#define XCVR_CTRL_DMONITOR_KL_MASK					(0x1F << XCVR_CTRL_DMONITOR_KL_SHIFT)
#define	XCVR_CTRL_DMONITOR_OS_SHIFT					(0)
#define XCVR_CTRL_DMONITOR_OS_MASK					(0x7F << XCVR_CTRL_DMONITOR_OS_SHIFT)
#define	XCVR_CTRL_DMONITOR_GC_SHIFT					(2)
#define XCVR_CTRL_DMONITOR_GC_MASK					(0x1F << XCVR_CTRL_DMONITOR_GC_SHIFT)

#define XCVR_CTRL_TX_DIFF_REG0						(0x2E)
#define XCVR_CTRL_TX_DIFF_REG1						(0x2F)
#define XCVR_CTRL_TX_DIFF_REG2						(0x30)
#define XCVR_CTRL_TX_DIFF_REG3						(0x31)
#define XCVR_CTRL_TX_DIFF_REG4						(0x32)
#define XCVR_CTRL_TX_DIFF_REG5						(0x33)
#define XCVR_CTRL_TX_DIFF_REG6						(0x34)
#define XCVR_CTRL_TX_DIFF_REG7						(0x35)
#define XCVR_CTRL_TX_DIFF_REG8						(0x36)
#define XCVR_CTRL_TX_DIFF_REG9						(0x37)

#define XCVR_CTRL_TX_DRIVER_CTRL_MASK				(0x1F)
#define XCVR_CTRL_TX_MAIN_CURSOR_CTRL_SHIFT         (5)
#define XCVR_CTRL_TX_MAIN_CURSOR_CTRL_MASK          (0x7F << XCVR_CTRL_TX_MAIN_CURSOR_CTRL_SHIFT)

#define XcvrControlSetReg( XcvrControlBase, RegIdx, NewVal )	XIo_Out32( (XcvrControlBase) + (RegIdx), (NewVal) )
#define XcvrControlGetReg( XcvrControlBase, RegIdx )			XIo_In32( (XcvrControlBase) + (RegIdx) )

#endif /*__HSXCVR_CONTROL_REGS_H_*/
//------------------------------------------------------------------------------------------------------------------------------------------
