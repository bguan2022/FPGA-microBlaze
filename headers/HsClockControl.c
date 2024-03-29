//--HsClockControl.c------------------------------------------------------------------------------------------------------------------------
#include <xparameters.h>
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "HsModule.h"
#include "HsClockControl.h"
#include "Drivers/pios_interface.h"
#include "Drivers/HsTimer.h"
#include "Drivers/Si5347.h"

//--Helpers---------------------------------------------------------------------------------------------------------------------------------
bool ClockConfig5338Modified( bool firstInit, double ClkIn, unsigned int RefClkInSelect, double ClkOut0, double ClkOut1, double ClkOut2, double ClkOut3, int clkMode1, int clkMode2);
bool ClockSet5338( unsigned int MasterControlBase, double ClkIn, unsigned int RefClkInSelect, double ClkOut0, double ClkOut1, double ClkOut2, double ClkOut3, bool isModule5338);

#define CLOCK_CONFIG_LUT_NUM_ENTRIES (sizeof(ClockConfigLut) / sizeof(struct ClockConfigLutEntry))

#define CLOCK_CACHE_LEN			2

static struct ClockCacheStruct ClockCacheLut[CLOCK_CACHE_LEN] = {
			{ 240000000000LL, 1, 250000, {{ 0x0B24, 0xC0 },{ 0x0B25, 0x00 },{ 0x0B4E, 0x1A },{ 0x0006, 0x00 },{ 0x0007, 0x00 },{ 0x0008, 0x00 },{ 0x000B, 0x6C },{ 0x0016, 0x0F },{ 0x0017, 0xDC },{ 0x0018, 0xFF },{ 0x0019, 0xFF },{ 0x001A, 0xFF },{ 0x0020, 0x01 },{ 0x002B, 0x02 },{ 0x002C, 0x00 },{ 0x002D, 0x00 },{ 0x002E, 0x00 },{ 0x002F, 0x00 },{ 0x0030, 0x00 },{ 0x0031, 0x00 },{ 0x0032, 0x00 },{ 0x0033, 0x00 },{ 0x0034, 0x00 },{ 0x0035, 0x00 },{ 0x0036, 0x00 },{ 0x0037, 0x00 },{ 0x0038, 0x00 },{ 0x0039, 0x00 },{ 0x003A, 0x00 },{ 0x003B, 0x00 },{ 0x003C, 0x00 },{ 0x003D, 0x00 },{ 0x003F, 0x00 },{ 0x0040, 0x04 },{ 0x0041, 0x00 },{ 0x0042, 0x00 },{ 0x0043, 0x00 },{ 0x0044, 0x00 },{ 0x0045, 0x0C },{ 0x0046, 0x00 },{ 0x0047, 0x00 },{ 0x0048, 0x00 },{ 0x0049, 0x00 },{ 0x004A, 0x00 },{ 0x004B, 0x00 },{ 0x004C, 0x00 },{ 0x004D, 0x00 },{ 0x004E, 0x00 },{ 0x004F, 0x00 },{ 0x0050, 0x0F },{ 0x0051, 0x00 },{ 0x0052, 0x00 },{ 0x0053, 0x00 },{ 0x0054, 0x00 },{ 0x0055, 0x00 },{ 0x0056, 0x00 },{ 0x0057, 0x00 },{ 0x0058, 0x00 },{ 0x0059, 0x00 },{ 0x005A, 0x00 },{ 0x005B, 0x00 },{ 0x005C, 0x00 },{ 0x005D, 0x00 },{ 0x005E, 0x00 },{ 0x005F, 0x00 },{ 0x0060, 0x00 },{ 0x0061, 0x00 },{ 0x0062, 0x00 },{ 0x0063, 0x00 },{ 0x0064, 0x00 },{ 0x0065, 0x00 },{ 0x0066, 0x00 },{ 0x0067, 0x00 },{ 0x0068, 0x00 },{ 0x0069, 0x00 },{ 0x0092, 0x00 },{ 0x0093, 0x00 },{ 0x0094, 0x00 },{ 0x0095, 0x00 },{ 0x0096, 0x00 },{ 0x0097, 0x00 },{ 0x0098, 0x00 },{ 0x0099, 0x00 },{ 0x009A, 0x00 },{ 0x009B, 0x00 },{ 0x009C, 0x00 },{ 0x009D, 0x00 },{ 0x009E, 0x00 },{ 0x009F, 0x00 },{ 0x00A0, 0x00 },{ 0x00A1, 0x00 },{ 0x00A2, 0x00 },{ 0x00A4, 0x00 },{ 0x00A5, 0x00 },{ 0x00A6, 0x00 },{ 0x00A7, 0x00 },{ 0x00A9, 0x00 },{ 0x00AA, 0x00 },{ 0x00AB, 0x00 },{ 0x00AC, 0x00 },{ 0x00AE, 0x00 },{ 0x00AF, 0x00 },{ 0x00B0, 0x00 },{ 0x00B1, 0x00 },{ 0x00B3, 0x00 },{ 0x00B4, 0x00 },{ 0x00B5, 0x00 },{ 0x00B6, 0x00 },{ 0x00E5, 0x01 },{ 0x00E6, 0x00 },{ 0x00E7, 0x00 },{ 0x00E8, 0x00 },{ 0x00E9, 0x00 },{ 0x00EA, 0x00 },{ 0x00EB, 0x00 },{ 0x00EC, 0x00 },{ 0x00ED, 0x00 },{ 0x00EE, 0x00 },{ 0x00EF, 0x00 },{ 0x00F0, 0x00 },{ 0x00F1, 0x00 },{ 0x00F2, 0x00 },{ 0x00F3, 0x00 },{ 0x00F4, 0x00 },{ 0x00F5, 0x00 },{ 0x0102, 0x01 },{ 0x0108, 0x06 },{ 0x0109, 0x09 },{ 0x010A, 0x3E },{ 0x010B, 0x18 },{ 0x010C, 0x01 },{ 0x0112, 0x06 },{ 0x0113, 0x09 },{ 0x0114, 0x3B },{ 0x0115, 0x28 },{ 0x0116, 0x01 },{ 0x0117, 0x06 },{ 0x0118, 0x09 },{ 0x0119, 0x3B },{ 0x011A, 0x28 },{ 0x011B, 0x01 },{ 0x011C, 0x06 },{ 0x011D, 0x09 },{ 0x011E, 0x3B },{ 0x011F, 0x28 },{ 0x0120, 0x01 },{ 0x0126, 0x01 },{ 0x0127, 0x09 },{ 0x0128, 0x3B },{ 0x0129, 0x28 },{ 0x012A, 0x00 },{ 0x012B, 0x01 },{ 0x012C, 0x09 },{ 0x012D, 0x3B },{ 0x012E, 0x28 },{ 0x012F, 0x00 },{ 0x0130, 0x06 },{ 0x0131, 0x09 },{ 0x0132, 0x3B },{ 0x0133, 0x2A },{ 0x0134, 0x03 },{ 0x013A, 0x01 },{ 0x013B, 0x09 },{ 0x013C, 0x3D },{ 0x013D, 0x18 },{ 0x013E, 0x00 },{ 0x013F, 0x00 },{ 0x0140, 0x00 },{ 0x0141, 0x40 },{ 0x0142, 0xFF },{ 0x0206, 0x00 },{ 0x0208, 0x00 },{ 0x0209, 0x00 },{ 0x020A, 0x00 },{ 0x020B, 0x00 },{ 0x020C, 0x00 },{ 0x020D, 0x00 },{ 0x020E, 0x00 },{ 0x020F, 0x00 },{ 0x0210, 0x00 },{ 0x0211, 0x00 },{ 0x0212, 0x00 },{ 0x0213, 0x00 },{ 0x0214, 0x00 },{ 0x0215, 0x00 },{ 0x0216, 0x00 },{ 0x0217, 0x00 },{ 0x0218, 0x00 },{ 0x0219, 0x00 },{ 0x021A, 0x00 },{ 0x021B, 0x00 },{ 0x021C, 0x00 },{ 0x021D, 0x00 },{ 0x021E, 0x00 },{ 0x021F, 0x00 },{ 0x0220, 0x00 },{ 0x0221, 0x00 },{ 0x0222, 0x00 },{ 0x0223, 0x00 },{ 0x0224, 0x00 },{ 0x0225, 0x00 },{ 0x0226, 0x00 },{ 0x0227, 0x00 },{ 0x0228, 0x00 },{ 0x0229, 0x00 },{ 0x022A, 0x00 },{ 0x022B, 0x00 },{ 0x022C, 0x00 },{ 0x022D, 0x00 },{ 0x022E, 0x00 },{ 0x022F, 0x00 },{ 0x0231, 0x0B },{ 0x0232, 0x0B },{ 0x0233, 0x0B },{ 0x0234, 0x0B },{ 0x0235, 0x00 },{ 0x0236, 0x00 },{ 0x0237, 0x00 },{ 0x0238, 0x00 },{ 0x0239, 0x94 },{ 0x023A, 0x00 },{ 0x023B, 0x00 },{ 0x023C, 0x00 },{ 0x023D, 0x00 },{ 0x023E, 0x80 },{ 0x024A, 0x00 },{ 0x024B, 0x00 },{ 0x024C, 0x00 },{ 0x0250, 0x00 },{ 0x0251, 0x00 },{ 0x0252, 0x00 },{ 0x0253, 0x00 },{ 0x0254, 0x00 },{ 0x0255, 0x00 },{ 0x0256, 0x00 },{ 0x0257, 0x00 },{ 0x0258, 0x00 },{ 0x025C, 0x00 },{ 0x025D, 0x00 },{ 0x025E, 0x00 },{ 0x025F, 0x00 },{ 0x0260, 0x00 },{ 0x0261, 0x00 },{ 0x0262, 0x00 },{ 0x0263, 0x00 },{ 0x0264, 0x00 },{ 0x0268, 0x00 },{ 0x0269, 0x00 },{ 0x026A, 0x00 },{ 0x026B, 0x35 },{ 0x026C, 0x33 },{ 0x026D, 0x34 },{ 0x026E, 0x37 },{ 0x026F, 0x45 },{ 0x0270, 0x56 },{ 0x0271, 0x42 },{ 0x0272, 0x32 },{ 0x028A, 0x00 },{ 0x028B, 0x00 },{ 0x028C, 0x00 },{ 0x028D, 0x00 },{ 0x028E, 0x00 },{ 0x028F, 0x00 },{ 0x0290, 0x00 },{ 0x0291, 0x00 },{ 0x0294, 0x88 },{ 0x0295, 0x88 },{ 0x0296, 0x00 },{ 0x0297, 0x00 },{ 0x0299, 0x00 },{ 0x029A, 0x00 },{ 0x029B, 0x00 },{ 0x029C, 0x00 },{ 0x029D, 0x00 },{ 0x029E, 0x00 },{ 0x029F, 0x00 },{ 0x02A0, 0x00 },{ 0x02A1, 0x00 },{ 0x02A2, 0x00 },{ 0x02A3, 0x00 },{ 0x02A4, 0x00 },{ 0x02A5, 0x00 },{ 0x02A6, 0x00 },{ 0x02A7, 0x00 },{ 0x02A8, 0x00 },{ 0x02A9, 0x00 },{ 0x02AA, 0x00 },{ 0x02AB, 0x00 },{ 0x02AC, 0x00 },{ 0x02AD, 0x00 },{ 0x02AE, 0x00 },{ 0x02AF, 0x00 },{ 0x02B0, 0x00 },{ 0x02B1, 0x00 },{ 0x02B7, 0xFF },{ 0x0302, 0x00 },{ 0x0303, 0x00 },{ 0x0304, 0x00 },{ 0x0305, 0x40 },{ 0x0306, 0x09 },{ 0x0307, 0x00 },{ 0x0308, 0x00 },{ 0x0309, 0x00 },{ 0x030A, 0x00 },{ 0x030B, 0xC8 },{ 0x030C, 0x00 },{ 0x030D, 0x00 },{ 0x030E, 0x00 },{ 0x030F, 0x00 },{ 0x0310, 0x00 },{ 0x0311, 0x00 },{ 0x0312, 0x00 },{ 0x0313, 0x00 },{ 0x0314, 0x00 },{ 0x0315, 0x00 },{ 0x0316, 0x00 },{ 0x0317, 0x00 },{ 0x0318, 0x00 },{ 0x0319, 0x00 },{ 0x031A, 0x00 },{ 0x031B, 0x40 },{ 0x031C, 0x09 },{ 0x031D, 0x00 },{ 0x031E, 0x00 },{ 0x031F, 0x00 },{ 0x0320, 0x00 },{ 0x0321, 0xC8 },{ 0x0322, 0x00 },{ 0x0323, 0x00 },{ 0x0324, 0x00 },{ 0x0325, 0x00 },{ 0x0326, 0x00 },{ 0x0327, 0x00 },{ 0x0328, 0x00 },{ 0x0329, 0x00 },{ 0x032A, 0x00 },{ 0x032B, 0x00 },{ 0x032C, 0x00 },{ 0x032D, 0x00 },{ 0x0338, 0x00 },{ 0x033B, 0x00 },{ 0x033C, 0x00 },{ 0x033D, 0x00 },{ 0x033E, 0x00 },{ 0x033F, 0x00 },{ 0x0340, 0x00 },{ 0x0341, 0x00 },{ 0x0342, 0x00 },{ 0x0343, 0x00 },{ 0x0344, 0x00 },{ 0x0345, 0x00 },{ 0x0346, 0x00 },{ 0x0347, 0x00 },{ 0x0348, 0x00 },{ 0x0349, 0x00 },{ 0x034A, 0x00 },{ 0x034B, 0x00 },{ 0x034C, 0x00 },{ 0x034D, 0x00 },{ 0x034E, 0x00 },{ 0x034F, 0x00 },{ 0x0350, 0x00 },{ 0x0351, 0x00 },{ 0x0352, 0x00 },{ 0x0408, 0x00 },{ 0x0409, 0x00 },{ 0x040A, 0x00 },{ 0x040B, 0x00 },{ 0x040C, 0x00 },{ 0x040D, 0x00 },{ 0x040E, 0x00 },{ 0x040F, 0x00 },{ 0x0410, 0x00 },{ 0x0411, 0x00 },{ 0x0412, 0x00 },{ 0x0413, 0x00 },{ 0x0415, 0x00 },{ 0x0416, 0x00 },{ 0x0417, 0x00 },{ 0x0418, 0x00 },{ 0x0419, 0x00 },{ 0x041A, 0x00 },{ 0x041B, 0x00 },{ 0x041C, 0x00 },{ 0x041D, 0x00 },{ 0x041E, 0x00 },{ 0x041F, 0x00 },{ 0x0421, 0x2B },{ 0x0422, 0x01 },{ 0x0423, 0x00 },{ 0x0424, 0x00 },{ 0x0425, 0x00 },{ 0x0426, 0x00 },{ 0x0427, 0x00 },{ 0x0428, 0x00 },{ 0x0429, 0x00 },{ 0x042A, 0x00 },{ 0x042B, 0x01 },{ 0x042C, 0x0F },{ 0x042D, 0x03 },{ 0x042E, 0x00 },{ 0x042F, 0x00 },{ 0x0431, 0x00 },{ 0x0432, 0x00 },{ 0x0433, 0x04 },{ 0x0434, 0x00 },{ 0x0435, 0x01 },{ 0x0436, 0x04 },{ 0x0437, 0x00 },{ 0x0438, 0x00 },{ 0x0439, 0x00 },{ 0x043D, 0x0A },{ 0x043E, 0x06 },{ 0x0442, 0x00 },{ 0x0443, 0x00 },{ 0x0444, 0x00 },{ 0x0445, 0x00 },{ 0x0489, 0x00 },{ 0x048A, 0x00 },{ 0x049B, 0x18 },{ 0x049D, 0x00 },{ 0x049E, 0x00 },{ 0x049F, 0x00 },{ 0x04A0, 0x00 },{ 0x04A1, 0x00 },{ 0x04A2, 0x00 },{ 0x04A6, 0x00 },{ 0x0508, 0x00 },{ 0x0509, 0x00 },{ 0x050A, 0x00 },{ 0x050B, 0x00 },{ 0x050C, 0x00 },{ 0x050D, 0x00 },{ 0x050E, 0x00 },{ 0x050F, 0x00 },{ 0x0510, 0x00 },{ 0x0511, 0x00 },{ 0x0512, 0x00 },{ 0x0513, 0x00 },{ 0x0515, 0x00 },{ 0x0516, 0x00 },{ 0x0517, 0x00 },{ 0x0518, 0x00 },{ 0x0519, 0x00 },{ 0x051A, 0x00 },{ 0x051B, 0x00 },{ 0x051C, 0x00 },{ 0x051D, 0x00 },{ 0x051E, 0x00 },{ 0x051F, 0x00 },{ 0x0521, 0x2B },{ 0x0522, 0x01 },{ 0x0523, 0x00 },{ 0x0524, 0x00 },{ 0x0525, 0x00 },{ 0x0526, 0x00 },{ 0x0527, 0x00 },{ 0x0528, 0x00 },{ 0x0529, 0x00 },{ 0x052A, 0x01 },{ 0x052B, 0x01 },{ 0x052C, 0x0F },{ 0x052D, 0x03 },{ 0x052E, 0x00 },{ 0x052F, 0x00 },{ 0x0531, 0x00 },{ 0x0532, 0x00 },{ 0x0533, 0x04 },{ 0x0534, 0x00 },{ 0x0536, 0x06 },{ 0x0537, 0x00 },{ 0x0538, 0x00 },{ 0x0539, 0x00 },{ 0x053D, 0x0A },{ 0x053E, 0x06 },{ 0x0589, 0x0C },{ 0x058A, 0x00 },{ 0x059B, 0x18 },{ 0x059D, 0x00 },{ 0x059E, 0x00 },{ 0x059F, 0x00 },{ 0x05A0, 0x00 },{ 0x05A1, 0x00 },{ 0x05A2, 0x00 },{ 0x05A6, 0x00 },{ 0x0602, 0x01 },{ 0x0608, 0x00 },{ 0x0609, 0x00 },{ 0x060A, 0x00 },{ 0x060B, 0x00 },{ 0x060C, 0x00 },{ 0x060D, 0x00 },{ 0x060E, 0x00 },{ 0x060F, 0x00 },{ 0x0610, 0x00 },{ 0x0611, 0x00 },{ 0x0612, 0x00 },{ 0x0613, 0x00 },{ 0x0615, 0x00 },{ 0x0616, 0x00 },{ 0x0617, 0x00 },{ 0x0618, 0x00 },{ 0x0619, 0x00 },{ 0x061A, 0x00 },{ 0x061B, 0x00 },{ 0x061C, 0x00 },{ 0x061D, 0x00 },{ 0x061E, 0x00 },{ 0x061F, 0x00 },{ 0x0621, 0x2B },{ 0x0622, 0x01 },{ 0x0623, 0x00 },{ 0x0624, 0x00 },{ 0x0625, 0x00 },{ 0x0626, 0x00 },{ 0x0627, 0x00 },{ 0x0628, 0x00 },{ 0x0629, 0x00 },{ 0x062A, 0x00 },{ 0x062B, 0x01 },{ 0x062C, 0x0F },{ 0x062D, 0x03 },{ 0x062E, 0x00 },{ 0x062F, 0x00 },{ 0x0631, 0x00 },{ 0x0632, 0x00 },{ 0x0633, 0x04 },{ 0x0634, 0x00 },{ 0x0635, 0x01 },{ 0x0636, 0x04 },{ 0x0637, 0x00 },{ 0x0638, 0x00 },{ 0x0639, 0x00 },{ 0x063D, 0x0A },{ 0x063E, 0x06 },{ 0x0642, 0x00 },{ 0x0643, 0x00 },{ 0x0644, 0x00 },{ 0x0645, 0x00 },{ 0x0689, 0x0C },{ 0x068A, 0x00 },{ 0x069B, 0x18 },{ 0x069D, 0x00 },{ 0x069E, 0x00 },{ 0x069F, 0x00 },{ 0x06A0, 0x00 },{ 0x06A1, 0x00 },{ 0x06A2, 0x00 },{ 0x06A6, 0x00 },{ 0x0702, 0x01 },{ 0x0709, 0x00 },{ 0x070A, 0x00 },{ 0x070B, 0x00 },{ 0x070C, 0x00 },{ 0x070D, 0x00 },{ 0x070E, 0x00 },{ 0x070F, 0x00 },{ 0x0710, 0x00 },{ 0x0711, 0x00 },{ 0x0712, 0x00 },{ 0x0713, 0x00 },{ 0x0714, 0x00 },{ 0x0716, 0x00 },{ 0x0717, 0x00 },{ 0x0718, 0x00 },{ 0x0719, 0x00 },{ 0x071A, 0x00 },{ 0x071B, 0x00 },{ 0x071C, 0x00 },{ 0x071D, 0x00 },{ 0x071E, 0x00 },{ 0x071F, 0x00 },{ 0x0720, 0x00 },{ 0x0722, 0x2B },{ 0x0723, 0x01 },{ 0x0724, 0x00 },{ 0x0725, 0x00 },{ 0x0726, 0x00 },{ 0x0727, 0x00 },{ 0x0728, 0x00 },{ 0x0729, 0x00 },{ 0x072A, 0x00 },{ 0x072B, 0x00 },{ 0x072C, 0x01 },{ 0x072D, 0x0F },{ 0x072E, 0x03 },{ 0x072F, 0x00 },{ 0x0730, 0x00 },{ 0x0732, 0x00 },{ 0x0733, 0x00 },{ 0x0734, 0x04 },{ 0x0735, 0x00 },{ 0x0737, 0x06 },{ 0x0738, 0x00 },{ 0x0739, 0x00 },{ 0x073A, 0x00 },{ 0x073E, 0x0A },{ 0x073F, 0x06 },{ 0x0789, 0x0C },{ 0x078A, 0x00 },{ 0x079B, 0x18 },{ 0x079D, 0x00 },{ 0x079E, 0x00 },{ 0x079F, 0x00 },{ 0x07A0, 0x00 },{ 0x07A1, 0x00 },{ 0x07A2, 0x00 },{ 0x07A6, 0x00 },{ 0x0802, 0x35 },{ 0x0803, 0x04 },{ 0x0804, 0x01 },{ 0x0805, 0x45 },{ 0x0806, 0x04 },{ 0x0807, 0x00 },{ 0x0808, 0x42 },{ 0x0809, 0x04 },{ 0x080A, 0x00 },{ 0x080B, 0x43 },{ 0x080C, 0x04 },{ 0x080D, 0x00 },{ 0x080E, 0x44 },{ 0x080F, 0x04 },{ 0x0810, 0x00 },{ 0x0811, 0x35 },{ 0x0812, 0x06 },{ 0x0813, 0x01 },{ 0x0814, 0x45 },{ 0x0815, 0x06 },{ 0x0816, 0x00 },{ 0x0817, 0x42 },{ 0x0818, 0x06 },{ 0x0819, 0x00 },{ 0x081A, 0x43 },{ 0x081B, 0x06 },{ 0x081C, 0x00 },{ 0x081D, 0x44 },{ 0x081E, 0x06 },{ 0x081F, 0x00 },{ 0x0820, 0x00 },{ 0x0821, 0x00 },{ 0x0822, 0x00 },{ 0x0823, 0x00 },{ 0x0824, 0x00 },{ 0x0825, 0x00 },{ 0x0826, 0x00 },{ 0x0827, 0x00 },{ 0x0828, 0x00 },{ 0x0829, 0x00 },{ 0x082A, 0x00 },{ 0x082B, 0x00 },{ 0x082C, 0x00 },{ 0x082D, 0x00 },{ 0x082E, 0x00 },{ 0x082F, 0x00 },{ 0x0830, 0x00 },{ 0x0831, 0x00 },{ 0x0832, 0x00 },{ 0x0833, 0x00 },{ 0x0834, 0x00 },{ 0x0835, 0x00 },{ 0x0836, 0x00 },{ 0x0837, 0x00 },{ 0x0838, 0x00 },{ 0x0839, 0x00 },{ 0x083A, 0x00 },{ 0x083B, 0x00 },{ 0x083C, 0x00 },{ 0x083D, 0x00 },{ 0x083E, 0x00 },{ 0x083F, 0x00 },{ 0x0840, 0x00 },{ 0x0841, 0x00 },{ 0x0842, 0x00 },{ 0x0843, 0x00 },{ 0x0844, 0x00 },{ 0x0845, 0x00 },{ 0x0846, 0x00 },{ 0x0847, 0x00 },{ 0x0848, 0x00 },{ 0x0849, 0x00 },{ 0x084A, 0x00 },{ 0x084B, 0x00 },{ 0x084C, 0x00 },{ 0x084D, 0x00 },{ 0x084E, 0x00 },{ 0x084F, 0x00 },{ 0x0850, 0x00 },{ 0x0851, 0x00 },{ 0x0852, 0x00 },{ 0x0853, 0x00 },{ 0x0854, 0x00 },{ 0x0855, 0x00 },{ 0x0856, 0x00 },{ 0x0857, 0x00 },{ 0x0858, 0x00 },{ 0x0859, 0x00 },{ 0x085A, 0x00 },{ 0x085B, 0x00 },{ 0x085C, 0x00 },{ 0x085D, 0x00 },{ 0x085E, 0x00 },{ 0x085F, 0x00 },{ 0x0860, 0x00 },{ 0x0861, 0x00 },{ 0x090E, 0x02 },{ 0x0943, 0x00 },{ 0x0949, 0x00 },{ 0x094A, 0x00 },{ 0x094E, 0x49 },{ 0x094F, 0x02 },{ 0x095E, 0x00 },{ 0x0A03, 0x05 },{ 0x0A04, 0x00 },{ 0x0A05, 0x05 },{ 0x0B44, 0x0F },{ 0x0B45, 0x0A },{ 0x0B46, 0x00 },{ 0x0B47, 0x0F },{ 0x0B48, 0x0F },{ 0x0B4A, 0x0A },{ 0x0B57, 0x0E },{ 0x0B58, 0x01 },{ 0x001C, 0x01 },{ 0x0B24, 0xC3 },{ 0x0B25, 0x02 }}},
			{ 200000000000LL, 1, 250000, {{ 0x0B24, 0xC0 },{ 0x0B25, 0x00 },{ 0x0B4E, 0x1A },{ 0x0006, 0x00 },{ 0x0007, 0x00 },{ 0x0008, 0x00 },{ 0x000B, 0x6C },{ 0x0016, 0x0F },{ 0x0017, 0xDC },{ 0x0018, 0xFF },{ 0x0019, 0xFF },{ 0x001A, 0xFF },{ 0x0020, 0x01 },{ 0x002B, 0x02 },{ 0x002C, 0x00 },{ 0x002D, 0x00 },{ 0x002E, 0x00 },{ 0x002F, 0x00 },{ 0x0030, 0x00 },{ 0x0031, 0x00 },{ 0x0032, 0x00 },{ 0x0033, 0x00 },{ 0x0034, 0x00 },{ 0x0035, 0x00 },{ 0x0036, 0x00 },{ 0x0037, 0x00 },{ 0x0038, 0x00 },{ 0x0039, 0x00 },{ 0x003A, 0x00 },{ 0x003B, 0x00 },{ 0x003C, 0x00 },{ 0x003D, 0x00 },{ 0x003F, 0x00 },{ 0x0040, 0x04 },{ 0x0041, 0x00 },{ 0x0042, 0x00 },{ 0x0043, 0x00 },{ 0x0044, 0x00 },{ 0x0045, 0x0C },{ 0x0046, 0x00 },{ 0x0047, 0x00 },{ 0x0048, 0x00 },{ 0x0049, 0x00 },{ 0x004A, 0x00 },{ 0x004B, 0x00 },{ 0x004C, 0x00 },{ 0x004D, 0x00 },{ 0x004E, 0x00 },{ 0x004F, 0x00 },{ 0x0050, 0x0F },{ 0x0051, 0x00 },{ 0x0052, 0x00 },{ 0x0053, 0x00 },{ 0x0054, 0x00 },{ 0x0055, 0x00 },{ 0x0056, 0x00 },{ 0x0057, 0x00 },{ 0x0058, 0x00 },{ 0x0059, 0x00 },{ 0x005A, 0x00 },{ 0x005B, 0x00 },{ 0x005C, 0x00 },{ 0x005D, 0x00 },{ 0x005E, 0x00 },{ 0x005F, 0x00 },{ 0x0060, 0x00 },{ 0x0061, 0x00 },{ 0x0062, 0x00 },{ 0x0063, 0x00 },{ 0x0064, 0x00 },{ 0x0065, 0x00 },{ 0x0066, 0x00 },{ 0x0067, 0x00 },{ 0x0068, 0x00 },{ 0x0069, 0x00 },{ 0x0092, 0x00 },{ 0x0093, 0x00 },{ 0x0094, 0x00 },{ 0x0095, 0x00 },{ 0x0096, 0x00 },{ 0x0097, 0x00 },{ 0x0098, 0x00 },{ 0x0099, 0x00 },{ 0x009A, 0x00 },{ 0x009B, 0x00 },{ 0x009C, 0x00 },{ 0x009D, 0x00 },{ 0x009E, 0x00 },{ 0x009F, 0x00 },{ 0x00A0, 0x00 },{ 0x00A1, 0x00 },{ 0x00A2, 0x00 },{ 0x00A4, 0x00 },{ 0x00A5, 0x00 },{ 0x00A6, 0x00 },{ 0x00A7, 0x00 },{ 0x00A9, 0x00 },{ 0x00AA, 0x00 },{ 0x00AB, 0x00 },{ 0x00AC, 0x00 },{ 0x00AE, 0x00 },{ 0x00AF, 0x00 },{ 0x00B0, 0x00 },{ 0x00B1, 0x00 },{ 0x00B3, 0x00 },{ 0x00B4, 0x00 },{ 0x00B5, 0x00 },{ 0x00B6, 0x00 },{ 0x00E5, 0x01 },{ 0x00E6, 0x00 },{ 0x00E7, 0x00 },{ 0x00E8, 0x00 },{ 0x00E9, 0x00 },{ 0x00EA, 0x00 },{ 0x00EB, 0x00 },{ 0x00EC, 0x00 },{ 0x00ED, 0x00 },{ 0x00EE, 0x00 },{ 0x00EF, 0x00 },{ 0x00F0, 0x00 },{ 0x00F1, 0x00 },{ 0x00F2, 0x00 },{ 0x00F3, 0x00 },{ 0x00F4, 0x00 },{ 0x00F5, 0x00 },{ 0x0102, 0x01 },{ 0x0108, 0x06 },{ 0x0109, 0x09 },{ 0x010A, 0x3B },{ 0x010B, 0x28 },{ 0x010C, 0x01 },{ 0x0112, 0x06 },{ 0x0113, 0x09 },{ 0x0114, 0x3B },{ 0x0115, 0x28 },{ 0x0116, 0x01 },{ 0x0117, 0x06 },{ 0x0118, 0x09 },{ 0x0119, 0x3B },{ 0x011A, 0x28 },{ 0x011B, 0x01 },{ 0x011C, 0x06 },{ 0x011D, 0x09 },{ 0x011E, 0x3B },{ 0x011F, 0x28 },{ 0x0120, 0x01 },{ 0x0126, 0x01 },{ 0x0127, 0x09 },{ 0x0128, 0x3B },{ 0x0129, 0x28 },{ 0x012A, 0x00 },{ 0x012B, 0x01 },{ 0x012C, 0x09 },{ 0x012D, 0x3B },{ 0x012E, 0x28 },{ 0x012F, 0x00 },{ 0x0130, 0x06 },{ 0x0131, 0x09 },{ 0x0132, 0x3B },{ 0x0133, 0x2A },{ 0x0134, 0x03 },{ 0x013A, 0x01 },{ 0x013B, 0x09 },{ 0x013C, 0x3D },{ 0x013D, 0x18 },{ 0x013E, 0x00 },{ 0x013F, 0x00 },{ 0x0140, 0x00 },{ 0x0141, 0x40 },{ 0x0142, 0xFF },{ 0x0206, 0x00 },{ 0x0208, 0x00 },{ 0x0209, 0x00 },{ 0x020A, 0x00 },{ 0x020B, 0x00 },{ 0x020C, 0x00 },{ 0x020D, 0x00 },{ 0x020E, 0x00 },{ 0x020F, 0x00 },{ 0x0210, 0x00 },{ 0x0211, 0x00 },{ 0x0212, 0x00 },{ 0x0213, 0x00 },{ 0x0214, 0x00 },{ 0x0215, 0x00 },{ 0x0216, 0x00 },{ 0x0217, 0x00 },{ 0x0218, 0x00 },{ 0x0219, 0x00 },{ 0x021A, 0x00 },{ 0x021B, 0x00 },{ 0x021C, 0x00 },{ 0x021D, 0x00 },{ 0x021E, 0x00 },{ 0x021F, 0x00 },{ 0x0220, 0x00 },{ 0x0221, 0x00 },{ 0x0222, 0x00 },{ 0x0223, 0x00 },{ 0x0224, 0x00 },{ 0x0225, 0x00 },{ 0x0226, 0x00 },{ 0x0227, 0x00 },{ 0x0228, 0x00 },{ 0x0229, 0x00 },{ 0x022A, 0x00 },{ 0x022B, 0x00 },{ 0x022C, 0x00 },{ 0x022D, 0x00 },{ 0x022E, 0x00 },{ 0x022F, 0x00 },{ 0x0231, 0x0B },{ 0x0232, 0x0B },{ 0x0233, 0x0B },{ 0x0234, 0x0B },{ 0x0235, 0x00 },{ 0x0236, 0x00 },{ 0x0237, 0x00 },{ 0x0238, 0x80 },{ 0x0239, 0x93 },{ 0x023A, 0x00 },{ 0x023B, 0x00 },{ 0x023C, 0x00 },{ 0x023D, 0x00 },{ 0x023E, 0x80 },{ 0x024A, 0x00 },{ 0x024B, 0x00 },{ 0x024C, 0x00 },{ 0x0250, 0x00 },{ 0x0251, 0x00 },{ 0x0252, 0x00 },{ 0x0253, 0x00 },{ 0x0254, 0x00 },{ 0x0255, 0x00 },{ 0x0256, 0x00 },{ 0x0257, 0x00 },{ 0x0258, 0x00 },{ 0x025C, 0x00 },{ 0x025D, 0x00 },{ 0x025E, 0x00 },{ 0x025F, 0x00 },{ 0x0260, 0x00 },{ 0x0261, 0x00 },{ 0x0262, 0x00 },{ 0x0263, 0x00 },{ 0x0264, 0x00 },{ 0x0268, 0x00 },{ 0x0269, 0x00 },{ 0x026A, 0x00 },{ 0x026B, 0x35 },{ 0x026C, 0x33 },{ 0x026D, 0x34 },{ 0x026E, 0x37 },{ 0x026F, 0x45 },{ 0x0270, 0x56 },{ 0x0271, 0x42 },{ 0x0272, 0x32 },{ 0x028A, 0x00 },{ 0x028B, 0x00 },{ 0x028C, 0x00 },{ 0x028D, 0x00 },{ 0x028E, 0x00 },{ 0x028F, 0x00 },{ 0x0290, 0x00 },{ 0x0291, 0x00 },{ 0x0294, 0x88 },{ 0x0295, 0x88 },{ 0x0296, 0x00 },{ 0x0297, 0x00 },{ 0x0299, 0x00 },{ 0x029A, 0x00 },{ 0x029B, 0x00 },{ 0x029C, 0x00 },{ 0x029D, 0x00 },{ 0x029E, 0x00 },{ 0x029F, 0x00 },{ 0x02A0, 0x00 },{ 0x02A1, 0x00 },{ 0x02A2, 0x00 },{ 0x02A3, 0x00 },{ 0x02A4, 0x00 },{ 0x02A5, 0x00 },{ 0x02A6, 0x00 },{ 0x02A7, 0x00 },{ 0x02A8, 0x00 },{ 0x02A9, 0x00 },{ 0x02AA, 0x00 },{ 0x02AB, 0x00 },{ 0x02AC, 0x00 },{ 0x02AD, 0x00 },{ 0x02AE, 0x00 },{ 0x02AF, 0x00 },{ 0x02B0, 0x00 },{ 0x02B1, 0x00 },{ 0x02B7, 0xFF },{ 0x0302, 0x00 },{ 0x0303, 0x00 },{ 0x0304, 0x00 },{ 0x0305, 0x10 },{ 0x0306, 0x0B },{ 0x0307, 0x00 },{ 0x0308, 0x00 },{ 0x0309, 0x00 },{ 0x030A, 0x00 },{ 0x030B, 0xC8 },{ 0x030C, 0x00 },{ 0x030D, 0x00 },{ 0x030E, 0x00 },{ 0x030F, 0x00 },{ 0x0310, 0x00 },{ 0x0311, 0x00 },{ 0x0312, 0x00 },{ 0x0313, 0x00 },{ 0x0314, 0x00 },{ 0x0315, 0x00 },{ 0x0316, 0x00 },{ 0x0317, 0x00 },{ 0x0318, 0x00 },{ 0x0319, 0x00 },{ 0x031A, 0x00 },{ 0x031B, 0x10 },{ 0x031C, 0x0B },{ 0x031D, 0x00 },{ 0x031E, 0x00 },{ 0x031F, 0x00 },{ 0x0320, 0x00 },{ 0x0321, 0xC8 },{ 0x0322, 0x00 },{ 0x0323, 0x00 },{ 0x0324, 0x00 },{ 0x0325, 0x00 },{ 0x0326, 0x00 },{ 0x0327, 0x00 },{ 0x0328, 0x00 },{ 0x0329, 0x00 },{ 0x032A, 0x00 },{ 0x032B, 0x00 },{ 0x032C, 0x00 },{ 0x032D, 0x00 },{ 0x0338, 0x00 },{ 0x033B, 0x00 },{ 0x033C, 0x00 },{ 0x033D, 0x00 },{ 0x033E, 0x00 },{ 0x033F, 0x00 },{ 0x0340, 0x00 },{ 0x0341, 0x00 },{ 0x0342, 0x00 },{ 0x0343, 0x00 },{ 0x0344, 0x00 },{ 0x0345, 0x00 },{ 0x0346, 0x00 },{ 0x0347, 0x00 },{ 0x0348, 0x00 },{ 0x0349, 0x00 },{ 0x034A, 0x00 },{ 0x034B, 0x00 },{ 0x034C, 0x00 },{ 0x034D, 0x00 },{ 0x034E, 0x00 },{ 0x034F, 0x00 },{ 0x0350, 0x00 },{ 0x0351, 0x00 },{ 0x0352, 0x00 },{ 0x0408, 0x00 },{ 0x0409, 0x00 },{ 0x040A, 0x00 },{ 0x040B, 0x00 },{ 0x040C, 0x00 },{ 0x040D, 0x00 },{ 0x040E, 0x00 },{ 0x040F, 0x00 },{ 0x0410, 0x00 },{ 0x0411, 0x00 },{ 0x0412, 0x00 },{ 0x0413, 0x00 },{ 0x0415, 0x00 },{ 0x0416, 0x00 },{ 0x0417, 0x00 },{ 0x0418, 0x00 },{ 0x0419, 0x00 },{ 0x041A, 0x00 },{ 0x041B, 0x00 },{ 0x041C, 0x00 },{ 0x041D, 0x00 },{ 0x041E, 0x00 },{ 0x041F, 0x00 },{ 0x0421, 0x2B },{ 0x0422, 0x01 },{ 0x0423, 0x00 },{ 0x0424, 0x00 },{ 0x0425, 0x00 },{ 0x0426, 0x00 },{ 0x0427, 0x00 },{ 0x0428, 0x00 },{ 0x0429, 0x00 },{ 0x042A, 0x00 },{ 0x042B, 0x01 },{ 0x042C, 0x0F },{ 0x042D, 0x03 },{ 0x042E, 0x00 },{ 0x042F, 0x00 },{ 0x0431, 0x00 },{ 0x0432, 0x00 },{ 0x0433, 0x04 },{ 0x0434, 0x00 },{ 0x0435, 0x01 },{ 0x0436, 0x04 },{ 0x0437, 0x00 },{ 0x0438, 0x00 },{ 0x0439, 0x00 },{ 0x043D, 0x0A },{ 0x043E, 0x06 },{ 0x0442, 0x00 },{ 0x0443, 0x00 },{ 0x0444, 0x00 },{ 0x0445, 0x00 },{ 0x0489, 0x00 },{ 0x048A, 0x00 },{ 0x049B, 0x18 },{ 0x049D, 0x00 },{ 0x049E, 0x00 },{ 0x049F, 0x00 },{ 0x04A0, 0x00 },{ 0x04A1, 0x00 },{ 0x04A2, 0x00 },{ 0x04A6, 0x00 },{ 0x0508, 0x00 },{ 0x0509, 0x00 },{ 0x050A, 0x00 },{ 0x050B, 0x00 },{ 0x050C, 0x00 },{ 0x050D, 0x00 },{ 0x050E, 0x00 },{ 0x050F, 0x00 },{ 0x0510, 0x00 },{ 0x0511, 0x00 },{ 0x0512, 0x00 },{ 0x0513, 0x00 },{ 0x0515, 0x00 },{ 0x0516, 0x00 },{ 0x0517, 0x00 },{ 0x0518, 0x00 },{ 0x0519, 0x00 },{ 0x051A, 0x00 },{ 0x051B, 0x00 },{ 0x051C, 0x00 },{ 0x051D, 0x00 },{ 0x051E, 0x00 },{ 0x051F, 0x00 },{ 0x0521, 0x2B },{ 0x0522, 0x01 },{ 0x0523, 0x00 },{ 0x0524, 0x00 },{ 0x0525, 0x00 },{ 0x0526, 0x00 },{ 0x0527, 0x00 },{ 0x0528, 0x00 },{ 0x0529, 0x00 },{ 0x052A, 0x01 },{ 0x052B, 0x01 },{ 0x052C, 0x0F },{ 0x052D, 0x03 },{ 0x052E, 0x00 },{ 0x052F, 0x00 },{ 0x0531, 0x00 },{ 0x0532, 0x00 },{ 0x0533, 0x04 },{ 0x0534, 0x00 },{ 0x0536, 0x06 },{ 0x0537, 0x00 },{ 0x0538, 0x00 },{ 0x0539, 0x00 },{ 0x053D, 0x0A },{ 0x053E, 0x06 },{ 0x0589, 0x0C },{ 0x058A, 0x00 },{ 0x059B, 0x18 },{ 0x059D, 0x00 },{ 0x059E, 0x00 },{ 0x059F, 0x00 },{ 0x05A0, 0x00 },{ 0x05A1, 0x00 },{ 0x05A2, 0x00 },{ 0x05A6, 0x00 },{ 0x0602, 0x01 },{ 0x0608, 0x00 },{ 0x0609, 0x00 },{ 0x060A, 0x00 },{ 0x060B, 0x00 },{ 0x060C, 0x00 },{ 0x060D, 0x00 },{ 0x060E, 0x00 },{ 0x060F, 0x00 },{ 0x0610, 0x00 },{ 0x0611, 0x00 },{ 0x0612, 0x00 },{ 0x0613, 0x00 },{ 0x0615, 0x00 },{ 0x0616, 0x00 },{ 0x0617, 0x00 },{ 0x0618, 0x00 },{ 0x0619, 0x00 },{ 0x061A, 0x00 },{ 0x061B, 0x00 },{ 0x061C, 0x00 },{ 0x061D, 0x00 },{ 0x061E, 0x00 },{ 0x061F, 0x00 },{ 0x0621, 0x2B },{ 0x0622, 0x01 },{ 0x0623, 0x00 },{ 0x0624, 0x00 },{ 0x0625, 0x00 },{ 0x0626, 0x00 },{ 0x0627, 0x00 },{ 0x0628, 0x00 },{ 0x0629, 0x00 },{ 0x062A, 0x00 },{ 0x062B, 0x01 },{ 0x062C, 0x0F },{ 0x062D, 0x03 },{ 0x062E, 0x00 },{ 0x062F, 0x00 },{ 0x0631, 0x00 },{ 0x0632, 0x00 },{ 0x0633, 0x04 },{ 0x0634, 0x00 },{ 0x0635, 0x01 },{ 0x0636, 0x04 },{ 0x0637, 0x00 },{ 0x0638, 0x00 },{ 0x0639, 0x00 },{ 0x063D, 0x0A },{ 0x063E, 0x06 },{ 0x0642, 0x00 },{ 0x0643, 0x00 },{ 0x0644, 0x00 },{ 0x0645, 0x00 },{ 0x0689, 0x0C },{ 0x068A, 0x00 },{ 0x069B, 0x18 },{ 0x069D, 0x00 },{ 0x069E, 0x00 },{ 0x069F, 0x00 },{ 0x06A0, 0x00 },{ 0x06A1, 0x00 },{ 0x06A2, 0x00 },{ 0x06A6, 0x00 },{ 0x0702, 0x01 },{ 0x0709, 0x00 },{ 0x070A, 0x00 },{ 0x070B, 0x00 },{ 0x070C, 0x00 },{ 0x070D, 0x00 },{ 0x070E, 0x00 },{ 0x070F, 0x00 },{ 0x0710, 0x00 },{ 0x0711, 0x00 },{ 0x0712, 0x00 },{ 0x0713, 0x00 },{ 0x0714, 0x00 },{ 0x0716, 0x00 },{ 0x0717, 0x00 },{ 0x0718, 0x00 },{ 0x0719, 0x00 },{ 0x071A, 0x00 },{ 0x071B, 0x00 },{ 0x071C, 0x00 },{ 0x071D, 0x00 },{ 0x071E, 0x00 },{ 0x071F, 0x00 },{ 0x0720, 0x00 },{ 0x0722, 0x2B },{ 0x0723, 0x01 },{ 0x0724, 0x00 },{ 0x0725, 0x00 },{ 0x0726, 0x00 },{ 0x0727, 0x00 },{ 0x0728, 0x00 },{ 0x0729, 0x00 },{ 0x072A, 0x00 },{ 0x072B, 0x00 },{ 0x072C, 0x01 },{ 0x072D, 0x0F },{ 0x072E, 0x03 },{ 0x072F, 0x00 },{ 0x0730, 0x00 },{ 0x0732, 0x00 },{ 0x0733, 0x00 },{ 0x0734, 0x04 },{ 0x0735, 0x00 },{ 0x0737, 0x06 },{ 0x0738, 0x00 },{ 0x0739, 0x00 },{ 0x073A, 0x00 },{ 0x073E, 0x0A },{ 0x073F, 0x06 },{ 0x0789, 0x0C },{ 0x078A, 0x00 },{ 0x079B, 0x18 },{ 0x079D, 0x00 },{ 0x079E, 0x00 },{ 0x079F, 0x00 },{ 0x07A0, 0x00 },{ 0x07A1, 0x00 },{ 0x07A2, 0x00 },{ 0x07A6, 0x00 },{ 0x0802, 0x35 },{ 0x0803, 0x04 },{ 0x0804, 0x01 },{ 0x0805, 0x45 },{ 0x0806, 0x04 },{ 0x0807, 0x00 },{ 0x0808, 0x42 },{ 0x0809, 0x04 },{ 0x080A, 0x00 },{ 0x080B, 0x43 },{ 0x080C, 0x04 },{ 0x080D, 0x00 },{ 0x080E, 0x44 },{ 0x080F, 0x04 },{ 0x0810, 0x00 },{ 0x0811, 0x35 },{ 0x0812, 0x06 },{ 0x0813, 0x01 },{ 0x0814, 0x45 },{ 0x0815, 0x06 },{ 0x0816, 0x00 },{ 0x0817, 0x42 },{ 0x0818, 0x06 },{ 0x0819, 0x00 },{ 0x081A, 0x43 },{ 0x081B, 0x06 },{ 0x081C, 0x00 },{ 0x081D, 0x44 },{ 0x081E, 0x06 },{ 0x081F, 0x00 },{ 0x0820, 0x00 },{ 0x0821, 0x00 },{ 0x0822, 0x00 },{ 0x0823, 0x00 },{ 0x0824, 0x00 },{ 0x0825, 0x00 },{ 0x0826, 0x00 },{ 0x0827, 0x00 },{ 0x0828, 0x00 },{ 0x0829, 0x00 },{ 0x082A, 0x00 },{ 0x082B, 0x00 },{ 0x082C, 0x00 },{ 0x082D, 0x00 },{ 0x082E, 0x00 },{ 0x082F, 0x00 },{ 0x0830, 0x00 },{ 0x0831, 0x00 },{ 0x0832, 0x00 },{ 0x0833, 0x00 },{ 0x0834, 0x00 },{ 0x0835, 0x00 },{ 0x0836, 0x00 },{ 0x0837, 0x00 },{ 0x0838, 0x00 },{ 0x0839, 0x00 },{ 0x083A, 0x00 },{ 0x083B, 0x00 },{ 0x083C, 0x00 },{ 0x083D, 0x00 },{ 0x083E, 0x00 },{ 0x083F, 0x00 },{ 0x0840, 0x00 },{ 0x0841, 0x00 },{ 0x0842, 0x00 },{ 0x0843, 0x00 },{ 0x0844, 0x00 },{ 0x0845, 0x00 },{ 0x0846, 0x00 },{ 0x0847, 0x00 },{ 0x0848, 0x00 },{ 0x0849, 0x00 },{ 0x084A, 0x00 },{ 0x084B, 0x00 },{ 0x084C, 0x00 },{ 0x084D, 0x00 },{ 0x084E, 0x00 },{ 0x084F, 0x00 },{ 0x0850, 0x00 },{ 0x0851, 0x00 },{ 0x0852, 0x00 },{ 0x0853, 0x00 },{ 0x0854, 0x00 },{ 0x0855, 0x00 },{ 0x0856, 0x00 },{ 0x0857, 0x00 },{ 0x0858, 0x00 },{ 0x0859, 0x00 },{ 0x085A, 0x00 },{ 0x085B, 0x00 },{ 0x085C, 0x00 },{ 0x085D, 0x00 },{ 0x085E, 0x00 },{ 0x085F, 0x00 },{ 0x0860, 0x00 },{ 0x0861, 0x00 },{ 0x090E, 0x02 },{ 0x0943, 0x00 },{ 0x0949, 0x00 },{ 0x094A, 0x00 },{ 0x094E, 0x49 },{ 0x094F, 0x02 },{ 0x095E, 0x00 },{ 0x0A03, 0x05 },{ 0x0A04, 0x00 },{ 0x0A05, 0x05 },{ 0x0B44, 0x0F },{ 0x0B45, 0x0A },{ 0x0B46, 0x00 },{ 0x0B47, 0x0F },{ 0x0B48, 0x0F },{ 0x0B4A, 0x0A },{ 0x0B57, 0x0E },{ 0x0B58, 0x01 },{ 0x001C, 0x01 },{ 0x0B24, 0xC3 },{ 0x0B25, 0x02 }}}
	        };

