/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/factory.hpp"
#include "ui/__/factory/new.hpp"

#include <core/base.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::ui
{

using config_t = boost::property_tree::ptree;

/**
 * @brief Base class for all fwGui's classes
 */
class UI_CLASS_API object : public sight::core::base_object
{
public:

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
            ui::registry::get()->add_factory(_functor_key, &ui::factory::make<T>);
        }
    };

    SIGHT_DECLARE_CLASS(object, core::base_object);

protected:

    object()           = default;
    ~object() override = default;
};

} // namespace sight::ui
