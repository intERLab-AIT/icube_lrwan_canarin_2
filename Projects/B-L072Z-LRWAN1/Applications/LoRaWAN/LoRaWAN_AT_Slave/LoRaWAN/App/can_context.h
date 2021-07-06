/*
 * can_context.h
 *
 *  Created on: Jul 2, 2021
 *      Author: rmukhia
 */

#ifndef APPLICATION_APP_CAN_CONTEXT_H_
#define APPLICATION_APP_CAN_CONTEXT_H_
#include <NvmCtxMgmt.h>

#ifdef __cplusplus
extern "C" {
#endif

void CanCtxMgmtEvent(LoRaMacNvmCtxModule_t module);
NvmCtxMgmtStatus_t CanCtxMgmtStore(void);
NvmCtxMgmtStatus_t CanCtxMgmtRestore(LoRaMacNvmCtxModule_t module, void *data, size_t size);


#ifdef __cplusplus
}
#endif


#endif /* APPLICATION_APP_CAN_CONTEXT_H_ */
