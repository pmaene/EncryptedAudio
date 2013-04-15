#include "nettle.h"

void
_nettle_write_be32(unsigned length, uint8_t *dst,
       uint32_t *src)
{
  unsigned i;
  unsigned words;
  unsigned leftover;

  words = length / 4;
  leftover = length % 4;

  for (i = 0; i < words; i++, dst += 4)
    WRITE_UINT32(dst, src[i]);

  if (leftover)
    {
      uint32_t word;
      unsigned j = leftover;

      word = src[i];

      switch (leftover)
  {
  default:
    abort();
  case 3:
    dst[--j] = (word >> 8) & 0xff;
    /* Fall through */
  case 2:
    dst[--j] = (word >> 16) & 0xff;
    /* Fall through */
  case 1:
    dst[--j] = (word >> 24) & 0xff;
  }
    }
}