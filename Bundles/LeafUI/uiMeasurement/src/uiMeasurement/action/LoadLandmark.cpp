/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <exception>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <arlcore/Misc.h>

#include "uiMeasurement/action/LoadLandmark.hpp"

namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::LoadLandmark , ::fwData::Image) ;


//------------------------------------------------------------------------------


LoadLandmark::LoadLandmark( ) throw()
{}

//------------------------------------------------------------------------------

LoadLandmark::~LoadLandmark() throw()
{}

//------------------------------------------------------------------------------

void LoadLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for load landmarks" << std::endl;
}

//------------------------------------------------------------------------------

void LoadLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image =  this->getObject< ::fwData::Image >();
    if (image->getBuffer() == NULL)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Load landmarks",
                "Sorry, it is impossible to load image landmarks. There is not loaded image in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load landmarks");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Text","*.txt");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if( result )
    {
        ::boost::filesystem::path filename = result->getPath();
        _sDefaultPath = filename.parent_path();
        //get landmarks
        ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(  image );
        ::fwData::PointList::sptr landmarks =  image->getFieldSingleElement< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
        SLM_ASSERT("landmarks not instanced", landmarks);


        std::vector< vnl_vector<double> > vPoints;
        std::vector< std::string > vNames;
        ::arlCore::load(vPoints, vNames, filename.string());
        SLM_ASSERT("Error in landmark reader", vPoints.size() == vNames.size());
        unsigned int index = 0;
        BOOST_FOREACH( vnl_vector<double> point, vPoints)
        {
            ::fwData::Point::sptr newPoint = ::fwData::Point::New(point[0], point[1], point[3]);
            ::fwData::String::sptr label = ::fwData::String::New(vNames[index]);
            newPoint->setFieldSingleElement( ::fwComEd::Dictionary::m_labelId , label );
            landmarks->getRefPoints().push_back( newPoint );
            ++index;
        }
        // notify
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ImageMsg::LANDMARK );
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
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

void LoadLandmark::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void LoadLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
