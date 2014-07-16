/*  
*   Byte-oriented AES-256 implementation.
*   All lookup tables replaced with 'on the fly' calculations. 
*
*   Copyright (c) 2007-2009 Ilya O. Levin, http://www.literatecode.com
*   Other contributors: Hal Finney
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef __ARDUINOAES256_H__
#define __ARDUINOAES256_H__
#include <Arduino.h>
#include <string.h>

class AES256
{
public:
    AES256() {}
    ~AES256() { done(); }
    void init(uint8_t * /* key */);
    void done();
    void encrypt_ecb(uint8_t * /* plaintext */);
    void decrypt_ecb(uint8_t * /* cipertext */);
    inline static uint8_t blockSize() { return 16; }
private:
  AES256(const AES256&);
  AES256& operator=(const AES256&);
  uint8_t ctx_key[32];
  uint8_t ctx_enckey[32];
  uint8_t ctx_deckey[32];
};

template<class T> 
class CTR_Mode
{
public:
  CTR_Mode(T* bc) : m_index(0), m_blockCipher(bc) 
  { 
    m_nounce=(uint8_t*)malloc(T::blockSize());
    m_pad=(uint8_t*)malloc(T::blockSize());
  }
  
  void init(uint8_t *nounce)
  {
    memcpy(m_nounce,nounce,T::blockSize());
    memcpy(m_pad,m_nounce,T::blockSize());
    m_blockCipher->encrypt_ecb(m_pad);
    m_index=0;
  }
  
  ~CTR_Mode()
  {
    done();
  }
  void done()
  {
    memset(m_nounce,0,T::blockSize());
    memset(m_pad,0,T::blockSize());
    free(m_nounce);
    free(m_pad);
  }
  
  void encrypt_ctr(uint8_t *buf, uint8_t size)
  {
    for(uint8_t i=0;i<size;++i)
    {
      if (m_index >= T::blockSize())
      {
        for (uint8_t n=T::blockSize();n;--n)
        {
          if (++m_nounce[n-1]) break;
        }
        memcpy(m_pad,m_nounce,T::blockSize());
        m_blockCipher->encrypt_ecb(m_pad);
        m_index = 0;
      }
      buf[i]^=m_pad[m_index++];
    }
  }
  
  void decrypt_ctr(uint8_t *buf, uint8_t size)
  {
    encrypt_ctr(buf,size);
  }
private:
  uint8_t *m_nounce;
  uint8_t *m_pad;
  uint8_t  m_index;
  T       *m_blockCipher;
};
#endif //__ARDUINOAES256_H__