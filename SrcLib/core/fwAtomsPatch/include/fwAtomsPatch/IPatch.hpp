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

#ifndef __FWATOMSPATCH_IPATCH_HPP__
#define __FWATOMSPATCH_IPATCH_HPP__

#include <string>
#include <map>

#include <fwCore/base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

/**
 * @class IPatch
 * @brief Base class of all patches
 */
class FWATOMSPATCH_CLASS_API IPatch : public ::fwCore::BaseObject
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro((IPatch));
    fwCoreAllowSharedFromThis();

    /**
     * @name Typedefs
     * @{ */
    typedef std::map< ::fwAtoms::Object::sptr, ::fwAtoms::Object::sptr > NewVersionsType;
    /**  @} */

    /// Constructor. Does nothing.
    FWATOMSPATCH_API IPatch();

    /// Copy constructor
    FWATOMSPATCH_API IPatch( const IPatch &cpy );

    /// Destructor. Does nothing.
    FWATOMSPATCH_API virtual ~IPatch();

    /// Applies the patch to the specified object
    FWATOMSPATCH_API virtual void apply(const ::fwAtoms::Object::sptr& previous,
                                        const ::fwAtoms::Object::sptr& current,
                                        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) = 0;

    /// Returns the origin classname of the object
    FWATOMSPATCH_API virtual const std::string& getOriginClassname() const;

    /// Returns the origin version of the object
    FWATOMSPATCH_API virtual const std::string& getOriginVersion() const;


protected:

    /// Origin classname of the object
    std::string m_originClassname;

    /// Origin version of the object
    std::string m_originVersion;

};


} //fwAtomsPatch
#endif /* __FWATOMSPATCH_IPATCH_HPP__ */
