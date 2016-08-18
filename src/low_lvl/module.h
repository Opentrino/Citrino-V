/*
 * module.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <vector>
#include <string>

#define PORT_INIT(PORT_PTR, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL) PORT_PTR = new Port(this, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL)
#define PORT_DECL(PORT_NAME) Port * PORT_NAME
#define PORT_NEW(PORT_NAME, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL) Port * PORT_INIT(PORT_NAME, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL)
#define PORT_CONNECT(PORT_SRC, PORT_DST) PORT_SRC->connect(PORT_DST)

#define GET_WIREVAL(modid, portid) Refresher::get_wireval(modid, portid)

class Port;

class Module {
public:
	std::string name;
	std::string desc;
	std::string author;
	std::vector<Port*> ports;
	uint32_t mod_id;

	Module();
	void addport(Port & port);
	void addport(Port * port);
	virtual ~Module() = 0;
	virtual void update()  = 0; /* The module will be updated when its inputs are also updated */
	virtual void initial() = 0; /* Every module will have atleast one initial function, without counting with the constructor */
};

#endif /* MODULE_H_ */
