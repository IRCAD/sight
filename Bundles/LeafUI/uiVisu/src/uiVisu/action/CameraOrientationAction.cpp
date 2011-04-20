/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwTools/Object.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include "uiVisu/action/CameraOrientationAction.hpp"
#include "fwServices/IEditionService.hpp"

namespace uiVisu
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiVisu::action::CameraOrientationAction , ::fwData::Image ) ;


CameraOrientationAction::CameraOrientationAction() throw() : m_orientation("axial")
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

CameraOrientationAction::~CameraOrientationAction() throw()
{}

//------------------------------------------------------------------------------

void CameraOrientationAction::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void CameraOrientationAction::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void CameraOrientationAction::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::initialize();

    if( this->m_configuration->size() > 0 )
    {
        ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
        SLM_ASSERT("Sorry, only one xml element \"orientation\" is accepted.", this->m_configuration->size() == 1 && (*iter)->getName() == "orientation" );
        SLM_ASSERT("Sorry, xml element \"orientation\" is empty.", ! (*iter)->getValue().empty() );
        m_orientation = (*iter)->getValue();
        ::boost::algorithm::trim(m_orientation);
        ::boost::algorithm::to_lower(m_orientation);

        if(m_orientation != "axial" && m_orientation != "frontal" && m_orientation != "sagittal")
        {
            SLM_FATAL("The value for the xml element \"orientation\" can only be axial, frontal or sagittal.");
        }
    }

}

//------------------------------------------------------------------------------

void CameraOrientationAction::updating() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::String::NewSptr dataInfo;
    dataInfo->value() = m_orientation;

    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->addEvent( "CAMERA_ORIENTATION", dataInfo );
    ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
}

//------------------------------------------------------------------------------

void CameraOrientationAction::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void CameraOrientationAction::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void CameraOrientationAction::info( std::ostream &_sstream )
{
}


//------------------------------------------------------------------------------

} // action
} // uiVisu

