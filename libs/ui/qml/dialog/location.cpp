/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/dialog/location.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location_base.hpp>
#include <ui/__/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QDir>
#include <QGuiApplication>

#include <filesystem>
#include <functional>

SIGHT_REGISTER_GUI(sight::ui::qml::dialog::location, sight::ui::dialog::location_base::REGISTRY_KEY);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

core::location::base::sptr location::show()
{
    const QString& caption    = QString::fromStdString(this->getTitle());
    const QString& path       = QString::fromStdString(this->getDefaultLocation()->to_string());
    const QStringList& filter = this->fileFilters();

    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath = core::runtime::get_library_resource_file_path(
        "ui_qml/dialog/location.qml"
    );

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("locationDialog", this);

    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);
    SIGHT_ASSERT("The Qml File location is not found or not loaded", dialog);

    dialog->setProperty("title", caption);
    dialog->setProperty("folder", QUrl::fromLocalFile(path));
    dialog->setProperty("nameFilters", filter);

    // check each option to set the property
    if(((m_style & READ) != 0) || ((m_style & FILE_MUST_EXIST) != 0))
    {
        dialog->setProperty("selectExisting", true);
    }
    else
    {
        dialog->setProperty("selectExisting", false);
    }

    if(m_type == MULTI_FILES)
    {
        SIGHT_ASSERT("MULTI_FILES type must have a READ style", m_style & READ);
        dialog->setProperty("selectFolder", false);
        dialog->setProperty("selectMultiple", true);
    }
    else if(m_type == FOLDER)
    {
        dialog->setProperty("selectExisting", true);
        dialog->setProperty("selectFolder", true);
    }
    else
    {
        dialog->setProperty("selectFolder", false);
    }

    QEventLoop loop;
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");

    loop.exec();
    delete dialog;

    return m_location;
}

//------------------------------------------------------------------------------

bool location::eventFilter(QObject* /*watched*/, QEvent* event)
{
    return event->type() != QEvent::Paint && event->type() != QEvent::MetaCall && !event->spontaneous();
}

//------------------------------------------------------------------------------

void location::resultDialog(const QVariant& msg)
{
    // get the list of selected files or folder
    auto files = msg.value<QList<QUrl> >();
    m_wildcard = m_filterSelected.toStdString();
    if(!files.isEmpty() && !files.first().isEmpty())
    {
        // convert all selected location into boost filesystem and add it in m_location
        if(m_type == MULTI_FILES)
        {
            std::vector<std::filesystem::path> paths;
            for(const QUrl& filename : files)
            {
                std::filesystem::path p(filename.toLocalFile().toStdString());
                paths.push_back(p);
            }

            const auto& multipleFiles = std::make_shared<core::location::multiple_files>();
            multipleFiles->set_files(paths);
            m_location = multipleFiles;
        }
        else if(m_type == SINGLE_FILE)
        {
            const auto& singleFile = std::make_shared<core::location::single_file>();
            singleFile->set_file(files.first().toLocalFile().toStdString());
            m_location = singleFile;
        }
        else if(m_type == FOLDER)
        {
            const auto& singleDirectory = std::make_shared<core::location::single_folder>();
            singleDirectory->set_folder(files.first().toLocalFile().toStdString());
            m_location = singleDirectory;
        }
    }
}

//------------------------------------------------------------------------------

void location::setType(location::Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

void location::setOption(location::Options option)
{
    switch(option)
    {
        case WRITE:
            m_style = static_cast<location::Options>(m_style & ~READ);
            m_style = static_cast<location::Options>(m_style | WRITE);
            break;

        case READ:
            m_style = static_cast<location::Options>(m_style & ~WRITE);
            m_style = static_cast<location::Options>(m_style | READ);
            break;

        case FILE_MUST_EXIST:
            m_style = static_cast<location::Options>(m_style | FILE_MUST_EXIST);
            break;

        default:
            m_style = static_cast<location::Options>(m_style & ~READ);
            m_style = static_cast<location::Options>(m_style | WRITE);
            break;
    }
}

//------------------------------------------------------------------------------

void location::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_filters.emplace_back(filterName, wildcardList);
}

//------------------------------------------------------------------------------

QStringList location::fileFilters()
{
    QStringList result;
    for(const auto& filter : m_filters)
    {
        const std::string& filterName   = filter.first;
        const std::string& rawWildcards = filter.second;
        const QString& qFilterName      = QString::fromStdString(filterName);
        const QString& qRawWildcards    = QString::fromStdString(rawWildcards);

        result += qFilterName + " (" + qRawWildcards + ")";
    }

    return result;
}

//------------------------------------------------------------------------------

std::string location::getCurrentSelection() const
{
    /// @todo deduplicate this from libs/ui/qt/dialog/location.cpp
    for(auto&& [filterName, rawWildcards] : m_filters)
    {
        const std::string& availableFilters = filterName + " (" + rawWildcards + ")";
        if(m_wildcard == availableFilters)
        {
            return rawWildcards;
        }
    }

    SIGHT_THROW("No filter found for wildcard '" + m_wildcard + "'");
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
