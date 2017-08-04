#ifndef __CODEC_ES8388_H__
#define __CODEC_ES8388_H__

#include <stdint.h>

/* Register Definitions */
#define ES8388_CONTROL1         	(0x00)		/* 0 */
#define ES8388_CONTROL2         	(0x01)		/* 1 */
#define ES8388_CHIPPOWER        	(0x02)		/* 2 */
#define ES8388_ADCPOWER         	(0x03)		/* 3 */
#define ES8388_DACPOWER         	(0x04)		/* 4 */
#define ES8388_CHIPLOPOW1       	(0x05)		/* 5 */
#define ES8388_CHIPLOPOW2       	(0x06)		/* 6 */
#define ES8388_ANAVOLMANAG      	(0x07)		/* 7 */
#define ES8388_MASTERMODE       	(0x08)		/* 8 */
#define ES8388_ADCCONTROL1      	(0x09)		/* 9 */
#define ES8388_ADCCONTROL2      	(0x0a)		/* 10 */
#define ES8388_ADCCONTROL3      	(0x0b)		/* 11 */
#define ES8388_ADCCONTROL4      	(0x0c)		/* 12 */
#define ES8388_ADCCONTROL5      	(0x0d)		/* 13 */
#define ES8388_ADCCONTROL6      	(0x0e)		/* 14 */
#define ES8388_ADCCONTROL7      	(0x0f)		/* 15 */
#define ES8388_ADCCONTROL8      	(0x10)		/* 16 */
#define ES8388_ADCCONTROL9      	(0x11)		/* 17 */
#define ES8388_ADCCONTROL10     	(0x12)		/* 18 */
#define ES8388_ADCCONTROL11     	(0x13)		/* 19 */
#define ES8388_ADCCONTROL12     	(0x14)		/* 20 */
#define ES8388_ADCCONTROL13     	(0x15)		/* 21 */
#define ES8388_ADCCONTROL14     	(0x16)		/* 22 */
#define ES8388_DACCONTROL1      	(0x17)		/* 23 */
#define ES8388_DACCONTROL2      	(0x18)		/* 24 */
#define ES8388_DACCONTROL3      	(0x19)		/* 25 */
#define ES8388_DACCONTROL4      	(0x1a)		/* 26 */
#define ES8388_DACCONTROL5      	(0x1b)		/* 27 */
#define ES8388_DACCONTROL6      	(0x1c)		/* 28 */
#define ES8388_DACCONTROL7      	(0x1d)		/* 29 */
#define ES8388_DACCONTROL8      	(0x1e)		/* 30 */
#define ES8388_DACCONTROL9      	(0x1f)		/* 31 */
#define ES8388_DACCONTROL10     	(0x20)		/* 32 */
#define ES8388_DACCONTROL11     	(0x21)		/* 33 */
#define ES8388_DACCONTROL12     	(0x22)		/* 34 */
#define ES8388_DACCONTROL13     	(0x23)		/* 35 */
#define ES8388_DACCONTROL14     	(0x24)		/* 36 */
#define ES8388_DACCONTROL15     	(0x25)		/* 37 */
#define ES8388_DACCONTROL16     	(0x26)		/* 38 */
#define ES8388_DACCONTROL17     	(0x27)		/* 39 */
#define ES8388_DACCONTROL18     	(0x28)		/* 40 reserved. */
#define ES8388_DACCONTROL19     	(0x29)		/* 41 reserved. */
#define ES8388_DACCONTROL20     	(0x2a)		/* 42 */
#define ES8388_DACCONTROL21     	(0x2b)		/* 43 */
#define ES8388_DACCONTROL22     	(0x2c)		/* 44 */
#define ES8388_DACCONTROL23     	(0x2d)		/* 45 */
#define ES8388_DACCONTROL24     	(0x2e)		/* 46 */
#define ES8388_DACCONTROL25     	(0x2f)		/* 47 */
#define ES8388_DACCONTROL26     	(0x30)		/* 48 */
#define ES8388_DACCONTROL27     	(0x31)		/* 49 */
#define ES8388_DACCONTROL28     	(0x32)		/* 50 */
#define ES8388_DACCONTROL29     	(0x33)		/* 51 */
#define ES8388_DACCONTROL30     	(0x34)		/* 52 */

