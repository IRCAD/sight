/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/Runtime.hpp"

#include "core/runtime/ExecutableFactory.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/IExecutable.hpp"
#include "core/runtime/operations.hpp"
#include <core/spyLog.hpp>

#include <boost/algorithm/string/trim.hpp>

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

Runtime::Runtime()
{
}

//------------------------------------------------------------------------------

Runtime::~Runtime()
{
}

//------------------------------------------------------------------------------

Runtime* Runtime::getDefault()
{
    return detail::Runtime::getDefault();
}

//------------------------------------------------------------------------------

Runtime& Runtime::get()
{
    return detail::Runtime::get();
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> Runtime::findExtension(const std::string& identifier) const
{
    std::shared_ptr<Extension> resExtension;
    for(const ExtensionContainer::value_type& extension : m_extensions)
    {
        if(extension->getIdentifier() == identifier && extension->isEnabled())
        {
            resExtension = extension;
            break;
        }
    }

    return resExtension;
}

//------------------------------------------------------------------------------

void Runtime::setWorkingPath(const std::filesystem::path&)
{
}

//-----------------------------------------------------------------------------

std::string filterID(const std::string& identifier)
{
    return boost::algorithm::trim_left_copy_if(identifier, [](auto x){return x == ':';});
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
