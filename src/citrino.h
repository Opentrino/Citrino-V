/*
 * citrino.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef CITRINO_H_
#define CITRINO_H_

#include "module_api.h"

MODULE_DECL(Citrino)
public:
	PORT_NEW(out, PORT_INPUT, PORT_REG, 8, 0);
};

#endif /* CITRINO_H_ */
