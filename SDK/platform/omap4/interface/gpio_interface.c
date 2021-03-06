/*
 * gpio_interface.c
 *
 *  Created on: Jan 13, 2013
 *      Author: XxXx
 */

#include <stdlib.h>
#include "gpio_interface.h"
#include "driver/gpio.h"
#include "clk/clk_gpio.h"
#include "pinmux/pin_mux_gpio.h"
#include "api/gpio_def.h"


/*#####################################################*/
void _gpio_init(unsigned int GpioModuleNr)
{
	unsigned int BaseAddr = 0;
	switch (GpioModuleNr)
	{
	case 0:
		BaseAddr = GPIO0_addr;
		break;
	case 1:
		BaseAddr = GPIO1_addr;
		break;
	case 2:
		BaseAddr = GPIO2_addr;
		break;
	case 3:
		BaseAddr = GPIO3_addr;
		break;
	case 4:
		BaseAddr = GPIO4_addr;
		break;
	case 5:
		BaseAddr = GPIO5_addr;
		break;
	}
	GPIOModuleClkConfig(GpioModuleNr);
    GPIOModuleReset(BaseAddr);
	GPIOModuleEnable(BaseAddr);
}
/*#####################################################*/
new_gpio *_gpio_assign(unsigned int PortNr, unsigned int Pin, unsigned int Direction, bool Multipin)
{
    new_gpio* GpioStruct = new_(new_gpio);
    if(GpioStruct == NULL) return NULL;
	unsigned int BaseAddr = 0;
	switch (PortNr)
	{
	case 0:
		BaseAddr = GPIO0_addr;
		break;
	case 1:
		BaseAddr = GPIO1_addr;
		break;
	case 2:
		BaseAddr = GPIO2_addr;
		break;
	case 3:
		BaseAddr = GPIO3_addr;
		break;
	case 4:
		BaseAddr = GPIO4_addr;
		break;
	case 5:
		BaseAddr = GPIO5_addr;
		break;
	default:
		return NULL;
	}
	GpioStruct->BaseAddr = BaseAddr;
	GpioStruct->Pin = Pin;
	GpioStruct->Direction = Direction;
	GpioStruct->PortNr = PortNr;
	GpioStruct->Multipin = Multipin;
	unsigned char cnt = 0;
	unsigned int tmp = Pin;
	if(GpioStruct->Multipin)
	{
		for(; cnt < 32; cnt++)
		{
			if(tmp & 1)
			{
				gpio_mux_setup(PortNr, cnt, 3);
				GPIODirModeSet(BaseAddr, cnt, Direction);
			}
			tmp = tmp >> 1;
		}
	}
	else
	{
		gpio_mux_setup(PortNr, Pin, 3);
		GPIODirModeSet(BaseAddr, Pin, Direction);
	}
	return GpioStruct;
}
/*#####################################################*/
void _gpio_free(new_gpio *gpio_struct)
{
	if(!gpio_struct) return;
	switch (gpio_struct->BaseAddr)
	{
	case GPIO0_addr:
		gpio_mux_setup(0, gpio_struct->Pin, 0);
		break;
	case GPIO1_addr:
		gpio_mux_setup(1, gpio_struct->Pin, 0);
		break;
	case GPIO2_addr:
		gpio_mux_setup(2, gpio_struct->Pin, 0);
		break;
	case GPIO3_addr:
		gpio_mux_setup(3, gpio_struct->Pin, 0);
		break;
	case GPIO4_addr:
		gpio_mux_setup(4, gpio_struct->Pin, 0);
		break;
	case GPIO5_addr:
		gpio_mux_setup(5, gpio_struct->Pin, 0);
		break;
	}
	free(gpio_struct);
}
/*#####################################################*/
bool _gpio_out(new_gpio *gpio_struct, unsigned int State)
{
	if(!gpio_struct) return false;
	if(gpio_struct->Multipin) GPIOMultiplePinsWrite(gpio_struct->BaseAddr, State & gpio_struct->Pin, (~State) & gpio_struct->Pin);
	else GPIOPinWrite(gpio_struct->BaseAddr, gpio_struct->Pin, State);
	return true;
}
/*#####################################################*/
bool _gpio_direction(new_gpio *gpio_struct, unsigned int Direction)
{
	if(!gpio_struct) return false;
	unsigned char cnt = 0;
	unsigned int tmp = gpio_struct->Pin;
	if(gpio_struct->Multipin)
	{
		for(; cnt < 32; cnt++)
		{
			if(tmp & 1)
			{
				GPIODirModeSet(gpio_struct->BaseAddr, cnt, Direction);
			}
			tmp = tmp >> 1;
		}
	}
	else
	{
		GPIODirModeSet(gpio_struct->BaseAddr, gpio_struct->Pin, Direction);
	}
	return true;
}
/*#####################################################*/
signed int _gpio_in(new_gpio *gpio_struct)
{
	if(!gpio_struct) return -1;
	if(gpio_struct->Multipin) return GPIOMultiplePinsRead(gpio_struct->BaseAddr, gpio_struct->Pin);
	else return GPIOPinRead(gpio_struct->BaseAddr, gpio_struct->Pin)>>gpio_struct->Pin;
}
/*#####################################################*/
bool _gpio_up_dn_enable(new_gpio *gpio_struct, bool enable)
{
	if(!gpio_struct) return false;
	return true;//GPIOUpDnEnable(gpio_struct->BaseAddr, gpio_struct->PinNr, enable);
}
/*#####################################################*/
bool _gpio_up_dn(new_gpio *gpio_struct, unsigned char value)
{
	if(!gpio_struct) return false;
	return true;//GPIOUpDn(gpio_struct->BaseAddr, gpio_struct->PinNr, value);
}
/*#####################################################*/
