/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/StringConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/String.hpp>

#include <igtl/igtlStringMessage.h>

namespace igtlProtocol
{
namespace converter
{
const std::string StringConverter::s_IGTL_TYPE          = "STRING";
const std::string StringConverter::s_FWDATA_OBJECT_TYPE = ::fwData::String::classname();

converterRegisterMacro(::igtlProtocol::converter::StringConverter);

StringConverter::StringConverter()
{
}

//-----------------------------------------------------------------------------

StringConverter::~StringConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer StringConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::igtl::StringMessage::Pointer dest;
    ::fwData::String::csptr srcStr = ::fwData::String::dynamicConstCast(src);

    dest = ::igtl::StringMessage::New();
    dest->SetString(srcStr->getValue().c_str());
    return igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

void StringConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                      ::fwData::Object::sptr& destObj) const throw (::igtlProtocol::exception::
                                                                                    Conversion)
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a ::fwData::String"),
                          destObj->getClassname() != StringConverter::s_FWDATA_OBJECT_TYPE);

    ::igtl::StringMessage::Pointer srcStr;
    ::fwData::String::sptr dest;


    dest   = ::fwData::String::dynamicCast(destObj);
    srcStr = ::igtl::StringMessage::Pointer(dynamic_cast< ::igtl::StringMessage* >(src.GetPointer()));
    dest->setValue(std::string(srcStr->GetString()));
}

//-----------------------------------------------------------------------------

IConverter::sptr StringConverter::New()
{
    return std::make_shared< StringConverter >();
}

//-----------------------------------------------------------------------------

std::string const& StringConverter::getIgtlType() const
{
    return StringConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& StringConverter::getFwDataObjectType() const
{
    return StringConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
