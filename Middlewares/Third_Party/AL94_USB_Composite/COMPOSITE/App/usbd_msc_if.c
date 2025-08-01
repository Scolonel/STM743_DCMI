/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v1.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "sdmmc.h" //  подключенная SD карточка
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t MSC_Storage[32*1024];
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  32*1024/512
#define STORAGE_BLK_SIZ                  512

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata[] = {/* 36 */
  
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA */

/* USER CODE BEGIN PRIVATE_VARIABLES */
HAL_SD_CardInfoTypeDef cardInfo; // <--- Добавлено

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDevice;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init(uint8_t lun);
static int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady(uint8_t lun);
static int8_t STORAGE_IsWriteProtected(uint8_t lun);
static int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init(uint8_t lun)
{
  /* USER CODE BEGIN 2 */
//    LED_START(1);
  return (USBD_OK);
  /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
  //UNUSED(lun);

   //*block_num  = STORAGE_BLK_NBR;
  // *block_size = STORAGE_BLK_SIZ;
     HAL_SD_GetCardInfo(&hsd2, &cardInfo);
  *block_num  = cardInfo.BlockNbr;
  *block_size = cardInfo.BlockSize;
  
   return (USBD_OK);
  
  /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady(uint8_t lun)
{
  /* USER CODE BEGIN 4 */
//    LED_START(0);
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
//  UNUSED(lun);
//  UNUSED(buf);
//  UNUSED(blk_addr);
//  UNUSED(blk_len);
//
//  return (USBD_OK);
    //LED_START(1);
  BusyUSB=150;
  ModeUSB = 3;
  // код из примера
     int8_t ret = -1;

  HAL_SD_ReadBlocks(&hsd2, buf, blk_addr, blk_len, HAL_MAX_DELAY);

  /* Wait until SD card is ready to use for new operation */
  while (HAL_SD_GetCardState(&hsd2) != HAL_SD_CARD_TRANSFER){}
  ret = 0;
   // LED_START(0);

  return ret;

  /* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
//  UNUSED(lun);
//  UNUSED(buf);
//  UNUSED(blk_addr);
//  UNUSED(blk_len);
//
//  return (USBD_OK);
  // код из примера
  //  LED_START(1);
  BusyUSB=150;
  ModeUSB = 3;
   int8_t ret = -1;

   HAL_SD_WriteBlocks(&hsd2, buf, blk_addr, blk_len, HAL_MAX_DELAY);


  /* Wait until SD card is ready to use for new operation */
  while (HAL_SD_GetCardState(&hsd2) != HAL_SD_CARD_TRANSFER){}
  ret = 0;
   // LED_START(0);

  return ret;

  /* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun(void)
{
  /* USER CODE BEGIN 8 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
