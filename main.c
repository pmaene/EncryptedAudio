#include <stdlib.h>
#include <time.h>

#ifdef __MACH__
    #include <mach/clock.h>
    #include <mach/mach.h>
#endif

#include "protocol.h"

#include "channel.h"
#include "sender.h"
#include "receiver.h"

void _handshake();
void _transmit();
void _getTime(struct timespec *ts);

int main(int argc, char **argv) {
    uint32_t i;

    struct timespec difference;
    struct timespec startTime;
    struct timespec stopTime;

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

    // Construct
    buffer_construct();
    channel_construct();
    sender_construct();
    receiver_construct();

    // Handshake
    _handshake();

    _getTime(&startTime);

    // Transmit
    for (i = 0; i < 1000; i++) {
        buffer_write(dataToEncrypt, ENC_DATA_SIZE_CHARS);
        _transmit();
        receiver_receiveData();
    }

    // Destruct
    receiver_destruct();
    sender_destruct();
    channel_destruct();
    buffer_destruct();

    // Execution Time
    _getTime(&stopTime);

    if ((stopTime.tv_nsec-startTime.tv_nsec) < 0) {
        difference.tv_sec = stopTime.tv_sec-startTime.tv_sec-1;
        difference.tv_nsec = 1000000000+stopTime.tv_nsec-startTime.tv_nsec;
    } else {
        difference.tv_sec = stopTime.tv_sec-startTime.tv_sec;
        difference.tv_nsec = stopTime.tv_nsec-startTime.tv_nsec;
    }

    printf("\n# Execution Time\n");
    printf("%lus %lums\n", difference.tv_sec, difference.tv_nsec/1000000);

    exit(EXIT_SUCCESS);
}

void _handshake() {
    #ifndef __ENC_NO_PRINTS__
        printf("\n# Key Exchange\n");
        printf("--------------\n\n");
    #endif

    // SenderHello
    sender_senderHello();

    // ReceiverHello
    if (ENC_ACCEPT_PACKET != receiver_receiverHello())
        _handshake();

    // SenderAcknowledge
    if (ENC_ACCEPT_PACKET != sender_senderAcknowledge())
        _handshake();
}

void _getTime(struct timespec *time) {
    #ifdef __MACH__
        clock_serv_t clock;
        mach_timespec_t machTime;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &clock);
        clock_get_time(clock, &machTime);
        mach_port_deallocate(mach_task_self(), clock);
        time->tv_sec = machTime.tv_sec;
        time->tv_nsec = machTime.tv_nsec;
    #else
        clock_gettime(CLOCK_REALTIME, time);
    #endif
}

void _transmit() {
    if(sender_sendData() == ENC_COUNTER_WRAPAROUND) {
        _handshake();
        _transmit();
    }
}

