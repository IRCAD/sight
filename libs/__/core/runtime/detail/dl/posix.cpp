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

#if defined(__unix__)

#include "core/runtime/detail/dl/posix.hpp"

#include "core/runtime/module.hpp"

namespace sight::core::runtime::detail::dl
{

//------------------------------------------------------------------------------

posix::posix(const std::filesystem::path& _module_path) noexcept :
    native(_module_path)
{
}

//------------------------------------------------------------------------------

posix::~posix() noexcept =
    default;

//------------------------------------------------------------------------------

bool posix::is_loaded() const noexcept
{
    return m_handle != nullptr;
}

//------------------------------------------------------------------------------

void* posix::get_symbol(const std::string& _name) const
{
    void* result = nullptr;
    if(is_loaded())
    {
        dlerror(); /* Clear existing error */
        result = dlsym(m_handle, _name.c_str());
        if(result == nullptr) /* Check for possible errors */
        {
            std::string message(dlerror());
            if(!message.empty())
            {
                throw runtime_exception("Symbol retrieval failed. " + message);
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void posix::load()
{
    if(m_handle == nullptr)
    {
        // Opens the dynamic library.
        m_handle = dlopen(get_full_path().string().c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if(m_handle == nullptr)
        {
            std::string message(dlerror());
            throw runtime_exception("Module load failed. " + message);
        }
    }
}

//------------------------------------------------------------------------------

void posix::unload()
{
    if(m_handle != nullptr)
    {
        int result = 0;
        result = dlclose(m_handle);
        if(result != 0)
        {
            std::string message(dlerror());
            throw runtime_exception("Module unload failed. " + message);
        }

        m_handle = nullptr;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::dl

#endif // #if defined(__unix__)
