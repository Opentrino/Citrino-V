/*
 * citrino.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */
#include "citrino.h"

MODULE_CONSTR(Citrino) {

}

char clk_val = 0;

MODULE_UPDATE(Citrino) {
	DRIVEI_ALL(clk, clk_val);
	clk_val = ~clk_val;
}

MODULE_INITIAL(Citrino) {

}
