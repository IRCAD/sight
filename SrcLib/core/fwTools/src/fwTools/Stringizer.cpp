/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef WIN32
#include <cxxabi.h>
#include <stdlib.h>
#endif

#include "fwTools/Stringizer.hpp"


namespace fwTools
{

template<>
std::string getString(const std::string &aString)
{
	return aString;
}


template<>
std::string getString(const std::type_info &ti)
{
#ifndef WIN32
	char *demangled = abi::__cxa_demangle(ti.name(),0,0,0);
	if (demangled)
	{
		std::string res(demangled);
		free(demangled);
		return res;
	}
	else
	{
		return ti.name();
	}
#else
	return ti.name();
#endif
}

template<>
std::string getString(const TypeInfo &ti)
{
	return getString(ti.type_info());
}


template<>
std::string getString(const signed char &c)
{
	return boost::lexical_cast<std::string>((short)c);
}


template<>
std::string getString(const  char &c)
{
	return boost::lexical_cast<std::string>((short)c);
}

template<>
std::string getString(const unsigned char &c)
{
	return boost::lexical_cast<std::string>((short)c);
}



}
