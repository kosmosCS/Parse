/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbcore.h
 * Purpose: USB Core Definitions
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __USBCORE_H__
#define __USBCORE_H__


/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA {
  volatile uint8_t  *pData;
  volatile uint16_t Count;
} USB_EP_DATA;

/* USB Core Global Variables */
extern volatile uint16_t USB_DeviceStatus;
extern volatile uint8_t  USB_DeviceAddress;
extern volatile uint8_t  USB_Configuration;
extern volatile uint32_t USB_EndPointMask;
extern volatile uint32_t USB_EndPointHalt;
extern volatile uint32_t USB_EndPointStall;
extern volatile uint8_t  USB_AltSetting[USB_IF_NUM];

/* USB Endpoint 0 Buffer */
//extern uint8_t  EP0Buf[USB_MAX_PACKET0];
union _uEP0
{
uint8_t   EP0Buf[USB_MAX_PACKET0];
uint16_t  EP0Buf16[USB_MAX_PACKET0/2];
uint32_t  EP0Buf32[USB_MAX_PACKET0/4];
};

extern union _uEP0 uEP0 __attribute__((aligned(4)));

/* USB Endpoint 0 Data Info */
extern USB_EP_DATA EP0Data;

/* USB Setup Packet */
extern USB_SETUP_PACKET SetupPacket;

/* USB Core Functions */
extern void USB_ResetCore (void);



#endif  /* __USBCORE_H__ */
