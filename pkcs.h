#ifndef NETTLE_PKCS_H_INCLUDED
#define NETTLE_PKCS_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

/* This file provides the prefixes to generate a PKCS #1 1.5
 * DigestInfo structure for various algorithms. By appending
 * the hash value to that prefix the DigestInfo structure
 * is formed.
 */

const uint8_t sha256_prefix[19];
const size_t sha256_prefix_size;

const uint8_t sha1_prefix[15];
const size_t sha1_prefix_size;

#endif /* NETTLE_PKCS_H_INCLUDED */
