/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/SaveLandmark.hpp"

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <vector>
#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::SaveLandmark, ::fwData::Image);


//------------------------------------------------------------------------------


SaveLandmark::SaveLandmark( ) throw()
{
}

//------------------------------------------------------------------------------

SaveLandmark::~SaveLandmark() throw()
{
}

//------------------------------------------------------------------------------

void SaveLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for save landmarks" << std::endl;
}

//------------------------------------------------------------------------------

void SaveLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (!::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Save landmarks",
            "It is impossible to save image landmarks. There is no loaded image in the software.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to save landmarks");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Landmark file","*.json");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if( result )
    {
        ::boost::filesystem::path path = result->getPath();
        _sDefaultPath                  = path.parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );

        this->save(path);
    }
}

//------------------------------------------------------------------------------

void SaveLandmark::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SaveLandmark::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SaveLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SaveLandmark::save(const ::boost::filesystem::path& path)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    //get landmarks
    ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(  image );
    ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >(
        ::fwComEd::Dictionary::m_imageLandmarksId);
    SLM_ASSERT("landmarks not instanced", landmarks);

    ::fwData::Composite::sptr replaceMap = ::fwData::Composite::New();
    (*replaceMap)["GENERIC_UID"]         = ::fwData::String::New(
        ::fwServices::registry::AppConfig::getUniqueIdentifier("SaveLandmarkApp")
        );
    (*replaceMap)["landmark"]                       = ::fwData::String::New(landmarks->getID());
    (*replaceMap)["file"]                           = ::fwData::String::New(path.string());
    ::fwRuntime::ConfigurationElement::csptr config =
        ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig("SaveLandmark", replaceMap);

    ::fwServices::AppConfigManager::sptr helper = ::fwServices::AppConfigManager::New();
    helper->setConfig( config );
    helper->launch();

    helper->stopAndDestroy();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
