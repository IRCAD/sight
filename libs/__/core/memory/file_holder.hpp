/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include <core/macros.hpp>

#include <filesystem>

namespace sight::core::memory
{

struct file_auto_delete;

class SIGHT_CORE_CLASS_API file_holder
{
public:

    file_holder()
    = default;

    SIGHT_CORE_API file_holder(const std::filesystem::path& _file, bool _autodelete = false);

    operator std::filesystem::path() const
    {
        return m_path;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool empty() const
    {
        return m_path.empty();
    }

    //------------------------------------------------------------------------------

    void clear()
    {
        m_path.clear();
        m_auto_delete.reset();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::string string() const
    {
        return m_path.string();
    }

protected:

    std::filesystem::path m_path;
    SPTR(file_auto_delete) m_auto_delete;
};

} // namespace sight::core::memory
