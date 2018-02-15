/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwStructuralPatch/config.hpp"

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwData
{

/// Structural creator for ::fwData::Landmarks version '1'.
class FWSTRUCTURALPATCH_CLASS_API Landmarks1 : public ::fwAtomsPatch::IStructuralCreator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Landmarks1)(::fwAtomsPatch::IStructuralCreator), (()), new Landmarks1);

    /// Constructor
    FWSTRUCTURALPATCH_API Landmarks1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Landmarks1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Landmarks1( const Landmarks1& cpy );

    /// Create the specified object (sets 'landmarks' attribute).
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwData
} // namespace creator
} // namespace fwStructuralPatch
