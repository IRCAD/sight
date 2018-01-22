/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwStructuralPatch/config.hpp"
#include "fwStructuralPatch/creator/fwMedData/Series1.hpp"

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

/// Structural creator for ::fwMedData::ActivitySeries version 1.
class FWSTRUCTURALPATCH_CLASS_API ActivitySeries1 : public ::fwStructuralPatch::creator::fwMedData::Series1
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (ActivitySeries1)(::fwStructuralPatch::creator::fwMedData::Series1), (()), new ActivitySeries1);

    /// Constructor
    FWSTRUCTURALPATCH_API ActivitySeries1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~ActivitySeries1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API ActivitySeries1( const ActivitySeries1& cpy );

    /**
     * @brief Creates the specified object.
     */
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
