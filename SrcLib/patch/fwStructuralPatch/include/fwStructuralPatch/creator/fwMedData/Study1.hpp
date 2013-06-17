/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_CREATOR_FWMEDDATA_STUDY1_HPP__
#define __FWSTRUCTURALPATCH_CREATOR_FWMEDDATA_STUDY1_HPP__

#include <fwAtomsPatch/IStructuralCreator.hpp>
#include "fwStructuralPatch/config.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

/// Structural creator for ::fwMedData::Study version '1'.
class FWSTRUCTURALPATCH_CLASS_API Study1 : public ::fwAtomsPatch::IStructuralCreator
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Study1)(::fwAtomsPatch::IStructuralCreator),(()), new Study1) ;

    /// Constructor
    FWSTRUCTURALPATCH_API Study1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~Study1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API Study1( const Study1 &cpy );

    /**
     * @brief Creates the specified object.
     *
     * Sets 'instance_uid', 'date', 'time', 'referring_physician_name', 'description' and 'patient_age' attributes.
     */
    FWSTRUCTURALPATCH_API virtual ::fwAtoms::Object::sptr create();

};

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch

#endif // __FWSTRUCTURALPATCH_CREATOR_FWMEDDATA_STUDY1_HPP__
