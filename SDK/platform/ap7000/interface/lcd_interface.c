/*
 * lcd_interface.c
 *
 *  Created on: Dec 2, 2012
 *      Author: XxXx
 */
/**********************************************/
#include <stdbool.h>
#include "lcd_interface.h"
#include "driver/lcd.h"
//#include "clk/clk_lcd.h"
//#include "aintc/aintc_lcd.h"
//#include "pinmux/pin_mux_lcd.h"
#include "api/lcd_def.h"
//#include "api/twi_def.h"
//#include "api/gpio_def.h"
//#include "api/pmic_api.h"
#include "lib/gfx/gfx_util.h"
#include "sys/cache.h"
/**********************************************/
//tDisplay* ScreenRander;
/**********************************************/
/**********************************************/
/*
** Configures raster to display image
*/
bool SetUpLCD(tDisplay* LcdStruct)
{
	//ScreenRander = LcdStruct;
	switch(LcdStruct->LcdType)
	{
		case S035Q01:
			LcdStruct->Width = 320;
			LcdStruct->Height = 240;
			//pmic_backlight_enable(LcdStruct->PmicTwiModuleStruct);
			//LcdStruct->BackLightLevel = 80;
			//pmic_backlight_level(LcdStruct->PmicTwiModuleStruct, LcdStruct->BackLightLevel);
			break;
		case AT070TN92:
			LcdStruct->Width = 800;
			LcdStruct->Height = 480;
			//LcdStruct->BackLight = gpio_assign(LcdStruct->BackLightPort, LcdStruct->BackLightPin, GPIO_DIR_OUTPUT);
			break;
		case TFT43AB_OMAP35x:
			LcdStruct->Width = 480;
			LcdStruct->Height = 272;
			//LcdStruct->BackLight = gpio_assign(LcdStruct->BackLightPort, LcdStruct->BackLightPin, GPIO_DIR_OUTPUT);
			break;
		case VGA:
			LcdStruct->Width = 1024;
			LcdStruct->Height = 768;
			//LcdStruct->BackLight = gpio_assign(LcdStruct->BackLightPort, LcdStruct->BackLightPin, GPIO_DIR_OUTPUT);
			break;
		case LVDS:
			LcdStruct->Width = 800;
			LcdStruct->Height = 600;
			//LcdStruct->BackLight = gpio_assign(LcdStruct->BackLightPort, LcdStruct->BackLightPin, GPIO_DIR_OUTPUT);
			break;
		default:
			return false;
	}
	LcdStruct->sClipRegion.sXMax = LcdStruct->Width;
	LcdStruct->sClipRegion.sYMax = LcdStruct->Height;
	//LcdStruct->DisplayData = memalign(sizeof(LcdStruct->DisplayData[0]) << 3, (LcdStruct->Width * LcdStruct->Height * sizeof(LcdStruct->DisplayData[0])) + 32);
	lcd_init(LcdStruct);
    return true;
}
/**********************************************/
void _lcd_enable()
{
	//RasterEnable(SOC_LCDC_0_REGS);
}
/**********************************************/
void _lcd_disable()
{
    //RasterDisable(SOC_LCDC_0_REGS);
}
//#######################################################################################
void _screen_backlight_on(tDisplay *pDisplay)
{
}
//#######################################################################################
void _screen_backlight_off(tDisplay *pDisplay)
{
}
//#######################################################################################
void _box_cache_clean(tDisplay *pDisplay, signed int x_start, signed int y_start, signed int x_len, signed int y_len)
{
	/*signed int x_end = x_start + x_len ,y_end = y_start + y_len;
	if(x_start >= pDisplay->sClipRegion.sXMax || y_start >= pDisplay->sClipRegion.sYMax || x_end < pDisplay->sClipRegion.sXMin || y_end < pDisplay->sClipRegion.sYMin) return;
	signed int LineCnt = y_start;
	if(LineCnt < pDisplay->sClipRegion.sYMin) LineCnt = pDisplay->sClipRegion.sYMin;
	signed int _x_start = x_start;
	if(_x_start < pDisplay->sClipRegion.sXMin) _x_start = pDisplay->sClipRegion.sXMin;
	signed int _x_end = x_end;
	if(_x_end > pDisplay->sClipRegion.sXMax) _x_end = pDisplay->sClipRegion.sXMax;
	unsigned int width_to_refresh = (_x_end - _x_start)+ 64;
	if((width_to_refresh + _x_start) > pDisplay->sClipRegion.sXMax) width_to_refresh = (pDisplay->sClipRegion.sXMax - _x_start) + 64;
	width_to_refresh *= sizeof(unsigned int);
	volatile unsigned int* ScreenBuff = pDisplay->DisplayData + (pDisplay->Buff_Offset>>3) + _x_start;
	for(; LineCnt < y_end; LineCnt++)
	{
		if(LineCnt >= pDisplay->sClipRegion.sYMax) return;
		CacheDataCleanInvalidateBuff((unsigned int)((unsigned int*)(ScreenBuff + (pDisplay->Width * LineCnt))), width_to_refresh);
	}*/
}
//#######################################################################################
void _put_rectangle(tDisplay *pDisplay, signed int x_start, signed int y_start, signed int x_len, signed int y_len, bool fill, unsigned int color)
{
	signed int x_end = x_start + x_len ,y_end = y_start + y_len;
	if(x_start >= pDisplay->sClipRegion.sXMax || y_start >= pDisplay->sClipRegion.sYMax || x_end < pDisplay->sClipRegion.sXMin || y_end < pDisplay->sClipRegion.sYMin) return;
	signed int LineCnt = y_start;
	//volatile unsigned int* ScreenBuff = pDisplay->DisplayData;
	unsigned int _color = color;
	if(fill)
	{
		if(LineCnt < pDisplay->sClipRegion.sYMin) LineCnt = pDisplay->sClipRegion.sYMin;
		signed int _x_start = x_start;
		if(_x_start < pDisplay->sClipRegion.sXMin) _x_start = pDisplay->sClipRegion.sXMin;
		signed int _x_end = x_end;
		if(_x_end > pDisplay->sClipRegion.sXMax) _x_end = pDisplay->sClipRegion.sXMax;
		unsigned int width_to_refresh = (_x_end - _x_start)+ 64;
		if((width_to_refresh + _x_start) > pDisplay->sClipRegion.sXMax) width_to_refresh = (pDisplay->sClipRegion.sXMax - _x_start) + 64;
		for( ; LineCnt < y_end; LineCnt++)
		{
			if(LineCnt >= pDisplay->sClipRegion.sYMax) return;
			int x = _x_start;
			for( ; x < _x_end ; x++)
			{
				put_pixel(pDisplay, x, LineCnt, _color);
				//ScreenBuff[x + (pDisplay->Width * LineCnt)] = _color;
			}
		}
		return;
	}

	int _x_end = x_end;
	int _x_start = x_start;
	if(_x_end > pDisplay->sClipRegion.sXMax) _x_end = pDisplay->sClipRegion.sXMax;
	if(_x_start < pDisplay->sClipRegion.sXMin) _x_start = pDisplay->sClipRegion.sXMin;
	if(y_start >= pDisplay->sClipRegion.sYMin)
	{
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++)
		{
			put_pixel(pDisplay, LineCnt, y_start, _color);
			//ScreenBuff[LineCnt + (pDisplay->Width * y_start)] = _color;
		}
	}

	if(y_end <= pDisplay->sClipRegion.sYMax)
	{
		for(LineCnt = _x_start ; LineCnt < _x_end ; LineCnt++)
		{
			put_pixel(pDisplay, LineCnt, y_end - 1, _color);
			//ScreenBuff[LineCnt + (pDisplay->Width * (y_end - 1))] = _color;
		}
	}

	int _y_end = y_end;
	if(_y_end > pDisplay->sClipRegion.sYMax) _y_end = pDisplay->sClipRegion.sYMax;
	int _y_start = y_start;
	if(_y_start < pDisplay->sClipRegion.sYMin) _y_start = pDisplay->sClipRegion.sYMin;
	if(x_start >= pDisplay->sClipRegion.sXMin)
	{
		for(LineCnt = _y_start ; LineCnt < _y_end ; LineCnt++)
		{
			put_pixel(pDisplay, x_start, LineCnt, _color);
			//ScreenBuff[x_start + (pDisplay->Width * LineCnt)] = _color;
		}
	}

	if(x_end <= pDisplay->sClipRegion.sXMax)
	{
		for(LineCnt = _y_start ; LineCnt < _y_end ; LineCnt++)
		{
			put_pixel(pDisplay, (x_end - 1), LineCnt, _color);
			//ScreenBuff[(x_end - 1) + (pDisplay->Width * LineCnt)] = _color;
		}
	}

	return;
}
//#######################################################################################
void _put_pixel(tDisplay *pDisplay, signed int X, signed int Y, unsigned int color)
{
	if(X >= pDisplay->sClipRegion.sXMin && Y >= pDisplay->sClipRegion.sYMin && X < pDisplay->sClipRegion.sXMax && Y < pDisplay->sClipRegion.sYMax)
	{
		volatile unsigned char* DisplayData = ((volatile unsigned char*)pDisplay->DisplayData) + ((X + (pDisplay->Width * Y)) * 3);
		*DisplayData++ = color;
		*DisplayData++ = color>>8;
		*DisplayData++ = color>>16;
	}
}
//#######################################################################################
void _screen_put_rgb_array_16(void *_pDisplay, unsigned short *rgb_buffer, unsigned int x1, unsigned int y1,unsigned int width, unsigned int height)
{
	tDisplay *pDisplay = (tDisplay *)_pDisplay;
	///Write rgb array to video memory
	unsigned char *Buff = (unsigned char *)rgb_buffer;
	//unsigned long Length = (width*height)<<1;

	int   x, y;
	volatile unsigned char Tmp1;
	volatile unsigned char Tmp2;
	unsigned int color;
	for(y =y1; y<(height+y1);y++)
	{
		for(x = x1; x < width+x1; x++)
		{
			Tmp2 = *Buff++;
			Tmp1 = *Buff++;
			//color.blue = Tmp1 & 0b11111000;
			//color.green = ((Tmp1<<5) | (Tmp2>>3)) & 0b11111100;
			//color.red = Tmp2<<3;
			color = RGB_TO_UINT((Tmp2<<3) & 0xF8, ((Tmp1<<5) | (Tmp2>>3)) & 0xFC, Tmp1 & 0xF8);
			put_pixel(pDisplay, x, y, color);
		}
		//CacheDataCleanBuff((unsigned int)&pDisplay->DisplayData +  + x1 + (pDisplay->Width * y), width * 4);
	}
	//lcd.dblbuf = dblbuf;
}
//#######################################################################################
void _screen_put_rgb_array_24(void *_pDisplay, unsigned char *rgb_buffer, unsigned long x1, unsigned long y1,unsigned long width, unsigned long height)
{
	tDisplay *pDisplay = (tDisplay *)_pDisplay;
	unsigned char *Buff = rgb_buffer;
	int   y;
	for(y =y1; y<(height+y1);y++)
	{
		if(y < pDisplay->sClipRegion.sYMin || y > pDisplay->sClipRegion.sYMax);
		else
		{
			unsigned char *DisplayStartLine = (unsigned char*)pDisplay->DisplayData + ((x1 + (pDisplay->Width * y)) * 3);
			unsigned char *DisplayEndLine = DisplayStartLine + (width * 3);
			//while(DisplayStartLine < DisplayEndLine)
			//{
				//*DisplayStartLine++= *Buff++;
				//*DisplayStartLine++= *Buff++;
				//*DisplayStartLine++= *Buff++;
				memcpy(DisplayStartLine, Buff, DisplayEndLine - DisplayStartLine);
				Buff+= DisplayEndLine - DisplayStartLine;
			//}
			//CacheDataCleanBuff((unsigned int)&pDisplay->DisplayData + x1 + (pDisplay->Width * y * 4), width * 4);
		}
	}
}
//#######################################################################################
void _screen_put_rgb_array_32(void *_pDisplay, unsigned char *rgb_buffer, unsigned int x1, unsigned int y1,unsigned int width, unsigned int height)
{
	tDisplay *pDisplay = (tDisplay *)_pDisplay;
	int   y;
	int _y = 0;
	unsigned int _width = width;
	if(x1 + _width > pDisplay->sClipRegion.sXMax) _width = pDisplay->sClipRegion.sXMax - x1;
	_width *= 4;
	for(y =y1; y<(height+y1);y++)
	{
		if(y < pDisplay->sClipRegion.sYMin || y > pDisplay->sClipRegion.sYMax);
		else
		{
			unsigned int *DisplayStartLine = (unsigned int *)pDisplay->DisplayData + x1 + (pDisplay->Width * y);
			unsigned char *Buff = rgb_buffer + (width * _y * 4) - 1;
			_y++;
			memcpy((void*)(DisplayStartLine), (void*)(Buff), width * 4);
			//CacheDataCleanBuff((unsigned int)((unsigned int*)(pDisplay->DisplayData + x1 + (pDisplay->Width * y))), _width);
		}
	}
}
//#######################################################################################
void _screen_put_horizontal_line(tDisplay *pDisplay, signed int X1, signed int X2, signed int Y, unsigned char width, unsigned int color)
{

	int X1_Tmp = X1, X2_Tmp = X1 + X2;
	if(X1_Tmp <= (int)pDisplay->sClipRegion.sXMin) X1_Tmp = (int)pDisplay->sClipRegion.sXMin;
	if(X1_Tmp >= (int)pDisplay->sClipRegion.sXMax) X1_Tmp = (int)pDisplay->sClipRegion.sXMax;
	if(X2_Tmp <= (int)pDisplay->sClipRegion.sXMin) X2_Tmp = (int)pDisplay->sClipRegion.sXMin;
	if(X2_Tmp >= (int)pDisplay->sClipRegion.sXMax) X2_Tmp = (int)pDisplay->sClipRegion.sXMax;
	if(Y < (int)pDisplay->sClipRegion.sYMin) Y = (int)pDisplay->sClipRegion.sYMin;
	if(Y >= (int)pDisplay->sClipRegion.sYMax) Y = (int)pDisplay->sClipRegion.sYMax;
	int Half_width1 = (width>>1);
	int Half_width2 = width-Half_width1;
	#ifdef USE_16_BIT_COLOR_DEPTH
	unsigned int _color = color;
	#else
	unsigned int _color = color<<8;
	#endif
	for(;X1_Tmp < X2_Tmp; X1_Tmp++)
	{
		int _Y_ = Y - Half_width1;
		for(; _Y_ < Y + Half_width2; _Y_++) put_pixel(pDisplay,(unsigned int)(X1_Tmp),(unsigned int)(_Y_), _color);
	}
}
//#######################################################################################
void _screen_put_vertical_line(tDisplay *pDisplay, signed int Y1, signed int Y2, signed int X, unsigned char width, unsigned int color)
{

	int Y1_Tmp = Y1, Y2_Tmp = Y1 + Y2;
	if(X <= (int)pDisplay->sClipRegion.sXMin) X = (int)pDisplay->sClipRegion.sXMin;
	if(X >= (int)pDisplay->sClipRegion.sXMax) X = (int)pDisplay->sClipRegion.sXMax;
	if(Y1_Tmp <= (int)pDisplay->sClipRegion.sYMin) Y1_Tmp = (int)pDisplay->sClipRegion.sYMin;
	if(Y1_Tmp >= (int)pDisplay->sClipRegion.sYMax) Y1_Tmp = (int)pDisplay->sClipRegion.sYMax;
	if(Y2_Tmp <= (int)pDisplay->sClipRegion.sYMin) Y2_Tmp = (int)pDisplay->sClipRegion.sYMin;
	if(Y2_Tmp >= (int)pDisplay->sClipRegion.sYMax) Y2_Tmp = (int)pDisplay->sClipRegion.sYMax;
	int Half_width1 = (width>>1);
	int Half_width2 = width-Half_width1;
	#ifdef USE_16_BIT_COLOR_DEPTH
	unsigned int _color = color;
	#else
	unsigned int _color = color<<8;
	#endif
	for(;Y1_Tmp < Y2_Tmp; Y1_Tmp++)
	{
		int _X_ = X - Half_width1;
		for(; _X_ < X + Half_width2; _X_++) put_pixel(pDisplay,(unsigned int)(_X_),(unsigned int)(Y1_Tmp), _color);
	}
}
//#######################################################################################
