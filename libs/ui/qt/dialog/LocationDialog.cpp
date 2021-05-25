/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/dialog/LocationDialog.hpp"

#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>
#include <ui/base/registry/macros.hpp>

#include <boost/tokenizer.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QString>

#include <filesystem>
#include <functional>

fwGuiRegisterMacro(::sight::ui::qt::dialog::LocationDialog, ::sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY);

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

LocationDialog::LocationDialog(ui::base::GuiBaseObject::Key key) :
    m_style(ui::base::dialog::ILocationDialog::NONE),
    m_type(ui::base::dialog::ILocationDialog::SINGLE_FILE)
{
}

//------------------------------------------------------------------------------

core::location::ILocation::sptr LocationDialog::show()
{
    QWidget* parent = qApp->activeWindow();
    QString caption = QString::fromStdString(this->getTitle());
    QString path    = QString::fromStdString(this->getDefaultLocation()->toString());
    QString filter  = this->fileFilters();
    core::location::ILocation::sptr location;

    QFileDialog dialog;
    dialog.setDirectory(path);
    dialog.setNameFilter(filter);
    dialog.setWindowTitle(caption);

    if(m_style & ui::base::dialog::ILocationDialog::READ)
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    }
    else
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    }

    if(m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
    {
        SIGHT_ASSERT("MULTI_FILES type must have a READ style", m_style & ui::base::dialog::ILocationDialog::READ);

        dialog.setFilter(QDir::Filter::Files);
        dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
        QStringList files;
        if(dialog.exec())
        {
            files      = dialog.selectedFiles();
            m_wildcard = dialog.selectedNameFilter().toStdString();
        }

        if(!files.isEmpty())
        {
            std::vector<std::filesystem::path> paths;
            for(QString filename : files)
            {
                std::filesystem::path bpath(filename.toStdString());
                paths.push_back(bpath);
            }

            const auto& multipleFiles = std::make_shared<core::location::MultipleFiles>();
            multipleFiles->setFiles(paths);
            location = multipleFiles;
        }
    }
    else if(m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE)
    {
        QString fileName;
        if((m_style & ui::base::dialog::ILocationDialog::READ)
           || (m_style & ui::base::dialog::ILocationDialog::FILE_MUST_EXIST))
        {
            dialog.setFileMode(QFileDialog::FileMode::ExistingFile);
            if(dialog.exec() && !dialog.selectedFiles().empty())
            {
                fileName   = dialog.selectedFiles()[0];
                m_wildcard = dialog.selectedNameFilter().toStdString();
            }
        }
        else if(m_style & ui::base::dialog::ILocationDialog::WRITE)
        {
            if(dialog.exec() && !dialog.selectedFiles().empty())
            {
                fileName   = dialog.selectedFiles()[0];
                m_wildcard = dialog.selectedNameFilter().toStdString();
            }
        }

        if(!fileName.isNull())
        {
            const auto& singleFile = std::make_shared<core::location::SingleFile>();
            singleFile->setFile(fileName.toStdString());
            location = singleFile;
        }
    }
    else if(m_type == ui::base::dialog::ILocationDialog::FOLDER)
    {
        dialog.setFilter(QDir::Filter::Dirs);
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
        dialog.setFileMode(QFileDialog::FileMode::Directory);

        QString dir;
        if(dialog.exec() && !dialog.selectedFiles().empty())
        {
            dir = dialog.selectedFiles()[0];
        }

        if(!dir.isNull())
        {
            const auto& singleDirectory = std::make_shared<core::location::SingleFolder>();
            singleDirectory->setFolder(dir.toStdString());
            location = singleDirectory;
        }
    }

    return location;
}

//------------------------------------------------------------------------------

void LocationDialog::setType(ui::base::dialog::ILocationDialog::Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

ui::base::dialog::ILocationDialog& LocationDialog::setOption(ui::base::dialog::ILocationDialog::Options option)
{
    if(option == ui::base::dialog::ILocationDialog::WRITE)
    {
        m_style = (ui::base::dialog::ILocationDialog::Options) (m_style & ~ui::base::dialog::ILocationDialog::READ);
        m_style = (ui::base::dialog::ILocationDialog::Options) (m_style | ui::base::dialog::ILocationDialog::WRITE);
    }
    else if(option == ui::base::dialog::ILocationDialog::READ)
    {
        m_style = (ui::base::dialog::ILocationDialog::Options) (m_style & ~ui::base::dialog::ILocationDialog::WRITE);
        m_style = (ui::base::dialog::ILocationDialog::Options) (m_style | ui::base::dialog::ILocationDialog::READ);
    }
    else if(option == ui::base::dialog::ILocationDialog::FILE_MUST_EXIST)
    {
        m_style =
            (ui::base::dialog::ILocationDialog::Options) (m_style
                                                          | ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);
    }

    return *this;
}

//------------------------------------------------------------------------------

// exemple ( addFilter("images","*.png *.jpg");
void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_filters.push_back(std::make_pair(filterName, wildcardList));
}

//------------------------------------------------------------------------------

// "BMP and GIF files (*.bmp *.gif)|*.bmp *.gif|PNG files (*.png)|*.png"
QString LocationDialog::fileFilters()
{
    std::string result;
    std::vector<std::pair<std::string, std::string> >::const_iterator iter;
    for(iter = m_filters.begin() ; iter != m_filters.end() ; ++iter)
    {
        std::string filterName   = iter->first;
        std::string rawWildcards = iter->second;

        if(iter != m_filters.begin())
        {
            result += ";;";
        }

        result += filterName + " (" + rawWildcards + ")";
    }

    return QString::fromStdString(result);
}

//------------------------------------------------------------------------------

std::string LocationDialog::getCurrentSelection() const
{
    std::string extension;
    std::vector<std::pair<std::string, std::string> >::const_iterator iter;
    for(iter = m_filters.begin() ; iter != m_filters.end() ; ++iter)
    {
        const std::string& filterName       = iter->first;
        const std::string& rawWildcards     = iter->second;
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

} //namespace sight::ui::qt
