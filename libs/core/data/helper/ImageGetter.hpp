/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include <data/Image.hpp>

namespace sight::data
{

namespace helper
{

/**
 * @brief   Defines an helper to modify an data::Image by adding few medical fields
 * and create in parallel the message to announce these modifications.
 *
 * @deprecated will be removed in sight 22.0, please use data::Image.
 */
class DATA_DEPRECATED_CLASS_API ImageGetter
{
public:

    typedef data::Image::SizeType SizeType;
    typedef data::Image::IndexType IndexType;
    typedef data::Image::BufferType BufferType;
    typedef data::Image::BufferIndexType BufferIndexType;

    /// Shared pointer type
    typedef std::shared_ptr<ImageGetter> sptr;
    /// Const shared pointer type
    typedef std::shared_ptr<const ImageGetter> csptr;
    /// Unique pointer type
    typedef std::unique_ptr<ImageGetter> uptr;
    /// Const unique pointer type
    typedef std::unique_ptr<const ImageGetter> cuptr;

    //------------------------------------------------------------------------------

    static sptr New(data::Image::csptr image)
    {
        return std::make_shared<ImageGetter>(image);
    }

    /// Constructor. Initialize parameters.
    [[deprecated("will be removed in sight 22.0, please use data::Image")]]
    DATA_API ImageGetter(data::Image::csptr image);

    /// Destrucotr. Do nothing.
    DATA_API virtual ~ImageGetter();

    /// Returns image buffer
    DATA_API void* getBuffer() const;

    /** @{
     * @brief Helpers for 3D images
     */
    DATA_API void* getPixelBuffer(
        SizeType::value_type x,
        SizeType::value_type y,
        SizeType::value_type z
    ) const;

    DATA_API void* getPixelBuffer(IndexType index) const;

    DATA_API const std::string getPixelAsString(
        SizeType::value_type x,
        SizeType::value_type y,
        SizeType::value_type z
    ) const;
// @}

private:

    data::Image::csptr m_image;

    core::memory::BufferObject::Lock m_lock;
};

} // helper

} // fwDataTools
