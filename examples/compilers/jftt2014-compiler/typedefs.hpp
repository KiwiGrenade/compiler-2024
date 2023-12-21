#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <memory>
#include <string>

#define ptr(TYPE) std::shared_ptr<TYPE>
#define new_ptr(TYPE, ...) std::make_shared<TYPE>(__VA_ARGS__)

typedef std::string ident;

typedef unsigned long long int	uint64;
typedef unsigned int		uint32;

typedef long long int		int64;
typedef int			int32;

#endif

