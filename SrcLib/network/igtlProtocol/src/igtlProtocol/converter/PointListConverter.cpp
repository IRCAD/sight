/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "igtlProtocol/converter/PointListConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"

#include <fwData/PointList.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPointMessage.h>

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
    ::fwData::PointList::csptr srcPoints = ::fwData::PointList::dynamicConstCast(src);

    ::igtl::PointMessage::Pointer dest = ::igtl::PointMessage::New();
    for(::fwData::Point::sptr const &srcPoint :  srcPoints->getPoints())
    {
        std::transform(srcPoint->getCoord().begin(), srcPoint->getCoord().end(), pos,
                       ::boost::numeric_cast<double, float>);
        elem = ::igtl::PointElement::New();
        elem->SetPosition(pos);
        dest->AddPointElement(elem);
    }
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr PointListConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    float igtlPos[3];
    ::igtl::PointElement::Pointer elem;
    std::vector< ::fwData::Point::sptr> fwPoints;
    ::fwData::Point::sptr fwPoint;

    ::igtl::PointMessage* msg               = dynamic_cast< ::igtl::PointMessage* >(src.GetPointer());
    ::igtl::PointMessage::Pointer srcPoints = ::igtl::PointMessage::Pointer(msg);
    ::fwData::PointList::sptr dest          = ::fwData::PointList::New();
    for (int i = 0; i < srcPoints->GetNumberOfPointElement(); ++i)
    {
        fwPoint = ::fwData::Point::New();
        srcPoints->GetPointElement(i, elem);
        elem->GetPosition(igtlPos);
        std::transform(&igtlPos[0], &igtlPos[3], fwPoint->getCoord().begin(),
                       ::boost::numeric_cast<float, double>);
        fwPoints.push_back(fwPoint);
    }
    dest->setPoints(fwPoints);

    return dest;
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
