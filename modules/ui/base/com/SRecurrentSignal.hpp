/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/base/config.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/HiResClock.hpp>
#include <core/thread/Timer.hpp>

#include <service/IController.hpp>

namespace sight::module::ui::base::com
{

/**
 * @brief   SRecurrentSignal service emits a signal every X milliseconds (X is configurable, default 100).
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service impl="sight::module::ui::base::com::SRecurrentSignal" type="::service::IController">
       <timeStep>30</timeStep>
   </service>
   @endcode
 * - \b timeStep : step in milliseconds
 *
 * @section Signals Signals
 * - \b repeated(core::HiResClock::HiResClockType)    : This signal is emitted every X milliseconds.
 */
class MODULE_UI_BASE_CLASS_API SRecurrentSignal : public sight::service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SRecurrentSignal, sight::service::IController);

    /**
     * @brief Constructor.
     */
    MODULE_UI_BASE_API SRecurrentSignal() noexcept;

    /**
     * @brief Destructor.
     */
    ~SRecurrentSignal() noexcept override =
        default;

    /**
     * @name Signals API
     * @{
     */
    MODULE_UI_BASE_API static const sight::core::com::Signals::SignalKeyType s_REPEATED_SIGNAL;
    typedef sight::core::com::Signal<void (sight::core::HiResClock::HiResClockType timestamp)> RepeatedSignalType;

    MODULE_UI_BASE_API static const sight::core::com::Signals::SignalKeyType s_REPEAT_STOPPED_SIGNAL;
    typedef sight::core::com::Signal<void ()> RepeatStoppedSignalType;

    ///@}

protected:

    /// Configures service
    MODULE_UI_BASE_API void configuring() override;

    /// This method is used to initialize the timer.
    MODULE_UI_BASE_API void starting() override;

    /// Stops the timer.
    MODULE_UI_BASE_API void stopping() override;

    /// Emits a new timestamp.
    MODULE_UI_BASE_API void updating() override;

private:

    unsigned int m_timeStep {100}; ///< Time step used for the update

    sight::core::thread::Timer::sptr m_timer; ///< Timer used for the update

    RepeatedSignalType::sptr m_sigRepeated; ///< signal to emit the timestamp
};

} // namespace sight::module::ui::base::com
