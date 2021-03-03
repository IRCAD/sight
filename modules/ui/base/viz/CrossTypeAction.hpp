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
 * @brief   This action allows change the cross type
 */
class MODULE_UI_BASE_CLASS_API CrossTypeAction : public ::sight::ui::base::IAction
{

public:

    SIGHT_DECLARE_SERVICE(CrossTypeAction, ::sight::ui::base::IAction)

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API CrossTypeAction() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~CrossTypeAction() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Starts action
    virtual void starting() override;

    /// Stops action
    virtual void stopping() override;

    /// Notifies message to change cross type
    void updating() override;

    /// Does nothing
    void swapping() override;

    /**
     * @brief Configure action.
     * @code{.xml}
       <service type="ui::base::IAction" impl="::uiData::CrossTypeAction" autoConnect="no">
        <crossType>hide</crossType>
       </service>
       @endcode
     * - \b crossType : must be full, half or hide
     */
    void configuring() override;

    virtual void info( std::ostream& _sstream ) override;

private:

    static std::map< std::string, float > m_scaleConversion;
    std::string m_crossType;

    typedef core::com::Signal< void (double) > CrossTypeModifiedSignalType;
    CrossTypeModifiedSignalType::sptr m_sigCrossTypeModified; ///< Signal emitted when cross type is modified
};

} // uiVisu
