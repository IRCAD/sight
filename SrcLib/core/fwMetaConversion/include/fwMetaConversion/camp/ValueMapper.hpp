/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __FWMETACONVERSION_VALUEMAPPER_HPP__
#define __FWMETACONVERSION_VALUEMAPPER_HPP__
#include <camp/valuemapper.hpp>
#include <fwMetaData/Blob.hpp>
#include <fwTools/BufferObject.hpp>

namespace camp_ext
{
template <>
struct ValueMapper< ::fwMetaData::Blob::sptr >
{
    typedef ::fwMetaData::Blob::sptr  ReturnType;
    static const int type = camp::userType;
    static const  ::fwTools::BufferObject::sptr to(const ReturnType& source) {
        return source->getBufferObject();
    }

    static ReturnType from(bool source)                    {CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));}
    static ReturnType from(long source)                    {CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));}
    static ReturnType from(double source)                  {CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));}
    static ReturnType from(const std::string& source)      {CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));}
    static ReturnType from(const camp::EnumObject& source) {CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));}
    static ReturnType from(const camp::UserObject& source)
    {
        ::fwTools::BufferObject::sptr tmp = source.get< ::fwTools::BufferObject::sptr>()->getSptr();
        return ::fwMetaData::Blob::New(tmp);
    }
};

}


#endif /* __FWMETACONVERSION_VALUEMAPPER_HPP__ */
