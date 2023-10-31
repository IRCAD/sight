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
#include <ui/qml/qml_engine.hpp>

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
    const QString& caption    = QString::fromStdString(this->get_title());
    const QString& path       = QString::fromStdString(this->get_default_location()->to_string());
    const QStringList& filter = this->file_filters();

    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::qml_engine) engine = ui::qml::qml_engine::get_default();

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialog_path = core::runtime::get_library_resource_file_path(
        "ui_qml/dialog/location.qml"
    );

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->get_root_context()));
    context->setContextProperty("locationDialog", this);

    // load the qml ui component
    QObject* dialog = engine->create_component(dialog_path, context);
    SIGHT_ASSERT("The Qml File location is not found or not loaded", dialog);

    dialog->setProperty("title", caption);
    dialog->setProperty("folder", QUrl::fromLocalFile(path));
    dialog->setProperty("nameFilters", filter);

    // check each option to set the property
    if(((m_style & read) != 0) || ((m_style & file_must_exist) != 0))
    {
        dialog->setProperty("selectExisting", true);
    }
    else
    {
        dialog->setProperty("selectExisting", false);
    }

    if(m_type == multi_files)
    {
        SIGHT_ASSERT("MULTI_FILES type must have a READ style", m_style & read);
        dialog->setProperty("selectFolder", false);
        dialog->setProperty("selectMultiple", true);
    }
    else if(m_type == folder)
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

bool location::eventFilter(QObject* /*watched*/, QEvent* _event)
{
    return _event->type() != QEvent::Paint && _event->type() != QEvent::MetaCall && !_event->spontaneous();
}

//------------------------------------------------------------------------------

void location::result_dialog(const QVariant& _msg)
{
    // get the list of selected files or folder
    auto files = _msg.value<QList<QUrl> >();
    m_wildcard = m_filter_selected.toStdString();
    if(!files.isEmpty() && !files.first().isEmpty())
    {
        // convert all selected location into boost filesystem and add it in m_location
        if(m_type == multi_files)
        {
            std::vector<std::filesystem::path> paths;
            for(const QUrl& filename : files)
            {
                std::filesystem::path p(filename.toLocalFile().toStdString());
                paths.push_back(p);
            }

            const auto& multiple_files = std::make_shared<core::location::multiple_files>();
            multiple_files->set_files(paths);
            m_location = multiple_files;
        }
        else if(m_type == single_file)
        {
            const auto& single_file = std::make_shared<core::location::single_file>();
            single_file->set_file(files.first().toLocalFile().toStdString());
            m_location = single_file;
        }
        else if(m_type == folder)
        {
            const auto& single_directory = std::make_shared<core::location::single_folder>();
            single_directory->set_folder(files.first().toLocalFile().toStdString());
            m_location = single_directory;
        }
    }
}

//------------------------------------------------------------------------------

void location::set_type(location::types _type)
{
    m_type = _type;
}

//------------------------------------------------------------------------------

void location::set_option(location::options _option)
{
    switch(_option)
    {
        case write:
            m_style = static_cast<location::options>(m_style & ~read);
            m_style = static_cast<location::options>(m_style | write);
            break;

        case read:
            m_style = static_cast<location::options>(m_style & ~write);
            m_style = static_cast<location::options>(m_style | read);
            break;

        case file_must_exist:
            m_style = static_cast<location::options>(m_style | file_must_exist);
            break;

        default:
            m_style = static_cast<location::options>(m_style & ~read);
            m_style = static_cast<location::options>(m_style | write);
            break;
    }
}

//------------------------------------------------------------------------------

void location::add_filter(const std::string& _filter_name, const std::string& _wildcard_list)
{
    m_filters.emplace_back(_filter_name, _wildcard_list);
}

//------------------------------------------------------------------------------

QStringList location::file_filters()
{
    QStringList result;
    for(const auto& filter : m_filters)
    {
        const std::string& filter_name   = filter.first;
        const std::string& raw_wildcards = filter.second;
        const QString& q_filter_name     = QString::fromStdString(filter_name);
        const QString& q_raw_wildcards   = QString::fromStdString(raw_wildcards);

        result += q_filter_name + " (" + q_raw_wildcards + ")";
    }

    return result;
}

//------------------------------------------------------------------------------

std::string location::get_current_selection() const
{
    /// @todo deduplicate this from libs/ui/qt/dialog/location.cpp
    for(auto&& [filterName, rawWildcards] : m_filters)
    {
        const std::string& available_filters = filterName + " (" + rawWildcards + ")";
        if(m_wildcard == available_filters)
        {
            return rawWildcards;
        }
    }

    SIGHT_THROW("No filter found for wildcard '" + m_wildcard + "'");
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
