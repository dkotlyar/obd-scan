#include "obd2.h"
#include "main.h"
#include "canlib/can_lib.h"
#include "usart_lib.h"

uint8_t obd2_request_sync(obd2_request_t *req) {
    if (req->timeout == 0) {
        req->timeout = OBD2_DEFAULT_TIMEOUT;
    }

    uint8_t response_buffer[8];
    st_cmd_t response_msg;
    response_msg.status = 0;
    response_msg.pt_data = &response_buffer[0];

    uint8_t tx_remote_buffer[8];
    st_cmd_t tx_remote_msg;
    tx_remote_msg.status = 0;
    tx_remote_msg.pt_data = &tx_remote_buffer[0];

    for (uint8_t i = 0; i < 8; i++) { tx_remote_buffer[i] = 0; }
    tx_remote_buffer[0] = 2;
    tx_remote_buffer[1] = req->service_number;
    tx_remote_buffer[2] = req->pid_code;

    response_msg.id.std = ODB2_RX_IDE; // This message object only accepts frames from Target IDs (0x80) to (0x80 + NB_TARGET)
    response_msg.idmsk.std = 0x7C0; // This message object only accepts frames from Target IDs (0x80) to (0x80 + NB_TARGET)
    response_msg.ctrl.ide = 0; // This message object accepts only standard (2.0A) CAN frames
    response_msg.ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
    response_msg.dlc = 8; // Number of bytes (8 max) of data to expect
    response_msg.cmd = CMD_RX_DATA_MASKED; // assign this as a "Receive Standard (2.0A) CAN frame" message object

    while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction)

    tx_remote_msg.id.std = ODB2_TX_IDE; // This message object only sends frames to Target IDs (0x80) to (0x80 + NB_TARGET)
    tx_remote_msg.ctrl.ide = 0; // This message object sends standard (2.0A) CAN frames
    tx_remote_msg.ctrl.rtr = 1; // This message object is requesting a remote node to transmit data back
    tx_remote_msg.dlc = 8; // Number of data bytes (8 max) requested from remote node
    tx_remote_msg.cmd = CMD_TX_DATA; // assign this as a "Request Standard (2.0A) Remote Data Frame" message object

    while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction) and send request

    while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED); // Wait for Tx to complete

    uint8_t response_status;
    for (uint16_t i = 0; i < req->timeout && (response_status = can_get_status(&response_msg)) == CAN_STATUS_NOT_COMPLETED; i++) {
        _delay_ms(1);
    }

    // If response is received
    if (response_status == CAN_STATUS_NOT_COMPLETED) {
        return OBD2_TIMEOUT;
    } else if (response_status == CAN_STATUS_COMPLETED) {
        uint8_t service_number = response_msg.pt_data[1];
        uint8_t pid_code = response_msg.pt_data[2];
        if (service_number == req->service_number + 0x40 && pid_code == req->pid_code) {
            req->response[0] = response_msg.pt_data[3];
            req->response[1] = response_msg.pt_data[4];
            req->response[2] = response_msg.pt_data[5];
            req->response[3] = response_msg.pt_data[6];
        }
        return OBD2_OK;
    } else {
        // If no response is received in 50ms, send abort-frame
        response_msg.cmd = CMD_ABORT;
        while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
        return OBD2_ERROR;
    }
}

void pid_request(void) {
    obd2_request_t req;
    req.service_number = 1;
    req.pid_code = 0;
    req.status = OBD2_NEW;

    uint8_t res = obd2_request_sync(&req);

    usart0_send_sync(res);
}