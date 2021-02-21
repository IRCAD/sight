/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "SPushField.hpp"

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/extension/AppConfig.hpp>
#include <service/registry/ObjectService.hpp>

namespace sight::module::ui::base
{
//------------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

static const std::string s_SOURCE_KEY      = "source";
static const std::string s_DESTINATION_KEY = "destination";

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

SPushField::SPushField() noexcept
{
    newSlot(s_UPDATE_OBJECTS_SLOT, &SPushField::updateObjects, this);
}

//------------------------------------------------------------------------------

SPushField::~SPushField() noexcept
{
}

//------------------------------------------------------------------------------

void SPushField::configuring()
{
    this->initialize();

    const ConfigurationType pushCfg = m_configuration->findConfigurationElement("push");

    SLM_ASSERT("<push> tag must have one attribute 'field'.", pushCfg->hasAttribute("field"));
    m_field = pushCfg->getAttributeValue("field");
}

//------------------------------------------------------------------------------

void SPushField::starting()
{
    this->actionServiceStarting();
    this->updateObjects();
}

//------------------------------------------------------------------------------

void SPushField::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SPushField::updating()
{
    data::Object::sptr objectSrc = this->getInOut< data::Object >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", objectSrc);

    data::Object::sptr obj = objectSrc->getField(m_field);

    SLM_WARN_IF("'" + m_field + "' not found in object '" + objectSrc->getID() + "'", obj == nullptr);
    if (obj)
    {
        this->setOutput(s_DESTINATION_KEY, obj);
    }
}

//------------------------------------------------------------------------------

void SPushField::updateObjects()
{
    data::Object::sptr objectSrc = this->getInOut< data::Object >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", objectSrc);

    const bool executable = (objectSrc->getField(m_field) != nullptr);

    this->::sight::ui::base::IAction::setIsExecutable( executable );
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SPushField::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SOURCE_KEY, data::Object::s_ADDED_FIELDS_SIG, s_UPDATE_OBJECTS_SLOT );
    connections.push( s_SOURCE_KEY, data::Object::s_REMOVED_FIELDS_SIG, s_UPDATE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base
