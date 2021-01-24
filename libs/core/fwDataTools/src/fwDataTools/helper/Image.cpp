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

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/helper/Image.hpp"

#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"
#include "fwDataTools/helper/Field.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Composite.hpp>
#include <data/PointList.hpp>
#include <data/TransferFunction.hpp>

namespace fwDataTools
{
namespace helper
{

//-----------------------------------------------------------------------------

Image::Image( data::Image::sptr image ) :
    m_image(image),
    m_sliceModified(false)
{
    FW_DEPRECATED_MSG("::fwDataTools::helper::Image is no longer supported, the methods have been moved to "
                      "data::Image", "22.0")
    if ( image )
    {
        m_lock = image->getDataArray()->getBufferObject()->lock();
    }
}

//-----------------------------------------------------------------------------

Image::~Image()
{
}

//------------------------------------------------------------------------------

bool Image::createLandmarks()
{
    bool fieldIsCreated = false;

    // Manage image landmarks
    if ( !m_image->getField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId ) )
    {
        data::PointList::sptr pl = data::PointList::New();
        m_image->setField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId, pl );
        fieldIsCreated = true;
    }

    return fieldIsCreated;
}

//------------------------------------------------------------------------------

bool Image::createTransferFunctionPool()
{
    bool fieldIsCreated             = false;
    const std::string poolFieldName = ::fwDataTools::fieldHelper::Image::m_transferFunctionCompositeId;
    data::Composite::sptr tfPool;

    tfPool = m_image->getField< data::Composite >(poolFieldName);
    // Transfer functions
    if ( !tfPool )
    {
        tfPool = data::Composite::New();

        // Set in selected image
        ::fwDataTools::helper::Field fieldHelper(m_image);
        fieldHelper.setField(poolFieldName, tfPool);
        fieldHelper.notify();

        // TF pool is modified
        fieldIsCreated = true;
    }

    const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
    if(tfPool->find(defaultTFName) == tfPool->end())
    {
        data::TransferFunction::sptr tf = data::TransferFunction::createDefaultTF();
        if (m_image->getWindowWidth() != 0 )
        {
            tf->setWindow( m_image->getWindowWidth() );
            tf->setLevel( m_image->getWindowCenter() );
        }
        else if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(m_image))
        {
            double min, max;
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(m_image, min, max);
            data::TransferFunction::TFValuePairType wlMinMax(min, max);
            tf->setWLMinMax(wlMinMax);
        }
        // Set in TFPool
        ::fwDataTools::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(defaultTFName, tf);
        compositeHelper.notify();
    }

    return fieldIsCreated;
}

//------------------------------------------------------------------------------

bool Image::createImageSliceIndex()
{
    bool fieldIsCreated = false;

    const data::Image::SizeType& imageSize = m_image->getSize();

    data::Integer::sptr axialIdx = m_image->getField< data::Integer >(
        ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId );
    data::Integer::sptr frontalIdx = m_image->getField< data::Integer >(
        ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId);
    data::Integer::sptr sagittalIdx = m_image->getField< data::Integer >(
        ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId );

    // Manage image slice index
    if ( !(axialIdx && frontalIdx && sagittalIdx) )
    {
        // Set value
        axialIdx = data::Integer::New(-1);
        m_image->setField( ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, axialIdx );

        frontalIdx = data::Integer::New(-1);
        m_image->setField( ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, frontalIdx );

        sagittalIdx = data::Integer::New(-1);
        m_image->setField( ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, sagittalIdx );

        fieldIsCreated = true;
    }

    SLM_ASSERT(
        "Information on image slice index is not correct, miss one of these fields : "
        "m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
        axialIdx && frontalIdx && sagittalIdx
        );

    // Get value
    if( axialIdx->value() < 0 || static_cast< int>(imageSize[2]) < axialIdx->value() )
    {
        axialIdx->value() = static_cast< data::Integer::ValueType >(imageSize[2] / 2);
        fieldIsCreated    = true;
    }

    if( frontalIdx->value() < 0 || static_cast< int>(imageSize[1]) < frontalIdx->value() )
    {
        frontalIdx->value() = static_cast< data::Integer::ValueType >(imageSize[1] / 2);
        fieldIsCreated      = true;
    }

    if( sagittalIdx->value() < 0 || static_cast< int>(imageSize[0]) < sagittalIdx->value() )
    {
        sagittalIdx->value() = static_cast< data::Integer::ValueType >(imageSize[0] / 2);
        fieldIsCreated       = true;
    }

    m_sliceModified |= fieldIsCreated;
    return fieldIsCreated;
}

//-----------------------------------------------------------------------------

void Image::notify()
{
    if(m_sliceModified)
    {
        auto axialIdx = m_image->getField< data::Integer >(
            ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId );
        auto frontalIdx = m_image->getField< data::Integer >(
            ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId);
        auto sagittalIdx = m_image->getField< data::Integer >(
            ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId );
        auto sig = m_image->signal< data::Image::SliceIndexModifiedSignalType >(
            data::Image::s_SLICE_INDEX_MODIFIED_SIG);
        sig->asyncEmit(axialIdx->getValue(), frontalIdx->getValue(), sagittalIdx->getValue());
    }

    auto sig = m_image->signal< data::Image::ModifiedSignalType >( data::Image::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void* Image::getBuffer()
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z )
{
    SizeType size    = m_image->getSize();
    IndexType offset = x + size[0]*y + z*size[0]*size[1];
    return this->getPixelBuffer(offset);
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( IndexType index )
{
    std::uint8_t imagePixelSize = m_image->getType().sizeOf() * m_image->getNumberOfComponents();
    BufferType* buf             = static_cast < BufferType* > (this->getBuffer());
    BufferIndexType bufIndex    = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void Image::setPixelBuffer( IndexType index, Image::BufferType* pixBuf)
{
    std::uint8_t imagePixelSize = m_image->getType().sizeOf() * m_image->getNumberOfComponents();
    BufferType* buf             = static_cast < BufferType* > (this->getPixelBuffer(index));

    std::copy(pixBuf, pixBuf+imagePixelSize, buf);
}

//------------------------------------------------------------------------------

const std::string Image::getPixelAsString(SizeType::value_type x,
                                          SizeType::value_type y,
                                          SizeType::value_type z )
{
    return m_image->getType().toString(this->getPixelBuffer(x, y, z));
}

//------------------------------------------------------------------------------

core::memory::BufferObject::Lock Image::getLock() const
{
    return m_lock;
}

//------------------------------------------------------------------------------

} // helper
} // fwDataTools
