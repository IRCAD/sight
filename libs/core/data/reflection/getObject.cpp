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

#include "data/reflection/getObject.hpp"

#include "data/Object.hpp"
#include "data/reflection/exception/NullPointer.hpp"
#include "data/reflection/exception/ObjectNotFound.hpp"
#include "data/reflection/visitor/GetObject.hpp"

#include <core/base.hpp>

namespace sight::data
{
namespace reflection
{
//------------------------------------------------------------------------------

data::Object::sptr getObject( data::Object::csptr object,
                              const std::string& path,
                              bool raiseException )
{
    SIGHT_FATAL_IF( "Path for desired object is empty.", path.empty() );
    SIGHT_FATAL_IF( "Path for desired object doesn't start with '@'.", path[0] != '@' );
    const std::string objectPath = path.substr( 1 );
    data::reflection::visitor::GetObject visitor( object, objectPath );
    data::Object::sptr subObject;
    try
    {
        subObject = visitor.get();
    }
    catch(const data::reflection::exception::NullPointer& np)
    {
        FW_FORWARD_EXCEPTION_IF(np, raiseException);
    }

    SIGHT_THROW_EXCEPTION_IF(
        data::reflection::exception::ObjectNotFound("Object '" + path + "' not found."),
        !visitor.objectsFound() && raiseException);

    return subObject;
}

} // namespace reflection
} // namespace sight::data
