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

#include "fwGuiQml/dialog/LocationDialog.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/tokenizer.hpp>

#include <filesystem>
#include <QDir>
#include <QGuiApplication>

#include <functional>

fwGuiRegisterMacro( ::fwGuiQml::dialog::LocationDialog, ::fwGui::dialog::ILocationDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

LocationDialog::LocationDialog(::fwGui::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    // If we don't create an alternative by creating an input dialog to save file,
    // The application freeze without opening the FileDialog
    // sight issue: https://git.ircad.fr/Sight/sight/issues/365
    // for more information: https://bugreports.qt.io/browse/QTBUG-77781
#ifdef __APPLE__
    if ( !(m_style& ::fwGui::dialog::ILocationDialog::READ) &&
         !(m_style& ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST))
    {
        const std::string& result = ::fwGui::dialog::InputDialog::showInputDialog(
            this->getTitle(), "This is a temporary dialog to save file under macOS. Write below the path of the file you want to save:",
            QDir::homePath().toStdString());
        std::filesystem::path bpath( result);
        m_location = ::fwData::location::SingleFile::New(bpath);
        return m_location;
    }
#endif

    const QString& caption                  = QString::fromStdString(this->getTitle());
    const std::filesystem::path defaultPath = this->getDefaultLocation();
    const QString& path                     = QString::fromStdString(defaultPath.string());
    const QStringList& filter               = this->fileFilters();

    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwGuiQml-" FWGUIQML_VER "/dialog/LocationDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("locationDialog", this);
    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);
    SLM_ASSERT("The Qml File LocationDialog is not found or not loaded", dialog);

    dialog->setProperty("title", caption);
    dialog->setProperty("folder", QUrl::fromLocalFile(path));
    dialog->setProperty("nameFilters", filter);

    // check each option to set the property
    if ( (m_style& ::fwGui::dialog::ILocationDialog::READ) ||
         (m_style & ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST))
    {
        dialog->setProperty("selectExisting", true);
    }
    else
    {
        dialog->setProperty("selectExisting", false);
    }

    if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
    {
        SLM_ASSERT("MULTI_FILES type must have a READ style", m_style & ::fwGui::dialog::ILocationDialog::READ);
        dialog->setProperty("selectFolder", false);
        dialog->setProperty("selectMultiple", true);
        QStringList files;
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
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
        if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
        {
            ::fwData::location::MultiFiles::sptr multifiles = ::fwData::location::MultiFiles::New();
            std::vector< std::filesystem::path > paths;
            for (const QUrl& filename : files)
            {
                std::filesystem::path bpath( filename.toLocalFile().toStdString() );
                paths.push_back(bpath);
            }
            multifiles->setPaths(paths);
            m_location = multifiles;
        }
        else if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE ||
                 m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
        {
            std::filesystem::path bpath( files.first().toLocalFile().toStdString());
            m_location = ::fwData::location::SingleFile::New(bpath);
        }
    }
}

//------------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type )
{
    m_type = type;
}

//------------------------------------------------------------------------------

::fwGui::dialog::ILocationDialog&  LocationDialog::setOption( ::fwGui::dialog::ILocationDialog::Options option)
{
    switch (option)
    {
        case ::fwGui::dialog::ILocationDialog::WRITE:
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style &
                                                                         ~::fwGui::dialog::ILocationDialog::READ);
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style |
                                                                         ::fwGui::dialog::ILocationDialog::WRITE);
            break;
        case ::fwGui::dialog::ILocationDialog::READ:
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style &
                                                                         ~::fwGui::dialog::ILocationDialog::WRITE);
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style |
                                                                         ::fwGui::dialog::ILocationDialog::READ);
            break;
        case ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST:
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style |
                                                                         ::fwGui::dialog::ILocationDialog::
                                                                         FILE_MUST_EXIST);
            break;
        default:
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style &
                                                                         ~::fwGui::dialog::ILocationDialog::READ);
            m_style =
                static_cast< ::fwGui::dialog::ILocationDialog::Options >(m_style |
                                                                         ::fwGui::dialog::ILocationDialog::WRITE);
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
} //namespace fwGuiQml
