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

#include "service/parser/Composite.hpp"

#include <data/Composite.hpp>

#include <boost/foreach.hpp>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

bool Composite::refObjectValidator(core::runtime::ConfigurationElement::sptr _cfgElement)
{
    bool isOk = true;

    for(auto& configEltIter : *_cfgElement)
    {
        std::string subElementName = configEltIter->getName();
        if(subElementName != "service"
           && subElementName != "serviceList")
        {
            SIGHT_ERROR(
                "xml subelement \"" << subElementName
                << "\" for element object is not supported for the moment when you use a reference on item composite."
            );
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void Composite::updating()
{
    SIGHT_FATAL("This method is deprecated, and this, shouldn't be used.");
}

//------------------------------------------------------------------------------

void Composite::createConfig(core::tools::Object::sptr _obj)
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    data::Composite::sptr dataComposite = data::Composite::dynamicCast(_obj);
    SIGHT_ASSERT("The passed object must be a data::Composite", dataComposite);

    for(core::runtime::ConfigurationElement::csptr elem : m_cfg->getElements())
    {
        if(elem->getName() == "item")
        {
            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if(elem->hasAttribute(OBJECT_BUILD_MODE))
            {
                buildMode = elem->getExistingAttributeValue(OBJECT_BUILD_MODE);
                SIGHT_ASSERT(
                    "The buildMode \"" << buildMode << "\" is not supported, it should be either BUILD_OBJECT"
                                                       "or GET_OBJECT.",
                    buildMode == BUILD_OBJECT || buildMode == GET_OBJECT
                );
            }

            SIGHT_ASSERT(
                "The xml element \"item\" must have an attribute named \"key\" .",
                elem->hasAttribute("key")
            );
            std::string key = elem->getExistingAttributeValue("key");
            SIGHT_ASSERT(
                "The xml element \"item\" must have an attribute named \"key\" which is not empty.",
                !key.empty()
            );
            SIGHT_ASSERT(
                "The xml element \"item\" must have one (and only one) xml sub-element \"object\".",
                elem->size() == 1 && (*elem->getElements().begin())->getName() == "object"
            );

            if(buildMode == BUILD_OBJECT)
            {
                // Test if key already exist in composite
                SIGHT_ASSERT(
                    "The key " << key << " already exists in the composite.",
                    dataComposite->find(
                        key
                    ) == dataComposite->end()
                );

                // Create and manage object config
                auto ctm = service::AppConfigManager::New();
                ctm->service::IAppConfigManager::setConfig(elem);

                m_ctmContainer.push_back(ctm);
                ctm->create();
                data::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SIGHT_ASSERT("A data::Composite can contain only data::Object", localObj);
                (*dataComposite)[key] = localObj;
            }
            else // if( buildMode == GET_OBJECT )
            {
                SIGHT_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void Composite::startConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Composite::updateConfig()
{
    for(const service::IAppConfigManager::sptr& ctm : m_ctmContainer)
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Composite::stopConfig()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(service::IAppConfigManager::sptr ctm, m_ctmContainer)
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Composite::destroyConfig()
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
