/*
 * Copyright (C) 2019 Siara Logics (cc)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author Arundale R.
 *
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shox96_0_2_0.h"

typedef unsigned char byte;

unsigned int  c_95[95] = {16384, 16256, 15744, 16192, 15328, 15344, 15360, 16064, 15264, 15296, 15712, 15200, 14976, 15040, 14848, 15104, 14528, 14592, 14656, 14688, 14720, 14752, 14784, 14816, 14832, 14464, 15552, 15488, 15616, 15168, 15680, 16000, 15872, 10752,  8576,  8192,  8320,  9728,  8672,  8608,  8384, 11264,  9024,  8992, 12160,  8544, 11520, 11008,  8512,  9008, 12032, 11776, 10240,  8448,  8960,  8640,  9040,  8688,  9048, 15840, 16288, 15856, 16128, 16224, 16368, 40960,  6144,     0,  2048, 24576,  7680,  6656,  3072, 49152, 13312, 12800, 63488,  5632, 53248, 45056,  5120, 13056, 61440, 57344, 32768,  4096, 12288,  7168, 13568,  7936, 13696, 15776, 16320, 15808, 16352};
unsigned char l_95[95] = {    3,    11,    11,    11,    12,    12,     9,    10,    11,    11,    11,    11,    10,    10,     9,    10,    10,    10,    11,    11,    11,    11,    11,    12,    12,    10,    10,    10,    10,    11,    11,    10,     9,     8,    11,     9,    10,     7,    12,    11,    10,     8,    12,    12,     9,    11,     8,     8,    11,    12,     9,     8,     7,    10,    11,    11,    13,    12,    13,    12,    11,    12,    10,    11,    12,     4,     7,     5,     6,     3,     8,     7,     6,     4,     8,     8,     5,     7,     4,     4,     7,     8,     5,     4,     3,     6,     7,     7,     9,     8,     9,    11,    11,    11,    12};
//unsigned char c[]    = {  ' ',   '!',   '"',   '#',   '$',   '%',   '&',  '\'',   '(',   ')',   '*',   '+',   ',',   '-',   '.',   '/',   '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',   ':',   ';',   '<',   '=',   '>',   '?',   '@',   'A',   'B',   'C',   'D',   'E',   'F',   'G',   'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',   'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W',   'X',   'Y',   'Z',   '[',  '\\',   ']',   '^',   '_',   '`',   'a',   'b',   'c',   'd',   'e',   'f',   'g',   'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',   'p',   'q',   'r',   's',   't',   'u',   'v',   'w',   'x',   'y',   'z',   '{',   '|',   '}',   '~'};
char *SET2_STR = "9012345678.,-/=+ ()$%&;:<>*\"{}[]@?'^#_!\\|~`";

enum {SHX_STATE_1 = 1, SHX_STATE_2};

unsigned int mask[] = {0x8000, 0xC000, 0xE000, 0xF000, 0xF800, 0xFC00, 0xFE00, 0xFF00};
long append_bits(char *out, long ol, unsigned int code, int clen, byte state) {

   byte cur_bit;
   byte blen;
   unsigned char a_byte;

   if (state == SHX_STATE_2) {
      // remove change state prefix
      if ((code >> 9) == 0x1C) {
         code <<= 7;
         clen -= 7;
      }
      //if (code == 14272 && clen == 10) {
      //   code = 9084;
      //   clen = 14;
      //}
   }
   while (clen > 0) {
     cur_bit = ol % 8;
     blen = (clen > 8 ? 8 : clen);
     a_byte = (code & mask[blen - 1]) >> 8;
     a_byte >>= cur_bit;
     if (blen + cur_bit > 8)
        blen = (8 - cur_bit);
     if (cur_bit == 0)
        out[ol / 8] = a_byte;
     else
        out[ol / 8] |= a_byte;
     code <<= blen;
     ol += blen;
     clen -= blen;
   }
   return ol;
}

// Context is NULL for encoding single strings. Context is used only
// for encoding large files too big to fit in memory
long shox96_0_2_0_compress(const char *in, long len, char *out, C_CONTEXT0_2 *c_ctx) {

  char *ptr;
  byte bits;
  byte state;

  long l, ll, ol;
  char c_in, c_next;
  byte is_upper, is_all_upper;

  ol = 0;
  if (c_ctx == NULL) {
     state = SHX_STATE_1;
     is_all_upper = 0;
  } else {
     state = c_ctx->state;
     is_all_upper = c_ctx->is_all_upper;
     if (c_ctx->term_rem_bits > 0) {
        ol = append_bits(out, ol, 14272<<(10-c_ctx->term_rem_bits), c_ctx->term_rem_bits, 1);
     }
  }
  for (l=0; l<len; l++) {
     c_in = in[l];
     c_next = 0;
     if (state == SHX_STATE_2) {
        if (c_in == ' ' && len-1>l)
           ptr = (char *) memchr(SET2_STR, in[l+1], 42);
        else
           ptr = (char *) memchr(SET2_STR, c_in, 42);
        if (ptr == NULL) {
           state = SHX_STATE_1;
           ol = append_bits(out, ol, 8192, 4, 1);
        }
     }
     is_upper = 0;
     if (c_in >= 'A' && c_in <= 'Z')
        is_upper = 1;
     else {
        if (is_all_upper) {
           is_all_upper = 0;
           ol = append_bits(out, ol, 8192, 4, state);
        }
     }
     if (is_upper && !is_all_upper) {
        for (ll=l+5; ll>=l && ll<len; ll--) {
            if (in[ll] >= 'a' && in[ll] <= 'z')
               break;
        }
        if (ll == l-1) {
           ol = append_bits(out, ol, 8704, 8, state);
           is_all_upper = 1;
        }
     }
     if (state == SHX_STATE_1 && c_in >= '0' && c_in <= '9') {
        ol = append_bits(out, ol, 14336, 7, state);
        state = SHX_STATE_2;
     }
     c_next = 0;
     if (l+1 < len)
        c_next = in[l+1];

     if (c_in >= 32 && c_in <= 126) {
        c_in -= 32;
        if (is_all_upper && is_upper)
           c_in += 32;
        if (c_in == 0 && state == SHX_STATE_2)
           ol = append_bits(out, ol, 15232, 11, state);
        else
           ol = append_bits(out, ol, c_95[c_in], l_95[c_in], state);
     } else
     if (c_in == 13 && c_next == 10) {
        ol = append_bits(out, ol, 13824, 9, state);
        l++;
     } else
     if (c_in == 10) {
        ol = append_bits(out, ol, 13952, 9, state);
     } else
     if (c_in == 13) {
        ol = append_bits(out, ol, 9064, 13, state);
     } else
     if (c_in == '\t') {
        ol = append_bits(out, ol, 9216, 7, state);
     }
  }
  bits = ol%8;
  if (bits) {
     ol = append_bits(out, ol, 14272, 8 - bits, 1);
  }
  if (c_ctx != NULL) {
     c_ctx->state = state;
     c_ctx->is_all_upper = is_all_upper;
     c_ctx->term_rem_bits = 0;
     if (bits)
        c_ctx->term_rem_bits = (10-(8-bits));
  }
  //printf("\n%ld\n", ol);
  return ol/8+(ol%8?1:0);

}

// Decoder is designed for using less memory, not speed
// Decode lookup table for code index and length
// First 2 bits 00, Next 3 bits indicate index of code from 0, 
// last 3 bits indicate code length in bits
//                0,            1,            2,            3,            4, 
char vcode[32] = {2 + (0 << 3), 3 + (3 << 3), 3 + (1 << 3), 4 + (6 << 3), 0, 
//                5,            6,            7,            8, 9, 10
                  4 + (4 << 3), 3 + (2 << 3), 4 + (8 << 3), 0, 0,  0,
//                11,          12, 13,            14, 15
                  4 + (7 << 3), 0,  4 + (5 << 3),  0,  5 + (9 << 3),
//                16, 17, 18, 19, 20, 21, 22, 23
                   0,  0,  0,  0,  0,  0,  0,  0,
//                24, 25, 26, 27, 28, 29, 30, 31
                   0, 0,  0,  0,  0,  0,  0,  5 + (10 << 3)};
//                0,            1,            2, 3,            4, 5, 6, 7,
char hcode[32] = {1 + (1 << 3), 2 + (0 << 3), 0, 3 + (2 << 3), 0, 0, 0, 5 + (3 << 3), 
//                8, 9, 10, 11, 12, 13, 14, 15,
                  0, 0,  0,  0,  0,  0,  0,  5 + (5 << 3),
//                16, 17, 18, 19, 20, 21, 22, 23
                   0, 0,  0,  0,  0,  0,  0,  5 + (4 << 3),
//                24, 25, 26, 27, 28, 29, 30, 31
                   0, 0,  0,  0,  0,  0,  0,  5 + (6 << 3)};

enum {SHX_SET1 = 0, SHX_SET1A, SHX_SET1B, SHX_SET2, SHX_SET3, SHX_SET4, SHX_SET4A};
char *sets[] = {"  etaoinsrl", "cdhupmbgwfy", "vkqjxz     ", " 9012345678", ".,-/=+ ()$%", "&;:<>*\"{}[]", "@?'^#_!\\|~`"};

int getBitVal(const char *in, long bit_no, int count) {
   return (in[bit_no >> 3] & (0x80 >> (bit_no % 8)) ? 1 << count : 0);
}

int getCodeIdx(char *code_type, const char *in, long len, long *bit_no_p) {
  int code = 0;
  int count = 0;
  do {
    if (*bit_no_p >= len)
      return 199;
    code += getBitVal(in, *bit_no_p, count);
    (*bit_no_p)++;
    count++;
    if (code_type[code] && 
        (code_type[code] & 0x07) == count) {
      return code_type[code] >> 3;
    }
  } while (count < 5);
  return 1; // skip if code not found
}

// Context is NULL for decoding single strings. Context is used only
// for decoding large files too big to fit in memory
long shox96_0_2_0_decompress(const char *in, long len, char *out, D_CONTEXT0_2 *d_ctx) {

  int dstate;
  long bit_no;
  byte is_all_upper;
  long ol = 0;
  if (d_ctx == NULL) {
    bit_no = 0;
    dstate = SHX_SET1;
    is_all_upper = 0;
  } else {
    bit_no = d_ctx->bit_no;
    dstate = d_ctx->dstate;
    is_all_upper = d_ctx->is_all_upper;
  }

  len <<= 3;
  out[ol] = 0;
  while (bit_no < len) {
    int h, v;
    char c;
    byte is_upper = is_all_upper;
    long orig_bit_no = bit_no;
    v = getCodeIdx(vcode, in, len, &bit_no);
    if (v == 199) {
      bit_no = orig_bit_no;
      break;
    }
    h = dstate;
    if (v == 0) {
      h = getCodeIdx(hcode, in, len, &bit_no);
      if (h == 199) {
        bit_no = orig_bit_no;
        break;
      }
      if (h == SHX_SET1) {
         if (dstate == SHX_SET1) {
           if (is_all_upper) {
             is_upper = is_all_upper = 0;
             continue;
           }
           v = getCodeIdx(vcode, in, len, &bit_no);
           if (v == 199) {
             bit_no = orig_bit_no;
             break;
           }
           if (v == 0) {
              h = getCodeIdx(hcode, in, len, &bit_no);
              if (h == 199) {
                bit_no = orig_bit_no;
                break;
              }
              if (h == SHX_SET1) {
                 is_all_upper = 1;
                 continue;
              }
           }
           is_upper = 1;
         } else {
            dstate = SHX_SET1;
            continue;
         }
      } else
      if (h == SHX_SET2) {
         if (dstate == SHX_SET1)
           dstate = SHX_SET2;
         continue;
      }
      if (h != SHX_SET1) {
        v = getCodeIdx(vcode, in, len, &bit_no);
        if (v == 199) {
          bit_no = orig_bit_no;
          break;
        }
      }
    }
    if (h < 64 && v < 32)
      c = sets[h][v];
    if (c >= 'a' && c <= 'z') {
      if (is_upper)
        c -= 32;
    } else {
      if (is_upper && dstate == SHX_SET1 && v == 1)
        c = '\t';
      if (h == SHX_SET1B) {
         switch (v) {
           case 6:
             out[ol++] = '\r';
             c = '\n';
             break;
           case 7:
             c = is_upper ? '\r' : '\n';
             break;
           case 10:
             continue;
         }
      }
    }
    out[ol++] = c;
  }

  if (d_ctx != NULL) {
    d_ctx->dstate = dstate;
    d_ctx->bit_no = bit_no % 8;
    d_ctx->is_all_upper = is_all_upper;
    d_ctx->remaining_byte_count = 0;
    if (len - bit_no)
      d_ctx->remaining_byte_count = (bit_no % 8 ? 1 : 0) + (len - bit_no) / 8;
  }

  return ol;

}
