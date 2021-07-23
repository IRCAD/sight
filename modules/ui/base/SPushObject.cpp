/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SPushObject.hpp"

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/fwID.hpp>

#include <data/Composite.hpp>
#include <data/helper/Composite.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

namespace sight::module::ui::base
{

static const core::com::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

static const std::string s_SOURCE_KEY      = "source";
static const std::string s_DESTINATION_KEY = "destination";

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

SPushObject::SPushObject() noexcept
{
    newSlot(s_UPDATE_OBJECTS_SLOT, &SPushObject::updateObjects, this);
}

//------------------------------------------------------------------------------

SPushObject::~SPushObject() noexcept
{
}

//------------------------------------------------------------------------------

void SPushObject::configuring()
{
    this->initialize();

    const ConfigurationType pushCfg = m_configuration->findConfigurationElement("push");

    SIGHT_ASSERT("<push> tag must have one attribute 'srcKey'.", pushCfg->hasAttribute("srcKey"));
    m_srcKey = pushCfg->getAttributeValue("srcKey");
}

//------------------------------------------------------------------------------

void SPushObject::starting()
{
    this->actionServiceStarting();
    this->updateObjects();
}

//------------------------------------------------------------------------------

void SPushObject::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SPushObject::updating()
{
    data::Composite::sptr compositeSrc = this->getInOut<data::Composite>(s_SOURCE_KEY);
    SIGHT_ASSERT(s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

    data::Object::sptr obj = compositeSrc->at<data::Object>(m_srcKey);

    SIGHT_WARN_IF("'" + m_srcKey + "' not found in composite '" + compositeSrc->getID() + "'", obj == nullptr);
    if(service::OSR::isRegistered(
           s_DESTINATION_KEY,
           data::Access::out,
           this->getSptr()
    ))
    {
        this->setOutput(s_DESTINATION_KEY, nullptr);
    }
    else if(obj)
    {
        this->setOutput(s_DESTINATION_KEY, obj);
    }
}

//------------------------------------------------------------------------------

void SPushObject::updateObjects()
{
    data::Composite::sptr compositeSrc = this->getInOut<data::Composite>(s_SOURCE_KEY);
    SIGHT_ASSERT(s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

    const bool executable = (compositeSrc->find(m_srcKey) != compositeSrc->end());

    this->::sight::ui::base::IAction::setIsExecutable(executable);

    if(executable && this->::sight::ui::base::IAction::getIsActive())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SPushObject::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SOURCE_KEY, data::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT);
    connections.push(s_SOURCE_KEY, data::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base
