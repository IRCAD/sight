/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
// cspell:ignore qgetenv

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
    const QString& caption = QString::fromStdString(this->getTitle());
    const QString& path    = QString::fromStdString(this->getDefaultLocation()->toString());
    const QString& filter  = this->fileFilters();
    core::location::ILocation::sptr location;

    QFileDialog dialog;
    dialog.setDirectory(path);
    dialog.setNameFilter(filter);
    dialog.setWindowTitle(caption);
    dialog.setOption(QFileDialog::Option::DontUseNativeDialog, !qgetenv("GUI_TESTS_ARE_RUNNING").isEmpty());

    if(m_style & ui::base::dialog::ILocationDialog::READ || m_type == ui::base::dialog::ILocationDialog::FOLDER)
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    }
    else
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    }

    if(m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE
       || m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
    {
        dialog.setFilter(QDir::Filter::Files);

        if((m_style & ui::base::dialog::ILocationDialog::READ)
           || (m_style & ui::base::dialog::ILocationDialog::FILE_MUST_EXIST))
        {
            if(m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE)
            {
                dialog.setFileMode(QFileDialog::FileMode::ExistingFile);
            }
            else
            {
                dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);
            }
        }
        else
        {
            dialog.setFileMode(QFileDialog::FileMode::AnyFile);
        }
    }
    else if(m_type == ui::base::dialog::ILocationDialog::FOLDER)
    {
        dialog.setFilter(QDir::Filter::Dirs);
        dialog.setFileMode(QFileDialog::FileMode::Directory);
        dialog.setOption(QFileDialog::Option::ShowDirsOnly, true);
    }

    if(dialog.exec() == QFileDialog::Accepted)
    {
        const auto& selectedFiles = dialog.selectedFiles();
        if(!selectedFiles.isEmpty())
        {
            m_wildcard = dialog.selectedNameFilter().toStdString();

            if(m_type == ui::base::dialog::ILocationDialog::SINGLE_FILE)
            {
                const auto& selectedFile = selectedFiles.constFirst();
                auto file                = std::make_shared<core::location::SingleFile>();
                file->setFile(selectedFile.toStdString());
                location = file;
            }
            else if(m_type == ui::base::dialog::ILocationDialog::MULTI_FILES)
            {
                std::vector<std::filesystem::path> paths;
                paths.reserve(selectedFiles.size());
                for(const QString& file : selectedFiles)
                {
                    paths.push_back(file.toStdString());
                }

                auto multifiles = std::make_shared<core::location::MultipleFiles>();
                multifiles->setFiles(paths);
                location = multifiles;
            }
            else if(m_type == ui::base::dialog::ILocationDialog::FOLDER)
            {
                const auto& selectedDirectory = selectedFiles.constFirst();
                auto folder                   = std::make_shared<core::location::SingleFolder>();
                folder->setFolder(selectedDirectory.toStdString());
                location = folder;
            }
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
    using ui::base::dialog::ILocationDialog;
    if(option == ILocationDialog::WRITE)
    {
        m_style = static_cast<ILocationDialog::Options>(m_style & ~ILocationDialog::READ);
        m_style = static_cast<ILocationDialog::Options>(m_style | ILocationDialog::WRITE);
    }
    else if(option == ILocationDialog::READ)
    {
        m_style = static_cast<ILocationDialog::Options>(m_style & ~ILocationDialog::WRITE);
        m_style = static_cast<ILocationDialog::Options>(m_style | ILocationDialog::READ);
    }
    else if(option == ILocationDialog::FILE_MUST_EXIST)
    {
        m_style = static_cast<ILocationDialog::Options>(m_style | ILocationDialog::FILE_MUST_EXIST);
    }

    return *this;
}

//------------------------------------------------------------------------------

// example ( addFilter("images","*.png *.jpg");
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
