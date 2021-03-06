/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * nvram_auto_gen.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   This file is intends for generating NVRAM information.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/***************************************************************************** 
* Include
*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "nvram_auto_gen.h"
#include "nvram_user_defs.h"
/***************************************************************************** 
* Define
*****************************************************************************/
#define BytesPerCluster 1024

/***************************************************************************** 
* Local Variable
*****************************************************************************/
kal_uint32 shadow_total_to_verify = 0; 
kal_uint32 shadow_size_to_verify = (2 * 2);
kal_uint32 nvram_disk_size = 0;
kal_uint16 custpack_total_to_verify = 0, j = 0;
kal_uint32 otp_total_to_verify = 0;
kal_uint32 otp_size_to_verify = 0;
kal_uint32 max_shadow_rec = 0;
custpack_nvram_header custpack_nvram_header_p;
custpack_nvram_header *custpack_nvram_header_ptr = &custpack_nvram_header_p;
kal_uint16 custpack_idx = 0;
#ifndef __SECURE_RO_ENABLE__
kal_uint16 secupack_idx = 0;
#endif
FILE* logOfSize;

void nvram_assign_table_entry(kal_uint16 position, ltable_entry_struct* ldi)
{
	kal_uint32 onefilesize = 0; 
    
    logical_data_item_table[position] = ldi;

#if defined(__SHADOW_NVRAM__)
    /* Calc SHADOW Total and Size */
    #if defined(__SHADOW_SUPPORT_MS__)
    if(logical_data_item_table[position]->category&NVRAM_CATEGORY_SHADOW_MS)
    #elif defined(__SHADOW_SUPPORT_SB__)
    if(logical_data_item_table[position]->category&NVRAM_CATEGORY_SHADOW)
    #endif
    {
        /* shadow items count*/
        shadow_total_to_verify++;
        
        /* shadow size */
        shadow_size_to_verify += logical_data_item_table[position]->size * logical_data_item_table[position]->total_records;

        /* max shadow record size */
        if (max_shadow_rec < logical_data_item_table[position]->total_records)
            max_shadow_rec = logical_data_item_table[position]->total_records;
            
        /* shadow table index */
        logical_data_item_table[position]->record_ID = j++;
    }
#endif          

        /* Output data item size */
        onefilesize = logical_data_item_table[position]->size * logical_data_item_table[position]->total_records;
        if(onefilesize)
        {
            fprintf(logOfSize, "%4d%20d                    %s\n", logical_data_item_table[position]->LID, onefilesize, logical_data_item_table[position]->description);

            if (NVRAM_IS_ATTR_MULTIPLE(logical_data_item_table[position]->attr))
                    fprintf(logOfSize, "%4d%20d                    %s\n", logical_data_item_table[position]->LID, onefilesize, logical_data_item_table[position]->description);

        }

        if (logical_data_item_table[position]->category & NVRAM_CATEGORY_CUSTPACK)
        {
            custpack_total_to_verify++;
        }

        if (logical_data_item_table[position]->attr & NVRAM_ATTR_OTP)
        {
            otp_total_to_verify++;
            otp_size_to_verify += logical_data_item_table[position]->size * logical_data_item_table[position]->total_records;
        }
}


/*****************************************************************************
* FUNCTION
*  nvram_output_statistic
* DESCRIPTION
*  Output NVRAM Statistic Information.
* PARAMETERS
*  None
* RETURNS
*  None
*****************************************************************************/
void nvram_output_statistic(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint16 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	logOfSize = fopen("..\\..\\..\\~nvram_lid_size.log","w+");
    fprintf(logOfSize, "-----------------------------------------------------------\n");

    i = 0;
    while (logical_data_item_table_core[i].LID != 0xF1F2)
    {
        if (logical_data_item_table_core[i].size == 0 || 
            logical_data_item_table_core[i].total_records == 0)
        {
			i++;
            continue;   /* invalid LID */
        }
        
        nvram_assign_table_entry(logical_data_item_table_core[i].LID, &logical_data_item_table_core[i]);
        i++;
    }

    i = 0;
    while (logical_data_item_table_comm_app[i].LID != 0xF1F2)
    {
        if (logical_data_item_table_comm_app[i].LID == 0 || 
            logical_data_item_table_comm_app[i].size == 0 || 
            logical_data_item_table_comm_app[i].total_records == 0)
        {
			i++;
            continue;   /* invalid LID */
        }
        
        nvram_assign_table_entry(logical_data_item_table_comm_app[i].LID, &logical_data_item_table_comm_app[i]);
        i++;        
    }
    
    for (i = NVRAM_LAST_COMMAPP_LID; i < NVRAM_TOTAL_LID; ++i)
    {
            if (logical_data_item_table_cust[i - NVRAM_LAST_COMMAPP_LID].LID == 0 ||
                logical_data_item_table_cust[i - NVRAM_LAST_COMMAPP_LID].size == 0 ||
                logical_data_item_table_cust[i - NVRAM_LAST_COMMAPP_LID].total_records == 0)
            {
                continue;   /* invalid LID */
            }

            /* Here could be some redundant data items; place to correct position */
            nvram_assign_table_entry(logical_data_item_table_cust[i - NVRAM_LAST_COMMAPP_LID].LID, 
                                     &logical_data_item_table_cust[i - NVRAM_LAST_COMMAPP_LID]);

    }
      
		
	
    fprintf(logOfSize, "-----------------------------------------------------------\n");
    fclose(logOfSize);

}

