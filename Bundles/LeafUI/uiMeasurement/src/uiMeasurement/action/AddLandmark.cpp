/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/AddLandmark.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::AddLandmark, ::fwData::Image );

//------------------------------------------------------------------------------

AddLandmark::AddLandmark( ) noexcept
{
}

//------------------------------------------------------------------------------

AddLandmark::~AddLandmark() noexcept
{
}

//------------------------------------------------------------------------------

void AddLandmark::info(std::ostream& _sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

// return true if label setting is NOT Canceled , name is modified !!!
bool defineLabel(std::string& name)
{
    bool res         = false;
    static int count = 1;
    name = "Label" + ::boost::lexical_cast< std::string >(count );

    ::fwData::String::sptr url = ::fwData::String::New();
    ::fwGui::dialog::InputDialog inputDlg;
    inputDlg.setTitle("Label");
    inputDlg.setMessage("Which label for the point?");
    inputDlg.setInput(name);
    std::string inputText = inputDlg.getInput();

    if(!inputText.empty())
    {
        name = inputText;
        count++; // reset count if operation canceled
        res = true;
    }
    return res;
}

//------------------------------------------------------------------------------

void AddLandmark::updating()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >("image");

    if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Add landmarks",
            "It is impossible to add image landmarks. There is no loaded image in the software.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    std::string value;
    if ( defineLabel(value) )
    {
        //get landmarks
        ::fwDataTools::fieldHelper::MedicalImageHelpers::checkLandmarks(  image );
        ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >(
            ::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
        SLM_ASSERT("landmarks not instanced", landmarks);

        // create a new point
        ::fwData::Point::sptr newPoint = ::fwDataTools::fieldHelper::MedicalImageHelpers::getImageSliceIndices( image );
        // transform slice to mm
        std::transform( newPoint->getRefCoord().begin(), newPoint->getRefCoord().end(),
                        image->getSpacing().begin(),
                        newPoint->getRefCoord().begin(),
                        std::multiplies<double>() );
        std::transform( newPoint->getRefCoord().begin(), newPoint->getRefCoord().end(),
                        image->getOrigin().begin(),
                        newPoint->getRefCoord().begin(),
                        std::plus<double>() );
        // append to landmark
        landmarks->getRefPoints().push_back( newPoint );

        // append to point the label
        ::fwData::String::sptr label = ::fwData::String::New();
        label->value()               = value;
        newPoint->setField( ::fwDataTools::fieldHelper::Image::m_labelId, label );

        image->setField("ShowLandmarks", ::fwData::Boolean::New(true));

        // notify
        auto sig = image->signal< ::fwData::Image::LandmarkAddedSignalType >(::fwData::Image::s_LANDMARK_ADDED_SIG);
        sig->asyncEmit(newPoint);
    }
}

//------------------------------------------------------------------------------

void AddLandmark::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void AddLandmark::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
