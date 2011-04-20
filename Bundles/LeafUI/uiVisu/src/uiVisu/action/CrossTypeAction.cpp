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
#include <fwData/Float.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include "uiVisu/action/CrossTypeAction.hpp"
#include "fwServices/IEditionService.hpp"

namespace uiVisu
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiVisu::action::CrossTypeAction , ::fwData::Image ) ;


std::map< std::string, float >
CrossTypeAction::m_scaleConversion
        = ::boost::assign::map_list_of(std::string("full"),1.0)
                                      (std::string("half"),0.5)
                                      (std::string("hide"),0.0);

CrossTypeAction::CrossTypeAction() throw()
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

CrossTypeAction::~CrossTypeAction() throw()
{}

//------------------------------------------------------------------------------

void CrossTypeAction::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void CrossTypeAction::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void CrossTypeAction::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::initialize();

    if( this->m_configuration->size() > 0 )
    {
        std::vector < ConfigurationType > vectConfig = this->m_configuration->find("crossType");
        SLM_ASSERT("Missing <crossType> tag!", !vectConfig.empty());
        m_crossType = vectConfig.at(0)->getValue();
        ::boost::algorithm::trim(m_crossType);
        ::boost::algorithm::to_lower(m_crossType);
        SLM_ASSERT("Unknown crossType", m_scaleConversion.find(m_crossType) != m_scaleConversion.end());
    }
}

//------------------------------------------------------------------------------

void CrossTypeAction::updating() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Float::NewSptr dataInfo;
    dataInfo->value() = m_scaleConversion[m_crossType];

    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->addEvent( "CROSS_TYPE", dataInfo );
    ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
}

//------------------------------------------------------------------------------

void CrossTypeAction::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void CrossTypeAction::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void CrossTypeAction::info( std::ostream &_sstream )
{
}


//------------------------------------------------------------------------------

} // action
} // uiVisu

