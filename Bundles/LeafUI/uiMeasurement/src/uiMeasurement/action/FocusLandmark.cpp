/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/FocusLandmark.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::FocusLandmark, ::fwData::Image );

//------------------------------------------------------------------------------

FocusLandmark::FocusLandmark( ) throw()
{
}

//------------------------------------------------------------------------------

FocusLandmark::~FocusLandmark() throw()
{
}

//------------------------------------------------------------------------------

void FocusLandmark::starting() throw ( ::fwTools::Failed )
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void FocusLandmark::stopping() throw ( ::fwTools::Failed )
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void FocusLandmark::configuring() throw ( ::fwTools::Failed )
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void FocusLandmark::info(std::ostream& _sstream )
{
    _sstream << "Action for focus a landmark distance" << std::endl;
}

//------------------------------------------------------------------------------

void FocusLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(pImage))
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Add landmarks");
        messageBox.setMessage(
            "It is impossible to add image landmarks. There is no loaded image in the software." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
        return;
    }

    else // Image is defined
    {
        // get landmarks
        namespace ns = ::fwDataTools::fieldHelper;
        ns::MedicalImageHelpers::checkLandmarks(  pImage );
        ::fwData::PointList::sptr landmarks = pImage->getField< ::fwData::PointList >(
            ::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
        SLM_ASSERT("landmarks not instanced", landmarks);

        if( landmarks->getCRefPoints().empty() )
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Focus landmarks");
            messageBox.setMessage(
                "It is impossible to focus image landmarks. There are no defined landmarks for this selected image." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        else
        {
            // Retrieve point names
            std::vector< std::string > names;
            std::map< std::string, ::fwData::Point::sptr > name2Point;

            ::fwData::PointList::PointListContainer points = landmarks->getCRefPoints();
            for(::fwData::Point::sptr point :  points)
            {
                std::string name =
                    point->getField< ::fwData::String >( ::fwDataTools::fieldHelper::Image::m_labelId )->value();
                OSLM_DEBUG( "Point name " << name );
                names.push_back( name );
                name2Point[name] = point;
            }

            // Propose to user to choose a landmark
            ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();
            selector->setTitle("Select a landmark");
            selector->setSelections(names);
            std::string selection = selector->show();
            if( !selection.empty() )
            {
                ::fwData::Point::sptr selectedPoint = name2Point[ selection ];
                SLM_ASSERT("selectedPoint not instanced", selectedPoint);
                ::fwData::Integer::sptr paramA = ::fwData::Integer::New();
                paramA->value()                =
                    static_cast<int>((selectedPoint->getRefCoord()[2] - pImage->getOrigin()[2] )/
                                     pImage->getSpacing()[2] +0.5);
                ::fwData::Integer::sptr paramF = ::fwData::Integer::New();
                paramF->value()                =
                    static_cast<int>((selectedPoint->getRefCoord()[1] -  pImage->getOrigin()[1])/
                                     pImage->getSpacing()[1] +0.5);
                ::fwData::Integer::sptr paramS = ::fwData::Integer::New();
                paramS->value()                =
                    static_cast<int>((selectedPoint->getRefCoord()[0] -  pImage->getOrigin()[0])/
                                     pImage->getSpacing()[0] +0.5);
                if( paramS->value() >= 0 &&
                    paramF->value() >= 0 &&
                    paramA->value() >= 0 &&
                    pImage->getSize()[0] > paramS->value() &&
                    pImage->getSize()[1] > paramF->value() &&
                    pImage->getSize()[2] > paramA->value() )
                {
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, paramA );
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, paramF );
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, paramS );

                    // notify
                    auto sig = pImage->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
                        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
                    sig->asyncEmit(paramA->value(), paramF->value(), paramS->value());
                }
                else
                {
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Focus landmarks",
                                                                      "It is impossible to focus image landmarks: "
                                                                      "landmark is outside image.",
                                                                      ::fwGui::dialog::IMessageDialog::WARNING);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
