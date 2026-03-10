/*******************************************************************************
* Example Code
*
* Copyright(C) 2026 NXP Semiconductors
* NXP Semiconductors Confidential and Proprietary
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* NXP products.  This software is supplied "AS IS" without any warranties
* of any kind, and NXP Semiconductors and its licensor disclaim any and
* all warranties, express or implied, including all implied warranties of
* merchantability, fitness for a particular purpose and non-infringement of
* intellectual property rights.  NXP Semiconductors assumes no responsibility
* or liability for the use of the software, conveys no license or rights
* under any patent, copyright, mask work right, or any other intellectual
* property rights in or to any products. NXP Semiconductors reserves the
* right to make changes in the software without notification. NXP
* Semiconductors also makes no representation or warranty that such
* application will be suitable for the specified use without further testing
* or modification.
*
* IN NO EVENT WILL NXP SEMICONDUCTORS BE LIABLE, WHETHER IN CONTRACT, 
* TORT, OR OTHERWISE, FOR ANY INCIDENTAL, SPECIAL, INDIRECT, CONSEQUENTIAL 
* OR PUNITIVE DAMAGES, INCLUDING, BUT NOT LIMITED TO, DAMAGES FOR ANY 
* LOSS OF USE, LOSS OF TIME, INCONVENIENCE, COMMERCIAL LOSS, OR LOST 
* PROFITS, SAVINGS, OR REVENUES, TO THE FULL EXTENT SUCH MAY BE DISCLAIMED  
* BY LAW. NXP SEMICONDUCTOR’S TOTAL LIABILITY FOR ALL COSTS, DAMAGES, 
* CLAIMS, OR LOSSES WHATSOEVER ARISING OUT OF OR IN CONNECTION WITH THE 
* SOFTWARE IS LIMITED TO THE AGGREGATE AMOUNT PAID BY YOU TO NXP SEMICONDUCTORS
* IN CONNECTION WITH THE SOFTWARE TO WHICH LOSSES OR DAMAGES ARE CLAIMED.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors' and its
* licensor's relevant copyrights in the software, without fee, provided
* that it is used in conjunction with NXP Semiconductors devices.  This
* copyright, permission, and disclaimer notice must appear in all copies
* of this code.
*******************************************************************************/

//
// MC33816_spi_map.h
//
//  MC33816 SPI Register Addresses
//
//

#ifndef MC33816_SPI_MAP_H_
#define MC33816_SPI_MAP_H_

// Channel 1 Register Addresses
#define ch1_flash_enable                 0x100
#define ch1_ctrl_reg_uc0                 0x101
#define ch1_ctrl_reg_uc1                 0x102
#define ch1_unlock_word                  0x103
#define ch1_start_config_reg             0x104
#define ch1_status_reg_uc0               0x105
#define ch1_status_reg_uc1               0x106
#define ch1_code_width                   0x107
#define ch1_checksum_h                   0x108
#define ch1_checksum_l                   0x109
#define ch1_uc0_entry_point              0x10A
#define ch1_uc1_entry_point              0x10B
#define ch1_diag_routine_addr            0x10C
#define ch1_driver_disabled_routine_addr 0x10D
#define ch1_sw_interrupt_routine_addr    0x10E
#define ch1_uc0_irq_status               0x10F
#define ch1_uc1_irq_status               0x110
#define ch1_counter34_prescaler          0x111
#define ch1_dac_rxtx_cr_config           0x112

// Channel 2 Register Addresses
#define ch2_flash_enable                 0x120
#define ch2_ctrl_reg_uc0                 0x121
#define ch2_ctrl_reg_uc1                 0x122
#define ch2_unlock_word                  0x123
#define ch2_start_config_reg             0x124
#define ch2_status_reg_uc0               0x125
#define ch2_status_reg_uc1               0x126
#define ch2_code_width                   0x127
#define ch2_checksum_h                   0x128
#define ch2_checksum_l                   0x129
#define ch2_uc0_entry_point              0x12A
#define ch2_uc1_entry_point              0x12B
#define ch2_diag_routine_addr            0x12C
#define ch2_driver_disabled_routine_addr 0x12D
#define ch2_sw_interrupt_routine_addr    0x12E
#define ch2_uc0_irq_status               0x12F
#define ch2_uc1_irq_status               0x130
#define ch2_counter34_prescaler          0x131
#define ch2_dac_rxtx_cr_config           0x132

