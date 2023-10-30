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

#include "io/igtl/detail/converter/MatrixConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/matrix4.hpp>

#include <igtlTransformMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string matrix_converter::IGTL_TYPE          = "TRANSFORM";
const std::string matrix_converter::FWDATA_OBJECT_TYPE = data::matrix4::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::matrix_converter);

matrix_converter::matrix_converter()
= default;

//-----------------------------------------------------------------------------

matrix_converter::~matrix_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer matrix_converter::from_fw_data_object(data::object::csptr _src) const
{
    data::matrix4::csptr src_matrix = std::dynamic_pointer_cast<const data::matrix4>(_src);
    ::igtl::TransformMessage::Pointer msg;
    ::igtl::Matrix4x4 dest;

    msg = ::igtl::TransformMessage::New();
    msg->GetMatrix(dest);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            dest[i][j] = float((*src_matrix)(i, j));
        }
    }

    msg->SetMatrix(dest);
    return {msg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr matrix_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    ::igtl::Matrix4x4 matrix;
    auto* msg                                       = dynamic_cast< ::igtl::TransformMessage*>(_src.GetPointer());
    ::igtl::TransformMessage::Pointer src_transform = ::igtl::TransformMessage::Pointer(msg);
    data::matrix4::sptr dest                        = std::make_shared<data::matrix4>();
    src_transform->GetMatrix(matrix);
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            (*dest)(i, j) = matrix[i][j];
        }
    }

    return dest;
}

//-----------------------------------------------------------------------------

base::sptr matrix_converter::New()
{
    return std::make_shared<matrix_converter>();
}

//-----------------------------------------------------------------------------

std::string const& matrix_converter::get_igtl_type() const
{
    return matrix_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& matrix_converter::get_fw_data_object_type() const
{
    return matrix_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
