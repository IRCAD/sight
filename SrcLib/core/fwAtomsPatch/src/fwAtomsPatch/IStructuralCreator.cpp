/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/types.hpp>

#include "fwAtomsPatch/IStructuralCreator.hpp"
#include "fwAtomsPatch/helper/functions.hpp"

namespace fwAtomsPatch
{

IStructuralCreator::IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::~IStructuralCreator()
{
}

// ----------------------------------------------------------------------------

IStructuralCreator::IStructuralCreator( const IStructuralCreator &cpy )
{
    m_classname =  cpy.getObjectClassname();
    m_version = cpy.getObjectVersion();
}

// ----------------------------------------------------------------------------

const std::string& IStructuralCreator::getObjectVersion() const
{
    return m_version;
}

// ----------------------------------------------------------------------------

const std::string& IStructuralCreator::getObjectClassname() const
{
    return m_classname;
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr IStructuralCreator::createObjBase() const
{
    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::New();

    ::fwAtomsPatch::helper::setClassname(obj, m_classname);
    ::fwAtomsPatch::helper::setVersion(obj, m_version);
    ::fwAtomsPatch::helper::generateID( obj );

    // All data objects has a parameter fields
    fwAtomsPatch::helper::cleanFields( obj );

    return obj;
}

// ----------------------------------------------------------------------------

} //fwAtomsPatch
