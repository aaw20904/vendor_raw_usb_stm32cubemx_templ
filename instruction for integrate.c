
/*
There are the next steps to create custom  "vendor" class with CubeMX
0)Create a project, enable midddleware, choose "Custom HID class"
 Stay all the filles, and enable "Custom HID class",  don`t disable a USB calass - because CubeIDE 
 removes all the middleware. The next files will be corrected:
 Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/
 ├── Inc/usbd_customhid.h
 └── Src/usbd_customhid.c

USB_DEVICE/
 └── usbd_desc.c

USB_DEVICE/
 └── usb_device.c   (only one line)

1)Create a project: FS device, endpoint size = 64bytes.
2)Convert HID to Vendor Bulk:
  a)Open "../USB_DEVICE/usbd_desc.c" file (for old middleware),
    or for more modern middleware:
    Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/
└── Src/usbd_customhid.c
  b)Find here a configuration descriptor:*/
  __ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_CfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END = { 
  //other members
  /************** Descriptor of CUSTOM HID interface ****************/ 
0x09,        /* bLength */
0x04,        /* bDescriptorType */
0x00,        /* bInterfaceNumber */
0x00,        /* bAlternateSetting */
0x02,        /* bNumEndpoints */
0x03,        /* bInterfaceClass = HID */
0x00,        /* bInterfaceSubClass */
0x00,        /* bInterfaceProtocol */
0x00,        /* iInterface */
///other members
/******************** Descriptor of Custom HID endpoints ********************/
0x07,        /* bLength */
0x05,        /* bDescriptorType */
CUSTOM_HID_EPOUT_ADDR,
0x03,        /* INTERRUPT */
0x40, 0x00,
0x20,

0x07,
0x05,
CUSTOM_HID_EPIN_ADDR,
0x03,        /* INTERRUPT */
0x40, 0x00,
0x20,
}
/*and change this part to the following (endpoints types, and bInterfaceClass) :*/
  __ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_CfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END = { 
  //other members
  /************** Descriptor of CUSTOM HID interface (MODIFIED)****************/ 
  /* 09 */
  0x09,                                               /* bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
  0x00,                                               /* bInterfaceNumber: Number of Interface */
  0x00,                                               /* bAlternateSetting: Alternate setting */
  0x02,                                               /* bNumEndpoints*/
  0xff,                                               /* bInterfaceClass: VENDOR */
  0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0x00,                                               /* iInterface: Index of string descriptor */
 /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */

  CUSTOM_HID_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
  0x02,                                               /* bmAttributes: Bulk endpoint */
  CUSTOM_HID_EPIN_SIZE,                               /* wMaxPacketSize: 64 Bytes max */
  0x00,
  0x00,                         					   /* bInterval: Polling Interval */
  /* 34 */

  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
  0x02,                                               /* bmAttributes: Bulk endpoint */
  CUSTOM_HID_EPOUT_SIZE,                              /* wMaxPacketSize: 64 Bytes max  */
  0x00,
  0x00,                            /* bInterval: Polling Interval */
}
// c)HID Descriptor block — DO NOT DELETE! Stay AS IS:
/*
-CubeMX expects it
-ST middleware expects it
-Host will ignore it because InterfaceClass = 0xFF
*/
/*
/* HID Descriptor */
0x09,
0x21,
0x11, 0x01,
0x00,
0x01,
0x22,
HID_REPORT_DESC_SIZE, 0x00,
*/
/*In The pgogram should be changed the next parameters:
| Item           | Must be       |
| -------------- | ------------- |
| InterfaceClass | `0xFF`        |
| Endpoint type  | `0x02` (Bulk) |
| IN EP          | `0x81`        |
| OUT EP         | `0x01`        |
| MaxPacket      | 64            |
---------------------------------------------------------------*/
 /*STEP 3 — Convert endpoint behavior from HID → BULK*/
   /*Open the next file: Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c
   Fine here the next function and change Endpoint type:
 */  
static uint8_t USBD_CustomHID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
//find the next functions:
   USBD_LL_OpenEP(pdev,
               CUSTOM_HID_EPIN_ADDR,
               USBD_EP_TYPE_INTR,
               CUSTOM_HID_EPIN_SIZE);

   USBD_LL_OpenEP(pdev,
               CUSTOM_HID_EPOUT_ADDR,
               USBD_EP_TYPE_INTR,
               CUSTOM_HID_EPOUT_SIZE);

