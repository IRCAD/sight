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

#pragma once

#include "core/config.hpp"
#include "core/memory/stream/in/factory.hpp"

#include <core/macros.hpp>

#include <filesystem>
#include <utility>

namespace sight::core::memory::stream::in
{

class CORE_CLASS_API raw_z : public factory
{
public:

    raw_z(std::filesystem::path _path) :
        m_path(std::move(_path))
    {
    }

protected:

    CORE_API SPTR(std::istream) get() override;

    std::filesystem::path m_path;
};

} // namespace sight::core::memory::stream::in
