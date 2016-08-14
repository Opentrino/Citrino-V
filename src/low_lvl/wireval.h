/*
 * wireval.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef LOW_LVL_WIREVAL_H_
#define LOW_LVL_WIREVAL_H_

#include "port.h"

inline uint64_t wireval_un(std::vector<WireVal> wireval, size_t sz) {
	uint64_t ret = 0;
	for(size_t i = 0, j = wireval.size() - 1; i < sz && i < wireval.size(); i++, j--)
		ret |= (wireval[j] == _1 ? 1 : 0) << i;
	return ret;
}

inline int64_t wireval_sn(std::vector<WireVal> wireval, size_t sz) {
	uint64_t ret = 0;
	for(size_t i = 0, j = wireval.size() - 1; i < sz && i < wireval.size(); i++, j--)
		ret |= (wireval[j] == _1 ? 1 : 0) << i;
	return ret;
}

inline uint64_t wireval_u64(std::vector<WireVal> wireval) {
	return wireval_un(wireval, 64);
}

inline uint32_t wireval_u32(std::vector<WireVal> wireval) {
	return wireval_un(wireval, 32);
}

inline uint16_t wireval_u16(std::vector<WireVal> wireval) {
	return wireval_un(wireval, 16);
}

inline uint8_t wireval_u8(std::vector<WireVal> wireval) {
	return wireval_un(wireval, 8);
}

inline int64_t wireval_s64(std::vector<WireVal> wireval) {
	return wireval_sn(wireval, 64);
}

inline int32_t wireval_s32(std::vector<WireVal> wireval) {
	return wireval_sn(wireval, 32);
}

inline int16_t wireval_s16(std::vector<WireVal> wireval) {
	return wireval_sn(wireval, 16);
}

inline int8_t wireval_s8(std::vector<WireVal> wireval) {
	return wireval_sn(wireval, 8);
}

template<typename T>
inline std::vector<WireVal> to_wireval(T val) {
	std::vector<WireVal> ret;
	int sz = sizeof(T) * 8;
	for(int i = sz - 1; i >= 0; i--)
		ret.push_back((val & (1 << i)) ? _1 : _0);
	return ret;
}

inline std::string wireval_to_str(std::vector<WireVal> val) {
	std::string ret;
	for(auto wire : val)
		ret += wire == _0 ? "0" : wire == _1 ? "1" : wire == _X ? "X" : "Z";
	return ret;
}

inline void print_wireval(std::vector<WireVal> val) {
	std::cout<<wireval_to_str(val);
}

#endif /* LOW_LVL_WIREVAL_H_ */
