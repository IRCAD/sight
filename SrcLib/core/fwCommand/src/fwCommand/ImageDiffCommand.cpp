/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCommand/ImageDiffCommand.hpp"

#include <fwTools/Type.hpp>

namespace fwCommand
{

//------------------------------------------------------------------------------

const size_t ImageDiffCommand::getSize() const
{
    size_t imgEltSize = m_img->getPixelType().sizeOf() * m_img->getNumberOfComponents();

    return m_diff.capacity() * sizeof(::fwDataTools::ImageDiffsType::value_type) + 2 * m_diff.size() * imgEltSize;
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::redo()
{
    ::fwDataTools::Image::applyDiff(m_img, m_diff);
    return true;
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::undo()
{
    ::fwDataTools::Image::revertDiff(m_img, m_diff);
    return true;
}

//------------------------------------------------------------------------------

const std::string ImageDiffCommand::getDescription() const
{
    return "ImageDiff";
}

} // namespace fwCommand
