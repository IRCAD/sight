/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwAtoms/Blob.hpp>

#include <fwMemory/BufferObject.hpp>

#include <camp/valuemapper.hpp>

namespace camp_ext
{

/// New value mapper to manage conversion in camp world between ::fwAtoms::Blob and a ::fwMemory::BufferObject
template <>
struct ValueMapper< ::fwAtoms::Blob::sptr >
{
    typedef ::fwAtoms::Blob::sptr ReturnType;

    static const int type = camp::userType;

    //------------------------------------------------------------------------------

    static const ::fwMemory::BufferObject::sptr to(const ReturnType& source)
    {
        return source->getBufferObject();
    }

    //------------------------------------------------------------------------------

    static ReturnType from(bool source)
    {
        CAMP_ERROR(camp::BadType(camp::boolType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(long source)
    {
        CAMP_ERROR(camp::BadType(camp::intType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(double source)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const std::string& source)
    {
        CAMP_ERROR(camp::BadType(camp::realType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::EnumObject& source)
    {
        CAMP_ERROR(camp::BadType(camp::enumType, camp::mapType<ReturnType>()));
    }

    //------------------------------------------------------------------------------

    static ReturnType from(const camp::UserObject& source)
    {
        ::fwMemory::BufferObject::sptr tmp = source.get< ::fwMemory::BufferObject::sptr>()->getSptr();
        return ::fwAtoms::Blob::New(tmp);
    }
};

}
