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
class DATA_DEPRECATED_CLASS_API Image
{

public:

    typedef data::Image::SizeType SizeType;
    typedef data::Image::IndexType IndexType;
    typedef data::Image::BufferType BufferType;
    typedef data::Image::BufferIndexType BufferIndexType;

    /// Shared pointer type
    typedef std::shared_ptr < data::helper::Image > sptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const data::helper::Image > csptr;
    /// Unique pointer type
    typedef std::unique_ptr < data::helper::Image > uptr;
    /// Const unique pointer type
    typedef std::unique_ptr < const data::helper::Image > cuptr;

    //------------------------------------------------------------------------------

    static sptr New(data::Image::sptr image)
    {
        return std::make_shared< data::helper::Image >(image);
    }

    /// Constructor. Initialize parameters.
    [[deprecated("will be removed in sight 22.0, please use data::Image")]]
    DATA_API Image( data::Image::sptr image );

    /// Destrucotr. Do nothing.
    DATA_API virtual ~Image();

    /**
     * @brief Create landmark fields if not exist, NOT GENERATE MESSAGE.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @return true if the field is added.
     *
     * @deprecated use::data::fieldHelper::MedicalImageHelpers::checkLandmarks() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::data::fieldHelper::MedicalImageHelpers::checkLandmarks()' instead.")]]
    DATA_API bool createLandmarks();

    /**
     * @brief Create transfer function pool if the pool is not present, NOT GENERATE MESSAGE.
     *
     * If the image has not transfer functions, the pool is
     * created and a grey level transfer function is added.
     *
     * @deprecated use::data::fieldHelper::MedicalImageHelpers::checkTransferFunctionPool() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::data::fieldHelper::MedicalImageHelpers::checkTransferFunctionPool()' instead.")]]
    DATA_API bool createTransferFunctionPool();

    /**
     * @brief Create slice index if indexes are not present, NOT GENERATE MESSAGE.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @return true if the fields are added.
     *
     * @deprecated use::data::fieldHelper::MedicalImageHelpers::checkImageSliceIndex() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::data::fieldHelper::MedicalImageHelpers::checkImageSliceIndex()' instead.")]]
    DATA_API bool createImageSliceIndex();

    /// Send the message of modification
    DATA_API void notify();

    /// Returns image buffer
    DATA_API void* getBuffer();

    /** @{
     * @brief Helpers for 3D images
     */
    DATA_API void* getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z );

    DATA_API void* getPixelBuffer( IndexType index );

    DATA_API void setPixelBuffer( IndexType index, Image::BufferType* pixBuf);

    DATA_API const std::string getPixelAsString(SizeType::value_type x,
                                                SizeType::value_type y,
                                                SizeType::value_type z );
    // @}

    /// Returns a copy of current lock on image
    DATA_API core::memory::BufferObject::Lock getLock() const;

private:

    data::Image::sptr m_image;

    core::memory::BufferObject::Lock m_lock;

    /// modification flags
    bool m_sliceModified;
};

} // helper
} // fwDataTools
