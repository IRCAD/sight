/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "gui/editor/IDialogEditor.hpp"

namespace gui
{
namespace editor
{


IDialogEditor::IDialogEditor() throw()
{}

//-----------------------------------------------------------------------------

IDialogEditor::~IDialogEditor() throw()
{}

//-----------------------------------------------------------------------------

void IDialogEditor::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

