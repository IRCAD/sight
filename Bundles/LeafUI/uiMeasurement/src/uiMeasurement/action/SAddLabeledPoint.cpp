/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <exception>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/PointListMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>

#include "uiMeasurement/action/SAddLabeledPoint.hpp"

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::SAddLabeledPoint, ::fwData::PointList );


//------------------------------------------------------------------------------


SAddLabeledPoint::SAddLabeledPoint( ) throw()
    : m_count(1)
{
}

//------------------------------------------------------------------------------

SAddLabeledPoint::~SAddLabeledPoint() throw()
{
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::info(std::ostream &_sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

// return true if label setting is NOT Canceled , name is modified !!!
bool SAddLabeledPoint::defineLabel(std::string &name)
{
    bool res = false;
    name = "Label" + ::boost::lexical_cast< std::string >(m_count);

    ::fwData::String::sptr url = ::fwData::String::New();
    ::fwGui::dialog::InputDialog inputDlg;
    inputDlg.setTitle("Label");
    inputDlg.setMessage("Which label for the point?");
    inputDlg.setInput (name);
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

void SAddLabeledPoint::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::PointList::sptr landmarks = this->getObject< ::fwData::PointList >();
    SLM_ASSERT("landmarks not instanced", landmarks);

    std::string value;
    if ( this->defineLabel(value) )
    {
        // create a new point
        ::fwData::Point::sptr newPoint = ::fwData::Point::New();

        // append to landmark
        landmarks->getRefPoints().push_back( newPoint );

        // append to point the label
        ::fwData::String::sptr label = ::fwData::String::New();
        label->value()               = value;
        newPoint->setField( ::fwComEd::Dictionary::m_labelId, label );

        // notify
        ::fwComEd::PointListMsg::sptr msgPointList = ::fwComEd::PointListMsg::New();
        msgPointList->addEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED, newPoint );
        msgPointList->setSource( this->getSptr());
        msgPointList->setSubject( landmarks);
        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = landmarks->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
            sig->asyncEmit( msgPointList);
        }
    }
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SAddLabeledPoint::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
