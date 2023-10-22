/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#pragma once

#include <data/image.hpp>

#include "io/itk/config.hpp"
#include <itkImageIOBase.h>

namespace sight::io::itk
{

/**
 * @brief Allocate a new data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so data::image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * data::image does not manage the image buffer.
 */
template<class ITKIMAGE>
data::image::sptr move_from_itk(typename ITKIMAGE::Pointer _itk_image, bool _buffer_manager_is_data_image = true);

/**
 * @brief Update a data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so data::image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * data::image does not manage the image buffer.
 */
template<class ITKIMAGE>
void move_from_itk(
    typename ITKIMAGE::Pointer _itk_image,
    data::image::sptr _data_image,
    bool _buffer_manager_is_data_image = true
);

/**
 * @brief Create an ITK image from an data::image.
 *
 * If bufferManagerIsDataImage = true, imageData conserve his buffer management
 * and so the created itkImage not manages it. If bufferManagerIsDataImage = false,
 * the created itkImage manage his image buffer and thus imageData releases his
 * buffer (in this case, imageData must be the owner of this buffer).
 *
 * @pre an assertion check if ITKIMAGE::pixel_t correspond to imageData->get_pixel_type
 * @pre an assertion check if ITKIMAGE dimension correspond to imageData dimension
 */
template<class ITKIMAGE>
typename ITKIMAGE::Pointer move_to_itk(data::image::csptr _image_data);

/**
 * @brief Map which associates itk types with sight types
 *
 */
static const std::map<::itk::ImageIOBase::IOComponentType, core::type> ITK_TYPE_CONVERTER =
{
    {::itk::ImageIOBase::UNKNOWNCOMPONENTTYPE, core::type::NONE},
    {::itk::ImageIOBase::UCHAR, core::type::UINT8},
    {::itk::ImageIOBase::CHAR, core::type::INT8},
    {::itk::ImageIOBase::USHORT, core::type::UINT16},
    {::itk::ImageIOBase::SHORT, core::type::INT16},
    {::itk::ImageIOBase::UINT, core::type::UINT32},
    {::itk::ImageIOBase::INT, core::type::INT32},
    {::itk::ImageIOBase::ULONG, core::type::UINT32},
    {::itk::ImageIOBase::LONG, core::type::INT32},
    {::itk::ImageIOBase::ULONGLONG, core::type::UINT64},
    {::itk::ImageIOBase::LONGLONG, core::type::INT64},
    {::itk::ImageIOBase::FLOAT, core::type::FLOAT},
    {::itk::ImageIOBase::DOUBLE, core::type::DOUBLE}
};

} // namespace sight::io::itk

#include "io/itk/itk.hxx"
