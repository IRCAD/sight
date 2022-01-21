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

#include "SExtractObj.hpp"

#include <data/mt/ObjectWriteLock.hpp>
#include <data/reflection/exception/NullPointer.hpp>
#include <data/reflection/exception/ObjectNotFound.hpp>
#include <data/reflection/getObject.hpp>

#include <service/registry/ObjectService.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

SExtractObj::SExtractObj()
{
}

//-----------------------------------------------------------------------------

SExtractObj::~SExtractObj()
{
}

//-----------------------------------------------------------------------------

void SExtractObj::configuring()
{
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType inoutCfg = m_configuration->findConfigurationElement("inout");
    SIGHT_ASSERT("At one 'inout' tag is required.", inoutCfg);

    const std::vector<ConfigurationType> extractCfg = inoutCfg->find("extract");
    SIGHT_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

    for(ConfigurationType cfg : extractCfg)
    {
        SIGHT_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        const std::string from = cfg->getAttributeValue("from");

        m_sourcePaths.push_back(from);
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::starting()
{
}

//-----------------------------------------------------------------------------

void SExtractObj::updating()
{
    auto sourceObject = m_source.lock();

    std::size_t index = 0;
    for(auto path : m_sourcePaths)
    {
        const std::string from = path;

        sight::data::Object::sptr object;
        try
        {
            object = sight::data::reflection::getObject(sourceObject.get_shared(), from, true);
        }
        catch(sight::data::reflection::exception::NullPointer&)
        {
            m_target[index] = nullptr;
        }
        catch(sight::data::reflection::exception::ObjectNotFound&)
        {
            SIGHT_WARN("Object from '" + from + "' not found");
        }
        catch(std::exception& e)
        {
            SIGHT_FATAL("Unhandled exception: " << e.what());
        }

        SIGHT_WARN_IF("Object from '" + from + "' not found", !object);
        if(object)
        {
            m_target[index] = object;
        }

        ++index;
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::stopping()
{
    // Unregister outputs
    for(std::size_t i = 0 ; i < m_target.size() ; ++i)
    {
        m_target[i] = nullptr;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
