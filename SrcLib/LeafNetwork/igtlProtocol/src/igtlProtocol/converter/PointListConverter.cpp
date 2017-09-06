/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/PointListConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/PointList.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtl/igtlPointMessage.h>
#include <algorithm>

namespace igtlProtocol
{
namespace converter
{
const std::string PointListConverter::s_IGTL_TYPE          = "POINT";
const std::string PointListConverter::s_FWDATA_OBJECT_TYPE = ::fwData::PointList::classname();

converterRegisterMacro(::igtlProtocol::converter::PointListConverter);

PointListConverter::PointListConverter()
{
}

//-----------------------------------------------------------------------------

PointListConverter::~PointListConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer PointListConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    float pos[3];
    ::igtl::PointElement::Pointer elem;
    ::igtl::PointMessage::Pointer dest;
    ::fwData::PointList::csptr srcPoints = ::fwData::PointList::dynamicConstCast(src);

    dest = ::igtl::PointMessage::New();
    for(::fwData::Point::sptr const &srcPoint :  srcPoints->getCRefPoints())
    {
        std::transform(srcPoint->getCRefCoord().begin(), srcPoint->getCRefCoord().end(), pos,
                       ::boost::numeric_cast<double, float>);
        elem = ::igtl::PointElement::New();
        elem->SetPosition(pos);
        dest->AddPointElement(elem);
    }
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

void PointListConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                         ::fwData::Object::sptr& destObj) const
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a ::fwData::PointList"),
                          destObj->getClassname() != PointListConverter::s_FWDATA_OBJECT_TYPE);

    float igtlPos[3];
    ::igtl::PointElement::Pointer elem;
    std::vector< ::fwData::Point::sptr> fwPoints;
    ::fwData::Point::sptr fwPoint;
    ::igtl::PointMessage::Pointer srcPoints;
    ::fwData::PointList::sptr dest;

    srcPoints = ::igtl::PointMessage::Pointer(dynamic_cast< ::igtl::PointMessage* >(src.GetPointer()));
    dest      = ::fwData::PointList::dynamicCast(destObj);
    for (int i = 0; i < srcPoints->GetNumberOfPointElement(); ++i)
    {
        fwPoint = ::fwData::Point::New();
        srcPoints->GetPointElement(i, elem);
        elem->GetPosition(igtlPos);
        std::transform(&igtlPos[0], &igtlPos[3], fwPoint->getRefCoord().begin(),
                       ::boost::numeric_cast<float, double>);
        fwPoints.push_back(fwPoint);
    }
    dest->setPoints(fwPoints);
}

//-----------------------------------------------------------------------------

IConverter::sptr PointListConverter::New()
{
    return std::make_shared< PointListConverter >();
}

//-----------------------------------------------------------------------------

std::string const& PointListConverter::getIgtlType() const
{
    return PointListConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& PointListConverter::getFwDataObjectType() const
{
    return PointListConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol}
