/*
 * citrino.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */
#include "citrino.h"

MODULE_CONSTR(Citrino) {

}

MODULE_UPDATE(Citrino) {
	wireval_t t = {_0,_Z,_1};
	DRIVE_ALL(out, t);
}

MODULE_INITIAL(Citrino) {

}
