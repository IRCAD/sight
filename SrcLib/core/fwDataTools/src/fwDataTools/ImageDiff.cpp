/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/ImageDiff.hpp"

namespace fwDataTools
{

//-----------------------------------------------------------------------------

ImageDiff::ImageDiff(const ::fwData::Image::IndexType index, const ::fwData::Image::BufferType* oldValue,
                     const ::fwData::Image::BufferType* newValue, const unsigned char imageTypeSize) :
    m_index(index),
    m_typeSize(imageTypeSize)
{
    m_oldValue = new ::fwData::Image::BufferType[imageTypeSize];
    m_newValue = new ::fwData::Image::BufferType[imageTypeSize];
    std::copy(oldValue, oldValue+imageTypeSize, m_oldValue);
    std::copy(newValue, newValue+imageTypeSize, m_newValue);
}

//-----------------------------------------------------------------------------

ImageDiff::~ImageDiff()
{
    if (m_oldValue)
    {
        delete[] m_oldValue;
    }
    if (m_newValue)
    {
        delete[] m_newValue;
    }
}

//-----------------------------------------------------------------------------

ImageDiff::ImageDiff(const ImageDiff& other) :
    m_index(other.m_index),
    m_oldValue(new ::fwData::Image::BufferType[other.m_typeSize]),
    m_newValue(new ::fwData::Image::BufferType[other.m_typeSize]),
    m_typeSize(other.m_typeSize)
{
    std::copy(other.m_oldValue, other.m_oldValue+m_typeSize, m_oldValue);
    std::copy(other.m_newValue, other.m_newValue+m_typeSize, m_newValue);
}

//-----------------------------------------------------------------------------

ImageDiff::ImageDiff(ImageDiff&& other) :
    m_index(other.m_index),
    m_oldValue(other.m_oldValue),
    m_newValue(other.m_newValue),
    m_typeSize(other.m_typeSize)
{
    other.m_oldValue = nullptr;
    other.m_newValue = nullptr;
}

//-----------------------------------------------------------------------------

ImageDiff& ImageDiff::operator =(const ImageDiff& other)
{
    ImageDiff tmpImageDiff(other);
    *this = std::move(tmpImageDiff);

    return *this;
}

//-----------------------------------------------------------------------------

ImageDiff& ImageDiff::operator=(ImageDiff&& other)
{
    m_index = other.m_index;

    delete[] m_oldValue;
    delete[] m_newValue;

    m_oldValue = other.m_oldValue;
    m_newValue = other.m_newValue;

    other.m_oldValue = nullptr;
    other.m_newValue = nullptr;

    m_typeSize = other.m_typeSize;

    return *this;
}

} // namespace fwDataTools
