/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <fwCore/base.hpp>

#include <exception>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include "uiMeasurement/action/RemoveLandmark.hpp"


namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::RemoveLandmark , ::fwData::Image ) ;


//------------------------------------------------------------------------------


RemoveLandmark::RemoveLandmark( ) throw()
{}

//------------------------------------------------------------------------------

RemoveLandmark::~RemoveLandmark() throw()
{}

//------------------------------------------------------------------------------

void RemoveLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for remove landmark" << std::endl;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr  RemoveLandmark::getLandmarkToRemove(::fwData::Image::sptr image, bool &removeAll)
{
    ::fwData::Point::sptr landmarkToRemove;
    removeAll = false;

    ::fwData::PointList::sptr landmarksBackup = image->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId );
    SLM_ASSERT("No Field ImageLandmarks", landmarksBackup);

    std::vector< std::string > selections;
    selections.push_back("ALL");
    std::map< std::string , ::fwData::Point::sptr > correspondance;

    BOOST_FOREACH(::fwData::Point::sptr landmark, landmarksBackup->getRefPoints())
    {
        ::fwData::String::sptr name = landmark->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId);
        SLM_ASSERT("No Field LabelId", name);
        selections.push_back( *name );
        correspondance[ *name ] = landmark;
    }

    ::fwGui::dialog::SelectorDialog::NewSptr selector;
    selector->setTitle("Select a landmark to remove");
    selector->setSelections(selections);
    std::string selection = selector->show();
    if( ! selection.empty() )
    {
        if (selection=="ALL")
        {
            removeAll=true;
        }
        else
        {
            removeAll=false;
            landmarkToRemove = correspondance[selection];
        }
    }

    return landmarkToRemove;
}

//------------------------------------------------------------------------------

void RemoveLandmark::notify( ::fwData::Image::sptr image , ::fwData::Object::sptr backup)
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::LANDMARK, backup );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void RemoveLandmark::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId );

    if (::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image) && landmarks)
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
                image->removeField( ::fwComEd::Dictionary::m_imageLandmarksId ); // erase field
                this->notify(image, landmarks);
            }
        }
    }
}

//------------------------------------------------------------------------------

void RemoveLandmark::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void RemoveLandmark::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void RemoveLandmark::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void RemoveLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
