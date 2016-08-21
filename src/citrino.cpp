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
	out->drive_all({_0,_1,_X,_Z});
}

MODULE_INITIAL(Citrino) {

}
