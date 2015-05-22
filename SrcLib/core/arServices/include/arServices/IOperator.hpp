/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
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
 *
 * @class   IOperator
 *
 * @date    2015
 */
class ARSERVICES_CLASS_API IOperator : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IOperator)(::fwServices::IService) );

    ARSERVICES_API static const ::fwCom::Signals::SignalKeyType s_COMPUTED_SIG;
    typedef ::fwCom::Signal<void ()> ComputedSignalType;

protected:

    /// IOperator constructor. Do nothing.
    ARSERVICES_API IOperator();

    /// IOperator destructor. Do nothing.
    ARSERVICES_API virtual ~IOperator();

    ComputedSignalType::sptr m_sigComputed; ///< Signal emitted when operator is computed.

};

}

#endif // __ARSERVICES_IOPERATOR_HPP__
