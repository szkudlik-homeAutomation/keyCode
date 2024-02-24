#pragma once

#include "../global.h"

// TLE8457_serial_lib required macros

/* OUTPUT_QUEUE_SIZE
 * allowed number of frames in output queue
 */

//TODO
#define OUTPUT_QUEUE_SIZE 3

/**
 * MAX_TRANSMIT_DELAY
 *
 * Maximum allowed delay between retransmissions
 */
#define MAX_TRANSMIT_DELAY 100

/**
 * Number of retransmissions in case of collision
 */
#define MAX_NUM_OF_RETRANSMISSIONS 5

/**
 * Number of retransmissions of every transmission
 */
#define NUM_OF_RETRANSMISSIONS 1

/**
 * COMMUNICATION_PAYLOAD_DATA_SIZE
 *
 * number of avaliable payload data in a communication frame
 */
#define COMMUNICATION_PAYLOAD_DATA_SIZE 8

/**
 * FRAME_TRANSMISSION_TIME
 *
 * estimated frame transmission time
 * calculate it based on baud rate and frame size
 */
#define FRAME_TRANSMISSION_TIME 20

#define TRANSMISSION_SPEED 9600
