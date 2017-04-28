/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    ::fwDataTools::helper::Image m_imgHelper;

    ::fwData::Image::BufferModifiedSignalType::sptr m_modifSig;

    ::fwDataTools::ImageDiff m_diff;

};

} // namespace fwCommand

#endif // __FWCOMMAND_IMAGEDIFFCOMMAND_HPP__
