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

#include "PoCDialog/LocationDialogs.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QObject>

LocationDialogs::LocationDialogs()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_existing                      = true;
    // add context for root Context
    engine->getRootContext()->setContextProperty("pocDialogLocationDialogs", this);
}

//------------------------------------------------------------------------------

LocationDialogs::~LocationDialogs()
{
}

//------------------------------------------------------------------------------

void LocationDialogs::open()
{
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_title.toStdString());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(m_folder.toStdString()) );

    if (m_isFolder)
    {
        dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
    }
    if (!m_existing)
    {
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    }
    else
    {
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
    }
    if (!m_multiple)
    {
        dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    }
    else
    {
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
        dialogFile.setType(::fwGui::dialog::ILocationDialog::MULTI_FILES);
    }

    for (int i = 0; i < m_filter.size(); ++i)
    {
        auto name      = m_filter.at(i).toStdString();
        auto extension = m_filterExt.at(i).toStdString();
        dialogFile.addFilter(name, extension);
    }

    m_result = "";
    if (!m_multiple)
    {
        ::fwData::location::SingleFile::sptr result
            = ::fwData::location::SingleFile::dynamicCast(dialogFile.show());
        if (result)
        {
            auto path = result->getPath();
            m_result = QString::fromStdString(path.string());
        }
        Q_EMIT onResultChanged();
    }
    else
    {
        ::fwData::location::MultiFiles::sptr result
            = ::fwData::location::MultiFiles::dynamicCast(dialogFile.show());
        if (result)
        {
            auto paths = result->getPaths();
            for (const auto& path : paths)
            {
                m_result += "|" + QString::fromStdString(path.string());
            }
        }
        Q_EMIT onResultChanged();
    }
}
