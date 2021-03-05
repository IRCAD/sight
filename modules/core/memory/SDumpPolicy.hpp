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

#include "modules/memory/config.hpp"

#include <core/memory/IPolicy.hpp>
#include <core/tools/Failed.hpp>

#include <service/IController.hpp>

namespace sight::module::memory
{

class MODULE_MEMORY_CLASS_API SDumpPolicy : public service::IController
{

public:

    SIGHT_DECLARE_SERVICE(SDumpPolicy, ::sight::service::IController)

    MODULE_MEMORY_API SDumpPolicy();

    MODULE_MEMORY_API virtual ~SDumpPolicy();

    /**
     * Configuration example :
       @code{.xml}
       <service ...>
       <config>
         <policy>ValveDump</policy>
         <params>
          <min_free_mem>524288000</min_free_mem>
          <hysteresis_offet>104857600</hysteresis_offet>
         </params>
       </config>
       </service>
       @endcode
     * */
    MODULE_MEMORY_API void configuring() override;

    /// Overrides
    MODULE_MEMORY_API void starting() override;

    /// Overrides
    MODULE_MEMORY_API void stopping() override;

    /// Overrides
    MODULE_MEMORY_API void updating() override;

    /// Overrides
    MODULE_MEMORY_API void swapping() override;

protected:

    typedef std::vector< std::pair< core::memory::IPolicy::ParamNamesType::value_type, std::string > > ParametersType;

    std::string m_policy;
    ParametersType m_policyParams;
};

} // namespace sight::module::memory
