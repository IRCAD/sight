/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/dialog/location.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/macros.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QString>

#include <filesystem>
#include <functional>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

core::location::base::sptr location::show()
{
    const QString& caption = QString::fromStdString(this->getTitle());
    const QString& path    = QString::fromStdString(this->getDefaultLocation()->to_string());
    const QString& filter  = this->fileFilters();
    core::location::base::sptr location;

    QFileDialog dialog;
    dialog.setDirectory(path);
    dialog.setNameFilter(filter);
    dialog.setWindowTitle(caption);
    dialog.setOption(QFileDialog::Option::DontUseNativeDialog, !qgetenv("GUI_TESTS_ARE_RUNNING").isEmpty());

    if(((m_style& ui::dialog::location::READ) != 0) || m_type == ui::dialog::location::FOLDER)
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    }
    else
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    }

    if(m_type == ui::dialog::location::SINGLE_FILE
       || m_type == ui::dialog::location::MULTI_FILES)
    {
        dialog.setFilter(QDir::Filter::Files);

        if(((m_style& ui::dialog::location::READ) != 0)
           || ((m_style& ui::dialog::location::FILE_MUST_EXIST) != 0))
        {
            if(m_type == ui::dialog::location::SINGLE_FILE)
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
    else if(m_type == ui::dialog::location::FOLDER)
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

            if(m_type == ui::dialog::location::SINGLE_FILE)
            {
                const auto& selectedFile = selectedFiles.constFirst();
                auto file                = std::make_shared<core::location::single_file>();
                file->set_file(selectedFile.toStdString());
                location = file;
            }
            else if(m_type == ui::dialog::location::MULTI_FILES)
            {
                std::vector<std::filesystem::path> paths;
                paths.reserve(size_t(selectedFiles.size()));
                for(const QString& file : selectedFiles)
                {
                    paths.emplace_back(file.toStdString());
                }

                auto multifiles = std::make_shared<core::location::multiple_files>();
                multifiles->set_files(paths);
                location = multifiles;
            }
            else if(m_type == ui::dialog::location::FOLDER)
            {
                const auto& selectedDirectory = selectedFiles.constFirst();
                auto folder                   = std::make_shared<core::location::single_folder>();
                folder->set_folder(selectedDirectory.toStdString());
                location = folder;
            }
        }
    }

    return location;
}

//------------------------------------------------------------------------------

void location::setType(location::Types type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

void location::setOption(location::Options option)
{
    if(option == location::WRITE)
    {
        m_style = static_cast<location::Options>(m_style & ~location::READ);
        m_style = static_cast<location::Options>(m_style | location::WRITE);
    }
    else if(option == location::READ)
    {
        m_style = static_cast<location::Options>(m_style & ~location::WRITE);
        m_style = static_cast<location::Options>(m_style | location::READ);
    }
    else if(option == location::FILE_MUST_EXIST)
    {
        m_style = static_cast<location::Options>(m_style | location::FILE_MUST_EXIST);
    }
}

//------------------------------------------------------------------------------

// example ( addFilter("images","*.png *.jpg");
void location::addFilter(const std::string& filterName, const std::string& wildcardList)
{
    m_filters.emplace_back(filterName, wildcardList);
}

//------------------------------------------------------------------------------

// "BMP and GIF files (*.bmp *.gif)|*.bmp *.gif|PNG files (*.png)|*.png"
QString location::fileFilters()
{
    std::string result;
    for(auto iter = m_filters.begin() ; iter != m_filters.end() ; ++iter)
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

std::string location::getCurrentSelection() const
{
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

} // namespace sight::ui::qt::dialog
