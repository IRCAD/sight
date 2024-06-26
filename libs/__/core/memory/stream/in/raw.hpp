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

#include "core/memory/file_holder.hpp"
#include "core/memory/stream/in/factory.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <utility>

namespace sight::core::memory::stream::in
{

class SIGHT_CORE_CLASS_API raw : public factory
{
public:

    raw(const std::filesystem::path& _path) :
        m_path(_path)
    {
    }

    raw(core::memory::file_holder _path) :
        m_path(std::move(_path))
    {
    }

protected:

    SIGHT_CORE_API SPTR(std::istream) get() override;

    core::memory::file_holder m_path;
};

} // namespace sight::core::memory::stream::in
