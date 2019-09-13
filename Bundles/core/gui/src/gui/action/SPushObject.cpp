/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "gui/action/SPushObject.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <regex>

namespace gui
{
namespace action
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

static const std::string s_SOURCE_KEY      = "source";
static const std::string s_DESTINATION_KEY = "destination";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SPushObject, ::fwData::Composite );

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

    SLM_ASSERT("<push> tag must have one attribute 'srcKey'.", pushCfg->hasAttribute("srcKey"));
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
    ::fwData::Composite::sptr compositeSrc = this->getInOut< ::fwData::Composite >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

    ::fwData::Object::sptr obj = compositeSrc->at< ::fwData::Object>(m_srcKey);

    SLM_WARN_IF("'" + m_srcKey + "' not found in composite '" + compositeSrc->getID() + "'", obj == nullptr);
    if (::fwServices::OSR::isRegistered(s_DESTINATION_KEY, ::fwServices::IService::AccessType::OUTPUT,
                                        this->getSptr()))
    {
        this->setOutput(s_DESTINATION_KEY, nullptr);
    }
    else if (obj)
    {
        this->setOutput(s_DESTINATION_KEY, obj);
    }
}

//------------------------------------------------------------------------------

void SPushObject::updateObjects()
{
    ::fwData::Composite::sptr compositeSrc = this->getInOut< ::fwData::Composite >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", compositeSrc);

    const bool executable = (compositeSrc->find(m_srcKey) != compositeSrc->end());
    OSLM_TRACE("start check : " << compositeSrc->getID() << "[" << m_srcKey << "] : " <<
               (compositeSrc->find(m_srcKey) != compositeSrc->end()) );

    this->::fwGui::IActionSrv::setIsExecutable( executable );

    if(executable && this->::fwGui::IActionSrv::getIsActive())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPushObject::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SOURCE_KEY, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT );
    connections.push( s_SOURCE_KEY, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace gui
