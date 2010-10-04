/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include "gui/frame/DefaultFrame.hpp"

namespace gui
{
namespace frame
{

REGISTER_SERVICE( ::fwGui::IFrameSrv , ::gui::frame::DefaultFrame , ::fwTools::Object ) ;


DefaultFrame::DefaultFrame() throw()
{}

//-----------------------------------------------------------------------------

DefaultFrame::~DefaultFrame() throw()
{}

//-----------------------------------------------------------------------------

void DefaultFrame::configuring() throw( ::fwTools::Failed )
{
    SLM_ASSERT( "<service> tag is required.", m_configuration->getName() == "service" );
    this->initialize();
}

//-----------------------------------------------------------------------------

void DefaultFrame::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void DefaultFrame::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void DefaultFrame::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultFrame::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultFrame::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}


//-----------------------------------------------------------------------------

}
}

