/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include "uiVisu/action/SSnapshot.hpp"

namespace uiVisu
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiVisu::action::SSnapshot , ::fwTools::Object ) ;


SSnapshot::SSnapshot() throw()
{
    this->handlingEventOff();
}

//------------------------------------------------------------------------------

SSnapshot::~SSnapshot() throw()
{}

//------------------------------------------------------------------------------

void SSnapshot::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SSnapshot::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SSnapshot::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IActionSrv::initialize();


    std::vector < Configuration > snapConfig = m_configuration->find("snap");
    if(!snapConfig.empty())
    {
        std::vector < Configuration > sceneConfig = snapConfig.at(0)->find("scene");
        for(unsigned int i =0; i < sceneConfig.size(); i++)
        {
            OSLM_ASSERT("Wrong tag name: " << sceneConfig.at(i)->getName(), sceneConfig.at(i)->getName() == "scene");
            SLM_ASSERT("UID attribute is missing", sceneConfig.at(i)->hasAttribute("uid"));

            std::string value(sceneConfig.at(i)->getAttributeValue("uid"));
            m_scenesUID.push_back(value);
        }
    }
}

//------------------------------------------------------------------------------

void SSnapshot::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    for(unsigned int i=0; i < m_scenesUID.size(); i++)
    {
        ::fwServices::IService::sptr service = ::fwServices::get(m_scenesUID.at(i));
        ::fwData::Composite::sptr composite = service->getObject< ::fwData::Composite >();
        SLM_ASSERT("SnapshotEditor sceneUID " << m_scenesUID.at(i) <<" isn't a GenericScene?" , composite);

        ::fwData::Object::NewSptr dataInfo;

        ::fwData::String::NewSptr sceneID;
        sceneID->value() = m_scenesUID.at(i);
        ::fwData::String::NewSptr filename;

        filename->value() = this->requestFileName();
        if(!filename->value().empty())
        {
            dataInfo->setField_NEWAPI("sceneID", sceneID);
            dataInfo->setField_NEWAPI("filename", filename);
            ::fwComEd::CompositeMsg::NewSptr compositeMsg;
            compositeMsg->addEvent( "SNAP", dataInfo );
            ::fwServices::IEditionService::notify(this->getSptr(), composite, compositeMsg);
        }
    }
}

//------------------------------------------------------------------------------

void SSnapshot::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void SSnapshot::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SSnapshot::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

std::string SSnapshot::requestFileName()
{
    std::string fileName = "";

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.addFilter("Image file","*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg","*.jpg *.jpeg");
    dialogFile.addFilter("bmp","*.bmp");
    dialogFile.addFilter("png","*.png");
    dialogFile.addFilter("tiff","*.tiff");
    dialogFile.addFilter("all","*.*");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        fileName = result->getPath().string();
    }

    return fileName;
}

//------------------------------------------------------------------------------

} // action
} // uiVisu

