/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/LineConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"

#include <fwData/Line.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtl/igtlPositionMessage.h>

#include <algorithm>

namespace igtlProtocol
{
namespace converter
{
const std::string LineConverter::s_IGTL_TYPE          = "POSITION";
const std::string LineConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Line::classname();

converterRegisterMacro(::igtlProtocol::converter::LineConverter);

LineConverter::LineConverter()
{
}

//-----------------------------------------------------------------------------

LineConverter::~LineConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer LineConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    float pos[3];
    float direction[3];
    ::igtl::PositionMessage::Pointer dest;
    ::fwData::Line::csptr srcLine = ::fwData::Line::dynamicConstCast(src);

    dest = ::igtl::PositionMessage::New();
    std::transform(srcLine->getPosition()->getCRefCoord().begin(),
                   srcLine->getPosition()->getCRefCoord().end(), &pos[0],
                   ::boost::numeric_cast<double, float>);
    std::transform(srcLine->getDirection()->getCRefCoord().begin(),
                   srcLine->getDirection()->getCRefCoord().end(), &direction[0],
                   ::boost::numeric_cast<double, float>);
    dest->SetPosition(pos);
    dest->SetQuaternion(direction);
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr LineConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    float igtlPos[3];
    float igtlDirection[3];

    ::fwData::Line::sptr dest                = ::fwData::Line::New();
    ::igtl::PositionMessage*  msg            = dynamic_cast< ::igtl::PositionMessage* >(src.GetPointer());
    ::igtl::PositionMessage::Pointer srcLine = ::igtl::PositionMessage::Pointer(msg);
    dest->setPosition(::fwData::Point::New());
    dest->setDirection(::fwData::Point::New());
    srcLine->GetPosition(igtlPos);
    srcLine->GetQuaternion(igtlDirection);
    std::transform(&igtlPos[0], &igtlPos[3], dest->getPosition()->getRefCoord().begin(),
                   ::boost::numeric_cast<float, double>);
    std::transform(&igtlDirection[0], &igtlDirection[3], dest->getDirection()->getRefCoord().begin(),
                   ::boost::numeric_cast<float, double>);

    return dest;
}

//-----------------------------------------------------------------------------

IConverter::sptr LineConverter::New()
{
    return std::make_shared< LineConverter >();
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::getIgtlType() const
{
    return LineConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& LineConverter::getFwDataObjectType() const
{
    return LineConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
