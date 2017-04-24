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
    m_index(index)
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

ImageDiff::ImageDiff(ImageDiff&& voxDiff)
{
    m_index    = voxDiff.m_index;
    m_oldValue = voxDiff.m_oldValue;
    m_newValue = voxDiff.m_newValue;

    voxDiff.m_oldValue = nullptr;
    voxDiff.m_newValue = nullptr;
}

} // namespace fwDataTools
