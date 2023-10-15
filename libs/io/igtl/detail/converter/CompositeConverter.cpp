/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/igtl/detail/converter/CompositeConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/composite.hpp>
#include <data/matrix4.hpp>

#include <igtlTrackingDataMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string CompositeConverter::s_IGTL_TYPE          = "TDATA";
const std::string CompositeConverter::s_FWDATA_OBJECT_TYPE = data::composite::classname();

converterRegisterMacro(io::igtl::detail::converter::CompositeConverter);

CompositeConverter::CompositeConverter()
= default;

//-----------------------------------------------------------------------------

CompositeConverter::~CompositeConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer CompositeConverter::fromFwDataObject(data::object::csptr src) const
{
    ::igtl::TrackingDataMessage::Pointer trackingMsg = ::igtl::TrackingDataMessage::New();

    data::composite::csptr composite = std::dynamic_pointer_cast<const data::composite>(src);
    for(const auto& elt : *composite)
    {
        data::matrix4::csptr transfoMatrix =
            std::dynamic_pointer_cast<const data::matrix4>(elt.second);
        if(transfoMatrix)
        {
            ::igtl::TrackingDataElement::Pointer trackElement = ::igtl::TrackingDataElement::New();
            const std::string name(elt.first);
            trackElement->SetName(name.c_str());
            trackElement->SetType(::igtl::TrackingDataElement::TYPE_TRACKER);

            ::igtl::Matrix4x4 matrix;
            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                for(std::size_t j = 0 ; j < 4 ; ++j)
                {
                    matrix[i][j] = float((*transfoMatrix)(i, j));
                }
            }

            trackElement->SetMatrix(matrix);
            trackingMsg->AddTrackingDataElement(trackElement);
        }
    }

    return {trackingMsg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr CompositeConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::TrackingDataMessage::Pointer trackingMsg;
    trackingMsg = ::igtl::TrackingDataMessage::Pointer(dynamic_cast< ::igtl::TrackingDataMessage*>(src.GetPointer()));
    const int nbTrackingElement = trackingMsg->GetNumberOfTrackingDataElements();

    SIGHT_THROW_EXCEPTION_IF(io::igtl::detail::exception::Conversion("TrackingDataElements"), nbTrackingElement < 0);

    data::composite::sptr composite = std::make_shared<data::composite>();
    for(int i = 0 ; i < nbTrackingElement ; ++i)
    {
        ::igtl::TrackingDataElement::Pointer trackElement = ::igtl::TrackingDataElement::New();
        trackingMsg->GetTrackingDataElement(i, trackElement);
        const std::string name = trackElement->GetName();

        data::matrix4::sptr transfoMatrix = std::make_shared<data::matrix4>();
        (*composite)[name] = transfoMatrix;

        ::igtl::Matrix4x4 matrix;
        trackElement->GetMatrix(matrix);
        for(std::size_t i2 = 0 ; i2 < 4 ; ++i2)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                (*transfoMatrix)(i2, j) = matrix[i2][j];
            }
        }
    }

    return composite;
}

//-----------------------------------------------------------------------------

base::sptr CompositeConverter::New()
{
    return std::make_shared<CompositeConverter>();
}

//-----------------------------------------------------------------------------

std::string const& CompositeConverter::getIgtlType() const
{
    return CompositeConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& CompositeConverter::getFwDataObjectType() const
{
    return CompositeConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
