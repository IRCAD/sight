/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwStructuralPatch/config.hpp"
#include "fwStructuralPatch/creator/fwMedData/Series1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

/// Structural creator for ::fwMedData::ModelSeries version '1'.
class FWSTRUCTURALPATCH_CLASS_API ModelSeries1 : public ::fwStructuralPatch::creator::fwMedData::Series1
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ModelSeries1)(::fwStructuralPatch::creator::fwMedData::Series1),
                                            (()), new ModelSeries1);

    /// Constructor
    FWSTRUCTURALPATCH_API ModelSeries1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~ModelSeries1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API ModelSeries1( const ModelSeries1& cpy );

    /// Creates the specified object (sets 'reconstruction_db' attribute) and calls creator 'Series1'.
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create() override;

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
