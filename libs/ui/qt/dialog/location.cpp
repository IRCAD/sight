/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
    const QString& caption = QString::fromStdString(this->get_title());
    const QString& path    = QString::fromStdString(this->get_default_location()->to_string());
    const QString& filter  = this->file_filters();
    core::location::base::sptr location;

    QFileDialog dialog;
    dialog.setDirectory(path);
    dialog.setNameFilter(filter);
    dialog.setWindowTitle(caption);
    // @todo: On Linux with GTK location dialog are stuck on .exec(), it seems that paint event is never posted.
    // Temporary we use qt dialog as a work-around.
#ifdef _WIN32_
    dialog.setOption(QFileDialog::Option::DontUseNativeDialog, !qgetenv("GUI_TESTS_ARE_RUNNING").isEmpty());
#else
    dialog.setOption(QFileDialog::Option::DontUseNativeDialog, true);
#endif

    if(((m_style& ui::dialog::location::read) != 0) || m_type == ui::dialog::location::folder)
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    }
    else
    {
        dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    }

    if(m_type == ui::dialog::location::single_file
       || m_type == ui::dialog::location::multi_files)
    {
        dialog.setFilter(QDir::Filter::Files);

        if(((m_style& ui::dialog::location::read) != 0)
           || ((m_style& ui::dialog::location::file_must_exist) != 0))
        {
            if(m_type == ui::dialog::location::single_file)
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
    else if(m_type == ui::dialog::location::folder)
    {
        dialog.setFilter(QDir::Filter::Dirs);
        dialog.setFileMode(QFileDialog::FileMode::Directory);
        dialog.setOption(QFileDialog::Option::ShowDirsOnly, true);
    }

    if(dialog.exec() == QFileDialog::Accepted)
    {
        const auto& selected_files = dialog.selectedFiles();
        if(!selected_files.isEmpty())
        {
            m_wildcard = dialog.selectedNameFilter().toStdString();

            if(m_type == ui::dialog::location::single_file)
            {
                const auto& selected_file = selected_files.constFirst();
                auto file                 = std::make_shared<core::location::single_file>();
                file->set_file(selected_file.toStdString());
                location = file;
            }
            else if(m_type == ui::dialog::location::multi_files)
            {
                std::vector<std::filesystem::path> paths;
                paths.reserve(size_t(selected_files.size()));
                for(const QString& file : selected_files)
                {
                    paths.emplace_back(file.toStdString());
                }

                auto multifiles = std::make_shared<core::location::multiple_files>();
                multifiles->set_files(paths);
                location = multifiles;
            }
            else if(m_type == ui::dialog::location::folder)
            {
                const auto& selected_directory = selected_files.constFirst();
                auto folder                    = std::make_shared<core::location::single_folder>();
                folder->set_folder(selected_directory.toStdString());
                location = folder;
            }
        }
    }

    return location;
}

//------------------------------------------------------------------------------

void location::set_type(location::types _type)
{
    m_type = _type;
}

//------------------------------------------------------------------------------

void location::set_option(location::options _option)
{
    if(_option == location::write)
    {
        m_style = static_cast<location::options>(m_style & ~location::read);
        m_style = static_cast<location::options>(m_style | location::write);
    }
    else if(_option == location::read)
    {
        m_style = static_cast<location::options>(m_style & ~location::write);
        m_style = static_cast<location::options>(m_style | location::read);
    }
    else if(_option == location::file_must_exist)
    {
        m_style = static_cast<location::options>(m_style | location::file_must_exist);
    }
}

//------------------------------------------------------------------------------

// example ( addFilter("images","*.png *.jpg");
void location::add_filter(const std::string& _filter_name, const std::string& _wildcard_list)
{
    m_filters.emplace_back(_filter_name, _wildcard_list);
}

//------------------------------------------------------------------------------

// "BMP and GIF files (*.bmp *.gif)|*.bmp *.gif|PNG files (*.png)|*.png"
QString location::file_filters()
{
    std::string result;
    for(auto iter = m_filters.begin() ; iter != m_filters.end() ; ++iter)
    {
        std::string filter_name   = iter->first;
        std::string raw_wildcards = iter->second;

        if(iter != m_filters.begin())
        {
            result += ";;";
        }

        result += filter_name + " (" + raw_wildcards + ")";
    }

    return QString::fromStdString(result);
}

//------------------------------------------------------------------------------

std::string location::get_current_selection() const
{
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

} // namespace sight::ui::qt::dialog
