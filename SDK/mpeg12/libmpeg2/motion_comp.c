/*
 * motion_comp.c
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../config.h"

#include <inttypes.h>
#include <string.h>

#include "../include/mpeg2.h"
#include "../include/attributes.h"
#include "mpeg2_internal.h"

//#define use_alternativemode


extern mpeg2_mc_t mpeg2_mc_c;
extern mpeg2_mc_t mpeg2_mc_mmx;
extern mpeg2_mc_t mpeg2_mc_mmxext;
extern mpeg2_mc_t mpeg2_mc_3dnow;
extern mpeg2_mc_t mpeg2_mc_altivec;
extern mpeg2_mc_t mpeg2_mc_alpha;
extern mpeg2_mc_t mpeg2_mc_vis;
extern mpeg2_mc_t mpeg2_mc_arm;
extern mpeg2_mc_t mpeg2_mc_neon;


mpeg2_mc_t mpeg2_mc;

void mpeg2_mc_init (uint32_t accel)
{
#ifdef ARCH_X86
    if (accel & MPEG2_ACCEL_X86_MMXEXT)
	mpeg2_mc = mpeg2_mc_mmxext;
    else if (accel & MPEG2_ACCEL_X86_3DNOW)
	mpeg2_mc = mpeg2_mc_3dnow;
    else if (accel & MPEG2_ACCEL_X86_MMX)
	mpeg2_mc = mpeg2_mc_mmx;
    else
#endif
#ifdef ARCH_PPC
    if (accel & MPEG2_ACCEL_PPC_ALTIVEC)
	mpeg2_mc = mpeg2_mc_altivec;
    else
#endif
#ifdef ARCH_ALPHA
    if (accel & MPEG2_ACCEL_ALPHA)
	mpeg2_mc = mpeg2_mc_alpha;
    else
#endif
#ifdef ARCH_SPARC
    if (accel & MPEG2_ACCEL_SPARC_VIS)
	mpeg2_mc = mpeg2_mc_vis;
    else
#endif
#ifdef ARCH_ARM_NEON
    if (accel && MPEG2_ACCEL_NEON)
	mpeg2_mc = mpeg2_mc_neon;
    else
#endif
#ifdef ARCH_ARM
    if (accel & MPEG2_ACCEL_ARM) {
	mpeg2_mc = mpeg2_mc_arm;
    } else
#endif
	mpeg2_mc = mpeg2_mc_c;
}

#define avg2(a,b) ((a+b+1)>>1)
#define avg4(a,b,c,d) ((a+b+c+d+2)>>2)

#define predict_o(i) (ref[i])
#define predict_x(i) (avg2 (ref[i], ref[i+1]))
#define predict_y(i) (avg2 (ref[i], (ref+stride)[i]))
#define predict_xy(i) (avg4 (ref[i], ref[i+1], \
			     (ref+stride)[i], (ref+stride)[i+1]))

#define put(predictor,i) dest[i] = predictor (i)
#define avg(predictor,i) dest[i] = avg2 (predictor (i), dest[i])

// dest = ref

#ifdef AVR32
static void MC_put_o_16_c (uint8_t * dest, const uint8_t * ref,
			      const int stride, int height)
{
    do {
	memcpy (dest, ref, 16);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_put_o_8_c (uint8_t * dest, const uint8_t * ref,
			     const int stride, int height)
{
    do {
	memcpy (dest, ref, 8);
	ref += stride;
	dest += stride;
    } while (--height);
}


static void MC_avg_o_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	do
    	{
    		*_dest = (*_ref++ + *_dest + 1) >>1;
    		_dest++;
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}
static void MC_avg_o_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	do
    	{
    		*_dest = (*_ref++ + *_dest + 1) >>1;
    		_dest++;
    	}while(cnt--);
    ref += stride;
	dest += stride;
    } while (--height);
}/* mc function template */

static void MC_put_x_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	do
    	{
    		*_dest = (*_ref++ + *_ref1++ + 1) >>1;
    		_dest++;
    	}while(cnt--);
		ref += stride;
		dest += stride;
    } while (--height);
}
static void MC_put_x_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
		unsigned char cnt = 8;
		uint8_t *_dest	= dest;
		const uint8_t *_ref	= ref;
		const uint8_t *_ref1	= ref+1;
		do
		{
			*_dest = (*_ref++ + *_ref1++ + 1) >>1;
			_dest++;
		}while(cnt--);
		ref += stride;
		dest += stride;
    } while (--height);
}

