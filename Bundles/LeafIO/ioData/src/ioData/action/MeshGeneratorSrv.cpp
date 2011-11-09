/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include "ioData/action/MeshGeneratorSrv.hpp"

namespace ioData
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::ioData::action::MeshGeneratorSrv , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

MeshGeneratorSrv::MeshGeneratorSrv() throw()
{
}

//-----------------------------------------------------------------------------

MeshGeneratorSrv::~MeshGeneratorSrv() throw()
{
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::configuring() throw( ::fwTools::Failed )
{
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("Sorry you must have one (and only one) <config functor=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "Sorry, missing attribute functor in <config> xml element.", configElement->hasAttribute("functor") );
    m_functor = configElement->getExistingAttributeValue("functor");
    OSLM_ASSERT("Wrong functor name "<<m_functor << " (required GenTriangle, GenQuad or GenTriangleQuad)",
            m_functor == "GenTriangle" || m_functor == "GenQuad" || m_functor == "GenTriangleQuad");
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwTools::Object::sptr obj = this->getObject();
    if(m_functor == "GenTriangle")
    {
        SLM_WARN("Todo GenTriangle");
    }
    else if(m_functor == "GenQuad")
    {
        SLM_WARN("Todo GenQuad");
    }
    else if(m_functor == "GenTriangleQuad")
    {
        SLM_WARN("Todo GenTriangleQuad");
    }
    ::fwServices::ObjectMsg::NewSptr objectMsg;
    objectMsg->addEvent( "VALUE_IS_MODIFIED" );
    ::fwServices::IEditionService::notify(this->getSptr(), obj, objectMsg);
}

//-----------------------------------------------------------------------------

void MeshGeneratorSrv::info(std::ostream &_sstream )
{
    _sstream << "MeshGenerator Action" << std::endl;
}

//-----------------------------------------------------------------------------

}
}

