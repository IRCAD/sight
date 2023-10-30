/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/igtl/detail/converter/PointListConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/point_list.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPointMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail::converter
{

const std::string point_list_converter::IGTL_TYPE          = "POINT";
const std::string point_list_converter::FWDATA_OBJECT_TYPE = data::point_list::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::point_list_converter);

point_list_converter::point_list_converter()
= default;

//-----------------------------------------------------------------------------

point_list_converter::~point_list_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer point_list_converter::from_fw_data_object(data::object::csptr _src) const
{
    std::array<float, 3> pos {};
    ::igtl::PointElement::Pointer elem;
    data::point_list::csptr src_points = std::dynamic_pointer_cast<const data::point_list>(_src);

    ::igtl::PointMessage::Pointer dest = ::igtl::PointMessage::New();
    for(data::point::sptr const& src_point : src_points->get_points())
    {
        std::transform(
            src_point->get_coord().begin(),
            src_point->get_coord().end(),
            pos.begin(),
            boost::numeric_cast<double, float>
        );
        elem = ::igtl::PointElement::New();
        elem->SetPosition(pos.data());
        dest->AddPointElement(elem);
    }

    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr point_list_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    std::array<float, 3> igtl_pos {};
    ::igtl::PointElement::Pointer elem;
    std::vector<data::point::sptr> fw_points;
    data::point::sptr fw_point;

    auto* msg                                = dynamic_cast< ::igtl::PointMessage*>(_src.GetPointer());
    ::igtl::PointMessage::Pointer src_points = ::igtl::PointMessage::Pointer(msg);
    data::point_list::sptr dest              = std::make_shared<data::point_list>();
    for(int i = 0 ; i < src_points->GetNumberOfPointElement() ; ++i)
    {
        fw_point = std::make_shared<data::point>();
        src_points->GetPointElement(i, elem);
        elem->GetPosition(igtl_pos.data());
        std::transform(
            igtl_pos.begin(),
            igtl_pos.end(),
            fw_point->get_coord().begin(),
            boost::numeric_cast<float, double>
        );
        fw_points.push_back(fw_point);
    }

    dest->set_points(fw_points);

    return dest;
}

//-----------------------------------------------------------------------------

base::sptr point_list_converter::New()
{
    return std::make_shared<point_list_converter>();
}

//-----------------------------------------------------------------------------

std::string const& point_list_converter::get_igtl_type() const
{
    return point_list_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& point_list_converter::get_fw_data_object_type() const
{
    return point_list_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
