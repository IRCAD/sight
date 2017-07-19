/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/editor/IDialogEditor.hpp"

#include <fwCore/base.hpp>


namespace gui
{
namespace editor
{


IDialogEditor::IDialogEditor() noexcept
{
}

//-----------------------------------------------------------------------------

IDialogEditor::~IDialogEditor() noexcept
{
}

//-----------------------------------------------------------------------------

void IDialogEditor::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

