#ifndef OBD_SCAN_OBD2_H
#define OBD_SCAN_OBD2_H

#include "main.h"

#define ODB2_TX_IDE   0x7DF
#define ODB2_RX_IDE   0x7E8

#define OBD2_NEW        0
#define OBD2_PENDING    1
#define OBD2_OK         100
#define OBD2_TIMEOUT    254
#define OBD2_ERROR      255

#define OBD2_DEFAULT_TIMEOUT    2000 // in ms

typedef struct {
    uint8_t service_number;
    uint8_t pid_code;

    uint8_t response[4];
    uint8_t status;

    uint16_t timeout;
} obd2_request_t;

uint8_t obd2_request_sync(obd2_request_t *req);
void pid_request(void);

#endif //OBD_SCAN_OBD2_H
