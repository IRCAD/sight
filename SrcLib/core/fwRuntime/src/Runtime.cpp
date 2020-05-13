/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/detail/Runtime.hpp"
#include "fwRuntime/Runtime.hpp"

#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/operations.hpp"

#include <fwCore/spyLog.hpp>

namespace fwRuntime
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

void Runtime::addDefaultBundles()
{
    // Now done in ::fwRuntime::init()
    ::fwRuntime::init();
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> Runtime::findExtension( const std::string& identifier ) const
{
    std::shared_ptr<Extension> resExtension;
    for(const ExtensionContainer::value_type& extension :  m_extensions)
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

void Runtime::setWorkingPath(const std::filesystem::path& )
{
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
