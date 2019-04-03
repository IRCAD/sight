/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <QFileDialog>
#include <QGuiApplication>
#include <QString>

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
    //    QWidget* parent                             = qGuiApp->focusWindow();
    QString caption                             = QString::fromStdString(this->getTitle());
    const ::boost::filesystem::path defaultPath = this->getDefaultLocation();
    QString path                                = QString::fromStdString(defaultPath.string());
    QStringList filter                          = this->fileFilters();
    ::fwData::location::ILocation::sptr location;

    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/LocationDialog.qml");
    // load the qml ui component
    m_dialog = engine->createComponent(dialogPath);
    m_dialog->setProperty("folder", QUrl(path));
    m_dialog->setProperty("title", caption);
    m_dialog->setProperty("nameFilters", filter);

    if (m_style & ::fwGui::dialog::ILocationDialog::READ)
    {
        m_dialog->setProperty("selectExisting", true);
    }
    else
    {
        m_dialog->setProperty("selectExisting", false);
    }

    if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
    {
        SLM_ASSERT("MULTI_FILES type must have a READ style", m_style & ::fwGui::dialog::ILocationDialog::READ);

        m_dialog->setProperty("selectFolder", false);
        m_dialog->setProperty("selectExisting", true);
        m_dialog->setProperty("selectMultiple", true);
        QStringList files;
//        if (dialog.exec())
//        {
//            files      = dialog.selectedFiles();
//            m_wildcard = dialog.selectedNameFilter().toStdString();

//        }
//        if(!files.isEmpty())
//        {
//            ::fwData::location::MultiFiles::sptr multifiles = ::fwData::location::MultiFiles::New();
//            std::vector< ::boost::filesystem::path > paths;
//            for (QString filename : files)
//            {
//                ::boost::filesystem::path bpath( filename.toStdString() );
//                paths.push_back(bpath);
//            }
//            multifiles->setPaths(paths);
//            location = multifiles;
//        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE)
    {
        if ( (m_style& ::fwGui::dialog::ILocationDialog::READ) ||
             (m_style & ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST) )
        {
            m_dialog->setProperty("selectExisting", true);
//            if (dialog.exec() && !dialog.selectedFiles().empty())
//            {
//                fileName   = dialog.selectedFiles()[0];
//                m_wildcard = dialog.selectedNameFilter().toStdString();
//            }
        }
        else if ( m_style & ::fwGui::dialog::ILocationDialog::WRITE )
        {
//            if (dialog.exec() && !dialog.selectedFiles().empty())
//            {
//                fileName   = dialog.selectedFiles()[0];
//                m_wildcard = dialog.selectedNameFilter().toStdString();
//            }

        }
//        if(!fileName.isNull())
//        {
//            ::boost::filesystem::path bpath( fileName.toStdString());
//            location = ::fwData::location::SingleFile::New(bpath);
//        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
    {
        m_dialog->setProperty("selectExisting", true);
        m_dialog->setProperty("selectFolder", true);

//        QString dir;
//        if (dialog.exec() && !dialog.selectedFiles().empty())
//        {
//            dir = dialog.selectedFiles()[0];
//        }

//        if(!dir.isNull())
//        {
//            ::boost::filesystem::path bpath( dir.toStdString()  );
//            location = ::fwData::location::Folder::New(bpath);
//        }
    }
    QMetaObject::invokeMethod(m_dialog, "open");
    return location;
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

// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList )
{
    m_filters.push_back( std::make_pair( filterName, wildcardList ));
}

//------------------------------------------------------------------------------

// "BMP and GIF files (*.bmp *.gif)|*.bmp *.gif|PNG files (*.png)|*.png"
QStringList LocationDialog::fileFilters()
{
    QStringList result;
    std::vector< std::pair < std::string, std::string > >::const_iterator iter;
    for ( iter = m_filters.begin(); iter != m_filters.end(); ++iter)
    {
        std::string filterName   = iter->first;
        std::string rawWildcards = iter->second;
        QString qFilterName      = QString::fromUtf8(filterName.data(), filterName.size());
        QString qRawWildcards    = QString::fromUtf8(rawWildcards.data(), rawWildcards.size());

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
