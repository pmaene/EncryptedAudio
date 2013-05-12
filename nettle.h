#ifndef __ENC_NETTLE_H__
#define __ENC_NETTLE_H__

#include <stdint.h>
#include <stdlib.h>

#define READ_UINT32(p)        \
(  (((uint32_t) (p)[0]) << 24)      \
 | (((uint32_t) (p)[1]) << 16)      \
 | (((uint32_t) (p)[2]) << 8)     \
 |  ((uint32_t) (p)[3]))

#define WRITE_UINT32(p, i)      \
do {            \
  (p)[0] = ((i) >> 24) & 0xff;      \
  (p)[1] = ((i) >> 16) & 0xff;      \
  (p)[2] = ((i) >> 8) & 0xff;     \
  (p)[3] = (i) & 0xff;        \
} while(0)

#define ROTL32(n,x) (((x)<<(n)) | ((x)>>(32-(n))))

#define MD_INCR(ctx) ((ctx)->count_high += !++(ctx)->count_low)

/* Takes the compression function f as argument. NOTE: also clobbers
   length and data. */
#define MD_UPDATE(ctx, length, data, f, incr)       \
  do {                  \
    if ((ctx)->index)             \
      {                 \
  /* Try to fill partial block */         \
  unsigned __md_left = sizeof((ctx)->block) - (ctx)->index; \
  if ((length) < __md_left)         \
    {               \
      memcpy((ctx)->block + (ctx)->index, (data), (length));  \
      (ctx)->index += (length);         \
      goto __md_done; /* Finished */        \
    }               \
  else                \
    {               \
      memcpy((ctx)->block + (ctx)->index, (data), __md_left); \
                  \
      f((ctx), (ctx)->block);         \
      (incr);             \
                  \
      (data) += __md_left;          \
      (length) -= __md_left;          \
    }               \
      }                 \
    while ((length) >= sizeof((ctx)->block))        \
      {                 \
  f((ctx), (data));           \
  (incr);               \
                  \
  (data) += sizeof((ctx)->block);         \
  (length) -= sizeof((ctx)->block);       \
      }                 \
    memcpy ((ctx)->block, (data), (length));        \
    (ctx)->index = (length);            \
  __md_done:                \
    ;                 \
  } while (0)

/* Pads the block to a block boundary with the bit pattern 1 0*,
   leaving size octets for the length field at the end. If needed,
   compresses the block and starts a new one. */
#define MD_PAD(ctx, size, f)            \
  do {                  \
    unsigned __md_i;              \
    __md_i = (ctx)->index;            \
                  \
    /* Set the first char of padding to 0x80. This is safe since there  \
       is always at least one byte free */        \
                  \
    assert(__md_i < sizeof((ctx)->block));          \
    (ctx)->block[__md_i++] = 0x80;            \
                  \
    if (__md_i > (sizeof((ctx)->block) - 2*sizeof((ctx)->count_low))) \
      { /* No room for length in this block. Process it and   \
     pad with another one */          \
  memset((ctx)->block + __md_i, 0, sizeof((ctx)->block) - __md_i); \
                  \
  f((ctx), (ctx)->block);           \
  __md_i = 0;             \
      }                 \
    memset((ctx)->block + __md_i, 0,          \
     sizeof((ctx)->block) - (size) - __md_i);     \
                      \
  } while (0)

void
_nettle_write_be32(unsigned length, uint8_t *restrict dst,
       uint32_t *restrict src);

#endif
