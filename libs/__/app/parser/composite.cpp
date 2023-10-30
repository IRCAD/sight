/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "app/parser/composite.hpp"

#include <data/composite.hpp>

#include <ranges>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void composite::updating()
{
    SIGHT_FATAL("This method is deprecated, and this, shouldn't be used.");
}

//------------------------------------------------------------------------------

void composite::create_config(core::tools::object::sptr _obj)
{
    // Declaration of attributes values
    const std::string object_build_mode = "src";
    const std::string build_object      = "new";
    const std::string get_object        = "ref";

    data::composite::sptr data_composite = std::dynamic_pointer_cast<data::composite>(_obj);
    SIGHT_ASSERT("The passed object must be a data::composite", data_composite);

    for(const auto& elem : m_cfg)
    {
        if(elem.first == "item")
        {
            const auto build_mode = elem.second.get<std::string>(object_build_mode, build_object);
            SIGHT_ASSERT(
                "The buildMode \"" << build_mode << "\" is not supported, it should be either BUILD_OBJECT"
                                                    "or GET_OBJECT.",
                build_mode == build_object || build_mode == get_object
            );

            const auto key = elem.second.get<std::string>("<xmlattr>.key");

            SIGHT_ASSERT(
                "The xml element \"item\" must have one (and only one) xml sub-element \"object\".",
                elem.second.count("object") == 1
            );

            if(build_mode == build_object)
            {
                // Test if key already exist in composite
                SIGHT_ASSERT(
                    "The key " << key << " already exists in the composite.",
                    data_composite->find(
                        key
                    ) == data_composite->end()
                );

                // Create and manage object config
                auto ctm = app::config_manager::make();
                ctm->app::config_manager::set_config(elem.second);

                m_ctm_container.push_back(ctm);
                ctm->create();
                data::object::sptr local_obj = ctm->get_config_root();

                // Add object
                SIGHT_ASSERT("A data::composite can contain only data::object", local_obj);
                (*data_composite)[key] = local_obj;
            }
            else // if( buildMode == GET_OBJECT )
            {
                SIGHT_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void composite::start_config()
{
    for(const app::config_manager::sptr& ctm : m_ctm_container)
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void composite::update_config()
{
    for(const app::config_manager::sptr& ctm : m_ctm_container)
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void composite::stop_config()
{
    for(auto& ctm : std::views::reverse(m_ctm_container))
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void composite::destroy_config()
{
    for(auto& ctm : std::views::reverse(m_ctm_container))
    {
        ctm->destroy();
    }

    m_ctm_container.clear();
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
