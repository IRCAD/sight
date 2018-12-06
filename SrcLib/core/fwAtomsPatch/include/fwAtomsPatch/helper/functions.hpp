/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWATOMSPATCH_HELPER_FUNCTIONS_HPP__
#define __FWATOMSPATCH_HELPER_FUNCTIONS_HPP__

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
namespace helper
{
/// Get classname of an object
FWATOMSPATCH_API std::string getClassname( const ::fwAtoms::Object::sptr & obj );

/// Set classname of an object
FWATOMSPATCH_API void setClassname( const ::fwAtoms::Object::sptr & obj, const std::string & newClassname );

/// Get version of an object
FWATOMSPATCH_API std::string getVersion( const ::fwAtoms::Object::sptr & obj );

/// Set version of an object
FWATOMSPATCH_API void setVersion( const ::fwAtoms::Object::sptr & obj, const std::string & newVersion );

/// Generates new id for the object (also creates it if it is missing )
FWATOMSPATCH_API void generateID( const ::fwAtoms::Object::sptr & obj );

/// Cleans object fields (also creates them if they are missing)
FWATOMSPATCH_API void cleanFields( const ::fwAtoms::Object::sptr & obj );

/// Generates new UUID for object and child objects
FWATOMSPATCH_API void changeUID( const ::fwAtoms::Object::sptr & obj );

/// Generates new UUID for map objects and child objects
FWATOMSPATCH_API void changeMapUID( const ::fwAtoms::Map::sptr & map );

/// Generates new UUID for seq objects and child objects
FWATOMSPATCH_API void changeSeqUID( const ::fwAtoms::Sequence::sptr & seq );

} //helper
} //fwAtomHelper

#endif // __FWATOMSPATCH_HELPER_FUNCTIONS_HPP__
