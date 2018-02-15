/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SPushField.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

namespace gui
{
namespace action
{

//------------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_OBJECTS_SLOT = "updateObject";

static const std::string s_SOURCE_KEY      = "source";
static const std::string s_DESTINATION_KEY = "destination";

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SPushField );

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
    ::fwData::Object::sptr objectSrc = this->getInOut< ::fwData::Object >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", objectSrc);

    ::fwData::Object::sptr obj = objectSrc->getField(m_field);

    SLM_WARN_IF("'" + m_field + "' not found in object '" + objectSrc->getID() + "'", obj == nullptr);
    if (obj)
    {
        this->setOutput(s_DESTINATION_KEY, obj);
    }
}

//------------------------------------------------------------------------------

void SPushField::updateObjects()
{
    ::fwData::Object::sptr objectSrc = this->getInOut< ::fwData::Object >(s_SOURCE_KEY);
    SLM_ASSERT( s_SOURCE_KEY + " doesn't exist or is not a composite", objectSrc);

    const bool executable = (objectSrc->getField(m_field) != nullptr);

    this->::fwGui::IActionSrv::setIsExecutable( executable );
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPushField::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SOURCE_KEY, ::fwData::Object::s_ADDED_FIELDS_SIG, s_UPDATE_OBJECTS_SLOT );
    connections.push( s_SOURCE_KEY, ::fwData::Object::s_REMOVED_FIELDS_SIG, s_UPDATE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace gui