//code  
};

static uint8_t USBD_CustomHID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
//find the next functions:
  USBD_LL_OpenEP(pdev,
               CUSTOM_HID_EPIN_ADDR,
               USBD_EP_TYPE_BULK,  ///CHANGE to Bulk
               CUSTOM_HID_EPIN_SIZE); ///! Check the size

USBD_LL_OpenEP(pdev,
               CUSTOM_HID_EPOUT_ADDR,
               USBD_EP_TYPE_BULK,
               CUSTOM_HID_EPOUT_SIZE); //! Check the size?

//code  
};
/**RX buffer arming (do NOT remove), stay this function as is:*/
USBD_LL_PrepareReceive(pdev,
                       CUSTOM_HID_EPOUT_ADDR,
                       hhid->Report_buf,
                       CUSTOM_HID_EPOUT_SIZE);
/*--------------------------------------------------------------------------------
  STEP 4 — Disable HID control logic (EP0)
  Make the device behave as pure Vendor Bulk
  Open it: Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c
 Find this function, and remove logic:
*/
static uint8_t USBD_CustomHID_Setup(USBD_HandleTypeDef *pdev,
                                   USBD_SetupReqTypedef *req)
{
    /* We are no longer HID.
       Ignore all class-specific requests.
       Let USB core handle standard requests. */

    return USBD_OK;
}

/*
Explanation (very important)
Vendor Bulk devices do NOT use class control requests
Returning USBD_OK:
Prevents ST from stalling EP0
Prevents HID state machine from running
Standard requests (SET_ADDRESS, SET_CONFIGURATION) are handled outside this function
If we left HID logic here → random stalls on Windows.
*/

/*
-— Disable EP0 callbacks (safety)
Change the bodies of these functions to following:
**/
static uint8_t USBD_CustomHID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
   //remove all the body
    return USBD_OK;
}

static uint8_t USBD_CustomHID_EP0_TxSent(USBD_HandleTypeDef *pdev)
{
   //remove all the body
    return USBD_OK;
}
/*
— Leave HID descriptor untouched, because:
CubeMX assumes it exists
ST middleware references its size
USB host ignores it because interface ≠ HID

/* HID Descriptor */
0x09, 0x21, ...

*/
   
/*
------------------------------------------------------------
5)Rx callback (OUT)
It is in the  usbd_customhid.c

*/

static uint8_t USBD_CUSTOM_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);
  USBD_CUSTOM_HID_HandleTypeDef *hhid;
  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return USBD_FAIL;
  }
  hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  uint32_t rx_len = USBD_LL_GetRxDataSize(pdev, CUSTOM_HID_EPOUT_ADDR);
  // ---- RAW BULK DATA AVAILABLE HERE ----
  // hhid->Report_buf[0 .. rx_len-1]
  usbSemaphore &= ~(0<<1);    ///user semaphore
  usb_out_callback(hhid->Report_buf, rx_len);   // <-- user extern function (in main.c)
  // ---- CRITICAL: re-arm OUT endpoint ----
  USBD_LL_PrepareReceive(pdev,
                         CUSTOM_HID_EPOUT_ADDR,
                         hhid->Report_buf,
                         CUSTOM_HID_EPOUT_SIZE);
  return USBD_OK;
}
//**************************************************************
//6) The Tx callback (IN). It is in the same file:
static uint8_t USBD_CustomHID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    // Transmission completed
    // You may set a flag here if needed
    return USBD_OK;
}
//***************************************************************
//7)Examples of callback functions - for echo:
void usb_out_callback(uint8_t *data, uint16_t len)
{
    // YOUR REAL LOGIC HERE
    GPIOB->BSRR = GPIO_BSRR_BS10;
    // parse commands, copy to queues, etc.
	memcpy(outUsbData,data,len);
	GPIOB->BSRR = GPIO_BSRR_BR10;
	GPIOB->BSRR = GPIO_BSRR_BS11;
	USBD_LL_Transmit(&hUsbDeviceFS, CUSTOM_HID_EPIN_ADDR, outUsbData,len);
	return;

}

void usb_in_callback(void){
	GPIOB->BSRR = GPIO_BSRR_BR11;
}