#define OUTPUT_MUTED                (0x92)
#define OUTPUT_0DB                  (0x00)

#define ES8388_LADC_VOL         	ES8388_ADCCONTROL8
#define ES8388_RADC_VOL         	ES8388_ADCCONTROL9
#define ES8388_LDAC_VOL         	ES8388_DACCONTROL4
#define ES8388_RDAC_VOL         	ES8388_DACCONTROL5
#define ES8388_LOUT1_VOL        	ES8388_DACCONTROL24
#define ES8388_ROUT1_VOL        	ES8388_DACCONTROL25
#define ES8388_LOUT2_VOL        	ES8388_DACCONTROL26
#define ES8388_ROUT2_VOL        	ES8388_DACCONTROL27
#define ES8388_ADC_MUTE         	ES8388_ADCCONTROL7
#define ES8388_DAC_MUTE         	ES8388_DACCONTROL3
#define ES8388_ADCIN			 	ES8388_ADCPOWER
#define ES8388_IFACE            	ES8388_MASTERMODE
#define ES8388_ADC_IFACE        	ES8388_ADCCONTROL4
#define ES8388_ADC_SRATE        	ES8388_ADCCONTROL5
#define ES8388_DAC_IFACE        	ES8388_DACCONTROL1
#define ES8388_DAC_SRATE        	ES8388_DACCONTROL2

#define ES8388_CACHEREGNUM      	(53)
#define ES8388_SYSCLK	        	(0)

#define ES8388_PLL1					(0)
#define ES8388_PLL2					(1)

/* clock inputs */
#define ES8388_MCLK					(0)
#define ES8388_PCMCLK				(1)

/* clock divider id's */
#define ES8388_PCMDIV				(0)
#define ES8388_BCLKDIV				(1)
#define ES8388_VXCLKDIV				(2)

/* PCM clock dividers */
#define ES8388_PCM_DIV_1			(0 << 6)
#define ES8388_PCM_DIV_3			(2 << 6)
#define ES8388_PCM_DIV_5_5			(3 << 6)
#define ES8388_PCM_DIV_2			(4 << 6)
#define ES8388_PCM_DIV_4			(5 << 6)
#define ES8388_PCM_DIV_6			(6 << 6)
#define ES8388_PCM_DIV_8			(7 << 6)

/* BCLK clock dividers */
#define ES8388_BCLK_DIV_1			(0 << 7)
#define ES8388_BCLK_DIV_2			(1 << 7)
#define ES8388_BCLK_DIV_4			(2 << 7)
#define ES8388_BCLK_DIV_8			(3 << 7)

/* VXCLK clock dividers */
#define ES8388_VXCLK_DIV_1			(0 << 6)
#define ES8388_VXCLK_DIV_2			(1 << 6)
#define ES8388_VXCLK_DIV_4			(2 << 6)
#define ES8388_VXCLK_DIV_8			(3 << 6)
#define ES8388_VXCLK_DIV_16			(4 << 6)

#define ES8388_DAI_HIFI				(0)
#define ES8388_DAI_VOICE			(1)

#define ES8388_1536FS 				(1536)
#define ES8388_1024FS				(1024)
#define ES8388_768FS				(768)
#define ES8388_512FS				(512)
#define ES8388_384FS				(384)
#define ES8388_256FS				(256)
#define ES8388_128FS				(128)

/* REGISTER 0 – CHIP CONTROL 1, DEFAULT 0000 0110 */
#define R00_VMID_DISABLE			(0)
#define R00_VMID_50K				(1)
#define R00_VMID_500K				(2)
#define R00_VMID_5K					(3)
#define R00_REF_EN					(1 << 2)
#define R00_REF_DIS					(0 << 2)

