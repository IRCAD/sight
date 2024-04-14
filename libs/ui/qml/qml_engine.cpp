/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "qml_engine.hpp"

#include <core/lazy_instantiator.hpp>
#include <core/spy_log.hpp>
#include <core/tools/os.hpp>

#include <QDir>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QString>
#include <QUrl>

#include <filesystem>
#include <iostream>

namespace sight::ui::qml
{

//-----------------------------------------------------------------------------

qml_engine::qml_engine() :
    m_engine(new QQmlApplicationEngine())
{
}

//-----------------------------------------------------------------------------

qml_engine::~qml_engine()
= default;

//-----------------------------------------------------------------------------

SPTR(qml_engine) qml_engine::get_default()
{
    auto engine_instance = core::lazy_instantiator<qml_engine>::get_instance();
    return engine_instance;
}

//-----------------------------------------------------------------------------

void qml_engine::load_main_component(const std::filesystem::path& _file)
{
    m_engine->load(QUrl::fromLocalFile(QString::fromStdString(_file.string())));
}

//-----------------------------------------------------------------------------

QObject* qml_engine::create_component(const std::filesystem::path& _file, QSharedPointer<QQmlContext>& _context)
{
    QQmlComponent component(m_engine, QUrl::fromLocalFile(QString::fromStdString(_file.string())));
    if(component.status() == QQmlComponent::Status::Error)
    {
        const auto err = component.errors();
        for(const auto& e : err)
        {
            SIGHT_ERROR(e.toString().toStdString());
        }
    }

    SIGHT_ASSERT("Component is not ready", component.status() == QQmlComponent::Status::Ready);
    return component.create(_context.get());
}

//-----------------------------------------------------------------------------

QObject* qml_engine::create_component(const std::filesystem::path& _file)
{
    QQmlComponent component(m_engine, QUrl::fromLocalFile(QString::fromStdString(_file.string())));
    if(component.status() == QQmlComponent::Status::Error)
    {
        const auto err = component.errors();
        for(const auto& e : err)
        {
            SIGHT_ERROR(e.toString().toStdString());
        }
    }

    SIGHT_ASSERT("Component is not ready", component.status() == QQmlComponent::Status::Ready);
    return component.create(m_engine->rootContext());
}

//-----------------------------------------------------------------------------

void qml_engine::import_module_path(const std::filesystem::path& _path)
{
    m_engine->addImportPath(QString::fromStdString(_path.string()));
}

//-----------------------------------------------------------------------------

QList<QObject*> qml_engine::get_root_objects()
{
    return m_engine->rootObjects();
}

//-----------------------------------------------------------------------------
QQmlContext* qml_engine::get_root_context()
{
    return m_engine->rootContext();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qml
