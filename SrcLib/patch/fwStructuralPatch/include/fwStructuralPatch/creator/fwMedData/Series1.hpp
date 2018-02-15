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

/// Structural creator for ::fwMedData::Series version 1.
class FWSTRUCTURALPATCH_CLASS_API Series1 : public ::fwAtomsPatch::IStructuralCreator
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Series1)(::fwAtomsPatch::IStructuralCreator), (()), new Series1);

    /// Constructor
    FWSTRUCTURALPATCH_API Series1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Series1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Series1( const Series1& cpy );

    /**
     * @brief Creates the specified object.
     *
     * Calls creators for 'patient', 'study' and 'equipment' attributes and sets remaining attributes.
     */
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
