#include "suble_common.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
//�㲥��ɨ����Ӧ����
#define  DEFAULT_ADV_DATA                                                   \
            {                                                               \
                3,                                                          \
                {                                                           \
                    0x02,                                                   \
                    GAP_AD_TYPE_FLAGS,                                      \
                    GAP_LE_GEN_DISCOVERABLE_FLG | GAP_BR_EDR_NOT_SUPPORTED, \
                },                                                          \
            }
            
#define  DEFAULT_SCAN_RSP                                                   \
            {                                                               \
                6,                                                          \
                {                                                           \
                    0x05,                                                   \
                    GAP_AD_TYPE_COMPLETE_NAME,                              \
                    'D', 'e', 'm', 'o',                                     \
                },                                                          \
            }

//�㲥��ɨ�����
#define  DEFAULT_ADV_PARAM                                                  \
            {                                                               \
                .adv_interval_min = SUBLE_ADV_INTERVAL_MIN,                 \
                .adv_interval_max = SUBLE_ADV_INTERVAL_MAX,                 \
                .adv_type         = GAPM_ADV_UNDIRECT,                      \
                .adv_power        = 0x00,                                   \
                .adv_channal_map  = 0x07,                                   \
            }

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static suble_scan_result_handler_t suble_scan_result_handler;

/*********************************************************************
 * VARIABLE
 */
adv_data_t      g_adv_data      = DEFAULT_ADV_DATA;
adv_data_t      g_scan_rsp      = DEFAULT_SCAN_RSP;
adv_param_t     g_adv_param     = DEFAULT_ADV_PARAM;

//���¹㲥����
volatile bool g_adv_restart_glag = false;

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: �����㲥
*/
void suble_adv_start(void)
{
    APPM_SET_FIELD(ADV_EN, 1);
    appm_scan_adv_con_schedule();
}

/*********************************************************
FN: ֹͣ�㲥
*/
void suble_adv_stop(void)
{
    APPM_SET_FIELD(ADV_EN, 0);
    appm_scan_adv_con_schedule();
}

/*********************************************************
FN: ���¹㲥��ɨ����Ӧ����
*/
void suble_adv_update_advDataAndScanRsp(void)
{
    appm_update_adv_data(g_adv_data.value, g_adv_data.len, g_scan_rsp.value, g_scan_rsp.len);
}

/*********************************************************
FN: ���ù㲥��������� g_adv_param ʹ��
*/
void suble_adv_param_set(void)
{
    if(ke_state_get(TASK_APPM) == APPM_ADVERTISING)
    {
        suble_adv_stop();
        g_adv_restart_glag = true;
    }
}




/*********************************************************  scan  *********************************************************/

/*********************************************************
FN: ����ɨ���¼�
*/
uint32_t g_scan_count = 0;
void suble_scan_evt_handler(struct adv_report const *adv)
{
//    if(adv->adv_addr.addr[0] == 0x49) {
//        elog_hexdump("1", 20, (void*)(&adv->adv_addr.addr[0]), 6);
//    }
//    elog_hexdump("1", 20, (void*)(&adv->adv_addr.addr[0]), 6);
//    g_scan_count++;
//    SUBLE_PRINTF("g_scan_count: %d", g_scan_count);
    
    suble_scan_result_handler(SUBLE_SCAN_EVT_ADV_REPORT, (void*)adv, 0);
}
void suble_scan_timeout_handler(void)
{
    suble_scan_result_handler(SUBLE_SCAN_EVT_SCAN_TIMEOUT, NULL, 0);
}

/*********************************************************
FN: ɨ���ʼ��
*/
void suble_scan_init(void)
{
}

/*********************************************************
FN: ����ɨ��
*/
void suble_scan_start(suble_scan_result_handler_t handler)
{
    g_scan_count = 0;
    suble_scan_result_handler = handler;
    
    APPM_SET_FIELD(SCAN_EN, 1);
    appm_scan_adv_con_schedule();
}

/*********************************************************
FN: ֹͣɨ��
*/
void suble_scan_stop(void)
{
    APPM_SET_FIELD(SCAN_EN, 0);
    appm_scan_adv_con_schedule();
}

/*********************************************************
FN: ����ɨ�����
*/
void suble_scan_param_set(void)
{
}

































