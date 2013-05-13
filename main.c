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
void transmitdata();

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

    // Initializations
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

    transmitdata();

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

void transmitdata() {
	size_t i;

    unsigned char dataToEncrypt[ENC_DATA_SIZE_CHARS] =
        "\x01\x02\x03\x04\xf6\x8d\x59\xaa\xc1\x93\x67\xc7\xde\x23\x4b"
        "\xe8\xc3\xb9\x20\xb8\x36\x2d\x21\xf5\x3e\x3c\x6b\xc8\x4e\xaa"
        "\x5c\x54\x8d\x84\x88\x73\x3a\xc3\x27\x8b\xcf\x66\xe6\x35\xbe"
        "\xbd\x20\x73\x4b\x4b\xea\xaa\x48\x46\xf1\xbc\xf4\x8d\xfa\x0d"
        "\x91\xa3\xbb\xd2\x97\x27\x21\x84\x25\x66\x78\x03\x6d\xd5\x66"
        "\x80\xc5\xa1\x63\x6b\x25\x36\xd7\xe2\x3e\xf6\x6a\xac\x1d\x62"
        "\xb7\xdb\x3c\x6f\xbe\x05\x75\x1b\x1b\x64\x2f\x7c\x1a\xba\x7c"
        "\x07\x4f\x48\x8e\x34\x7b\xf4\xd7\xff\x25\x5f\x2d\x13\x4d\x87"
        "\x4b\x06\x54\x19\x04\x03\x02\x01";

    // Transmit
    for (i = 0; i < 1; i++) {
        buffer_write(dataToEncrypt, ENC_DATA_SIZE_CHARS);
        _transmit();
        receiver_receiveData();
    }
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
