/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/SAddLabeledPoint.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::SAddLabeledPoint, ::fwData::PointList );

static const ::fwServices::IService::KeyType s_POINTLIST_INOUT = "pointlist";

//------------------------------------------------------------------------------

SAddLabeledPoint::SAddLabeledPoint( ) noexcept :
    m_count(1)
{
}

//------------------------------------------------------------------------------

SAddLabeledPoint::~SAddLabeledPoint() noexcept
{
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::info(std::ostream& _sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

// return true if label setting is NOT Canceled , name is modified !!!
bool SAddLabeledPoint::defineLabel(std::string& name)
{
    bool res = false;
    name = "Label" + ::boost::lexical_cast< std::string >(m_count);

    ::fwData::String::sptr url = ::fwData::String::New();
    ::fwGui::dialog::InputDialog inputDlg;
    inputDlg.setTitle("Label");
    inputDlg.setMessage("Which label for the point?");
    inputDlg.setInput(name);
    std::string inputText = inputDlg.getInput();

    if(!inputText.empty())
    {
        name = inputText;
        m_count++;
        res = true;
    }
    return res;
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::updating()
{
    ::fwData::PointList::sptr landmarks = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("The inout key '" + s_POINTLIST_INOUT + "' is not defined.", landmarks);

    std::string value;
    if ( this->defineLabel(value) )
    {
        // create a new point
        ::fwData::Point::sptr newPoint = ::fwData::Point::New();

        // append to landmark
        landmarks->getPoints().push_back( newPoint );

        // append to point the label
        ::fwData::String::sptr label = ::fwData::String::New();
        label->value()               = value;
        newPoint->setField( ::fwDataTools::fieldHelper::Image::m_labelId, label );

        // notify
        auto sig =
            landmarks->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit(newPoint);
        }
    }
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
