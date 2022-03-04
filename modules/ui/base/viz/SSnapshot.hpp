/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/tools/Failed.hpp>

#include <ui/base/IAction.hpp>

#include <map>
#include <string>

namespace sight::module::ui::base::viz
{

/**
 * @brief   This action allows to snapshot a generic scene
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class MODULE_UI_BASE_CLASS_API SSnapshot : public sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SSnapshot, sight::ui::base::IAction);

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SSnapshot() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~SSnapshot() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Starts action
    void starting() override;

    /// Stops action
    void stopping() override;

    /// Show a file dialog and notify the scene which must be printed.
    void updating() override;

    /**
     * @brief Configure action.
     * @code{.xml}
       <service type="ui::base::IAction" impl="::uiData::SSnapshot" autoConnect="false" />
       @endcode
     */
    void configuring() override;

    void info(std::ostream& _sstream) override;

private:

    /**
     * @name Signals
     * @{
     */

    /// Type of signal to snap shot
    typedef core::com::Signal<void (std::string)> SnappedSignalType;
    static const core::com::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    std::string requestFileName();
};

} // uiVisu
