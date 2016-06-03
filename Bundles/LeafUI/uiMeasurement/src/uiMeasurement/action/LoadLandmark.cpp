/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/LoadLandmark.hpp"

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

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::LoadLandmark, ::fwData::Image);


//------------------------------------------------------------------------------


LoadLandmark::LoadLandmark( ) throw()
{
}

//------------------------------------------------------------------------------

LoadLandmark::~LoadLandmark() throw()
{
}

//------------------------------------------------------------------------------

void LoadLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for load landmarks" << std::endl;
}

//------------------------------------------------------------------------------

void LoadLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (!::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Load landmarks",
            "It is impossible to load image landmarks. There is no loaded image in the software.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load landmarks");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Landmark file","*.json");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if( result )
    {
        ::boost::filesystem::path path = result->getPath();
        _sDefaultPath                  = path.parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );

        this->load(path);

        // notify
        auto sig = image->signal< ::fwData::Image::LandmarkAddedSignalType >(::fwData::Image::s_LANDMARK_ADDED_SIG);
        sig->asyncEmit(::fwData::Point::sptr());
    }
}

//------------------------------------------------------------------------------

void LoadLandmark::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void LoadLandmark::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void LoadLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void LoadLandmark::load(const ::boost::filesystem::path& path)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    //get landmarks
    ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(  image );
    ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >(
        ::fwComEd::Dictionary::m_imageLandmarksId);
    SLM_ASSERT("landmarks not instanced", landmarks);

    ::fwData::PointList::sptr newLandmarks = ::fwData::PointList::New();
    ::fwData::Composite::sptr replaceMap   = ::fwData::Composite::New();
    (*replaceMap)["GENERIC_UID"]           = ::fwData::String::New(
        ::fwServices::registry::AppConfig::getUniqueIdentifier("LoadLandmarkApp")
        );
    (*replaceMap)["landmark"]                       = ::fwData::String::New(newLandmarks->getID());
    (*replaceMap)["file"]                           = ::fwData::String::New(path.string());
    ::fwRuntime::ConfigurationElement::csptr config =
        ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig("LoadLandmark", replaceMap);

    ::fwServices::IAppConfigManager::sptr helper = ::fwServices::IAppConfigManager::New();
    helper->setConfig( config );
    helper->launch();
    helper->stopAndDestroy();

    for(::fwData::Point::sptr landmark :  newLandmarks->getCRefPoints())
    {
        landmarks->getRefPoints().push_back( landmark );
    }
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
