/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/memory/policy/base.hpp>
#include <core/tools/failed.hpp>

#include <service/controller.hpp>

namespace sight::module::memory
{

/**
 * @brief Configures the dumping policy.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service ...>
       <config>
           <policy>ValveDump</policy>
           <params>
               <min_free_mem>524288000</min_free_mem>
               <hysteresis_offset>104857600</hysteresis_offset>
           </params>
       </config>
   </service>
 * @endcode
 */
class dump_policy : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(dump_policy, sight::service::controller);

    dump_policy();

    ~dump_policy() override;

    void configuring() override;

    /// Overrides
    void starting() override;

    /// Overrides
    void stopping() override;

    /// Overrides
    void updating() override;

protected:

    using parameters_t = std::vector<std::pair<core::memory::policy::base::param_names_type::value_type, std::string> >;

    std::string m_policy;
    parameters_t m_policy_params;
};

} // namespace sight::module::memory
