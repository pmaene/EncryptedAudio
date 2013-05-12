#include <stdlib.h>
#include <time.h>

#include "channel.h"
#include "handshake.h"
#include "protocol.h"
#include "receiver.h"
#include "sender.h"

#include "wavpcm_io.h"
#include "globals.h"
#include "codec.h"
#include "encode.h"
#include "decode.h"
#include "functions.h"

void _handshake();
void _transmit();

enum state handshakeState;

int main(int argc, char **argv) {
	/*
	size_t bufPos;
	size_t read;

	short buffer[BUFFERSIZE];
	short encoded[BUFFERSIZE];

	struct wavpcm_input input;
	struct wavpcm_output output;

	struct decode_chunk_struct decode_chunk_left;
	struct decode_chunk_struct decode_chunk_right;
	struct encode_chunk_struct encode_chunk_left;
	struct encode_chunk_struct encode_chunk_right;
	*/

    // Initialize RNG
    srand(time(NULL));

    
    _convFromOctets();

    // Construct
    buffer_construct();
    channel_construct();
    sender_construct();
    receiver_construct();

    // Handshake
	#ifndef __ENC_NO_PRINTS__
		printf("\n# Key Exchange\n");
		printf("--------------\n\n");
	#endif

    handshakeState = SENDER_HELLO;
    while (HANDSHAKE_FINISHED != handshakeState)
        _handshake();

    /*
    // Transmit
	memset(&input, 0, sizeof(struct wavpcm_input));
	input.resource = INPUTWAVFILE;
	memset(&output, 0, sizeof(struct wavpcm_output));
	output.resource = OUTPUTWAVFILE;

	memset(&encode_chunk_left, 0, sizeof(struct encode_chunk_struct));
	memset(&encode_chunk_right, 0, sizeof(struct encode_chunk_struct));
	memset(&decode_chunk_left, 0, sizeof(struct decode_chunk_struct));
	memset(&decode_chunk_right, 0, sizeof(struct decode_chunk_struct));

	for (bufPos = 0; bufPos < 4; bufPos++) {
		encode_chunk_left.Qstep[bufPos] = QSTART;
		encode_chunk_right.Qstep[bufPos] = QSTART;
		decode_chunk_left.Qstep[bufPos] = QSTART;
		decode_chunk_right.Qstep[bufPos] = QSTART;
	}

	wavpcm_input_open(&input);
	wavpcm_output_copy_settings(&input, &output);
	wavpcm_output_open(&output);

	for (bufPos = 0; bufPos < input.samplesAvailable ; bufPos += (BUFFERSIZE/2)) {
		read = wavpcm_input_read(&input, buffer);
		encode(buffer, &encode_chunk_left, &encode_chunk_right, encoded);

		while (buffer_isModified()) {}
		buffer_write((field_t *) buffer, BUFFERSIZE*sizeof(short));

		_transmit();
		receiver_receiveData();

		buffer_read((field_t *) buffer, BUFFERSIZE*sizeof(short));

		decode(&decode_chunk_left, &decode_chunk_right, encoded, buffer);
		wavpcm_output_write(&output, buffer, read);
	}

	wavpcm_output_close(&output);
	*/

    exit(EXIT_SUCCESS);
}

void _handshake() {
    switch (handshakeState) {
        case SENDER_HELLO:
            sender_senderHello();
            handshakeState = RECEIVER_HELLO;
            break;
        case RECEIVER_HELLO:
            if (ENC_ACCEPT_PACKET == receiver_receiverHello())
                handshakeState = SENDER_ACKNOWLEDGE;
            break;
        case SENDER_ACKNOWLEDGE:
            if (ENC_ACCEPT_PACKET == sender_senderAcknowledge())
                handshakeState = RECEIVER_CHECK_ACKNOWLEDGE;
            break;
        case RECEIVER_CHECK_ACKNOWLEDGE:
            if (ENC_ACCEPT_PACKET == receiver_checkSenderAcknowledge())
                handshakeState = HANDSHAKE_FINISHED;
            break;
        case HANDSHAKE_FINISHED:
            break;
    }
}

void _transmit() {
    if(sender_sendData() == ENC_COUNTER_WRAPAROUND) {
        handshakeState = SENDER_HELLO;
        while (HANDSHAKE_FINISHED != handshakeState)
            _handshake();

        _transmit();
    }
}