/* REGISTER 1 – CHIP CONTROL 2, DEFAULT 0001 1100 */
#define R01_VREFBUF_ON				(0 << 0)
#define R01_VREFBUF_PDN				(1 << 0)
#define R01_VREF_ON					(0 << 1)
#define R01_VREF_PDN				(1 << 1)
#define R01_IBIASGEN_ON				(0 << 2)
#define R01_IBIASGEN_PDN			(1 << 2)
#define R01_ANA_ON					(0 << 3)
#define R01_ANA_PND					(0 << 3)
#define R01_VREFBUF_LP				(1 << 4)
#define R01_VCMMOD_LP				(1 << 5)

/* REGISTER 2 – CHIP POWER MANAGEMENT, DEFAULT 1100 0011 */
#define R02_DACVREF_UP				(0 << 0)
#define R02_DACVREF_DOWN			(1 << 0)
#define R02_ADCVREF_UP				(0 << 1)
#define R02_ADCVREF_DOWN			(1 << 1)
#define R02_DACDLL_UP		    	(0 << 2)	/* only ES8388 */
#define R02_DACDLL_DOWN		    	(1 << 2)	/* only ES8388 */
#define R02_ADCDLL_UP		    	(0 << 3)	/* only ES8388 */
#define R02_ADCDLL_DOWN		    	(1 << 3)	/* only ES8388 */
#define R02_ADC_STM_RST		    	(1 << 4)	/* ADC state machine */
#define R02_DAC_STM_RST		    	(1 << 5)	/* DAC state machine */
#define R02_DAC_DIG_PDN		    	(1 << 6)	/* resets DAC DSM, DEM, filter and serial data port */
#define R02_ADC_DIG_PDN		    	(1 << 7)	/* resets ADC DEM, filter and serial data port */

/* REGISTER 3 – ADC POWER MANAGEMENT, DEFAULT 1111 1100 */
#define R03_INT_NORMAL				(0 << 0)
#define R03_INT_LP					(1 << 0)
#define R03_ADC_NORMAL				(0 << 1)
#define R03_ADC_LP					(1 << 1)
#define R03_ADC_BIASGEN_NORMAL		(0 << 2)
#define R03_ADC_BIASGEN_PND			(1 << 2)
#define R03_MICB_NORMAL				(0 << 3)
#define R03_MICB_PND				(1 << 3)
#define R03_ADCR_NORMAL				(0 << 4) /* ES8388 only. */
#define R03_ADCR_PND				(1 << 4) /* ES8388 only. */
#define R03_ADCL_NORMAL				(0 << 5)
#define R03_ADCL_PND				(1 << 5)
#define R03_AINR_NORMAL				(0 << 6) /* ES8388 only. */
#define R03_AINR_PND				(1 << 6) /* ES8388 only. */
#define R03_AINL_NORMAL				(0 << 7)
#define R03_AINL_PND				(1 << 7)

/* REGISTER 4 – DAC POWER MANAGEMENT, DEFAULT 1100 0000 */
#define R04_ROUT_DIS				(0 << 2) /* ES8388S only. */
#define R04_ROUT_EN					(1 << 2) /* ES8388S only. */
#define R04_LOUT_DIS				(0 << 3) /* ES8388S only. */
#define R04_LOUT_EN					(1 << 3) /* ES8388S only. */
#define R04_ROUT2_DIS				(0 << 2) /* ES8388 only. */
#define R04_ROUT2_EN				(1 << 2) /* ES8388 only. */
#define R04_LOUT2_DIS				(0 << 3) /* ES8388 only. */
#define R04_LOUT2_EN				(1 << 3) /* ES8388 only. */
#define R04_ROUT1_DIS				(0 << 4) /* ES8388 only. */
#define R04_ROUT1_EN				(1 << 4) /* ES8388 only. */
#define R04_LOUT1_DIS				(0 << 5) /* ES8388 only. */
#define R04_LOUT1_EN				(1 << 5) /* ES8388 only. */
#define R04_DACR_UP					(0 << 6)
#define R04_DACR_PDN				(1 << 6)
#define R04_DACL_UP					(0 << 7)
#define R04_DACL_PDN				(1 << 7)

