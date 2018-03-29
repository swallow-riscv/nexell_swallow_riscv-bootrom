#include <nx_pll.h>
#include <nx_clock.h>
#include <nx_cpuif_regmap.h>
#include <nx_chip_sfr.h>

#ifdef QEMU_RISCV
#include <nx_qemu_platform.h>
#else
#include <nx_swallow_platform.h>
#endif

unsigned int *__PLL_CPUIF_BASEADDR[NUMBER_OF_PLL_CPUIF_MODULE];

extern _SFR_INFO SFR_INFO;

struct nx_pll_info {
	unsigned short m;
	unsigned short s;
} const pllpms[] =
{
	{125, 3},	/* 24M * 125 / 3 / 2^3 = 125MHz */
	{250, 2},	/* 24M * 250 / 3 / 2^2 = 500MHz */
	{200, 1},	/* 24M * 200 / 3 / 2^1 = 800MHz */
	{250, 1}	/* 24M * 250 / 3 / 2^1 = 1000MHz */
};
struct nx_cpuclkdiv_info {
	unsigned char axi;
	unsigned char atclk;
	unsigned char cntclk;
	unsigned char tsclk;
	unsigned char dbgapb;
	unsigned char apb;
} const cpuclkdiv[] = {
	{
		1,	/* axi: 125 / 1 = 125MHz */
		1,	/* atclk: 125 / 1 = 125MHz */
		1,	/* cntclk: 125 / 1 = 125MHz */
		1,	/* tsclk: 125 / 1 = 125MHz */
		2,	/* dbgapb: 125 / 2 = 62.5MHz */
		2	/* apb: 125 / 2 = 62.5MHz */
	}, {
		1,	/* axi: 500 / 2 = 250MHz */
		3,	/* atclk: 500 / 4 = 125MHz */
		3,	/* cntclk: 500 / 4 = 125MHz */
		3,	/* tsclk: 500 / 4 = 125MHz */
		4,	/* dbgapb: 500 / 5 = 100MHz */
		4	/* apb: 500 / 5 = 100MHz */
	}, {
		1,	/* axi: 800 / 2 = 400MHz */
		3,	/* atclk: 800 / 4 = 200MHz */
		3,	/* cntclk: 800 / 4 = 200MHz */
		3,	/* tsclk: 800 / 4 = 200MHz */
		7,	/* dbgapb: 800 / 8 = 100MHz */
		7	/* apb: 800 / 8 = 100MHz */
	}
};

struct nx_sysclkdiv_info {
	unsigned char axi;
	unsigned char apb;
	unsigned char hsif_axi;
	unsigned char hsif_apb;
	unsigned char cmu_apb;
	unsigned char smc_axi;
};

void __PLL_CPUIF_SET_BASEADDR (unsigned int moduleindex, unsigned int *baseaddr) {
    __PLL_CPUIF_BASEADDR[moduleindex] = baseaddr;
} 

void NX_PLL_SetBaseAddress(unsigned int inst_index, unsigned int BaseAddress)
{
    __PLL_CPUIF_SET_BASEADDR (inst_index, BaseAddress);
}

struct __nx_cpuif_PLL_CPUIFregmap_struct__ g_PLL_CPUIFregmap[NUMBER_OF_PLL_CPUIF_MODULE];

void NX_PLL_SetOSCMUX( unsigned int inst_index, unsigned int MUXSEL )
{
    _dprintf("<<bootrom>> %s, g_PLL_CPUIFregmap[0x%x].OSCCLK_MUXSEL=0x%x\n",__func__, inst_index, g_PLL_CPUIFregmap[inst_index].OSCCLK_MUXSEL);
    nx_cpuif_reg_write_one(g_PLL_CPUIFregmap[inst_index].OSCCLK_MUXSEL, MUXSEL);
}

void nxSetClockInit(void)
{
    //    unsigned int rm_blk_usb, rm_blk_mm;
    _dprintf("<<bootrom>> %s, SFR_INFO.PLL[0]=0x%x\n",__func__, SFR_INFO.PLL[0]);
    _dprintf("<<bootrom>> %s, SFR_INFO.PLL[1]=0x%x\n",__func__, SFR_INFO.PLL[1]);
    NX_PLL_SetBaseAddress(INDEX_PLL_0, SFR_INFO.PLL[0]);
    NX_PLL_SetBaseAddress(INDEX_PLL_1, SFR_INFO.PLL[1]);

    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__group_clock_source     , 1  ); // PLL[0]

    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK400__dynamic_divider_value	, 1-1); // div 1
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___AXI__dynamic_divider_value	, 2-1); // div 2
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___APB__dynamic_divider_value	, 4-1); // div 4
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK133__dynamic_divider_value	, 3-1); // div 3
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK50__dynamic_divider_value	, 8-1); // div 8
    nx_cpuif_reg_write_one(CMU_INFO_DEF__SYS_0___CLK40__dynamic_divider_value	,10-1); // div 10

    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__group_clock_source       , 2  ); // PLL[1]
    nx_cpuif_reg_write_one(CMU_INFO_DEF__CPU_0___CORE__dynamic_divider_value    , 2-1); // div 2

    _dprintf("<<bootrom>> %s, PLL setting Done\n",__func__);
    //TODO
    /* while(1) { */
    /*     if (lock == 1) { */
            NX_PLL_SetOSCMUX(INDEX_PLL_0, PLL_MUX_PLL_FOUT);
            NX_PLL_SetOSCMUX(INDEX_PLL_1, PLL_MUX_PLL_FOUT);
            //            __asm__ __volatile__ ("dmb");
    /*         break; */
    /*     } */
                     
    /* } */

}