// Diag Register Addresses
#define diag_ls1_diag_config1            0x140
#define diag_ls1_diag_config2            0x141
#define diag_ls1_output_config           0x142
#define diag_ls2_diag_config1            0x143
#define diag_ls2_diag_config2            0x144
#define diag_ls2_output_config           0x145
#define diag_ls3_diag_config1            0x146
#define diag_ls3_diag_config2            0x147
#define diag_ls3_output_config           0x148
#define diag_ls4_diag_config1            0x149
#define diag_ls4_diag_config2            0x14A
#define diag_ls4_output_config           0x14B
#define diag_ls5_diag_config1            0x14C
#define diag_ls5_diag_config2            0x14D
#define diag_ls5_output_config           0x14E
#define diag_ls6_diag_config1            0x14F
#define diag_ls6_diag_config2            0x150
#define diag_ls6_output_config           0x151
#define diag_ls7_output_config           0x152
#define diag_hs1_diag_config_1           0x153
#define diag_hs1_diag_config_2           0x154
#define diag_hs1_output_config           0x155
#define diag_hs2_diag_config_1           0x156
#define diag_hs2_diag_config_2           0x157
#define diag_hs2_output_config           0x158
#define diag_hs3_diag_config_1           0x159
#define diag_hs3_diag_config_2           0x15A
#define diag_hs3_output_config           0x15B
#define diag_hs4_diag_config_1           0x15C
#define diag_hs4_diag_config_2           0x15D
#define diag_hs4_output_config           0x15E
#define diag_hs5_diag_config_1           0x15F
#define diag_hs5_diag_config_2           0x160
#define diag_hs5_output_config           0x161
#define diag_err_uc0ch1_part1            0x162
#define diag_err_uc0ch1_part2            0x163
#define diag_err_uc1ch1_part1            0x164
#define diag_err_uc1ch1_part2            0x165
#define diag_err_uc0ch2_part1            0x166
#define diag_err_uc0ch2_part2            0x167
#define diag_err_uc1ch2_part1            0x168
#define diag_err_uc1ch2_part2            0x169
#define diag_fw_ext_req                  0x16A
#define diag_iagnosis_option             0x16B

// IO Register Addresses
#define io_fbk_sens_uc0_ch1              0x180
#define io_fbk_sens_uc1_ch1              0x181
#define io_fbk_sens_uc0_ch2              0x182
#define io_fbk_sens_uc1_ch2              0x183
#define io_out_acc_uc0_ch1               0x184
#define io_out_acc_uc1_ch1               0x185
#define io_out_acc_uc0_ch2               0x186
#define io_out_acc_uc1_ch2               0x187
#define io_cur_block_access_part1        0x188
#define io_cur_block_access_part2        0x189
#define io_vds_thresholds_hs             0x18A
#define io_vsrc_thresholds_hs            0x18B
#define io_vds_thresholds_ls_part1_orig  0x18C
#define io_vds_thresholds_ls_part2_orig  0x18D
#define io_hs_slewrate                   0x18E
#define io_ls_slewrate                   0x18F
#define io_offset_compensation1          0x190
#define io_offset_compensation2          0x191
#define io_offset_compensation3          0x192
#define io_offset_compensation4          0x193
#define io_adc1_result                   0x194
#define io_adc2_result                   0x195
#define io_adc3_result                   0x196
#define io_adc4_result                   0x197
#define io_current_filter12              0x198
#define io_current_filter34l             0x199
#define io_current_filter4h4neg          0x19A
#define io_boost_dac                     0x19B
#define io_boost_dac_access              0x19C
#define io_boost_filter                  0x19D
#define io_dac1_value                    0x19E
#define io_dac2_value                    0x19F
#define io_dac3_value                    0x1A0
#define io_dac4l_value                   0x1A1
#define io_dac4h_value                   0x1A2
#define io_dac4neg_value                 0x1A3
#define io_bias_config                   0x1A4
#define io_bootstrap_charged             0x1A5
#define io_hs12_ls_act                   0x1A6
#define io_hs34_ls_act                   0x1A7
#define io_hs5_ls_act                    0x1A8
#define io_dac_settling_time             0x1A9
#define io_oa_out1_config                0x1AA
#define io_oa_out2_config                0x1AB

// Main Register Addresses
#define main_Clock_Prescaler             0x1C0
#define main_Flags_Direction             0x1C1
#define main_Flags_Polarity              0x1C2
#define main_Flags_source                0x1C3
#define main_compensation_prescaler      0x1C4
#define main_Driver_Config               0x1C5
#define main_PLL_config                  0x1C6
#define main_Backup_Clock_Status         0x1C7
#define main_SPI_config                  0x1C8
#define main_Cipher_config               0x1C9
#define main_Trace_start                 0x1CA
#define main_Trace_stop                  0x1CB
#define main_Trace_config                0x1CC
#define main_Device_lock                 0x1CD
#define main_Reset_behavior              0x1CE
#define main_Device_unlock               0x1CF
#define main_Global_reset_code_part1     0x1D0
#define main_Global_reset_code_part2     0x1D1
#define main_Driver_Status               0x1D2
#define main_SPI_error_code              0x1D3
#define main_Interrupt_register          0x1D4
#define main_Device_Identifier           0x1D5
#define main_Reset_source                0x1D6
#define main_BIST_interface              0x1DC

#define selection_register               0x3FF

#define COMMON_PAGE                      0x04
#define IDENTIFER_REVISION               0x9C43
#define IDENTIFER_REVISION1              0x9CB4  // C3-Si
#define IDENTIFER_REVISION2              0x9CC4  // C4-Si
#define IDENTIFER_REVISION3              0x9CD4
#define IDENTIFER_REVISION4              0x9DC4
#define IDENTIFER_REVISION5              0x9DD4
#define BIST_PASSWORD                    0xB157
#define DEVICE_UNLOCK_PASSWORD           0x1337
#define BOOTSTRP_CHARGE                  0x1F


#endif // MC33816_SPI_MAP_H_
