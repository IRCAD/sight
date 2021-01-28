/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "uiGenericQt/config.hpp"

#include <ui/base/dialog/NotificationDialog.hpp>
#include <ui/base/IActionSrv.hpp>

namespace dial = sight::ui::base::dialog;

namespace uiGenericQt
{

namespace action
{

/**
 * @brief  This action displays test notifications by calling udapting().
 *
 * @section Slots Slots
 * - \b setEnumParameter (std::string _value, std::string _key) : call this slot when changing the position or
 * the type of displayed notification( accepted _key are 'position' & 'type').
 *   - Values for 'position' key : ALL, TOP_LEFT, TOP_RIGHT, CENTERED_TOP, CENTERED, BOTTOM_LEFT, BOTTOM_RIGHT,
 * CENTERED_BOTTOM.
 *   - Values for 'type' key : INFO, SUCCESS, FAILURE.
 * - \b setBoolParameterbool _val, std::string _key): call this slot when changing "m_useSNotifier" behavior.
 */
class UIGENERICQT_CLASS_API SDisplayTestNotifications final : public ::sight::ui::base::IActionSrv
{

public:
    fwCoreServiceMacro(SDisplayTestNotifications,  ui::base::IActionSrv)

    /**
     * @name Constructor/Destructor
     * @{ */
    /// Initializes slot.
    UIGENERICQT_API SDisplayTestNotifications() noexcept;
    UIGENERICQT_API virtual ~SDisplayTestNotifications() noexcept override;
    /**  @} */

    /// Slot: This method is used to set an enum parameter.
    UIGENERICQT_API void setEnumParameter(std::string _val, std::string _key);

    /// Slot: This method is used to set a bool parameter.
    UIGENERICQT_API void setBoolParameter(bool _val, std::string _key);

protected:

    /// Initializes IActionSrv.
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Display notification(s).
    void updating() override;

    /// Stops action
    void stopping() override;

    /// Prints service info
    void info(std::ostream& _sstream) override;

private:

    /// Position of the notification relative to active window.
    ::dial::NotificationDialog::Position m_position {::dial::NotificationDialog::Position::DEFAULT};
    /// Type of the notification.
    ::dial::NotificationDialog::Type m_type {::dial::NotificationDialog::Type::DEFAULT};
    /// Display notification at all position, default true.
    bool m_displayAll {false};
    /// Use centralized Mode.
    bool m_useSNotifier {true};

};

} // namespace action

} // namespace uiGenericQt
