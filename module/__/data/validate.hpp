/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/object.hpp>
#include <data/validator/base.hpp>

#include <service/controller.hpp>

#include <optional>

namespace sight::module::data
{

/**
 * @brief Generic service to validate data object given a specified validator class.
 * The validator inherits from sight::data::validator::base and registered with the macro SIGHT_REGISTER_DATA_VALIDATOR.
 * The service auto_connects all signals declared by the validator with the function auto_connect_signals() to the
 * update slot, triggering a validation check.
 *
 * @section Signals Signals
 * - \b valid() : Emitted when the object is valid given the specified validator.
 * - \b invalid() : Emitted when the object is invalid given the specified validator.
 * - \b is_valid(bool) : Emitted with true if the object is valid, false otherwise.
 * - \b is_invalid(bool) : Emitted with true if the object is invalid, false otherwise.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::validate" >
           <in key="data" uid="..."/>
           <config id="..." />
        </service>
       @endcode
 * @subsection Input Input
 * - \b data [sight::data::object]: data object to validate.
 *
 * @subsection Inout Inout
 * - \b valid [sight::data::boolean] (optional): true if valid, false otherwise.
 * - \b invalid [sight::data::boolean] (optional): true if invalid, false otherwise.
 *
 * @subsection Configuration Configuration
 * - \b id (string): class identifier of the validator
 *
 * @subsection Properties Properties
 * - \b on_change (sight::data::boolean): if true, signals are only emitted when the validation state changes
 * (default: false).
 */
class validate final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(validate, service::controller);

    struct signals
    {
        using void_t = sight::core::com::signal<void ()>;
        using bool_t = sight::core::com::signal<void (bool)>;

        inline static const sight::core::com::signals::key_t VALID      = "valid";
        inline static const sight::core::com::signals::key_t INVALID    = "invalid";
        inline static const sight::core::com::signals::key_t IS_VALID   = "is_valid";
        inline static const sight::core::com::signals::key_t IS_INVALID = "is_invalid";
    };

    /// Constructor
    validate();

    /// Destructor
    ~validate() final = default;

protected:

    /// Configures the service
    void configuring(const service::config_t& _config) final;

    /// Does nothing
    void starting() final;

    /// Does nothing
    void stopping() final;

    /// Validates the data object and emits the result signals
    void updating() final;

    /// Updates when object is modified, according to the validator auto_connect_signals() function
    service::connections_t auto_connections() const final;

private:

    sight::data::validator::base::sptr m_validator;
    sight::data::ptr<sight::data::object, sight::data::access::in> m_data {this, "data"};
    sight::data::property<sight::data::boolean> m_on_change {this, "on_change", false};

    /// Store the validation state and track changes
    sight::data::ptr<sight::data::boolean, sight::data::access::inout> m_valid {this, "valid", true};
    sight::data::ptr<sight::data::boolean, sight::data::access::inout> m_invalid {this, "invalid", true};

    /// Tracks the previous validation result to detect state changes
    std::optional<bool> m_previous_result;
};

} // namespace sight::module::data
