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
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <QDebug>
#include <QGuiApplication>

#include <functional>

fwGuiRegisterMacro( ::fwGuiQml::dialog::LocationDialog, ::fwGui::dialog::ILocationDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

LocationDialog::LocationDialog(::fwGui::GuiBaseObject::Key key) :
    m_style(::fwGui::dialog::ILocationDialog::NONE),
    m_type(::fwGui::dialog::ILocationDialog::SINGLE_FILE)
{
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    QString caption                             = QString::fromStdString(this->getTitle());
    const ::boost::filesystem::path defaultPath = this->getDefaultLocation();
    QString path                                = QString::fromStdString(defaultPath.string());
    QStringList filter                          = this->fileFilters();

    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/LocationDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("locationDialog", this);
    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);

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
    if(event->type() == QEvent::Paint || event->spontaneous())
    {
        return false;
    }

    return true;
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
            std::vector< ::boost::filesystem::path > paths;
            for (QUrl filename : files)
            {
                ::boost::filesystem::path bpath( filename.toLocalFile().toStdString() );
                paths.push_back(bpath);
            }
            multifiles->setPaths(paths);
            m_location = multifiles;
        }
        else if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE ||
                 m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
        {
            ::boost::filesystem::path bpath( files.first().toLocalFile().toStdString());
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
    if ( option == ::fwGui::dialog::ILocationDialog::WRITE )
    {
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style & ~::fwGui::dialog::ILocationDialog::READ);
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style | ::fwGui::dialog::ILocationDialog::WRITE);
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::READ )
    {
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style & ~::fwGui::dialog::ILocationDialog::WRITE);
        m_style = (::fwGui::dialog::ILocationDialog::Options) (m_style | ::fwGui::dialog::ILocationDialog::READ);
    }
    else if ( option == ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST )
    {
        m_style =
            (::fwGui::dialog::ILocationDialog::Options) (m_style |
                                                         ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
    }

    return *this;
}

//------------------------------------------------------------------------------

void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList )
{
    m_filters.push_back( std::make_pair( filterName, wildcardList ));
}

//------------------------------------------------------------------------------

QStringList LocationDialog::fileFilters()
{
    QStringList result;
    std::vector< std::pair < std::string, std::string > >::const_iterator iter;
    for ( iter = m_filters.begin(); iter != m_filters.end(); ++iter)
    {
        std::string filterName   = iter->first;
        std::string rawWildcards = iter->second;
        QString qFilterName      = QString::fromStdString(filterName);
        QString qRawWildcards    = QString::fromStdString(rawWildcards);

        result += qFilterName +" (" +  qRawWildcards +")";
    }
    return result;
}

//------------------------------------------------------------------------------

std::string LocationDialog::getCurrentSelection() const
{
    std::string extension;
    std::vector< std::pair < std::string, std::string > >::const_iterator iter;
    for ( iter = m_filters.begin(); iter != m_filters.end(); ++iter)
    {
        const std::string& filterName       = iter->first;
        const std::string& rawWildcards     = iter->second;
        const std::string& availableFilters = filterName + " (" +  rawWildcards + ")";
        if (!m_wildcard.compare(availableFilters) && iter != m_filters.begin())
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
