/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Image.hpp>

#include "io/itk/config.hpp"
#include <itkImageIOBase.h>

namespace sight::io::itk
{

/**
 * @brief Allocate a new data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so data::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * data::Image does not manage the image buffer.
 */
template<class ITKIMAGE>
data::Image::sptr moveFromItk(typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage = true);

/**
 * @brief Update a data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so data::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * data::Image does not manage the image buffer.
 */
template<class ITKIMAGE>
void moveFromItk(
    typename ITKIMAGE::Pointer itkImage,
    data::Image::sptr _dataImage,
    bool bufferManagerIsDataImage = true
);

/**
 * @brief Create an ITK image from an data::Image.
 *
 * If bufferManagerIsDataImage = true, imageData conserve his buffer management
 * and so the created itkImage not manages it. If bufferManagerIsDataImage = false,
 * the created itkImage manage his image buffer and thus imageData releases his
 * buffer (in this case, imageData must be the owner of this buffer).
 *
 * @pre an assertion check if ITKIMAGE::PixelType correspond to imageData->getPixelType
 * @pre an assertion check if ITKIMAGE dimension correspond to imageData dimension
 */
template<class ITKIMAGE>
typename ITKIMAGE::Pointer moveToItk(data::Image::csptr imageData);

/**
 * @brief Map which associates itk types with sight types
 *
 */
static const std::map<::itk::ImageIOBase::IOComponentType, core::Type> ITK_TYPE_CONVERTER =
{
    {::itk::ImageIOBase::UNKNOWNCOMPONENTTYPE, core::Type::NONE},
    {::itk::ImageIOBase::UCHAR, core::Type::UINT8},
    {::itk::ImageIOBase::CHAR, core::Type::INT8},
    {::itk::ImageIOBase::USHORT, core::Type::UINT16},
    {::itk::ImageIOBase::SHORT, core::Type::INT16},
    {::itk::ImageIOBase::UINT, core::Type::UINT32},
    {::itk::ImageIOBase::INT, core::Type::INT32},
    {::itk::ImageIOBase::ULONG, core::Type::UINT32},
    {::itk::ImageIOBase::LONG, core::Type::INT32},
    {::itk::ImageIOBase::ULONGLONG, core::Type::UINT64},
    {::itk::ImageIOBase::LONGLONG, core::Type::INT64},
    {::itk::ImageIOBase::FLOAT, core::Type::FLOAT},
    {::itk::ImageIOBase::DOUBLE, core::Type::DOUBLE}
};

} // namespace sight::io::itk

#include "io/itk/itk.hxx"
