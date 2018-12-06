/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWCOMMAND_IMAGEDIFFCOMMAND_HPP__
#define __FWCOMMAND_IMAGEDIFFCOMMAND_HPP__

#include "fwCommand/config.hpp"
#include "fwCommand/ICommand.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/ImageDiff.hpp>

namespace fwCommand
{

class FWCOMMAND_CLASS_API ImageDiffCommand : public ICommand
{
public:

    /// Constructor, uses an image and a change list for that image.
    FWCOMMAND_API ImageDiffCommand(const ::fwData::Image::sptr& img, ::fwDataTools::ImageDiff diff);

    /// The diff size.
    FWCOMMAND_API virtual size_t getSize() const;

    /// Apply diff.
    FWCOMMAND_API virtual bool redo();

    /// Revert diff.
    FWCOMMAND_API virtual bool undo();

    /// Returns "ImageDiff".
    FWCOMMAND_API virtual const std::string getDescription() const;

private:

    ::fwData::Image::sptr m_img;

    ::fwData::Image::BufferModifiedSignalType::sptr m_modifSig;

    ::fwDataTools::ImageDiff m_diff;

};

} // namespace fwCommand

#endif // __FWCOMMAND_IMAGEDIFFCOMMAND_HPP__
