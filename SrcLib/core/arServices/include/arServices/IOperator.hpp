/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSERVICES_IOPERATOR_HPP__
#define __ARSERVICES_IOPERATOR_HPP__

#include "arServices/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwServices/IService.hpp>

namespace arServices
{

/**
 * @brief   This interface defines operator service API.

 * @section Slots Slots
 * - \b setBoolParameter() : Receive the value of a boolean parameter along with its identifier.
 * - \b setDoubleParameter() : Receive the value of a double parameter along with its identifier.
 * - \b setIntParameter() : Receive the value of a integer parameter along with its identifier.
 */
class ARSERVICES_CLASS_API IOperator : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IOperator)(::fwServices::IService) );

    ARSERVICES_API static const ::fwCom::Signals::SignalKeyType s_COMPUTED_SIG;
    typedef ::fwCom::Signal<void ()> ComputedSignalType;

    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    ARSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;

protected:

    /// IOperator constructor. Do nothing.
    ARSERVICES_API IOperator();

    /// IOperator destructor. Do nothing.
    ARSERVICES_API virtual ~IOperator();

    /// Slot: This method is used to set an boolean parameter.
    virtual void setBoolParameter(std::string key, bool val);

    /// Slot: This method is used to set a double parameter.
    virtual void setDoubleParameter(std::string key, double val);

    /// Slot: This method is used to set an integer parameter.
    virtual void setIntParameter(std::string key, int val);

    ComputedSignalType::sptr m_sigComputed; ///< Signal emitted when operator is computed.
};

}

#endif // __ARSERVICES_IOPERATOR_HPP__
