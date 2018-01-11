/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/RemoveLandmark.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <exception>
#include <sstream>


namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::RemoveLandmark, ::fwData::Image );


//------------------------------------------------------------------------------


RemoveLandmark::RemoveLandmark( ) noexcept
{
}

//------------------------------------------------------------------------------

RemoveLandmark::~RemoveLandmark() noexcept
{
}

//------------------------------------------------------------------------------

void RemoveLandmark::info(std::ostream& _sstream )
{
    _sstream << "Action for remove landmark" << std::endl;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr RemoveLandmark::getLandmarkToRemove(::fwData::Image::sptr image, bool& removeAll)
{
    ::fwData::Point::sptr landmarkToRemove;
    removeAll = false;

    ::fwData::PointList::sptr landmarksBackup = image->getField< ::fwData::PointList >(
        ::fwDataTools::fieldHelper::Image::m_imageLandmarksId );
    SLM_ASSERT("No Field ImageLandmarks", landmarksBackup);

    std::vector< std::string > selections;
    selections.push_back("ALL");
    std::map< std::string, ::fwData::Point::sptr > correspondance;

    for(::fwData::Point::sptr landmark :  landmarksBackup->getRefPoints())
    {
        ::fwData::String::sptr name = landmark->getField< ::fwData::String >(
            ::fwDataTools::fieldHelper::Image::m_labelId);
        SLM_ASSERT("No Field LabelId", name);
        selections.push_back( *name );
        correspondance[ *name ] = landmark;
    }

    ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();
    selector->setTitle("Select a landmark to remove");
    selector->setSelections(selections);
    std::string selection = selector->show();
    if( !selection.empty() )
    {
        if (selection=="ALL")
        {
            removeAll = true;
        }
        else
        {
            removeAll        = false;
            landmarkToRemove = correspondance[selection];
        }
    }

    return landmarkToRemove;
}

//------------------------------------------------------------------------------

void RemoveLandmark::notify( ::fwData::Image::sptr image, ::fwData::Point::sptr backup)
{
    auto sig = image->signal< ::fwData::Image::LandmarkRemovedSignalType >(::fwData::Image::s_LANDMARK_REMOVED_SIG);
    sig->asyncEmit(backup);
}

//------------------------------------------------------------------------------

void RemoveLandmark::updating( )
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image         = this->getObject< ::fwData::Image >();
    ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >(
        ::fwDataTools::fieldHelper::Image::m_imageLandmarksId );

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image) && landmarks)
    {
        if (!landmarks->getRefPoints().empty())
        {
            bool requestAll;
            ::fwData::Point::sptr landmarkToRemove = this->getLandmarkToRemove(image, requestAll );

            // perform action only available distance
            if ( landmarkToRemove )
            {

                ::fwData::PointList::PointListContainer::iterator itr;
                itr = std::find( landmarks->getRefPoints().begin(), landmarks->getRefPoints().end(), landmarkToRemove );
                if (itr != landmarks->getRefPoints().end())
                {
                    landmarks->getRefPoints().erase(itr);
                    this->notify(image, landmarkToRemove);
                }
            }
            if ( requestAll )
            {
                // backup
                image->removeField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId ); // erase field
                this->notify(image, landmarkToRemove);
            }
        }
    }
}

//------------------------------------------------------------------------------

void RemoveLandmark::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void RemoveLandmark::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void RemoveLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