/* REGISTER 9 */
#define R09_MICAMPL_DB(db)			((db/3) << 4)
#define R09_MICAMPR_DB(db)			((db/3) << 0) /* ES8388 only. */

/* REGISTER 10 */
#define R10_CAP_MODE_DIS			(0 << 0) /* ES8388S only. */
#define R10_CAP_MODE_EN 			(1 << 0) /* ES8388S only. */
#define R10_DSR_LIN1_RIN1 			(0 << 2) /* ES8388 only. */
#define R10_DSR_LIN2_RIN2 			(1 << 2) /* ES8388 only. */
#define R10_DSSEL_ONE	 			(0 << 3) /* ES8388 only. */
#define R10_DSSEL_TWO	 			(1 << 3) /* ES8388 only. */
#define R10_RINSEL_RIN1 			(0 << 4) /* ES8388 only. */
#define R10_RINSEL_RIN2 			(1 << 4) /* ES8388 only. */
#define R10_RINSEL_DIFFERENTIAL		(3 << 4) /* ES8388 only. */
#define R10_LINSEL_LIN1 			(0 << 6)
#define R10_LINSEL_LIN2 			(1 << 6)
#define R10_LINSEL_DIFFERENTIAL		(3 << 6)

/* REGISTER 11 */
#define R11_ASDOUT_EN				(0 << 2)
#define R11_ASDOUT_DIS				(1 << 2)
#define R11_LDCM_DIS				(0 << 6)
#define R11_LDCM_EN					(1 << 6)
#define R11_DS_LIN1_RIN1			(0 << 7)
#define R11_DS_LIN2_LIN2			(1 << 7)

/* REGISTER 12 */
#define R12_ADC_FMT_I2S				(0 << 0)
#define R12_ADC_FMT_LEFT			(1 << 0)
#define R12_ADC_FMT_PCM				(3 << 0)
#define R12_ADCWL_24BIT				(0 << 2)
#define R12_ADCWL_20BIT				(1 << 2)
#define R12_ADCWL_18BIT				(2 << 2)
#define R12_ADCWL_16BIT				(3 << 2)
#define R12_ADCWL_32BIT				(4 << 2)

/* REGISTER 13 */
#define R13_ADC_FS_256				(0x02 << 0)
#define R13_ADC_FS_1500				(0x1B << 0)
#define R13_ADC_DUAL_MODE			(1 << 5)
#define R13_ADC_RATIO_AUTO			(0 << 6) /* ES8388S only. */
#define R13_ADC_RATIO_MANUAL 		(1 << 6) /* ES8388S only. */

/* REGISTER 14 */
#define R14_ADC_RHPF_DIS			(0 << 4) /* ES8388 only. */
#define R14_ADC_RHPF_EN				(1 << 4) /* ES8388 only. */
#define R14_ADC_LHPF_DIS			(0 << 5)
#define R14_ADC_LHPF_EN				(1 << 5)
#define R14_ADC_R_INVERTED			(1 << 6) /* ES8388 only. */
#define R14_ADC_L_INVERTED			(1 << 7)

/* REGISTER 15 */
#define R15_ADC_UNMUTE				(0 << 2)
#define R15_ADC_MUTE				(1 << 2)
#define R15_ADC_SOFT_RAMP_DIS   	(0 << 5)
#define R15_ADC_SOFT_RAMP_EN    	(1 << 5)
#define R15_ADC_RAMP_4LRCK			(0 << 6)
#define R15_ADC_RAMP_8LRCK			(1 << 6)
#define R15_ADC_RAMP_16LRCK			(2 << 6)
#define R15_ADC_RAMP_32LRCK			(3 << 6)

