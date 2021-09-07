/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "AppManager.hpp"

#include <ui/qml/IQmlEditor.hpp>

#include <QVariant>

using namespace sight;

namespace Tuto05EditorQml
{

static const std::string s_EDITED_STRING = "editedString";

//------------------------------------------------------------------------------

AppManager::AppManager() noexcept
{
}

//------------------------------------------------------------------------------

AppManager::~AppManager() noexcept
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();

    m_string = data::String::New("Edit this line...");

    this->addObject(m_string, s_EDITED_STRING);

    this->startServices();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void AppManager::onServiceCreated(const QVariant& obj)
{
    // check that the service is a IQmlEditor
    ui::qml::IQmlEditor::sptr srv(obj.value<ui::qml::IQmlEditor*>());
    if(srv)
    {
        // check if it is the SOrganMaterialEditor
        if(srv->isA("Tuto05EditorQml::SStringEditor"))
        {
            // register the new service in the AppManager, it will be automatically started when the reconstruction is
            // added
            srv->setObjectId("string", s_EDITED_STRING);
            this->addService(srv, true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace Tuto05EditorQml
