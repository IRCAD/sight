/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <exception>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/PatientMsg.hpp>
#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>

#include "uiMeasurement/action/AddLandmark.hpp"

namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::AddLandmark , ::fwData::Image ) ;


//------------------------------------------------------------------------------


AddLandmark::AddLandmark( ) throw()
{}

//------------------------------------------------------------------------------

AddLandmark::~AddLandmark() throw()
{}

//------------------------------------------------------------------------------

void AddLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

// return true if label setting is NOT Canceled , name is modified !!!
bool defineLabel(std::string &name)
{
    bool res = false;
    static int count = 1;
    name = "Label" + ::boost::lexical_cast< std::string >(count );

    ::fwData::String::NewSptr url;
    ::fwGui::dialog::InputDialog inputDlg;
    inputDlg.setTitle("Label");
    inputDlg.setMessage("Which label for the point?");
    inputDlg.setInput (name);
    std::string inputText = inputDlg.getInput();

    if(!inputText.empty())
    {
        name = inputText;
        count++; // reset count if operation canceled
        res = true;
    }
    return res ;
}


//------------------------------------------------------------------------------

void AddLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (image->getBuffer()==NULL )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Add landmarks",
                "Sorry, it is impossible to add image landmarks. There is not loaded image in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    std::string value;
    if ( defineLabel(value) )
    {
        //get landmarks
        ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(  image );
        ::fwData::PointList::sptr landmarks =  image->getFieldSingleElement< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
        SLM_ASSERT("landmarks not instanced", landmarks);

        // create a new point
        ::fwData::Point::sptr newPoint = ::fwComEd::fieldHelper::MedicalImageHelpers::getImageSliceIndices( image );
        // transform slice to mm
        std::transform( newPoint->getRefCoord().begin(),newPoint->getRefCoord().end(),
                image->getSpacing().begin(),
                newPoint->getRefCoord().begin(),
                std::multiplies<double>() );
        std::transform( newPoint->getRefCoord().begin(),newPoint->getRefCoord().end(),
                image->getOrigin().begin(),
                newPoint->getRefCoord().begin(),
                std::plus<double>() );
        // append to landmark
        landmarks->getRefPoints().push_back( newPoint );

        // append to point the label
        ::fwData::String::NewSptr label;
        label->value() = value;
        newPoint->setFieldSingleElement( ::fwComEd::Dictionary::m_labelId , label );

        image->setFieldSingleElement("ShowLandmarks", ::fwData::Boolean::NewSptr(true));

        // notify
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ImageMsg::LANDMARK );
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
    }
}

//------------------------------------------------------------------------------

void AddLandmark::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void AddLandmark::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddLandmark::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void AddLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
