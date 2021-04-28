/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/runtime/operations.hpp>

#include <data/location/Folder.hpp>
#include <data/location/MultiFiles.hpp>
#include <data/location/SingleFile.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/registry/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QDir>
#include <QGuiApplication>

#include <filesystem>
#include <functional>

fwGuiRegisterMacro( ::sight::ui::qml::dialog::LocationDialog,
                    ::sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY );

namespace sight::ui::qml
{
namespace dialog
{

//------------------------------------------------------------------------------

LocationDialog::LocationDialog(ui::base::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

data::location::ILocation::sptr LocationDialog::show()
{
    // If we don't create an alternative by creating an input dialog to save file,
    // The application freeze without opening the FileDialog
    // sight issue: https://git.ircad.fr/Sight/sight/issues/365
    // for more information: https://bugreports.qt.io/browse/QTBUG-77781
#ifdef __APPLE__
    if ( !(m_style& ui::base::dialog::ILocationDialog::READ) &&
         !(m_style& ui::base::dialog::ILocationDialog::FILE_MUST_EXIST))
    {
        const std::string& result = ui::base::dialog::InputDialog::showInputDialog(
            this->getTitle(), "This is a temporary dialog to save file under macOS. Write below the path of the file you want to save:",
            QDir::homePath().toStdString());
        std::filesystem::path bpath( result);
        m_location = data::location::SingleFile::New(bpath);
        return m_location;
    }
#endif

    const QString& caption                  = QString::fromStdString(this->getTitle());
    const std::filesystem::path defaultPath = this->getDefaultLocation();
    const QString& path                     = QString::fromStdString(defaultPath.string());
    const QStringList& filter               = this->fileFilters();

    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();
    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath = core::runtime::getLibraryResourceFilePath(
        "fwGuiQml/dialog/LocationDialog.qml");
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
    if ( (m_style& ui::base::dialog::ILocationDialog::READ) ||
         (m_style & ui::base::dialog::ILocationDialog::FILE_MUST_EXIST))
    {
        dialog->setProperty("selectExisting", true);
    }
    else
    {
        dialog->setProperty("selectExisting", false);
    }

    if (m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
    {
        SIGHT_ASSERT("MULTI_FILES type must have a READ style", m_style & ui::base::dialog::ILocationDialog::READ);
        dialog->setProperty("selectFolder", false);
        dialog->setProperty("selectMultiple", true);
        QStringList files;
    }
    else if (m_type == ui::base::dialog::ILocationDialog::FOLDER)
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
    // If we don't make NoneModal the FileDialog is not render so we simulate a Modal behaviour
    // by creating a filter of event to prevent all event that are not inside the dialog
    // for more information: https://bugreports.qt.io/browse/QTBUG-76102
#ifdef __APPLE__
    qGuiApp->installEventFilter(this);
#endif
    loop.exec();
#ifdef __APPLE__
    qGuiApp->removeEventFilter(this);
#endif
    delete dialog;

    return m_location;
}

//------------------------------------------------------------------------------

bool LocationDialog::eventFilter(QObject* watched, QEvent* event)
{
    return (event->type() != QEvent::Paint && event->type() != QEvent::MetaCall && !event->spontaneous());
}

//------------------------------------------------------------------------------

void LocationDialog::resultDialog(const QVariant& msg)
{
    // get the list of selected files or folder
    QList<QUrl> files = msg.value<QList<QUrl> >();
    m_wildcard = m_filterSelected.toStdString();
    if (!files.isEmpty() && !files.first().isEmpty())
    {
        // convert all selected location into boost filesystem and add it in m_location
        if (m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
        {
            data::location::MultiFiles::sptr multifiles = data::location::MultiFiles::New();
            std::vector< std::filesystem::path > paths;
            for (const QUrl& filename : files)
            {
                std::filesystem::path bpath( filename.toLocalFile().toStdString() );
                paths.push_back(bpath);
            }
            multifiles->setPaths(paths);
            m_location = multifiles;
        }
        else if (m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE ||
                 m_type == ui::base::dialog::ILocationDialog::FOLDER)
        {
            std::filesystem::path bpath( files.first().toLocalFile().toStdString());
            m_location = data::location::SingleFile::New(bpath);
        }
    }
}

//------------------------------------------------------------------------------

void LocationDialog::setType( ui::base::dialog::ILocationDialog::Types type )
{
    m_type = type;
}

//------------------------------------------------------------------------------

ui::base::dialog::ILocationDialog&  LocationDialog::setOption( ui::base::dialog::ILocationDialog::Options option)
{
    switch (option)
    {
        case ui::base::dialog::ILocationDialog::WRITE:
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style &
                                                                          ~ui::base::dialog::ILocationDialog::READ);
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style |
                                                                          ui::base::dialog::ILocationDialog::WRITE);
            break;
        case ui::base::dialog::ILocationDialog::READ:
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style &
                                                                          ~ui::base::dialog::ILocationDialog::WRITE);
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style |
                                                                          ui::base::dialog::ILocationDialog::READ);
            break;
        case ui::base::dialog::ILocationDialog::FILE_MUST_EXIST:
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style |
                                                                          ui::base::dialog::ILocationDialog::
                                                                          FILE_MUST_EXIST);
            break;
        default:
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style &
                                                                          ~ui::base::dialog::ILocationDialog::READ);
            m_style =
                static_cast< ui::base::dialog::ILocationDialog::Options >(m_style |
                                                                          ui::base::dialog::ILocationDialog::WRITE);
            break;
    }
    return *this;
}

//------------------------------------------------------------------------------

void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList )
{
    m_filters.push_back( std::make_pair( filterName, wildcardList ));
}

//------------------------------------------------------------------------------

const QStringList LocationDialog::fileFilters()
{
    QStringList result;
    for ( const auto& filter : m_filters)
    {
        const std::string& filterName   = filter.first;
        const std::string& rawWildcards = filter.second;
        const QString& qFilterName      = QString::fromStdString(filterName);
        const QString& qRawWildcards    = QString::fromStdString(rawWildcards);

        result += qFilterName +" (" +  qRawWildcards +")";
    }
    return result;
}

//------------------------------------------------------------------------------

std::string LocationDialog::getCurrentSelection() const
{
    std::string extension;
    for (const auto& filter : m_filters)
    {
        const std::string& filterName       = filter.first;
        const std::string& rawWildcards     = filter.second;
        const std::string& availableFilters = filterName + " (" +  rawWildcards + ")";
        if (!m_wildcard.compare(availableFilters))
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