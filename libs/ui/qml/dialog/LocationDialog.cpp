/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "ui/qml/dialog/LocationDialog.hpp"

#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/runtime/operations.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/registry/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QDir>
#include <QGuiApplication>

#include <filesystem>
#include <functional>

fwGuiRegisterMacro(
    sight::ui::qml::dialog::LocationDialog,
    sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY
);

namespace sight::ui::qml
{

namespace dialog
{

//------------------------------------------------------------------------------

LocationDialog::LocationDialog(ui::base::GuiBaseObject::Key)
{
}

//------------------------------------------------------------------------------

core::location::ILocation::sptr LocationDialog::show()
{
    const QString& caption    = QString::fromStdString(this->getTitle());
    const QString& path       = QString::fromStdString(this->getDefaultLocation()->toString());
    const QStringList& filter = this->fileFilters();

    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath = core::runtime::getLibraryResourceFilePath(
        "ui_qml/dialog/LocationDialog.qml"
    );

    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("locationDialog", this);

    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);
    SIGHT_ASSERT("The Qml File LocationDialog is not found or not loaded", dialog);

    dialog->setProperty("title", caption);
    dialog->setProperty("folder", QUrl::fromLocalFile(path));
    dialog->setProperty("nameFilters", filter);

    // check each option to set the property
    if((m_style & ui::base::dialog::ILocationDialog::READ)
       || (m_style & ui::base::dialog::ILocationDialog::FILE_MUST_EXIST))
    {
        dialog->setProperty("selectExisting", true);
    }
    else
    {
        dialog->setProperty("selectExisting", false);
    }

    if(m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
    {
        SIGHT_ASSERT("MULTI_FILES type must have a READ style", m_style & ui::base::dialog::ILocationDialog::READ);
        dialog->setProperty("selectFolder", false);
        dialog->setProperty("selectMultiple", true);
    }
    else if(m_type == ui::base::dialog::ILocationDialog::FOLDER)
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

bool LocationDialog::eventFilter(QObject*, QEvent* event)
{
    return event->type() != QEvent::Paint && event->type() != QEvent::MetaCall && !event->spontaneous();
}

//------------------------------------------------------------------------------

void LocationDialog::resultDialog(const QVariant& msg)
{
    // get the list of selected files or folder
    QList<QUrl> files = msg.value<QList<QUrl> >();
    m_wildcard = m_filterSelected.toStdString();
    if(!files.isEmpty() && !files.first().isEmpty())
    {
        // convert all selected location into boost filesystem and add it in m_location
        if(m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
        {
            std::vector<std::filesystem::path> paths;
            for(const QUrl& filename : files)
            {
                std::filesystem::path p(filename.toLocalFile().toStdString());
                paths.push_back(p);
            }

            const auto& multipleFiles = std::make_shared<core::location::MultipleFiles>();
            multipleFiles->setFiles(paths);
            m_location = multipleFiles;
        }
        else if(m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE)
        {
            const auto& singleFile = std::make_shared<core::location::SingleFile>();
            singleFile->setFile(files.first().toLocalFile().toStdString());
            m_location = singleFile;
        }
        else if(m_type == ui::base::dialog::ILocationDialog::FOLDER)
        {
            const auto& singleDirectory = std::make_shared<core::location::SingleFolder>();
            singleDirectory->setFolder(files.first().toLocalFile().toStdString());
            m_location = singleDirectory;
        }
    }
}

//------------------------------------------------------------------------------

void LocationDialog::setType(ui::base::dialog::ILocationDialog::Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

ui::base::dialog::ILocationDialog& LocationDialog::setOption(ui::base::dialog::ILocationDialog::Options option)
{
    switch(option)
    {
        case ui::base::dialog::ILocationDialog::WRITE:
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        & ~ui::base::dialog::ILocationDialog::READ);
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        | ui::base::dialog::ILocationDialog::WRITE);
            break;

        case ui::base::dialog::ILocationDialog::READ:
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        & ~ui::base::dialog::ILocationDialog::WRITE);
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        | ui::base::dialog::ILocationDialog::READ);
            break;

        case ui::base::dialog::ILocationDialog::FILE_MUST_EXIST:
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        | ui::base::dialog::ILocationDialog::
                                                                        FILE_MUST_EXIST);
            break;

        default:
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        & ~ui::base::dialog::ILocationDialog::READ);
            m_style =
                static_cast<ui::base::dialog::ILocationDialog::Options>(m_style
                                                                        | ui::base::dialog::ILocationDialog::WRITE);
            break;
    }

    return *this;
}

//------------------------------------------------------------------------------

void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_filters.push_back(std::make_pair(filterName, wildcardList));
}

//------------------------------------------------------------------------------

const QStringList LocationDialog::fileFilters()
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

std::string LocationDialog::getCurrentSelection() const
{
    std::string extension;
    for(const auto& filter : m_filters)
    {
        const std::string& filterName       = filter.first;
        const std::string& rawWildcards     = filter.second;
        const std::string& availableFilters = filterName + " (" + rawWildcards + ")";
        if(!m_wildcard.compare(availableFilters))
        {
            extension = &rawWildcards[1];
            break;
        }
    }

    return extension;
}

//------------------------------------------------------------------------------

} // namespace dialog

} //namespace sight::ui::qml
