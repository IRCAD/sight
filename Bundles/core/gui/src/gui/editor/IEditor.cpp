/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/editor/IEditor.hpp"

#include <fwCore/base.hpp>


namespace gui
{
namespace editor
{


IEditor::IEditor() noexcept
{
}

//-----------------------------------------------------------------------------

IEditor::~IEditor() noexcept
{
}

//-----------------------------------------------------------------------------

void IEditor::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

