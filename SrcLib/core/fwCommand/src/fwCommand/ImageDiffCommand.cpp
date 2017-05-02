/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCommand/ImageDiffCommand.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwTools/Type.hpp>

namespace fwCommand
{

//------------------------------------------------------------------------------

ImageDiffCommand::ImageDiffCommand(const ::fwData::Image::sptr& img, ::fwDataTools::ImageDiff diff) :
    m_img(img),
    m_modifSig(img->signal< ::fwData::Image::BufferModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG)),
    m_diff(diff)
{
    m_diff.shrink();
}

//------------------------------------------------------------------------------

size_t ImageDiffCommand::getSize() const
{
    return sizeof(*this) + m_diff.getSize();
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::redo()
{
    m_diff.applyDiff(m_img);

    m_modifSig->asyncEmit();

    return true;
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::undo()
{
    m_diff.revertDiff(m_img);

    m_modifSig->asyncEmit();

    return true;
}

//------------------------------------------------------------------------------

const std::string ImageDiffCommand::getDescription() const
{
    return "ImageDiff";
}

} // namespace fwCommand
