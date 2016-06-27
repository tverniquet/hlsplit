/*
 * Author: Tristan Verniquet
 * Date: June 2016
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*#include <immintrin.h>*/
#include <nmmintrin.h>

#include <inttypes.h>


char outbuf[64*1024];
char *o = outbuf;

static void
flush()
{
   if (write(STDOUT_FILENO, outbuf, o-outbuf) < 0)
     exit(EXIT_FAILURE);
   o = outbuf;
}


static void
add(char *p, size_t cnt)
{
   memcpy(o, p, cnt);
   if ( (o += cnt) > outbuf + 32*1024)
      flush();
}


/*
 * Usage:
 *
 * wc -l mode is argv[1] == "-1"
 * print mode is argv[1] == "0"
 *
 * else print every argv[1] line out of argv[2]
 */
int
main(int argc, char *argv[])
{
   char inbuf[32*1024+64] __attribute__((aligned(32)));
   char resbuf[32] __attribute__((aligned(32)));
   char resout[32*1024];
   ssize_t cnt;
   char *p, *n, *r;
   uint64_t *r2;
   int ind;

   int mod   = argc>2 ? atoi(argv[2]) : 3;
   int state = mod - (argc>1 ? atoi(argv[1]) : 1);
   int mode = state == mod ? 1 : state > mod ? 2 : 0;

   __m128i lf = _mm_set1_epi8('\n');
   __m128i *res = (__m128i *)resbuf;
   __m128i *a;
   int c;
   uint64_t count = 0;

   while ((cnt = read(STDIN_FILENO, inbuf, 32*1024)) > 0) {
      a = (__m128i *)inbuf;
      r = resout;

      if (cnt < 32*1024)
         memset(inbuf + cnt, 0, 32*1024 - cnt);

      while ((char *)a < inbuf + 32*1024) {
         *res = _mm_cmpistrm(*a, lf, _SIDD_CMP_EQUAL_EACH);
         memcpy(r, res, 2);
         r+=2;
         a++;
      }

      /* Now go through and find the newlines */
      n = p = inbuf;
      r2 = (uint64_t *)resout;
      c = 32*1024/8 + 1;
      if (mode == 2) {
         while (--c)
            count += __builtin_popcountl(*r2++);
      }
      else {
         while (--c) {
            while (*r2 != 0) {
	       ind = __builtin_ctzl(*r2);
	       if (mode || ++state == mod) {
	          state = 0;
	          add(n, p + ind + 1 -n);
	       }
	       n = p + ind + 1;
	       *r2 &=~ 1ul<<ind;
	    }
	    p += 64;
	    r2++;
         }
      }
      if ((mode || (state + 1 == mod)) && n < inbuf + cnt)
	 add (n, inbuf + cnt - n);
   }

   if (mode == 2)
      printf("%ld\n", count);
   else
      flush();

   exit (EXIT_SUCCESS);
}
