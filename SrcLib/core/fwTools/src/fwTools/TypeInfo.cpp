/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string.h>

#include "fwTools/TypeInfo.hpp"



namespace fwTools
{

TypeInfo::TypeInfo()
{
    class Nil {};
    pInfo_ = &typeid(Nil);
    assert(pInfo_);
}

TypeInfo::TypeInfo(const std::type_info& ti)
: pInfo_(&ti)
{
    assert(pInfo_);
}

bool TypeInfo::before(const TypeInfo& rhs) const
{
    assert(pInfo_);
    // type_info::before return type is int in some VC libraries
    return pInfo_->before(*rhs.pInfo_) != 0;
}

const std::type_info& TypeInfo::type_info() const
{
    assert(pInfo_);
    return *pInfo_;
}

const char* TypeInfo::name() const
{
    assert(pInfo_);
    return pInfo_->name();
}



TypeInfo &TypeInfo::operator=(const TypeInfo &rhs)
{
    pInfo_ = rhs.pInfo_;
    return *this;
}


// Comparison operators

bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
// type_info::operator== return type is int in some VC libraries
// typeinfo can have different ptr on the same strucure !!!!
//http://gcc.gnu.org/ml/gcc/2002-05/msg02085.html
{
    return strcmp(lhs.type_info().name() , rhs.type_info().name() )== 0;
}


bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs == rhs);
}


bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return strcmp(lhs.type_info().name() , rhs.type_info().name() )<  0;
}



bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return strcmp(lhs.type_info().name() , rhs.type_info().name() )>  0;
}

bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return strcmp(lhs.type_info().name() , rhs.type_info().name() )<= 0;
}

bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return strcmp(lhs.type_info().name() , rhs.type_info().name() )>= 0;
}


} // end namespace fwTools

