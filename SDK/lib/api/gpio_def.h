/*
 *  lib/api/gpio_def.h
 *
 *  Copyright (C) 2013  Iulian Gheorghiu <morgoth.creator@gmail.com>
 *
 *  This file is part of Multiplatform SDK.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GPIO_DEF_H_
#define GPIO_DEF_H_
/*#####################################################*/
#include <stdlib.h>
#include <stdbool.h>
/*#####################################################*/
typedef enum
{
	GPIO_DIR_OUTPUT = 0x0u,
	GPIO_DIR_INPUT = 0x1u
}gpio_dir_enum;
/*#####################################################*/
#define pin_mux_convert_to_pin(port, pin) ((port<<5) + pin)

#define pin_mux_convert_to_port_pin(pin) (pin>>5), (pin - ((pin>>5)<<5))
/*#####################################################*/
typedef struct
{
	unsigned int BaseAddr;
	unsigned int Pin;
	unsigned char PortNr;
	unsigned char Direction;
	bool Multipin;
}Gpio_t;
/*#####################################################*/
#define new_gpio Gpio_t
#ifndef new_
#define new_(structure) (structure*)calloc(1,sizeof(structure));
#endif
#define free_gpio(address) free(address);
/*#####################################################*/
#endif /* GPIO_DEF_H_ */
