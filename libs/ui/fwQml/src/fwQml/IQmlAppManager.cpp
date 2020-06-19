/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwQml/IQmlAppManager.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

namespace fwQml
{

//------------------------------------------------------------------------------

IQmlAppManager::IQmlAppManager() noexcept
{

}

//------------------------------------------------------------------------------

IQmlAppManager::~IQmlAppManager() noexcept
{
}

//------------------------------------------------------------------------------

void IQmlAppManager::initialize()
{
    this->create();

    m_isInitialized = this->checkInputs();
    if (!m_isInitialized)
    {
        const std::string msg = "All the required inputs are not present, '" +this->getID() +
                                "' activity will not be launched";
        ::fwGui::dialog::MessageDialog::showMessageDialog("Manager Initialization",
                                                          msg,
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

void IQmlAppManager::uninitialize()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void IQmlAppManager::onServiceCreated(const QVariant& )
{
    SLM_ASSERT("The method 'onServiceCreated(srv)' must be implemented to properly register the service instantiated "
               "in Qml", false);
}

//------------------------------------------------------------------------------

void IQmlAppManager::replaceInputs(const QVariant& variant)
{
    QMap<QString, QVariant> map                   = variant.toMap();
    QMap<QString, QVariant>::iterator it          = map.begin();
    const QMap<QString, QVariant>::iterator itEnd = map.end();
    for (; it != itEnd; ++it)
    {
        const std::string replace = it.key().toStdString();
        const std::string by      = it.value().toString().toStdString();
        this->replaceInput(replace, by);
    }
}

//------------------------------------------------------------------------------

}// namespace fwiQml
