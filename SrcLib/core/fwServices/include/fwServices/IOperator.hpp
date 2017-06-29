/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_IOPERATOR_HPP__
#define __FWSERVICES_IOPERATOR_HPP__

#include "fwServices/config.hpp"
#include "fwServices/IParametersService.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

namespace fwServices
{

/**
 * @brief   This interface defines operator service API.

 * @section Signals Signals
 * - \b computed() : Emitted when something was computed.
 */
class FWSERVICES_CLASS_API IOperator : public ::fwServices::IParametersService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IOperator)(::fwServices::IParametersService) );

    FWSERVICES_API static const ::fwCom::Signals::SignalKeyType s_COMPUTED_SIG;
    typedef ::fwCom::Signal<void ()> ComputedSignalType;

protected:

    /// IOperator constructor. Do nothing.
    FWSERVICES_API IOperator();

    /// IOperator destructor. Do nothing.
    FWSERVICES_API virtual ~IOperator();

    ComputedSignalType::sptr m_sigComputed; ///< Signal emitted when operator is computed.
};

}

#endif // __FWSERVICES_IOPERATOR_HPP__
