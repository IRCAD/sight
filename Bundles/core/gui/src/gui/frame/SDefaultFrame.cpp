/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/frame/SDefaultFrame.hpp"

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>


namespace gui
{
namespace frame
{

fwServicesRegisterMacro( ::fwGui::IFrameSrv, ::gui::frame::SDefaultFrame, ::fwData::Object );


SDefaultFrame::SDefaultFrame() throw()
{
}

//-----------------------------------------------------------------------------

SDefaultFrame::~SDefaultFrame() throw()
{
}

//-----------------------------------------------------------------------------

void SDefaultFrame::configuring() throw( ::fwTools::Failed )
{
    SLM_ASSERT( "<service> tag is required.", m_configuration->getName() == "service" );
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefaultFrame::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}


//-----------------------------------------------------------------------------

}
}

