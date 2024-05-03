/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <core/com/signals.hpp>

#include <data/matrix4.hpp>

#include <service/controller.hpp>
#include <service/notifier.hpp>

#include <filesystem>

namespace sight::module::io::matrix
{

/**
 * @brief This service checks if a matrix4 is valid as a transformation matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::validator">
       <in key="matrix" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrix [sight::data::matrix4]: matrix to check.
 *
 * @subsection Signals Signals
 * - \b valid : sent when matrix is valid.
 * - \b invalid : sent when matrix is invalid.
 *
 * @subsection Slots Slots
 * - \b update : Checks input matrix.
 */
class validator final : public sight::service::controller,
                        public sight::service::notifier
{
public:

    struct signals
    {
        using key_t         = sight::core::com::signals::key_t;
        using void_signal_t = sight::core::com::signal<void ()>;

        static inline const key_t VALID   = "valid";
        static inline const key_t INVALID = "invalid";
    };

    SIGHT_DECLARE_SERVICE(validator, sight::service::controller);

    /// Constructor, does nothing.
    validator();

    /// Destructor, does nothing.
    ~validator() final = default;

    /// Super class of reader services
    using super_class = sight::service::controller;

protected:

    void starting() final;
    void stopping() final;
    void configuring() final;
    void updating() final;

private:

    static constexpr std::string_view MATRIX_IN = "matrix";

    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_matrix {this, MATRIX_IN};
};

} // namespace sight::module::io::matrix
