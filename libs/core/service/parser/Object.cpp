/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/parser/Object.hpp"

#include <boost/foreach.hpp>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

void Object::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus, shouldn't be used.");
}

//------------------------------------------------------------------------------

void Object::createConfig(core::tools::Object::sptr _obj)
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    data::Object::sptr associatedObject = data::Object::dynamicCast(_obj);
    SIGHT_ASSERT("associatedObject not instanced", associatedObject);

    const auto config = m_cfg.get_child("object");
    for(const auto& elem : config)
    {
        if(elem.first == "item")
        {
            const auto buildMode = elem.second.get<std::string>(OBJECT_BUILD_MODE, BUILD_OBJECT);
            SIGHT_ASSERT(
                "The buildMode \"" << buildMode << "\" is not supported, it should be either BUILD_OBJECT"
                                                   "or GET_OBJECT.",
                buildMode == BUILD_OBJECT || buildMode == GET_OBJECT
            );

            const auto key = elem.second.get<std::string>("<xmlattr>.key");
            SIGHT_ASSERT(
                "The xml element \"item\" must have one (and only one) xml sub-element \"object\".",
                elem.second.count("object") == 1
            );

            if(buildMode == BUILD_OBJECT)
            {
                // Test if key already exist in object
                SIGHT_ASSERT(
                    "The key " << key << " already exists in the object.",
                    !associatedObject->getField(
                        key
                    )
                );

                // Create and manage object config
                auto ctm = service::AppConfigManager::New();
                ctm->service::IAppConfigManager::setConfig(config);

                m_ctmContainer.push_back(ctm);
                ctm->create();
                data::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                associatedObject->setField(key, localObj);
            }
            else // if( buildMode == GET_OBJECT )
            {
                SIGHT_FATAL("ACH => Todo");
                // ToDo
            }
        }
    }
}

//------------------------------------------------------------------------------

void Object::startConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Object::updateConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Object::stopConfig()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(service::IAppConfigManager::sptr ctm, m_ctmContainer)
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Object::destroyConfig()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(service::IAppConfigManager::sptr ctm, m_ctmContainer)
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} // namespace sight::service::parser
