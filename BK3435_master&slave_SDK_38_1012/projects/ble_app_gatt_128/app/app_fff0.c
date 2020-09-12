#include "rwip_config.h"     // SW configuration
#include <string.h>
#include "app_fff0.h"              // Battery Application Module Definitions
#include "appm.h"                    // Application Definitions
#include "appm_task.h"             // application task definitions
#include "appc.h"                    // Application Definitions
#include "appc_task.h"             // application task definitions
#include "fff0s_task.h"           // health thermometer functions
#include "co_bt.h"
#include "prf_types.h"             // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "fff0s.h"
#include "ke_timer.h"
#include "uart.h"




/*********************************************************************
 * LOCAL CONSTANTS
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION
 */

/*********************************************************************
 * VARIABLES
 */
/// fff0 Application Module Environment Structure
struct app_fff0_env_tag app_fff0_env;




/*********************************************************
FN: 
*/
void app_fff0_init(void)
{
    // Reset the environment
    memset(&app_fff0_env, 0, sizeof(struct app_fff0_env_tag));
}

/*********************************************************
FN: 
*/
void app_fff0_add_fff0s(void)
{
   struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APPM,
                                                  gapm_profile_task_add_cmd, sizeof(struct fff0s_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD; // main_road 51
    req->sec_lvl =   0;
    req->prf_task_id = TASK_ID_FFF0S;
    req->app_task = TASK_APPM;
    req->start_hdl = 0; //req->start_hdl = 0; dynamically allocated

   //����ɾ����ɾ������ʹ��notify
   struct fff0s_db_cfg *db_cfg;
    // Set parameters
    db_cfg = (struct fff0s_db_cfg* ) req->param;
    // Sending of notifications is supported
    db_cfg->features = FFF0_FFF1_LVL_NTF_SUP;
   
    // Send the message
    ke_msg_send(req);
}

/*********************************************************
FN: ��Ϊ�ӻ� notify
*/
void app_fff1_send_lvl(uint8_t* buf, uint8_t len)
{
    // Allocate the message
    struct fff0s_fff1_level_upd_req * req = KE_MSG_ALLOC(FFF0S_FFF1_LEVEL_UPD_REQ,
                                                        prf_get_task_from_id(TASK_ID_FFF0S),
                                                        TASK_APPM,
                                                        fff0s_fff1_level_upd_req);
    // Fill in the parameter structure	
    req->length = len;
	memcpy(req->fff1_level, buf, len);

    // Send the message
    ke_msg_send(req);
}

/*********************************************************
FN: ʹ��/ʧ�� notify
*/
static int fff0s_fff1_level_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
                                               struct fff0s_fff1_level_ntf_cfg_ind const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
//	UART_PRINTF("param->ntf_cfg = %x\r\n", param->ntf_cfg);
	if(param->ntf_cfg == PRF_CLI_STOP_NTFIND) {
        //�ر� notify
	} else {
        //��notify
	}
    return (KE_MSG_CONSUMED);
}

/*********************************************************
FN: 
*/
static int fff1_level_upd_handler(ke_msg_id_t const msgid,
                                      struct fff0s_fff1_level_upd_rsp const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	if(param->status == GAP_ERR_NO_ERROR)
	{
		//uint8_t buf[128];
		//memset(buf, 0xcc, 128);
		//app_fff1_send_lvl(buf, 128);
	}
	
    return (KE_MSG_CONSUMED);
}

/*********************************************************
FN: Ĭ�ϴ���
PM: - msgid     Id of the message received.
    - param     Pointer to the parameters of the message.
    - dest_id   ID of the receiving task instance (TASK_GAP).
    - src_id    ID of the sending task instance.
RT: If the message was consumed or not.
*/
static int app_fff0_msg_dflt_handler(ke_msg_id_t const msgid,
                                     void const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
	UART_PRINTF("%s\r\n", __func__);
    // Drop the message
    return (KE_MSG_CONSUMED);
}

/*********************************************************
FN: FFF2 ����д���ӻ����յ�����
*/
static int fff2_writer_req_handler(ke_msg_id_t const msgid,
                                     struct fff0s_fff2_writer_ind *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
//    SUBLE_HEXDUMP("rx from master", param->fff2_value, param->length);
    suble_svc_receive_data(param->fff2_value, param->length);
    
    return (KE_MSG_CONSUMED);
}




/*********************************************************************
 * LOCAL VARIABLE DEFINITIONS
 */
/// Default State handlers definition
const struct ke_msg_handler app_fff0_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_fff0_msg_dflt_handler},
    {FFF0S_FFF1_LEVEL_NTF_CFG_IND,  (ke_msg_func_t)fff0s_fff1_level_ntf_cfg_ind_handler},
    {FFF0S_FFF1_LEVEL_UPD_RSP,      (ke_msg_func_t)fff1_level_upd_handler},
    {FFF0S_FFF2_WRITER_REQ_IND,		(ke_msg_func_t)fff2_writer_req_handler},
};

const struct ke_state_handler app_fff0_table_handler =
    {&app_fff0_msg_handler_list[0], (sizeof(app_fff0_msg_handler_list)/sizeof(struct ke_msg_handler))};






