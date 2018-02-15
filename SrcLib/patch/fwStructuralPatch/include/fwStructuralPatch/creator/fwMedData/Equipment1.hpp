/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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
namespace fwMedData
{

/// Structural creator for ::fwMedData::Equipment version '1'.
class FWSTRUCTURALPATCH_CLASS_API Equipment1 : public ::fwAtomsPatch::IStructuralCreator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Equipment1)(::fwAtomsPatch::IStructuralCreator), (()), new Equipment1);

    /// Constructor
    FWSTRUCTURALPATCH_API Equipment1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Equipment1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Equipment1( const Equipment1& cpy );

    /// Create the specified object (sets 'institution_name' attribute).
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
