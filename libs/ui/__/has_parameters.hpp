/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

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
 * - \b set_bool_parameter(bool, std::string) : Receive a boolean parameter along with the identifier.
 * - \b set_color_parameter(std::array<std::uint8_t, 4>, std::string) : Receive a color along with the identifier.
 * - \b set_double_parameter(double, std::string) : Receive a real parameter along with the identifier.
 * - \b set_double2_parameter(double, double, std::string) : Receive two real parameters along with the identifier.
 * - \b set_double3_parameter(double, double, double, std::string) : Receive three real parameters along with the
 * identifier.
 * - \b set_int_parameter(int, std::string) : Receive an integer parameter along with the identifier.
 * - \b set_int2_parameter(int, int, std::string) : Receive two integer parameters along with the identifier.
 * - \b set_int3_parameter(int, int, int, std::string) : Receive three integer parameters along with the identifier.
 * - \b set_enum_parameter(std::string, std::string) : Receive an enum parameter along with the identifier.
 */
class SIGHT_UI_CLASS_API has_parameters
{
public:

    SIGHT_UI_API static const core::com::slots::key_t SET_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_COLOR_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_DOUBLE2_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_DOUBLE3_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_INT_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_INT2_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_INT3_PARAMETER_SLOT;
    SIGHT_UI_API static const core::com::slots::key_t SET_ENUM_PARAMETER_SLOT;

protected:

    /// Constructor. Create all default slots.
    /// @param _slots map of slots of the child class. For services, just pass the protected variable base::m_slots.
    SIGHT_UI_API has_parameters(sight::core::com::slots& _slots);

    /// Destructor. Do nothing.
    SIGHT_UI_API virtual ~has_parameters();

    /// Slot: This method is used to set an boolean parameter.
    SIGHT_UI_API virtual void set_parameter(sight::ui::parameter_t _val, std::string _key);

    /// Slot: This method is used to set an boolean parameter.
    SIGHT_UI_API virtual void set_bool_parameter(bool _val, std::string _key);

    /// Slot: This method is used to set a color parameter.
    SIGHT_UI_API virtual void set_color_parameter(std::array<std::uint8_t, 4> _color, std::string _key);

    /// Slot: This method is used to set a double parameter.
    SIGHT_UI_API virtual void set_double_parameter(double _val, std::string _key);

    /// Slot: This method is used to set two double parameters.
    SIGHT_UI_API virtual void set_double2_parameter(double _val0, double _val1, std::string _key);

    /// Slot: This method is used to set three double parameters.
    SIGHT_UI_API virtual void set_double3_parameter(double _val0, double _val1, double _val2, std::string _key);

    /// Slot: This method is used to set an integer parameter.
    SIGHT_UI_API virtual void set_int_parameter(int _val, std::string _key);

    /// Slot: This method is used to set two int parameters.
    SIGHT_UI_API virtual void set_int2_parameter(int _val0, int _val1, std::string _key);

    /// Slot: This method is used to set three int parameters.
    SIGHT_UI_API virtual void set_int3_parameter(int _val0, int _val1, int _val2, std::string _key);

    /// Slot: This method is used to set an enum parameter.
    SIGHT_UI_API virtual void set_enum_parameter(std::string _val, std::string _key);
};

} // namespace sight::ui
