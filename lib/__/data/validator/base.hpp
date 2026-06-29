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

#include <sight/data/config.hpp>

#include "data/object.hpp"
#include "data/validator/factory/new.hpp"
#include "data/validator/registry/detail.hpp"

#include <core/base.hpp>
#include <core/com/signals.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::data
{

class object;

} // namespace sight::data

namespace sight::data::validator
{

using config_t = boost::property_tree::ptree;
using return_t = std::pair<bool, std::string>;

/**
 * @brief Base class for all data validator's classes.
 */
class SIGHT_DATA_CLASS_API base : public core::base_object
{
public:

    using auto_connect_signals_t = std::vector<sight::core::com::signals::key_t>;

    /**
     * @brief Class used to register a class factory in the validator registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        explicit registry(std::string _functor_key)
        {
            sight::data::validator::registry::get()->add_factory(
                _functor_key,
                &sight::data::validator::factory::make<T>
            );
        }
    };
    SIGHT_DECLARE_CLASS(base, core::base_object);

    /**
     * @brief Configures the validator with the given configuration.
     * @param _config configuration to apply to the validator, this is used to initialize the validator.
     */
    SIGHT_DATA_API virtual void configure(const config_t& _config);

    /**
     * @brief Performs the validation of the given data.
     * @note  This data could be a single data, or a Vector or a Map of one type of data.
     */
    SIGHT_DATA_API virtual return_t validate(const CSPTR(data::object)& _current_data) const = 0;

    /**
     * @brief Allows to specify the signals that require a new validation. Default to sight::data::signals::MODIFIED.
     */
    SIGHT_DATA_API virtual auto_connect_signals_t auto_connect_signals() const;
};

//------------------------------------------------------------------------------

inline void base::configure(const config_t& /*_config*/)
{
}

//------------------------------------------------------------------------------

inline base::auto_connect_signals_t base::auto_connect_signals() const
{
    return {sight::data::signals::MODIFIED};
}

} // namespace sight::data::validator