/* REGISTER 18 */
#define R18_ADC_ALC_OFF				(0 << 6)
#define R18_ADC_ALC_RIGHT			(1 << 6)
#define R18_ADC_ALC_LEFT			(2 << 6)
#define R18_ADC_ALC_STEREO			(3 << 6)
#define R18_ALC_MAXGAIN_N6_5DB		(0 << 3)
#define R18_ALC_MAXGAIN_N0_5DB		(1 << 3)
#define R18_ALC_MAXGAIN_11_5DB		(3 << 3)
#define R18_ALC_MAXGAIN_17_5DB		(4 << 3)
#define R18_ALC_MAXGAIN_23_5DB		(5 << 3)
#define R18_ALC_MAXGAIN_29_5DB		(6 << 3)
#define R18_ALC_MAXGAIN_35_5DB		(7 << 3)
#define R18_ALC_MINGAIN_N12DB		(0 << 0)
#define R18_ALC_MINGAIN_N6DB		(1 << 0)
#define R18_ALC_MINGAIN_0DB			(2 << 0)
#define R18_ALC_MINGAIN_6DB			(3 << 0)
#define R18_ALC_MINGAIN_12DB		(4 << 0)

/* REGISTER 19 */
#define R19_ADC_ALC_LVL_DB(db)	((int)((db+16.5)/1.5) << 4)
#define R19_ADC_ALC_HLD(hld)	(hld << 0)

/* REGISTER 20 */
#define R20_ADC_ALCDCY_410US		(0 << 4)
#define R20_ADC_ALCDCY_820US		(1 << 4)
#define R20_ADC_ALCATK_104US		(0 << 0)
#define R20_ADC_ALCATK_416US		(2 << 0)

/* REGISTER 21 */
#define R21_ADC_ALCMODE				(0 << 7)
#define R21_ADC_ALCZC_OFF			(0 << 6)
#define R21_ADC_TIME_OUT_OFF		(0 << 5)
#define R21_ADC_ALC_WIN_SIZE_96		(6 << 0)
#define R21_ADC_ALC_WIN_SIZE_102    (7 << 0)

/* REGISTER 22 */
#define R22_ADC_ALC_NGAT_DIS	    (0 << 0)
#define R22_ADC_ALC_NGAT_EN			(1 << 0)
#define R22_ADC_ALC_NGG_PGA_HELD	(0 << 1)
#define R22_ADC_ALC_NGG_MUTE_ADC	(1 << 1)
#define R22_ADC_ALC_NGTH_N73_5DB	(2 << 3)
#define R22_ADC_ALC_NGTH_N51DB		(17 << 3)
#define R22_ADC_ALC_NGTH_N40_5DB    (24 << 3)
#define R22_ADC_ALC_NGTH_N39DB      (25 << 3)
#define R22_ADC_ALC_NGTH_N37_5DB    (26 << 3)
#define R22_ADC_ALC_NGTH_N36DB      (27 << 3)
#define R22_ADC_ALC_NGTH_N34_5DB    (28 << 3)
#define R22_ADC_ALC_NGTH_N33DB      (29 << 3)
#define R22_ADC_ALC_NGTH_N31_5DB    (30 << 3)
#define R22_ADC_ALC_NGTH_N30DB      (31 << 3)
#define R22_ADC_ALC_NGTH_DB(db)     ((int)((db + 76.5)/1.5) << 3) /* -30 ~ -76.5, 1.5db/setp */

/* REGISTER 23 */
#define R23_DAC_FMT_I2S			(0 << 0)
#define R23_DAC_FMT_LEFT		(1 << 0)
#define R23_DAC_FMT_PCM			(3 << 0)
#define R23_DAC_DACWL_24BIT		(0 << 3)
#define R23_DAC_DACWL_20BIT		(1 << 3)
#define R23_DAC_DACWL_18BIT		(2 << 3)
#define R23_DAC_DACWL_16BIT		(3 << 3)
#define R23_DAC_DACWL_32BIT		(4 << 3)
#define R23_DAC_LRP_INVERTED	(1 << 6)
#define R23_DAC_DACLRSWAP		(1 << 7)

/* REGISTER 24 */
#define R24_DAC_FS_256			(0x02 << 0)
#define R24_DAC_FS_1500			(0x1B << 0)
#define R24_DAC_RATIO_AUTO		(0 << 6)

