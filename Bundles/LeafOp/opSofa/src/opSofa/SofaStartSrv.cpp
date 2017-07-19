/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>

#include <fwMedData/ModelSeries.hpp>

#include "opSofa/SofaStartSrv.hpp"

namespace opSofa
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::opSofa::SofaStartSrv, ::fwMedData::ModelSeries );

/**
 * @brief Constructor
 */
SofaStartSrv::SofaStartSrv() noexcept
{
}

/**
 * @brief Destructor
 */
SofaStartSrv::~SofaStartSrv() noexcept
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaStartSrv::configuring()
{
}

/**
 * @brief Used to launch the service.
 */
void SofaStartSrv::starting()
{
}

/**
 * @brief Used to stop the service.
 */
void SofaStartSrv::stopping()
{
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param msg : Incoming message
 */
void SofaStartSrv::receiving( ::fwServices::ObjectMsg::csptr msg )
{
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaStartSrv::updating()
{
    ::fwMedData::ModelSeries::sptr ms = this->getObject< ::fwMedData::ModelSeries >();
    SLM_ASSERT("Invalid object", ms);

    // Create message start or stop Sofa, half the time
    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent("START_STOP_SOFA");

    // Send message
    msg->setSource(this->getSptr());
    msg->setSubject( ms);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = ms->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
}

/**
 * @brief info of the class
 */
void SofaStartSrv::info ( std::ostream &_sstream )
{
}

}
