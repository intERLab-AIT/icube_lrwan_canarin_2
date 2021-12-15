/*
 * can_context.c
 *
 *  Created on: Jul 2, 2021
 *      Author: rmukhia
 */
#include "can_context.h"
#include "lora_at.h"

/*!
 * LoRaMAC Structure holding contexts changed status
 * in case of a \ref MLME_NVM_CTXS_UPDATE indication.
 */
typedef union uLoRaMacCtxsUpdateInfo
{
  /*!
   * Byte-access to the bits
   */
  uint8_t Value;
  /*!
   * The according context bit will be set to one
   * if the context changed or 0 otherwise.
   */
  struct sElements
  {
    /*!
     * Mac core nvm context
     */
    uint8_t Mac : 1;
    /*!
     * Region module nvm contexts
     */
    uint8_t Region : 1;
    /*!
     * Crypto module context
     */
    uint8_t Crypto : 1;
    /*!
     * Secure Element driver context
     */
    uint8_t SecureElement : 1;
    /*!
     * MAC commands module context
     */
    uint8_t Commands : 1;
    /*!
     * Class B module context
     */
    uint8_t ClassB : 1;
    /*!
     * Confirm queue module context
     */
    uint8_t ConfirmQueue : 1;
    /*!
     * FCnt Handler module context
     */
    uint8_t FCntHandlerNvmCtx : 1;
  } Elements;
} LoRaMacCtxUpdateStatus_t;


static LoRaMacCtxUpdateStatus_t CtxUpdateStatus = { .Value = 0 };

static void print_data(uint8_t *data, size_t size)
{
  size_t i = 0;
  for (i = 0; i < size - 8; i += 8)
    AT_PPRINTF("%02x%02x%02x%02x%02x%02x%02x%02x",
        data[i], data[i + 1], data [i + 2], data[i + 3],
        data[i + 4], data[i + 5], data [i + 6], data[i + 7]);

  for (; i < size; i++)
    AT_PPRINTF("%02x", data[i]);
}

void CanCtxMgmtEvent(LoRaMacNvmCtxModule_t module)
{
  switch (module)
  {
    case LORAMAC_NVMCTXMODULE_MAC:
    {
      CtxUpdateStatus.Elements.Mac = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_REGION:
    {
      CtxUpdateStatus.Elements.Region = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CRYPTO:
    {
      CtxUpdateStatus.Elements.Crypto = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_SECURE_ELEMENT:
    {
      CtxUpdateStatus.Elements.SecureElement = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_COMMANDS:
    {
      CtxUpdateStatus.Elements.Commands = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CLASS_B:
    {
      CtxUpdateStatus.Elements.ClassB = 1;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE:
    {
      CtxUpdateStatus.Elements.ConfirmQueue = 1;
      break;
    }
    default:
    {
      break;
    }
  }
}


NvmCtxMgmtStatus_t CanCtxMgmtStore(void)
{
  MibRequestConfirm_t mibReq;
  mibReq.Type = MIB_NVM_CTXS;
  if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
  {
    return NVMCTXMGMT_STATUS_FAIL;
  }

  LoRaMacCtxs_t *MacContexts = mibReq.Param.Contexts;

  /* Stop loramac */
  if (LoRaMacStop() != LORAMAC_STATUS_OK)
  {
     return NVMCTXMGMT_STATUS_FAIL;
  }

  if (CtxUpdateStatus.Elements.Crypto == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_CRYPTO, MacContexts->CryptoNvmCtxSize);
    print_data(MacContexts->CryptoNvmCtx, MacContexts->CryptoNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.SecureElement == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_SECURE_ELEMENT, MacContexts->SecureElementNvmCtxSize);
    print_data(MacContexts->SecureElementNvmCtx, MacContexts->SecureElementNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.Mac == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_MAC, MacContexts->MacNvmCtxSize);
    print_data(MacContexts->MacNvmCtx, MacContexts->MacNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.Region == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_REGION, MacContexts->RegionNvmCtxSize);
    print_data(MacContexts->RegionNvmCtx, MacContexts->RegionNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.Commands == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_COMMANDS, MacContexts->CommandsNvmCtxSize);
    print_data(MacContexts->CommandsNvmCtx, MacContexts->CommandsNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.ClassB == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_CLASS_B, MacContexts->ClassBNvmCtxSize);
    print_data(MacContexts->ClassBNvmCtx, MacContexts->ClassBNvmCtxSize);
    AT_PPRINTF("\r\n");
  }

  if (CtxUpdateStatus.Elements.ConfirmQueue == 1)
  {
    AT_PPRINTF("+CTX=%u:%u:", LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE, MacContexts->ConfirmQueueNvmCtxSize);
    print_data(MacContexts->ConfirmQueueNvmCtx, MacContexts->ConfirmQueueNvmCtxSize);
    AT_PPRINTF("\r\n");
  }
  CtxUpdateStatus.Value = 0x00;
  LoRaMacStart();

  return NVMCTXMGMT_STATUS_SUCCESS;
}


NvmCtxMgmtStatus_t CanCtxMgmtRestore(LoRaMacNvmCtxModule_t module, void *data, size_t size)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_NVM_CTXS;
  if (LoRaMacMibGetRequestConfirm(&mibReq) != LORAMAC_STATUS_OK)
  {
    return NVMCTXMGMT_STATUS_FAIL;
  }

  LoRaMacCtxs_t *MacContexts = mibReq.Param.Contexts;
  LoRaMacCtxs_t contexts;

  memcpy1((void *)&contexts, (void *)MacContexts, sizeof(LoRaMacCtxs_t));

  if (LoRaMacStop() != LORAMAC_STATUS_OK)
  {
     return NVMCTXMGMT_STATUS_FAIL;
  }

  switch (module)
  {
    case LORAMAC_NVMCTXMODULE_MAC:
    {
      if (size != MacContexts->MacNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.MacNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_REGION:
    {
      if (size != MacContexts->RegionNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.RegionNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CRYPTO:
    {
      if (size != MacContexts->CryptoNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.CryptoNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_SECURE_ELEMENT:
    {
      if (size != MacContexts->SecureElementNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.SecureElementNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_COMMANDS:
    {
      if (size != MacContexts->CommandsNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.CommandsNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CLASS_B:
    {
      if (size != MacContexts->ClassBNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.ClassBNvmCtx = data;
      break;
    }
    case LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE:
    {
      if (size != MacContexts->ConfirmQueueNvmCtxSize)
      {
        LoRaMacStart();
        return NVMCTXMGMT_STATUS_FAIL;
      }
      contexts.ConfirmQueueNvmCtx = data;
      break;
    }
    default:
    {
      break;
    }
  }

  if (LmHandlerRestoreContext(&contexts) != LORAMAC_HANDLER_SUCCESS)
  {
    LoRaMacStart();
    return NVMCTXMGMT_STATUS_FAIL;
  }

  LoRaMacStart();

  return NVMCTXMGMT_STATUS_SUCCESS;
}

