/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/view/SDefaultView.hpp"

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

namespace gui
{
namespace view
{

fwServicesRegisterMacro( ::gui::view::IView, ::gui::view::SDefaultView, ::fwData::Object );

SDefaultView::SDefaultView() throw()
{
}

//-----------------------------------------------------------------------------

SDefaultView::~SDefaultView() throw()
{
}

//-----------------------------------------------------------------------------

void SDefaultView::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultView::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultView::info(std::ostream& _sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefaultView::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