/*****************************************************************************
* FUNCTION
*  main
* DESCRIPTION
*  main function.
* PARAMETERS
*  None
* RETURNS
*  None
*****************************************************************************/
int main()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	FILE* outputHeader;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	nvram_output_statistic();

	outputHeader=fopen("..\\..\\..\\custom\\common\\nvram_lid_statistics.h","w+");
	fprintf(outputHeader,"/*****************************************************************************\n"\
	"*  Copyright Statement:\n"\
	"*  --------------------\n"\
	"*  This software is protected by Copyright and the information contained\n"\
	"*  herein is confidential. The software may not be copied and the information\n"\
	"*  contained herein may not be used or disclosed except with the written\n"\
	"*  permission of MediaTek Inc. (C) 2006\n"\
	"*\n"\
	"*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES\n"\
	"*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS (\"MEDIATEK SOFTWARE\")\n"\
	"*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON\n"\
	"*  AN \"AS-IS\" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,\n"\
	"*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF\n"\
	"*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.\n"\
	"*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE\n"\
	"*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR\n"\
	"*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH\n"\
	"*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO\n"\
	"*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S\n"\
	"*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.\n"\
	"*\n"\
	"*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE\n"\
	"*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,\n"\
	"*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,\n"\
	"*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO\n"\
	"*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. \n"\
	"*\n"\
	"*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE\n"\
	"*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF\n"\
	"*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND\n"\
	"*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER\n"\
	"*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).\n"\
	"*\n"\
	"*****************************************************************************/\n"\
	"\n"\
	"/*****************************************************************************\n"\
	" *\n"\
	" * Filename:\n"\
	" * ---------\n"\
	" * nvram_lid_statistics.h\n"\
	" *\n"\
	" * Project:\n"\
	" * --------\n"\
	" *   MAUI\n"\
	" *\n"\
	" * Description:\n"\
	" * ------------\n"\
	" *   This file is intends for NVRAM auto gen statistics\n"\
	" *\n"\
	" * Author:\n"\
	" * -------\n"\
	" * Cylen Yao (mtk00911)\n"\
	" *\n"\
	" *============================================================================\n"\
	" *\n"\
	" * This file is automatically generated by the nvram gen procedure\n"\
	" *\n"\
	" ****************************************************************************/\n"\
	"#ifndef NVRAM_LID_STATISTIC_H\n#define NVRAM_LID_STATISTIC_H\n");		

    fprintf(outputHeader,"#ifdef __MTK_TARGET__\n");
    fprintf(outputHeader,"#define NVRAM_SHADOW_SIZE   %d\n", shadow_size_to_verify);
    fprintf(outputHeader,"#else\n");
    fprintf(outputHeader,"#define NVRAM_SHADOW_SIZE   %d + 1024\n", shadow_size_to_verify);            
    fprintf(outputHeader,"#endif /* __MTK_TARGET__ */\n");    
    fprintf(outputHeader,"#define NVRAM_SHADOW_TOTAL  %d\n", shadow_total_to_verify);    
    
    fprintf(outputHeader, "#define NVRAM_OTP_SIZE    %d\n", otp_size_to_verify);
    fprintf(outputHeader, "#define NVRAM_OTP_TOTAL   %d\n", otp_total_to_verify);
	
    fprintf(outputHeader, "#define NVRAM_CUSTPACK_TOTAL  %d\n", custpack_total_to_verify - 1); /* -CustPack verion */
    fprintf(outputHeader,"#define NVRAM_MAX_SHADOW_REC %d /* max_shadow_rec = %d */ \n", ((max_shadow_rec/32) + ((max_shadow_rec%32)?1:0)), max_shadow_rec);
    fprintf(outputHeader,"\n#endif /* NVRAM_LID_STATISTIC_H */\n\n");
    fclose(outputHeader);


}
