#include "main.h"
#include "usart_lib.h"
// Library from example "CAN ATmega32M1" http://www.hpinfotech.ro/cvavr-examples.html | https://forum.digikey.com/t/can-example-atmega32m1-stk600/13039
// but original Atmel Library does not accessible by link http://www.atmel.com/tools/cansoftwarelibrary.aspx
#include "canlib/can_lib.h"

#define blink() {LED_ON();_delay_ms(50);LED_OFF();_delay_ms(50);}
#define long_blink() {LED_ON();_delay_ms(400);LED_OFF();_delay_ms(250);}

#define ODB2_TX_IDE   0x7DF
#define ODB2_RX_IDE   0x7E8
#define ID_TAG_BASE 0x80
#define MY_ID_TAG   0x91

st_cmd_t reply_message;
uint8_t response_data[2];

void init(void) {
    usart0_init(9600);
    usart0_println_sync("Start up firmware. Build 2");
    uint8_t can_init_result = can_init(0);
    usart0_print_sync("Can init result: ");
    usart0_println_sync(can_init_result == 0 ? "research of bit timing configuration failed" : "baudrate performed");

    SETBIT_1(LED_DDR, LED_Pn);
    LED_OFF();
}

void slave(void) {
    blink();

    // Simulate collecting local sensor data: put test bytes in response buffer
    response_data[0] = MY_ID_TAG;
    if(MY_ID_TAG == 0x80) { response_data[1] = 0x3C; }
    if(MY_ID_TAG == 0x81) { response_data[1] = 0x0F; }

    reply_message.pt_data = &response_data[0]; // point message object to first element of data buffer
    reply_message.ctrl.ide = 0; // standard CAN frame type (2.0A)
    reply_message.dlc = 2; // Number of bytes being sent (8 max)
    reply_message.id.std = MY_ID_TAG; // populate ID field with ID Tag
    reply_message.cmd = CMD_TX_DATA; // assign this as a "Standard (2.0A) Reply" message object

    while(can_cmd(&reply_message) != CAN_CMD_ACCEPTED); // wait for MOb to configure

    while(can_get_status(&reply_message) == CAN_STATUS_NOT_COMPLETED); // wait for a transmit request to come in, and send a response

    blink();
    _delay_ms(450);
}

void master(void) {

    st_cmd_t response_msg;
    uint8_t response_buffer[8];
    response_msg.pt_data = &response_buffer[0]; // Point Response MOb to first element of buffer
    response_msg.status = 0; // clear status
    response_msg.id.std = ID_TAG_BASE + 0; // This message object only accepts frames from Target IDs (0x80) to (0x80 + NB_TARGET)
    response_msg.idmsk.std = 0; // This message object only accepts frames from Target IDs (0x80) to (0x80 + NB_TARGET)
    response_msg.ctrl.ide = 0; // This message object accepts only standard (2.0A) CAN frames
    response_msg.ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
    response_msg.dlc = 8; // Number of bytes (8 max) of data to expect
    response_msg.cmd = CMD_RX_DATA_MASKED; // assign this as a "Receive Standard (2.0A) CAN frame" message object

    while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED);

    uint8_t response_status;
    for (uint16_t i = 0; i < 1000 && (response_status = can_get_status(&response_msg)) == CAN_STATUS_NOT_COMPLETED; i++) {
        _delay_ms(1);
    }

    // If response is received
    if (response_status == CAN_STATUS_COMPLETED) {
        usart0_send_sync(response_msg.id.std >> 8 & 0xFF);
        usart0_send_sync(response_msg.id.std & 0xFF);
        usart0_send_sync(response_msg.dlc);
        for (uint8_t i = 0; i < response_msg.dlc; i++) {
            usart0_send_sync(response_msg.pt_data[i]);
        }
        usart0_send_sync(0);
    } else {
        long_blink();
        // If no response is received in 50ms, send abort-frame
        response_msg.cmd = CMD_ABORT;
        while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
    }

//    _delay_ms(450);
}

void pid_request(void) {
    uint8_t response_buffer[8];
    st_cmd_t response_msg;
    response_msg.status = 0;
    response_msg.pt_data = &response_buffer[0];

    uint8_t tx_remote_buffer[8];
    st_cmd_t tx_remote_msg;
    tx_remote_msg.status = 0;
    tx_remote_msg.pt_data = &tx_remote_buffer[0];

    for (uint8_t i = 0; i < 8; i++) { tx_remote_buffer[i] = 0xCC; }
    tx_remote_buffer[0] = 2;
    tx_remote_buffer[1] = 01;
    tx_remote_buffer[2] = 00;

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
    tx_remote_msg.cmd = CMD_TX_REMOTE; // assign this as a "Request Standard (2.0A) Remote Data Frame" message object

    while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction) and send request

    while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED); // Wait for Tx to complete

    uint8_t response_status;
    for (uint16_t i = 0; i < 2000 && (response_status = can_get_status(&response_msg)) == CAN_STATUS_NOT_COMPLETED; i++) {
        _delay_ms(1);
    }

//    _delay_ms(50);
//    uint8_t response_status = can_get_status(&response_msg);

    // If response is received
    if (response_status == CAN_STATUS_COMPLETED) {
        blink();
        usart0_send_sync(response_msg.id.std >> 8);
        usart0_send_sync(response_msg.id.std);
        usart0_send_sync(response_msg.dlc);
        usart0_send_sync(0);
    } else {
        usart0_send_sync(response_status);
        long_blink();
        // If no response is received in 50ms, send abort-frame
        response_msg.cmd = CMD_ABORT;
        while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
    }
}

void loop(void) {
//    slave();
//    master();
    pid_request();
}

int main(void) {
	cli();
	init();
	sei();
	
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
	while(1) {
	    loop();
	}
#pragma clang diagnostic pop
}
