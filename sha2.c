/* sha256.c
 *
 * The sha256 hash function.
 *
 * See http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001 Niels Möller
 *
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02111-1301, USA.
 */

/* Modelled after the sha1.c code by Peter Gutmann. */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nettle.h"
#include "sha2.h"

/* #define Choice(x,y,z) ( ( (x) & (y) ) | ( ~(x) & (z) ) ) */
#define Choice(x,y,z)   ( (z) ^ ( (x) & ( (y) ^ (z) ) ) )
/* #define Majority(x,y,z) ( ((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)) ) */
#define Majority(x,y,z) ( ((x) & (y)) ^ ((z) & ((x) ^ (y))) )

#define S0(x) (ROTL32(30,(x)) ^ ROTL32(19,(x)) ^ ROTL32(10,(x)))
#define S1(x) (ROTL32(26,(x)) ^ ROTL32(21,(x)) ^ ROTL32(7,(x)))

#define s0(x) (ROTL32(25,(x)) ^ ROTL32(14,(x)) ^ ((x) >> 3))
#define s1(x) (ROTL32(15,(x)) ^ ROTL32(13,(x)) ^ ((x) >> 10))

/* The initial expanding function.  The hash function is defined over an
   64-word expanded input array W, where the first 16 are copies of the input
   data, and the remaining 64 are defined by

        W[ t ] = s1(W[t-2]) + W[t-7] + s0(W[i-15]) + W[i-16]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.
*/

#define EXPAND(W,i) \
( W[(i) & 15 ] += (s1(W[((i)-2) & 15]) + W[((i)-7) & 15] + s0(W[((i)-15) & 15])) )

/* The prototype SHA sub-round.  The fundamental sub-round is:

        T1 = h + S1(e) + Choice(e,f,g) + K[t] + W[t]
  T2 = S0(a) + Majority(a,b,c)
  a' = T1+T2
  b' = a
  c' = b
  d' = c
  e' = d + T1
  f' = e
  g' = f
  h' = g

   but this is implemented by unrolling the loop 8 times and renaming
   the variables
   ( h, a, b, c, d, e, f, g ) = ( a, b, c, d, e, f, g, h ) each
   iteration. */

/* It's crucial that DATA is only used once, as that argument will
 * have side effects. */
#define ROUND(a,b,c,d,e,f,g,h,k,data) do {    \
  uint32_t T = h + S1(e) + Choice(e,f,g) + k + data;  \
  d += T;           \
  h = T + S0(a) + Majority(a,b,c);      \
} while (0)

