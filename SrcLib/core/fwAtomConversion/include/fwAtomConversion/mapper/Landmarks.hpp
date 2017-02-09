/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_LANDMARKS_HPP__
#define __FWATOMCONVERSION_MAPPER_LANDMARKS_HPP__

#include "fwAtomConversion/AtomVisitor.hpp"
#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/mapper/Base.hpp"

namespace fwAtoms
{
class Object;
}

namespace fwData
{
class Object;
}

namespace fwAtomConversion
{
namespace mapper
{

/**
 * @brief Specific mapper used to convert a ::fwData::Landmarks.
 *
 * The attribute ::fwData::Landmarks::m_landmarks is represented in fwAtoms world like a fwAtoms::Map that contains
 * fwAtoms::Object with five attributes "color", "size", "shape", "visibility" and "points". The points are store in a
 * ::fwAtoms::Sequence.
 */
class FWATOMCONVERSION_CLASS_API Landmarks : public Base
{
public:

    /**
     * @brief Convert a ::fwData::Object to a ::fwAtoms::Object.
     * @param object data to convert. It must be a ::fwData::Landmarks.
     * @param cache  cache to register the data already converted, used when a data is referenced multiple times.
     */
    FWATOMCONVERSION_API virtual SPTR(::fwAtoms::Object) convert( SPTR(::fwData::Object) object,
                                                                  DataVisitor::AtomCacheType & cache );

    /**
     * @brief Convert a ::fwAtoms::Object to a ::fwData::Object (which represent a ::fwData::Landmarks).
     * @param atom atom to convert
     * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times.
     * @param uuidPolicy AtomVisitor policy
     */
    FWATOMCONVERSION_API virtual SPTR(::fwData::Object) convert( SPTR(::fwAtoms::Object) atom,
                                                                 AtomVisitor::DataCacheType & cache,
                                                                 const AtomVisitor::IReadPolicy &uuidPolicy
                                                                 );

};

}
}

#endif /*  __FWATOMCONVERSION_MAPPER_LANDMARKS_HPP__*/
