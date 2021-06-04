/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "QmlEngine.hpp"

#include <core/LazyInstantiator.hpp>
#include <core/runtime/Runtime.hpp>

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

QmlEngine::QmlEngine()
{
    m_engine = new QQmlApplicationEngine();

    // check if './qml' directory is in the local folder (used by installed application) or in the deps folder
    const auto runtimePath = core::runtime::Runtime::getDefault()->getWorkingPath();
    const auto qmlDir      = runtimePath / "qml";
    if(std::filesystem::exists(qmlDir))
    {
        m_engine->addImportPath(QString::fromStdString(qmlDir.string()));
    }

    // Maybe still needed for MSVC ?
    // else
    // {
    //     m_engine->addImportPath(QML_IMPORT_PATH);
    // }
}

//-----------------------------------------------------------------------------

QmlEngine::~QmlEngine()
{
}

//-----------------------------------------------------------------------------

SPTR(QmlEngine) QmlEngine::getDefault()
{
    auto engineInstance = core::LazyInstantiator<QmlEngine>::getInstance();
    return engineInstance;
}

//-----------------------------------------------------------------------------

void QmlEngine::loadMainComponent(const std::filesystem::path& file)
{
    m_engine->load(QUrl::fromLocalFile(QString::fromStdString(file.string())));
}

//-----------------------------------------------------------------------------

QObject* QmlEngine::createComponent(const std::filesystem::path& file, QSharedPointer<QQmlContext>& context)
{
    QQmlComponent component(m_engine, QUrl::fromLocalFile(QString::fromStdString(file.string())));
    return component.create(context.get());
}

//-----------------------------------------------------------------------------

QObject* QmlEngine::createComponent(const std::filesystem::path& file)
{
    QQmlComponent component(m_engine, QUrl::fromLocalFile(QString::fromStdString(file.string())));
    return component.create(m_engine->rootContext());
}

//-----------------------------------------------------------------------------

void QmlEngine::importModulePath(const std::filesystem::path& path)
{
    m_engine->addImportPath(QString::fromStdString(path.string()));
}

//-----------------------------------------------------------------------------

QList<QObject*> QmlEngine::getRootObjects()
{
    return m_engine->rootObjects();
}

//-----------------------------------------------------------------------------
QQmlContext* QmlEngine::getRootContext()
{
    return m_engine->rootContext();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
