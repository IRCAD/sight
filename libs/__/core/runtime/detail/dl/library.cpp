/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/runtime/detail/dl/library.hpp"

#include "core/runtime/detail/dl/posix.hpp"
#include "core/runtime/detail/dl/win32.hpp"

namespace sight::core::runtime::detail::dl
{

//------------------------------------------------------------------------------

#if defined(__unix__)
using library_impl_type = posix;
#else
typedef win32 library_impl_type;
#endif

//------------------------------------------------------------------------------

library::library(const std::filesystem::path& _module_path) noexcept :
    m_implementor(std::make_unique<library_impl_type>(_module_path))
{
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::dl