/* REGISTER 25 */
#define R25_DAC_UNMUTE			(0 << 2)
#define R25_DAC_MUTE			(1 << 2)
#define R25_DAC_LeR				(1 << 3) /* both channel gain control is set by DAC left gain control register */
#define R25_DAC_SOFT_RAMP_DIS	(0 << 5)
#define R25_DAC_SOFT_RAMP_EN	(1 << 5)
#define R25_DAC_RAMP_4LRCK		(0 << 6)
#define R25_DAC_RAMP_32LRCK		(1 << 6)
#define R25_DAC_RAMP_64LRCK		(2 << 6)
#define R25_DAC_RAMP_128LRCK	(3 << 6)

/* REGISTER 26 - LDAC Digital volume control */
/* REGISTER 27 - RDAC Digital volume control */

/* REGISTER 38 */
#define R38_RMIXSEL_LIN1		(0 << 0)
#define R38_RMIXSEL_LIN2		(1 << 0)
#define R38_RMIXSEL_DF2SE		(2 << 0)
#define R38_RMIXSEL_ADC			(3 << 0)
#define R38_LMIXSEL_LIN1		(0 << 3)
#define R38_LMIXSEL_LIN2		(1 << 3)
#define R38_LMIXSEL_DF2SE		(2 << 3)
#define R38_LMIXSEL_ADC			(3 << 3)

/* REGISTER 39 */
#define R39_MIXBOTH				(1 << 2) /* ES8388S only. */
#define R39_LI2LOVOL_GAIN_6DB	(0 << 3)
#define R39_LI2LOVOL_GAIN_3DB	(1 << 3)
#define R39_LI2LOVOL_GAIN_0DB	(2 << 3)
#define R39_LI2LOVOL_GAIN_N3DB	(3 << 3)
#define R39_LI2LOVOL_GAIN_N6DB	(4 << 3)
#define R39_LI2LOVOL_GAIN_N9DB	(5 << 3)
#define R39_LI2LOVOL_GAIN_N12DB	(6 << 3)
#define R39_LI2LOVOL_GAIN_N15DB	(7 << 3) /* default */
#define R39_LI2LO_DIS			(0 << 6) /* LIN to left mixer */
#define R39_LI2LO_EN			(1 << 6)
#define R39_LD2LO_DIS			(0 << 7) /* left DAC to left mixer */
#define R39_LD2LO_EN			(1 << 7)

/* REGISTER 42 */
#define R42_RI2ROVOL_GAIN_6DB	(0 << 3)
#define R42_RI2ROVOL_GAIN_3DB	(1 << 3)
#define R42_RI2ROVOL_GAIN_0DB	(2 << 3)
#define R42_RI2ROVOL_GAIN_N3DB	(3 << 3)
#define R42_RI2ROVOL_GAIN_N6DB	(4 << 3)
#define R42_RI2ROVOL_GAIN_N9DB	(5 << 3)
#define R42_RI2ROVOL_GAIN_N12DB	(6 << 3)
#define R42_RI2ROVOL_GAIN_N15DB	(7 << 3) /* default */
#define R42_RI2RO_DIS			(0 << 6) /* RIN to right mixer */
#define R42_RI2RO_EN			(1 << 6)
#define R42_RD2RO_DIS			(0 << 7) /* right DAC to right mixer */
#define R42_RD2RO_EN			(1 << 7)

/* REGISTER 43 */
#define R43_DAC_ANACLK_UP		(0 << 2)
#define R43_DAC_ANACLK_PND		(1 << 2)
#define R43_ADC_ANACLK_UP		(0 << 3)
#define R43_ADC_ANACLK_PND		(1 << 3)
#define R43_MCLK_EN				(0 << 4)
#define R43_MCLK_DIS 			(1 << 4)
#define R43_OFFSET_EN			(0 << 5)
#define R43_OFFSET_DIS 			(1 << 5)
#define R43_LRCK_SEL_DAC 		(0 << 6) /* LRCK select if slrck = 1 */
#define R43_LRCK_SEL_ADC 		(1 << 6)
#define R43_SLRCK_SAME			(1 << 7)

#endif	/* __CODEC_ES8388_H__ */
