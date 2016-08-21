/*
 * memory.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "memory.h"

MODULE_CONSTR(MainMemory) {

}

MODULE_UPDATE(MainMemory) {
	out->drive_all({_0,_1});
}

MODULE_INITIAL(MainMemory) {

}
