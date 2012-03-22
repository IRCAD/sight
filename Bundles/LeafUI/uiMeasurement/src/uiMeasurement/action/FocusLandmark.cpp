/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include "uiMeasurement/action/FocusLandmark.hpp"

namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::FocusLandmark , ::fwData::Image ) ;

//------------------------------------------------------------------------------

FocusLandmark::FocusLandmark( ) throw()
{}

//------------------------------------------------------------------------------

FocusLandmark::~FocusLandmark() throw()
{}

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

void FocusLandmark::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{
    // Do nothing. this method must be not used.
    SLM_FATAL("Action should not receive a message, correct your configuration");
}

//------------------------------------------------------------------------------

void FocusLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for focus a landmark distance" << std::endl;
}

//------------------------------------------------------------------------------

void FocusLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    if ( pImage->getBuffer()==NULL )
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Add landmarks");
        messageBox.setMessage( "Sorry, it is impossible to add image landmarks. There is not loaded image in the software." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
        return;
    }

    else // Image is defined
    {
        // get landmarks
        namespace ns = ::fwComEd::fieldHelper;
        ns::MedicalImageHelpers::checkLandmarks(  pImage );
        ::fwData::PointList::sptr landmarks =  pImage->getField< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);
        SLM_ASSERT("landmarks not instanced", landmarks);

        if( landmarks->getCRefPoints().empty() )
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Focus landmarks");
            messageBox.setMessage( "Sorry, it is impossible to focus image landmarks. There are not defined landmarks for this selected image." );
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
            BOOST_FOREACH(::fwData::Point::sptr point, points)
            {
                std::string name =  point->getField< ::fwData::String >( ::fwComEd::Dictionary::m_labelId )->value();
                OSLM_DEBUG( "Point name " << name );
                names.push_back( name );
                name2Point[name] = point;
            }

            // Propose to user to choose a landmark
            ::fwGui::dialog::SelectorDialog::NewSptr selector;
            selector->setTitle("Select a landmark");
            selector->setSelections(names);
            std::string selection = selector->show();
            if( ! selection.empty() )
            {
                ::fwData::Point::sptr selectedPoint = name2Point[ selection ];
                SLM_ASSERT("selectedPoint not instanced", selectedPoint);
                ::fwData::Integer::NewSptr paramA;
                paramA->value() = static_cast<int>((selectedPoint->getRefCoord()[2] - pImage->getOrigin()[2] )/  pImage->getSpacing()[2] +0.5);
                ::fwData::Integer::NewSptr paramF;
                paramF->value() = static_cast<int>((selectedPoint->getRefCoord()[1] -  pImage->getOrigin()[1])/  pImage->getSpacing()[1] +0.5);
                ::fwData::Integer::NewSptr paramS;
                paramS->value() = static_cast<int>((selectedPoint->getRefCoord()[0] -  pImage->getOrigin()[0])/  pImage->getSpacing()[0] +0.5);
                if( paramS->value() >= 0 &&
                        paramF->value() >= 0 &&
                        paramA->value() >= 0 &&
                        pImage->getSize()[0] > paramS->value() &&
                        pImage->getSize()[1] > paramF->value() &&
                        pImage->getSize()[2] > paramA->value() )
                {
                    pImage->setField( ::fwComEd::Dictionary::m_axialSliceIndexId, paramA );
                    pImage->setField( ::fwComEd::Dictionary::m_frontalSliceIndexId, paramF );
                    pImage->setField( ::fwComEd::Dictionary::m_sagittalSliceIndexId, paramS );

                    // notify
                    ::fwComEd::ImageMsg::NewSptr msg;
                    msg->setSliceIndex( paramA, paramF, paramS );
                    ::fwServices::IEditionService::notify(this->getSptr(), pImage, msg);
                }
                else
                {
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Focus landmarks",
                            "Sorry, it is impossible to focus image landmarks: landmark outside image.",
                            ::fwGui::dialog::IMessageDialog::WARNING);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
