/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwData/Object.hpp>

#include "gui/view/DefaultView.hpp"

namespace gui
{
namespace view
{

REGISTER_SERVICE( ::gui::view::IView , ::gui::view::DefaultView , ::fwTools::Object ) ;


DefaultView::DefaultView() throw()
{}

//-----------------------------------------------------------------------------

DefaultView::~DefaultView() throw()
{}

//-----------------------------------------------------------------------------

void DefaultView::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void DefaultView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void DefaultView::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void DefaultView::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultView::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
}

