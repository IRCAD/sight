/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "fwAtomsPatch/config.hpp"

#include <atoms/Base.hpp>
#include <atoms/Map.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>

namespace fwAtomsPatch
{
namespace helper
{
/// Get classname of an object
FWATOMSPATCH_API std::string getClassname( const atoms::Object::sptr& obj );

/// Set classname of an object
FWATOMSPATCH_API void setClassname( const atoms::Object::sptr& obj, const std::string& newClassname );

/// Get version of an object
FWATOMSPATCH_API std::string getVersion( const atoms::Object::sptr& obj );

/// Set version of an object
FWATOMSPATCH_API void setVersion( const atoms::Object::sptr& obj, const std::string& newVersion );

/// Generates new id for the object (also creates it if it is missing )
FWATOMSPATCH_API void generateID( const atoms::Object::sptr& obj );

/// Cleans object fields (also creates them if they are missing)
FWATOMSPATCH_API void cleanFields( const atoms::Object::sptr& obj );

/// Generates new UUID for object and child objects
FWATOMSPATCH_API void changeUID( const atoms::Object::sptr& obj );

/// Generates new UUID for map objects and child objects
FWATOMSPATCH_API void changeMapUID( const atoms::Map::sptr& map );

/// Generates new UUID for seq objects and child objects
FWATOMSPATCH_API void changeSeqUID( const atoms::Sequence::sptr& seq );

} //helper
} //fwAtomHelper
