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

#include "fwGuiQt/dialog/LocationDialog.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/registry/macros.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QString>

#include <functional>

fwGuiRegisterMacro( ::fwGuiQt::dialog::LocationDialog, ::fwGui::dialog::ILocationDialog::REGISTRY_KEY );

namespace fwGuiQt
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
    QWidget* parent                             = qApp->activeWindow();
    QString caption                             = QString::fromStdString(this->getTitle());
    const ::boost::filesystem::path defaultPath = this->getDefaultLocation();
    QString path                                = QString::fromStdString(defaultPath.string());
    QString filter                              = this->fileFilters();
    ::fwData::location::ILocation::sptr location;

    QFileDialog dialog;
    dialog.setDirectory(path);
    dialog.setNameFilter(filter);
    dialog.setWindowTitle(caption);

    if (m_style & ::fwGui::dialog::ILocationDialog::READ)
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    }
    else
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    }

    if (m_type == ::fwGui::dialog::ILocationDialog::MULTI_FILES)
    {
        SLM_ASSERT("MULTI_FILES type must have a READ style", m_style & ::fwGui::dialog::ILocationDialog::READ);

        dialog.setFilter(QDir::Filter::Files);
        dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
        QStringList files;
        if (dialog.exec())
        {
            files      = dialog.selectedFiles();
            m_wildcard = dialog.selectedNameFilter().toStdString();

        }
        if(!files.isEmpty())
        {
            ::fwData::location::MultiFiles::sptr multifiles = ::fwData::location::MultiFiles::New();
            std::vector< ::boost::filesystem::path > paths;
            for (QString filename : files)
            {
                ::boost::filesystem::path bpath( filename.toStdString() );
                paths.push_back(bpath);
            }
            multifiles->setPaths(paths);
            location = multifiles;
        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::SINGLE_FILE)
    {
        QString fileName;
        if ( (m_style& ::fwGui::dialog::ILocationDialog::READ) ||
             (m_style & ::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST) )
        {
            dialog.setFileMode(QFileDialog::FileMode::ExistingFile);
            if (dialog.exec() && !dialog.selectedFiles().empty())
            {
                fileName   = dialog.selectedFiles()[0];
                m_wildcard = dialog.selectedNameFilter().toStdString();
            }
        }
        else if ( m_style & ::fwGui::dialog::ILocationDialog::WRITE )
        {
            if (dialog.exec() && !dialog.selectedFiles().empty())
            {
                fileName   = dialog.selectedFiles()[0];
                m_wildcard = dialog.selectedNameFilter().toStdString();
            }

        }
        if(!fileName.isNull())
        {
            ::boost::filesystem::path bpath( fileName.toStdString());
            location = ::fwData::location::SingleFile::New(bpath);
        }
    }
    else if (m_type == ::fwGui::dialog::ILocationDialog::FOLDER)
    {
        dialog.setFilter(QDir::Filter::Dirs);
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
        dialog.setFileMode(QFileDialog::FileMode::Directory);

        QString dir;
        if (dialog.exec() && !dialog.selectedFiles().empty())
        {
            dir = dialog.selectedFiles()[0];
        }

        if(!dir.isNull())
        {
            ::boost::filesystem::path bpath( dir.toStdString()  );
            location = ::fwData::location::Folder::New(bpath);
        }
    }
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
QString LocationDialog::fileFilters()
{
    std::string result;
    std::vector< std::pair < std::string, std::string > >::const_iterator iter;
    for ( iter = m_filters.begin(); iter != m_filters.end(); ++iter)
    {
        std::string filterName   = iter->first;
        std::string rawWildcards = iter->second;

        if (iter != m_filters.begin() )
        {
            result += ";;";
        }
        result += filterName +" (" +  rawWildcards +")";
    }
    return QString::fromStdString(result);
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
} //namespace fwGuiQt
