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

#include "app/parser/vector.hpp"

#include <data/vector.hpp>

#include <ranges>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void vector::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void vector::createConfig(core::tools::object::sptr _obj)
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    auto vector = std::dynamic_pointer_cast<data::vector>(_obj);
    SIGHT_ASSERT("The passed object must be a data::vector", vector);

    for(const auto& elem : m_cfg)
    {
        if(elem.first == "item")
        {
            const auto buildMode = elem.second.get<std::string>(OBJECT_BUILD_MODE, BUILD_OBJECT);
            SIGHT_ASSERT(
                "The buildMode \"" << buildMode << "\" is not supported, it should be either BUILD_OBJECT"
                                                   "or GET_OBJECT.",
                buildMode == BUILD_OBJECT || buildMode == GET_OBJECT
            );

            if(buildMode == BUILD_OBJECT)
            {
                // Create and manage object config
                auto ctm = app::config_manager::make();
                ctm->app::config_manager::setConfig(elem.second);

                m_ctmContainer.push_back(ctm);
                ctm->create();
                data::object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SIGHT_ASSERT("A data::vector can contain only data::object", localObj);
                vector->push_back(localObj);
            }
            else // if( buildMode == GET_OBJECT )
            {
                SIGHT_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void vector::startConfig()
{
    for(const app::config_manager::sptr& ctm : m_ctmContainer)
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void vector::updateConfig()
{
    for(const app::config_manager::sptr& ctm : m_ctmContainer)
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void vector::stopConfig()
{
    for(auto& ctm : std::views::reverse(m_ctmContainer))
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void vector::destroyConfig()
{
    for(auto& ctm : std::views::reverse(m_ctmContainer))
    {
        ctm->destroy();
    }

    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
