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
void _getTime(struct timespec *ts);

int main(int argc, char **argv) {
    struct timespec difference;
    struct timespec startTime;
    struct timespec stopTime;

    _getTime(&startTime);

    // Construct
    channel_construct();
    sender_construct();
    receiver_construct();

    // Handshake
    _handshake();
    sender_checkEncryption();

    // Destruct
    sender_destruct();
    receiver_destruct();

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
    printf("\n# Key Exchange\n");
    printf("--------------\n\n");

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
