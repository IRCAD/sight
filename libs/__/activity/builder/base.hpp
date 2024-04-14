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

#include <sight/activity/config.hpp>

#include "activity/builder/factory/new.hpp"
#include "activity/builder/registry/detail.hpp"
#include "activity/extension/activity.hpp"

#include <core/base.hpp>

#include <data/activity.hpp>

namespace sight::data
{

class vector;

} // namespace sight::data

namespace sight::activity::builder
{

/**
 * @brief Base class for all activity builder's classes.
 */
class SIGHT_ACTIVITY_CLASS_API base : public core::base_object
{
public:

    using config_id_t = data::activity::config_id_t;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        registry(std::string _functor_key)
        {
            sight::activity::builder::registry::get()->add_factory(
                _functor_key,
                &sight::activity::builder::factory::make<T>
            );
        }
    };

    SIGHT_DECLARE_CLASS(base, core::base_object);

    /**
     * @brief Build an Activity with required data present in currentSelection and defined in configuration.
     * @param[in] _activity_info a structure which contains all the Activity configuration
     * @param[in] _current_selection a vector which contains current selected data.
     * @return specific data Activity for the specified Activity.
     */
    SIGHT_ACTIVITY_API virtual data::activity::sptr build_data(
        const activity::extension::activity_info& _activity_info,
        const CSPTR(data::vector)& _current_selection
    ) const = 0;

protected:

    SIGHT_ACTIVITY_API virtual SPTR(data::vector) type(
        const CSPTR(data::vector) & _current_selection,
        const std::string& _type
    ) const;
};

} // namespace sight::activity::builder