void
_nettle_sha256_compress(uint32_t *restrict state, const uint8_t *restrict input, const uint32_t *restrict k)
{
  uint32_t data[SHA256_DATA_LENGTH];
  uint32_t A, B, C, D, E, F, G, H;     /* Local vars */
  unsigned i;
  uint32_t *d;

  for (i = 0; i < SHA256_DATA_LENGTH; i++, input+= 4)
    {
      data[i] = READ_UINT32(input);
    }

  /* Set up first buffer and local data buffer */
  A = state[0];
  B = state[1];
  C = state[2];
  D = state[3];
  E = state[4];
  F = state[5];
  G = state[6];
  H = state[7];

  /* Heavy mangling */
  /* First 16 subrounds that act on the original data */

  for (i = 0, d = data; i<16; i+=8, k += 8, d+= 8)
    {
      ROUND(A, B, C, D, E, F, G, H, k[0], d[0]);
      ROUND(H, A, B, C, D, E, F, G, k[1], d[1]);
      ROUND(G, H, A, B, C, D, E, F, k[2], d[2]);
      ROUND(F, G, H, A, B, C, D, E, k[3], d[3]);
      ROUND(E, F, G, H, A, B, C, D, k[4], d[4]);
      ROUND(D, E, F, G, H, A, B, C, k[5], d[5]);
      ROUND(C, D, E, F, G, H, A, B, k[6], d[6]);
      ROUND(B, C, D, E, F, G, H, A, k[7], d[7]);
    }

  for (; i<64; i += 16, k+= 16)
    {
      ROUND(A, B, C, D, E, F, G, H, k[ 0], EXPAND(data,  0));
      ROUND(H, A, B, C, D, E, F, G, k[ 1], EXPAND(data,  1));
      ROUND(G, H, A, B, C, D, E, F, k[ 2], EXPAND(data,  2));
      ROUND(F, G, H, A, B, C, D, E, k[ 3], EXPAND(data,  3));
      ROUND(E, F, G, H, A, B, C, D, k[ 4], EXPAND(data,  4));
      ROUND(D, E, F, G, H, A, B, C, k[ 5], EXPAND(data,  5));
      ROUND(C, D, E, F, G, H, A, B, k[ 6], EXPAND(data,  6));
      ROUND(B, C, D, E, F, G, H, A, k[ 7], EXPAND(data,  7));
      ROUND(A, B, C, D, E, F, G, H, k[ 8], EXPAND(data,  8));
      ROUND(H, A, B, C, D, E, F, G, k[ 9], EXPAND(data,  9));
      ROUND(G, H, A, B, C, D, E, F, k[10], EXPAND(data, 10));
      ROUND(F, G, H, A, B, C, D, E, k[11], EXPAND(data, 11));
      ROUND(E, F, G, H, A, B, C, D, k[12], EXPAND(data, 12));
      ROUND(D, E, F, G, H, A, B, C, k[13], EXPAND(data, 13));
      ROUND(C, D, E, F, G, H, A, B, k[14], EXPAND(data, 14));
      ROUND(B, C, D, E, F, G, H, A, k[15], EXPAND(data, 15));
    }

  /* Update state */
  state[0] += A;
  state[1] += B;
  state[2] += C;
  state[3] += D;
  state[4] += E;
  state[5] += F;
  state[6] += G;
  state[7] += H;
}

/* Generated by the shadata program. */
static const uint32_t
K[64] =
{
  0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
  0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
  0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
  0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
  0xe49b69c1UL, 0xefbe4786UL, 0xfc19dc6UL, 0x240ca1ccUL,
  0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
  0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
  0xc6e00bf3UL, 0xd5a79147UL, 0x6ca6351UL, 0x14292967UL,
  0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
  0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
  0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
  0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
  0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
  0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
  0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
  0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};

#define COMPRESS(ctx, data) (_nettle_sha256_compress((ctx)->state, (data), K))

/* Initialize the SHA values */

void
sha256_init(struct sha256_ctx *restrict ctx)
{
  /* Initial values, also generated by the shadata program. */
  static const uint32_t H0[_SHA256_DIGEST_LENGTH] =
  {
    0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
    0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL,
  };

  memcpy(ctx->state, H0, sizeof(H0));

  /* Initialize bit count */
  ctx->count_low = ctx->count_high = 0;

  /* Initialize buffer */
  ctx->index = 0;
}

void
sha256_update(struct sha256_ctx *restrict ctx,
	      unsigned length, const uint8_t *restrict data)
{
  MD_UPDATE (ctx, length, data, COMPRESS, MD_INCR(ctx));
}

static void
sha256_write_digest(struct sha256_ctx *restrict ctx,
		    unsigned length,
		    uint8_t *restrict digest)
{
  uint32_t high, low;

  assert(length <= SHA256_DIGEST_SIZE);

  MD_PAD(ctx, 8, COMPRESS);

  /* There are 512 = 2^9 bits in one block */
  high = (ctx->count_high << 9) | (ctx->count_low >> 23);
  low = (ctx->count_low << 9) | (ctx->index << 3);

  /* This is slightly inefficient, as the numbers are converted to
     big-endian format, and will be converted back by the compression
     function. It's probably not worth the effort to fix this. */
  WRITE_UINT32(ctx->block + (SHA256_DATA_SIZE - 8), high);
  WRITE_UINT32(ctx->block + (SHA256_DATA_SIZE - 4), low);
  COMPRESS(ctx, ctx->block);

  _nettle_write_be32(length, digest, ctx->state);
}

void
sha256_digest(struct sha256_ctx *restrict ctx,
	      unsigned length,
	      uint8_t *restrict digest)
{
  sha256_write_digest(ctx, length, digest);
  sha256_init(ctx);
}
