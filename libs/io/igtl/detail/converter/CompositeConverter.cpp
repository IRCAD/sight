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

const std::string composite_converter::IGTL_TYPE          = "TDATA";
const std::string composite_converter::FWDATA_OBJECT_TYPE = data::composite::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::composite_converter);

composite_converter::composite_converter()
= default;

//-----------------------------------------------------------------------------

composite_converter::~composite_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer composite_converter::from_fw_data_object(data::object::csptr _src) const
{
    ::igtl::TrackingDataMessage::Pointer tracking_msg = ::igtl::TrackingDataMessage::New();

    data::composite::csptr composite = std::dynamic_pointer_cast<const data::composite>(_src);
    for(const auto& elt : *composite)
    {
        data::matrix4::csptr transfo_matrix =
            std::dynamic_pointer_cast<const data::matrix4>(elt.second);
        if(transfo_matrix)
        {
            ::igtl::TrackingDataElement::Pointer track_element = ::igtl::TrackingDataElement::New();
            const std::string name(elt.first);
            track_element->SetName(name.c_str());
            track_element->SetType(::igtl::TrackingDataElement::TYPE_TRACKER);

            ::igtl::Matrix4x4 matrix;
            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                for(std::size_t j = 0 ; j < 4 ; ++j)
                {
                    matrix[i][j] = float((*transfo_matrix)(i, j));
                }
            }

            track_element->SetMatrix(matrix);
            tracking_msg->AddTrackingDataElement(track_element);
        }
    }

    return {tracking_msg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr composite_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    ::igtl::TrackingDataMessage::Pointer tracking_msg;
    tracking_msg = ::igtl::TrackingDataMessage::Pointer(dynamic_cast< ::igtl::TrackingDataMessage*>(_src.GetPointer()));
    const int nb_tracking_element = tracking_msg->GetNumberOfTrackingDataElements();

    SIGHT_THROW_EXCEPTION_IF(io::igtl::detail::exception::conversion("TrackingDataElements"), nb_tracking_element < 0);

    data::composite::sptr composite = std::make_shared<data::composite>();
    for(int i = 0 ; i < nb_tracking_element ; ++i)
    {
        ::igtl::TrackingDataElement::Pointer track_element = ::igtl::TrackingDataElement::New();
        tracking_msg->GetTrackingDataElement(i, track_element);
        const std::string name = track_element->GetName();

        data::matrix4::sptr transfo_matrix = std::make_shared<data::matrix4>();
        (*composite)[name] = transfo_matrix;

        ::igtl::Matrix4x4 matrix;
        track_element->GetMatrix(matrix);
        for(std::size_t i2 = 0 ; i2 < 4 ; ++i2)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                (*transfo_matrix)(i2, j) = matrix[i2][j];
            }
        }
    }

    return composite;
}

//-----------------------------------------------------------------------------

base::sptr composite_converter::New()
{
    return std::make_shared<composite_converter>();
}

//-----------------------------------------------------------------------------

std::string const& composite_converter::get_igtl_type() const
{
    return composite_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& composite_converter::get_fw_data_object_type() const
{
    return composite_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
