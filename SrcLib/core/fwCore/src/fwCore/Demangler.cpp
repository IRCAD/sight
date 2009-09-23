/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */


#include <string>
#include <vector>
#include <typeinfo>

#ifndef _WIN32
#include <cxxabi.h>
#include <cstdlib>
#endif

#include "fwCore/Demangler.hpp"


namespace fwCore 
{

#define COLONS std::string("::")
#define LT std::string("<")


Demangler::Demangler(const std::type_info &t) : m_name(t.name())
{ }


Demangler::Demangler(const std::string &s) : m_name(s)
{ }


Demangler::~Demangler()
{ }


std::string Demangler::getLeafClassname()
{
    std::string demangled(this->demangle());

    size_t lt_pos = demangled.find(LT);
    size_t colons_pos = demangled.rfind(COLONS, lt_pos);

    colons_pos = (colons_pos == std::string::npos)? 0 : colons_pos+COLONS.size();
    return demangled.replace(0,colons_pos,"");
}


std::string Demangler::getClassname()
{
    return getRootedClassname();
}


std::string Demangler::getFullClassname()
{
    std::string demangled(this->demangle());
    return demangled;
}


std::string Demangler::getRootedClassname()
{
    std::string demangled(this->demangle());
    return COLONS + demangled;
}



std::string Demangler::getFullNamespace()
{
    std::string demangled(this->demangle());

    size_t lt_pos = demangled.find(LT);
    size_t colons_pos = demangled.rfind(COLONS, lt_pos);

    if(colons_pos == std::string::npos)
    {
        return "";
    }
    return demangled.replace(colons_pos, std::string::npos,"");
}


std::string Demangler::getRootedNamespace()
{
    return COLONS + getFullNamespace();
}


std::string Demangler::demangle(  )
{
    const char * mangled = m_name.c_str();
#ifndef _WIN32
    char * c_demangled = abi::__cxa_demangle( mangled, 0, 0, 0);
    std::string res(c_demangled);
    free(c_demangled);
    return res;
#else
   static std::vector<std::string> keywords;
   typedef std::vector<std::string>::iterator keyword_iterator;
   if ( 0 == keywords.size() ) 
   {
      keywords.push_back("__cdecl");
      keywords.push_back("class ");
      keywords.push_back("enum ");
      keywords.push_back("struct ");
      keywords.push_back("union ");
   }
   std::string res(mangled);
   for (keyword_iterator iter = keywords.begin(); iter != keywords.end(); ++iter ) 
   {
      while (res.find(*iter) != std::string::npos)
         res = res.replace(res.find(*iter), iter->size(), "");
      while (res.find(" *") != std::string::npos)
         res = res.replace(res.find(" *"), 2, "*");
      while (res.find(" &") != std::string::npos)
         res = res.replace(res.find(" &"), 2, "&");
   }
   return res;
#endif
}

#undef COLONS
#undef LT

} //namespace fwCore
