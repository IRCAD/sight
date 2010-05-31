/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/helper.hpp>
#include <fwCore/base.hpp>


#include "gui/editor/IEditor.hpp"

namespace gui
{
namespace editor
{


IEditor::IEditor() throw()
{}

//-----------------------------------------------------------------------------

IEditor::~IEditor() throw()
{}

//-----------------------------------------------------------------------------

void IEditor::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    if (m_configuration)
    {
        SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );
    }
}

//-----------------------------------------------------------------------------

void IEditor::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void IEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initGuiParentContainer();
}

//-----------------------------------------------------------------------------

void IEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->resetGuiParentContainer();
}

//-----------------------------------------------------------------------------

}
}