static void MC_avg_x_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	uint8_t *_dest1	= dest+1;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	do
    	{
    		*_dest++ = (((((*_ref++ + *_ref1++ +1)>>1))+ *_dest1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}


static void MC_avg_x_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	uint8_t *_dest1	= dest+1;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	do
    	{
    		*_dest++ = (((((*_ref++ + *_ref1++ +1)>>1))+ *_dest1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_put_y_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+stride;
    	do
    	{
        	*_dest++ = ((*_ref++ + *_ref1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_put_y_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+stride;
    	do
    	{
        	*_dest++ = ((*_ref++ + *_ref1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_avg_y_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+stride;
    	do
    	{
        	*_dest = (((((*_ref++ +*_ref1++ +1)>>1))+*_dest +1)>>1);
        	_dest++;
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_avg_y_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+stride;
    	do
    	{
        	*_dest = (((((*_ref++ +*_ref1++ +1)>>1))+*_dest +1)>>1);
        	_dest++;
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_put_xy_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	const uint8_t *_ref2	= ref+stride;
    	const uint8_t *_ref3	= ref+stride+1;
    	do
    	{
        	*_dest++ = (((*_ref++ +*_ref1++ +*_ref2++ +*_ref3++ +2)>>2));
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_put_xy_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	const uint8_t *_ref2	= ref+stride;
    	const uint8_t *_ref3	= ref+stride+1;
    	do
    	{
        	*_dest++ = (((*_ref++ +*_ref1++ +*_ref2++ +*_ref3++ +2)>>2));
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_avg_xy_16_c (uint8_t * dest, const uint8_t * ref,
				   const int stride, int height)
{
    do {
    	unsigned char cnt = 16;
    	uint8_t *_dest	= dest;
    	uint8_t *_dest1	= dest + 1;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	const uint8_t *_ref2	= ref+stride;
    	const uint8_t *_ref3	= ref+stride+1;
    	do
    	{
        	*_dest++ = (((((*_ref++ +*_ref1++ +*_ref2++ +*_ref3++ +2)>>2))+*_dest1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}

static void MC_avg_xy_8_c (uint8_t * dest, const uint8_t * ref,
				  const int stride, int height)
{
    do {
    	unsigned char cnt = 8;
    	uint8_t *_dest	= dest;
    	uint8_t *_dest1	= dest + 1;
    	const uint8_t *_ref	= ref;
    	const uint8_t *_ref1	= ref+1;
    	const uint8_t *_ref2	= ref+stride;
    	const uint8_t *_ref3	= ref+stride+1;
    	do
    	{
        	*_dest++ = (((((*_ref++ +*_ref1++ +*_ref2++ +*_ref3++ +2)>>2))+*_dest1++)>>1);
    	}while(cnt--);
	ref += stride;
	dest += stride;
    } while (--height);
}
#endif

#define MC_FUNC(op,xy)							\
static void MC_##op##_##xy##_16_c (uint8_t * dest, const uint8_t * ref,	\
				   const int stride, int height)	\
{									\
    do {								\
	op (predict_##xy, 0);						\
	op (predict_##xy, 1);						\
	op (predict_##xy, 2);						\
	op (predict_##xy, 3);						\
	op (predict_##xy, 4);						\
	op (predict_##xy, 5);						\
	op (predict_##xy, 6);						\
	op (predict_##xy, 7);						\
	op (predict_##xy, 8);						\
	op (predict_##xy, 9);						\
	op (predict_##xy, 10);						\
	op (predict_##xy, 11);						\
	op (predict_##xy, 12);						\
	op (predict_##xy, 13);						\
	op (predict_##xy, 14);						\
	op (predict_##xy, 15);						\
	ref += stride;							\
	dest += stride;							\
    } while (--height);							\
}									\
static void MC_##op##_##xy##_8_c (uint8_t * dest, const uint8_t * ref,	\
				  const int stride, int height)		\
{									\
    do {								\
	op (predict_##xy, 0);						\
	op (predict_##xy, 1);						\
	op (predict_##xy, 2);						\
	op (predict_##xy, 3);						\
	op (predict_##xy, 4);						\
	op (predict_##xy, 5);						\
	op (predict_##xy, 6);						\
	op (predict_##xy, 7);						\
	ref += stride;							\
	dest += stride;							\
    } while (--height);							\
}

/* definitions of the actual mc functions */
#ifndef AVR32
MC_FUNC (put,o)
MC_FUNC (avg,o)
MC_FUNC (put,x)
MC_FUNC (avg,x)
MC_FUNC (put,y)
MC_FUNC (avg,y)
MC_FUNC (put,xy)
MC_FUNC (avg,xy)
#endif
MPEG2_MC_EXTERN (c)