static struct Si5347RegStruct Si5347RegsCache[800];

//--Init------------------------------------------------------------------------------------------------------------------------------------
void ClockControlInit()
{
	memset( Si5347RegsCache, 0xFF, sizeof(Si5347RegsCache)); // reset the si5347RegsCache to all registers are programmed at init

	//init si5347
	gpModuleState->Si5347Reset = true;
	Si5347Init();
	//Si5338Init();

	//--Default ref clock mode--------------------------------------------------------------------------------------------------------------
	gpUserState->RefClockMode = CLOCK_REF_CLOCK_MODE_DEFAULT ;

	//--Default rates-----------------------------------------------------------------------------------------------------------------------
	gpUserState->DataRate = CLOCK_DATA_RATE_DEFAULT; //TODO: read from pio
	gpUserState->RefClockFrequency_kHz = CLOCK_REF_CLOCK_FREQ_DEFAULT_kHz ;

	//--Default clk out rates----------------------------------------------------------------------------------------------------------------
	gpUserState->ClkOut1Mode = CLK_OUT_LVDS;
	gpUserState->ClkOut1Freq_kHz = CLOCK_OUT_FREQ_DEFAULT_kHz;

	gpUserState->ClkOut2Mode = CLK_OUT_LVDS;
	gpUserState->ClkOut2Freq_kHz = CLOCK_OUT_FREQ_DEFAULT_kHz;

	// apply default settings
	ClockControlApplySettings();
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateTestConfig(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq)
{
	if(!ClockValidateRefClockRate(refClkFreq))
		return false;

	if(!ClockValidateRefClockMode(refClkMode))
		return false;

	if(!ClockValidateDataRate(DataRate))
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
bool SearchClockCache(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq, struct ClockCacheStruct **ClockCache)
{


	for(*ClockCache = ClockCacheLut; *ClockCache < ClockCacheLut+CLOCK_CACHE_LEN; ++*ClockCache)
	{
		if (((*ClockCache)->DataRate == DataRate)&&((*ClockCache)->refClkMode == refClkMode)&&((*ClockCache)->refClkFreq == refClkFreq))
			return true;
	}
	return false;
}

bool ClockCacheCheck(unsigned long long DataRate, unsigned int refClkMode, unsigned int refClkFreq)
{
	int i;
	for(i=0; i<CLOCK_CACHE_LEN; i++)
	{
		if ((ClockCacheLut[i].DataRate == DataRate)&&(ClockCacheLut[i].refClkMode == refClkMode)&&(ClockCacheLut[i].refClkFreq == refClkFreq))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockControlApplySettings()	// returns true => failed
{
	struct ClockCacheStruct *ClockCache;

	// lookup user rate in cache
	bool Result = SearchClockCache(gpUserState->DataRate, gpUserState->RefClockMode, gpUserState->RefClockFrequency_kHz, &ClockCache);
	//if entry is not found commit is not completed. flag.
	if(!Result)
		return false;

	gpUserState->TxPllRatio = 40;

	//--Program Si5326 and Si5338------------------------------------------------------------------------------------------------------------
	(void) ClockConfigureRefClock(ClockCache, Si5347RegsCache);
	ClockSetSi5338Si5347LockStatus();
	memcpy(Si5347RegsCache, ClockCache->Si5347Regs, sizeof(ClockCache->Si5347Regs));

	//-- Configure Data Transceivers-------------------------------------------------------------------------------------------------------------
	//usleep(2000);
	//XcvrFullReset(XPAR_AXI_CONTROL_REGS_BASEADDR);
	//usleep(2000);

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateRefClockMode( enum eRefClockMode RefClockMode )
{
	return ( (RefClockMode == CLOCK_MODE_INTERNAL) || (RefClockMode == CLOCK_MODE_EXTERNAL) );
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateDataRate( unsigned long long Rate )
{
	return (Rate >= CLOCK_MIN_DATA_RATE) && (Rate <= CLOCK_MAX_DATA_RATE) && (Rate <= gpModuleState->MaxDataRate);
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateRefClockRate( unsigned int RefFreq )
{
	return (RefFreq >= REF_FREQ_MIN_RATE) && (RefFreq <= REF_FREQ_MAX_RATE);
}

//------------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateOutClockMode( enum eClkOutMode OutClockMode)
{
	return (OutClockMode < CLK_OUT_MAX);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
bool ClockValidateOutClockFreq( unsigned int ClkOutFreq )
{
	return (ClkOutFreq >= CLOCK_OUT_MIN_RATE) && (ClkOutFreq <= CLOCK_OUT_MAX_RATE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void ClockSetSi5338Si5347LockStatus()
{
	//bool si5338Status = Si5338LockStatus();
	bool si5347Status = Si5347LockStatus(true);

	if(si5347Status)
		pios_set_bit(OUT_CLOCK_CHIP_LOCK_BASE, &OUT_CLOCK_CHIP_LOCK_PREV, OUT_CLOCK_CHIP_LOCK_MSK);
	else
		pios_clear_bit(OUT_CLOCK_CHIP_LOCK_BASE, &OUT_CLOCK_CHIP_LOCK_PREV, OUT_CLOCK_CHIP_LOCK_MSK);
}

bool ClockConfigureRefClock(struct ClockCacheStruct *ClockCache, struct Si5347RegStruct *Si5347RegsCache)
{
	bool Success = true;


	gpModuleState->configRetryCnt = 0;

	do
	{
		//----START SI5347 PROGRAM----
		Success = Si5347Configure(SI5347_NUM_REGS, ClockCache, Si5347RegsCache);

		if(!Success)
		{
			pios_set_bit(OUT_SI5347_RESET_BASE, &OUT_SI5347_RESET_PREV, OUT_SI5347_RESET_MSK);
			usleep(100);
			pios_clear_bit(OUT_SI5347_RESET_BASE, &OUT_SI5347_RESET_PREV, OUT_SI5347_RESET_MSK);
			usleep(1000);
			//indicate that Si5347 has been reset
			gpModuleState->Si5347Reset = true;

			Si5347Init();

			gpModuleState->configRetryCnt += 1;
		}

		//----END SI5347 PROGRAM----
	}while(!Success);


	return Success;
}

/*
bool ClockConfig5338Modified( bool firstInit, double ClkIn, unsigned int RefClkInSelect, double ClkOut0, double ClkOut1, double ClkOut2, double ClkOut3, int ClkMode1, int ClkMode2)
{
	static double prevClkIn;
	static double prevRefClkInSelect;
	static double prevClkOut0, prevClkOut1, prevClkOut2, prevClkOut3;
	static int prevClkMode1, prevClkMode2;

	if(firstInit)
	{
		prevClkIn = ClkIn;
		prevRefClkInSelect = RefClkInSelect;
		prevClkOut0 = ClkOut0;
		prevClkOut1 = ClkOut1;
		prevClkOut2 = ClkOut2;
		prevClkOut3 = ClkOut3;
		prevClkMode1 = ClkMode1;
		prevClkMode2 = ClkMode2;
		return true;
	}
	else //compare states
	{
		if((prevClkIn == ClkIn) && (prevRefClkInSelect == RefClkInSelect) && (prevClkOut0 == ClkOut0) && (prevClkOut1 == ClkOut1) && (prevClkOut2 == ClkOut2) && (prevClkOut3 == ClkOut3) && (prevClkMode1 == ClkMode1) && (prevClkMode2 == ClkMode2))
			return false;
		else
		{
			prevClkIn = ClkIn;
			prevRefClkInSelect = RefClkInSelect;
			prevClkOut0 = ClkOut0;
			prevClkOut1 = ClkOut1;
			prevClkOut2 = ClkOut2;
			prevClkOut3 = ClkOut3;
			prevClkMode1 = ClkMode1;
			prevClkMode2 = ClkMode2;
			return true;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------
bool ClockSet5338(unsigned int MasterControlBase, double ClkInFreq, unsigned int RefClkInSel, double ClkOut0Freq, double ClkOut1Freq, double ClkOut2Freq, double ClkOut3Freq, bool isModule5338)
{
	double pdFreq, vcoFreq;
	struct outputVals out0, out1, out2, out3;
	int tries = 0;

	if(isModule5338)
		Si5338SetOutputParamsModule(&out0, &out1, &out2, &out3);
	else
		Si5338SetOutputParamsBoard(&out0, &out1, &out2, &out3);

	//following flowchart as detailed on p21 of si5338 datasheet.
	//Si5338Init(MasterControlBase);					//i2c initialization
	Si5338DisableOutputs(MasterControlBase);
	Si5338PauseLOL(MasterControlBase);

	Si5338ConfigInputSelection(MasterControlBase, ClkInFreq, RefClkInSel);	//setup regs for the specified clock configuration.
	//out0 = userOut1, out1 = userOut2, out2 = SV1_Out, out3 = aux_out
	Si5338ConfigFreqPlan(MasterControlBase, ClkInFreq, ClkOut0Freq, ClkOut1Freq, ClkOut2Freq, ClkOut3Freq, &pdFreq, &vcoFreq, isModule5338);
	Si5338ConfigPllParams(MasterControlBase, pdFreq, vcoFreq);
	Si5338ConfigOutputDrivers(MasterControlBase, out0, out1, out2, out3, isModule5338, RefClkInSel);
	Si5338SetMiscRegs(MasterControlBase);

	if(!Si5338WaitForInputClk(MasterControlBase, 10000000, RefClkInSel))	//10 seconds
		return false;

	Si5338ConfigPLLForLock(MasterControlBase);
	usleep(50000);			////50 millis wait
	Si5338InitPLLLock(MasterControlBase);
	Si5338RestartLOL(MasterControlBase);
	usleep(25000);
	while(!Si5338WaitForPLLLock(MasterControlBase, 500000))		//500ms timeout
	{
		if(tries < 5)
		{
			Si5338InitPLLLock(MasterControlBase);
			usleep(10000);
		}
		else
		{
			return false;
		}
		tries++;
	}
	Si5338CopyFcalValues(MasterControlBase);
	Si5338SetPllToUseFcal(MasterControlBase);
	Si5338EnableOutputs(MasterControlBase);

	return true;
}
*/
