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

/// Structural creator for ::fwMedData::Patient version '1'.
class FWSTRUCTURALPATCH_CLASS_API Patient1 : public ::fwAtomsPatch::IStructuralCreator
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Patient1)(::fwAtomsPatch::IStructuralCreator), (()), new Patient1);

    /// Constructor
    FWSTRUCTURALPATCH_API Patient1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Patient1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Patient1( const Patient1& cpy );

    /// Creates the specified object (sets 'name', 'patient_id', 'birth_date' and 'sex' attributes).
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
