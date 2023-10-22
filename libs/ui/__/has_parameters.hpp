/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "ui/__/config.hpp"

#include <core/com/slots.hpp>

#include <ui/__/parameter.hpp>

#include <array>
#include <cstdint>

namespace sight::ui
{

/**
 * @brief   This interface defines default slots to receive parameter values from signals.
 *
 * It is designed to be used in conjunction with sight::module::ui::editor::parameters, to help receiving
 * parameters. Instead of redefining all slots, thus you can simply inherit from this interface and override the virtual
 * function you need. Then connect the module::ui::editor::parameters signal(s) with the slot(s) of this
 * interface in the* configuration.

 * @section Slots Slots
 * - \b setBoolParameter(bool, std::string) : Receive a boolean parameter along with the identifier.
 * - \b setColorParameter(std::array<std::uint8_t, 4>, std::string) : Receive a color along with the identifier.
 * - \b setDoubleParameter(double, std::string) : Receive a real parameter along with the identifier.
 * - \b setDouble2Parameter(double, double, std::string) : Receive two real parameters along with the identifier.
 * - \b setDouble3Parameter(double, double, double, std::string) : Receive three real parameters along with the
 * identifier.
 * - \b setIntParameter(int, std::string) : Receive an integer parameter along with the identifier.
 * - \b setInt2Parameter(int, int, std::string) : Receive two integer parameters along with the identifier.
 * - \b setInt3Parameter(int, int, int, std::string) : Receive three integer parameters along with the identifier.
 * - \b setEnumParameter(std::string, std::string) : Receive an enum parameter along with the identifier.
 */
class UI_CLASS_API has_parameters
{
public:

    UI_API static const core::com::slots::key_t SET_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_COLOR_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_DOUBLE2_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_DOUBLE3_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_INT_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_INT2_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_INT3_PARAMETER_SLOT;
    UI_API static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT;

protected:

    /// Constructor. Create all default slots.
    /// @param _slots map of slots of the child class. For services, just pass the protected variable base::m_slots.
    UI_API has_parameters(sight::core::com::slots& _slots);

    /// Destructor. Do nothing.
    UI_API virtual ~has_parameters();

    /// Slot: This method is used to set an boolean parameter.
    UI_API virtual void setParameter(sight::ui::parameter_t _val, std::string _key);

    /// Slot: This method is used to set an boolean parameter.
    UI_API virtual void setBoolParameter(bool _val, std::string _key);

    /// Slot: This method is used to set a color parameter.
    UI_API virtual void setColorParameter(std::array<std::uint8_t, 4> _color, std::string _key);

    /// Slot: This method is used to set a double parameter.
    UI_API virtual void setDoubleParameter(double _val, std::string _key);

    /// Slot: This method is used to set two double parameters.
    UI_API virtual void setDouble2Parameter(double _val0, double _val1, std::string _key);

    /// Slot: This method is used to set three double parameters.
    UI_API virtual void setDouble3Parameter(double _val0, double _val1, double _val2, std::string _key);

    /// Slot: This method is used to set an integer parameter.
    UI_API virtual void setIntParameter(int _val, std::string _key);

    /// Slot: This method is used to set two int parameters.
    UI_API virtual void setInt2Parameter(int _val0, int _val1, std::string _key);

    /// Slot: This method is used to set three int parameters.
    UI_API virtual void setInt3Parameter(int _val0, int _val1, int _val2, std::string _key);

    /// Slot: This method is used to set an enum parameter.
    UI_API virtual void setEnumParameter(std::string _val, std::string _key);
};

} // namespace sight::ui
