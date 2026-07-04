#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

// MCU and board
#define CFG_TUSB_MCU                OPT_MCU_STM32F4
#define CFG_TUSB_OS                 OPT_OS_NONE
#define CFG_TUSB_DEBUG              0

// device configuration
#define CFG_TUD_ENABLED             1

// CDC class
#define CFG_TUD_CDC                 1
#define CFG_TUD_CDC_RX_BUFSIZE      256
#define CFG_TUD_CDC_TX_BUFSIZE      256

// not using these
#define CFG_TUD_MSC                 0
#define CFG_TUD_HID                 0
#define CFG_TUD_MIDI                0
#define CFG_TUD_VENDOR              0

// endpoint config
#define CFG_TUD_CDC_EP_NOTIF_SIZE   8

#endif