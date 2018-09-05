/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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
    ::fwData::String::csptr srcStr = ::fwData::String::dynamicConstCast(src);

    ::igtl::StringMessage::Pointer dest = ::igtl::StringMessage::New();
    dest->SetString(srcStr->getValue().c_str());
    return igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr StringConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::fwData::String::sptr dest = ::fwData::String::New();

    ::igtl::StringMessage* msg            = dynamic_cast< ::igtl::StringMessage* >(src.GetPointer());
    ::igtl::StringMessage::Pointer srcStr = ::igtl::StringMessage::Pointer(msg);
    dest->setValue(std::string(srcStr->GetString()));

    return dest;
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
