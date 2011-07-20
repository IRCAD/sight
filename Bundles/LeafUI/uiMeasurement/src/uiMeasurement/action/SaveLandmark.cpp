/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <exception>

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

#include "uiMeasurement/action/SaveLandmark.hpp"

namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::SaveLandmark , ::fwData::Image) ;


//------------------------------------------------------------------------------


SaveLandmark::SaveLandmark( ) throw()
{}

//------------------------------------------------------------------------------

SaveLandmark::~SaveLandmark() throw()
{}

//------------------------------------------------------------------------------

void SaveLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for save landmarks" << std::endl;
}

//------------------------------------------------------------------------------

void SaveLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image =  this->getObject< ::fwData::Image >();
    if (image->getBuffer()==NULL)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Save landmarks",
                "Sorry, it is impossible to save image landmarks. There is not loaded image in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to save landmarks");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Text","*.txt");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

        vnl_vector<double> ratio(3);
        ratio[0] = image->getSpacing()[0];
        ratio[1] = image->getSpacing()[1];
        ratio[2] = image->getSpacing()[2];

        std::vector< ::fwData::Point::sptr > vLandmarks =  landmarks->getPoints();
        std::vector< vnl_vector<double> > vPoints;
        std::vector< std::string > vNames;

        std::vector< ::fwData::Point::sptr >::iterator iter;
        for (iter = vLandmarks.begin() ; iter != vLandmarks.end() ; ++iter)
        {
            vnl_vector<double> point(3);
            point[0] = (*iter)->getCoord()[0]/ratio[0];
            point[1] = (*iter)->getCoord()[1]/ratio[1];
            point[2] = (*iter)->getCoord()[2]/ratio[2];
            vPoints.push_back(point);

            std::string str = (*iter)->getFieldSingleElement< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value();
            vNames.push_back(str);
        }
        ::arlCore::save(vPoints, ratio, vNames, filename.string(), true);
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

void SaveLandmark::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void SaveLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
