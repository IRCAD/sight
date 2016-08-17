/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

void LineConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                    ::fwData::Object::sptr& destObj) const throw (::igtlProtocol::exception::Conversion)
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a ::fwData::Line"),
                          destObj->getClassname() != LineConverter::s_FWDATA_OBJECT_TYPE);

    float igtlPos[3];
    float igtlDirection[3];
    ::igtl::PositionMessage::Pointer srcLine;
    ::fwData::Line::sptr dest;

    dest    = ::fwData::Line::dynamicCast(destObj);
    srcLine = ::igtl::PositionMessage::Pointer(dynamic_cast< ::igtl::PositionMessage* >(src.GetPointer()));
    dest->setPosition(::fwData::Point::New());
    dest->setDirection(::fwData::Point::New());
    srcLine->GetPosition(igtlPos);
    srcLine->GetQuaternion(igtlDirection);
    std::transform(&igtlPos[0], &igtlPos[3], dest->getPosition()->getRefCoord().begin(),
                   ::boost::numeric_cast<float, double>);
    std::transform(&igtlDirection[0], &igtlDirection[3], dest->getDirection()->getRefCoord().begin(),
                   ::boost::numeric_cast<float, double>);
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
