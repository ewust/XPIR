/*
 * File:   lattisigns512-20130329/fastrandombytes.c
 * Author: Gim Güneysu, Tobias Oder, Thomas Pöppelmann, Peter Schwabe
 * Public Domain
 */

#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include <omp.h>
#include <inttypes.h>
#include <iostream>
#include <string.h>

static int init = 0;
static unsigned char key[crypto_stream_salsa20_KEYBYTES];
static unsigned char nonce[crypto_stream_salsa20_NONCEBYTES] = {0};

void fastrandombytes(unsigned char *r, unsigned long long rlen)
{
  unsigned long long n=0;
  int i;
  if(!init)
  {
    randombytes(key, crypto_stream_salsa20_KEYBYTES);
    init = 1;
  }
  //crypto_stream(r,rlen,nonce,key);
  crypto_stream_salsa20(r,rlen,nonce,key);

  // Increase 64-bit counter (nonce)
  for(i=0;i<8;i++)
    n ^= ((unsigned long long)nonce[i]) << 8*i;
  n++;
  for(i=0;i<8;i++)
    nonce[i] = (n >> 8*i) & 0xff;
}

void setrandomseed(uint8_t *seed, size_t len)
{
    if (len > crypto_stream_salsa20_KEYBYTES) {
        len = crypto_stream_salsa20_KEYBYTES;
    }
    printf("Setting seed...\n");

    memset(key, 0, crypto_stream_salsa20_KEYBYTES);
    memcpy(key, seed, len);

    memset(nonce, 0, crypto_stream_salsa20_NONCEBYTES);
    init = 1;
}

/*int main(int argc, char**argv) {
    int randlen = 1024;
    unsigned char t[randlen*sizeof(unsigned long long)];
    double start = omp_get_wtime();
    fastrandombytes((unsigned char *)t, randlen*sizeof(unsigned long long));
    double end= omp_get_wtime();
    std::cout << (end-start) << std::endl;
    for(int i=0;i<1024*sizeof(unsigned long long);i++) std::cout<<std::hex<<(int)t[i]<<" ";
    std::cout<<std::endl;
    
    
}*/
