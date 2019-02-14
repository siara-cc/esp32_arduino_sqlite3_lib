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
#ifndef shox96_0_2_0
#define shox96_0_2_0

// Compressor / Decompressor context is used when
// several blocks in a file are coded
// It is passed as null for coding a single string
typedef struct {
  char term_rem_bits;
  char state;
  char is_all_upper;
} C_CONTEXT0_2;

typedef struct {
  char dstate;
  char bit_no;
  char is_all_upper;
  char remaining_byte_count;
} D_CONTEXT0_2;

extern long shox96_0_2_0_compress(const char *in, long len, char *out, C_CONTEXT0_2 *c_ctx);
extern long shox96_0_2_0_decompress(const char *in, long len, char *out, D_CONTEXT0_2 *d_ctx);
#endif

