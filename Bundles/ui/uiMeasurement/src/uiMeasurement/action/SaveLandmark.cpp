/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/SaveLandmark.hpp"

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <exception>
#include <vector>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::SaveLandmark, ::fwData::Image);

//------------------------------------------------------------------------------

SaveLandmark::SaveLandmark( ) noexcept
{
}

//------------------------------------------------------------------------------

SaveLandmark::~SaveLandmark() noexcept
{
}

//------------------------------------------------------------------------------

void SaveLandmark::info(std::ostream& _sstream )
{
    _sstream << "Action for save landmarks" << std::endl;
}

//------------------------------------------------------------------------------

void SaveLandmark::updating()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
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
    dialogFile.addFilter("Landmark file", "*.json");
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

void SaveLandmark::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SaveLandmark::starting()
{
    FW_DEPRECATED_MSG("This service is deprecated and will be removed.")

    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SaveLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SaveLandmark::save(const ::boost::filesystem::path& path)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >("image");

    ::fwData::PointList::csptr landmarks = image->getField< ::fwData::PointList >(
        ::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    if (landmarks)
    {
        ::fwServices::registry::AppConfig::FieldAdaptorType replaceMap;
        replaceMap["GENERIC_UID"] = ::fwServices::registry::AppConfig::getUniqueIdentifier("SaveLandmarkApp");
        replaceMap["landmark"]    = landmarks->getID();
        replaceMap["file"]        = path.string();

        ::fwRuntime::ConfigurationElement::csptr config =
            ::fwServices::registry::AppConfig::getDefault()->
            getAdaptedTemplateConfig("SaveLandmark2", replaceMap, true);

        ::fwServices::IAppConfigManager::sptr helper = ::fwServices::IAppConfigManager::New();
        helper->setConfig( config );
        helper->launch();
        helper->stopAndDestroy();
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Save Landmarks", "There is no landmark to save.");
    }
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
