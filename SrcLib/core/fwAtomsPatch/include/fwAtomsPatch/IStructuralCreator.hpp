/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_ISTRUCTURALCREATOR_HPP__
#define __FWATOMSPATCH_ISTRUCTURALCREATOR_HPP__

#include <string>

#include <fwCore/base.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomsPatch/config.hpp"


namespace fwAtomsPatch
{

/**
 * @class IStructuralCreator
 * @brief Base class of all structural creators. Creates a new atom object with the default values corresponding
 * to a data object.
 */
class FWATOMSPATCH_CLASS_API IStructuralCreator : public ::fwCore::BaseObject
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro((IStructuralCreator));
    fwCoreAllowSharedFromThis();

    /// Constructor
    FWATOMSPATCH_API IStructuralCreator();

    /// Copy constructor
    FWATOMSPATCH_API IStructuralCreator( const IStructuralCreator &cpy );

    /// Destructor
    FWATOMSPATCH_API virtual ~IStructuralCreator();

    /// Creates the specified object
    FWATOMSPATCH_API virtual ::fwAtoms::Object::sptr create() = 0;

    /// Returns the version of the object
    FWATOMSPATCH_API virtual const std::string& getObjectVersion() const;

    /// Returns the classname of the object
    FWATOMSPATCH_API virtual const std::string& getObjectClassname() const;

protected:

    /// Create the object base : create meta infos : classname, version and id
    FWATOMSPATCH_API ::fwAtoms::Object::sptr createObjBase() const;

    /// Object classname
    std::string m_classname;

    /// Object version
    std::string m_version;
};


} //fwAtomsPatch
#endif /* __FWATOMSPATCH_ISTRUCTURALCREATOR_HPP__ */
