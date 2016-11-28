/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_ATTACHMENTSERIES_HPP__
#define __FWATOMCONVERSION_MAPPER_ATTACHMENTSERIES_HPP__


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
 * @brief Specific mapper used to convert a ::fwMedData::AttachmentSeries.
 */
class FWATOMCONVERSION_CLASS_API AttachmentSeries : public Base
{
public:

    /**
     * @brief Convert a ::fwData::Object to a ::fwAtoms::Object.
     * @param object data to convert. It must be a ::fwMedData::AttachmentSeries.
     * @param cache  cache to register the data already converted, used when a data is referenced multiple times.
     */
    FWATOMCONVERSION_API virtual SPTR(::fwAtoms::Object) convert ( SPTR(::fwData::Object) object,
                                                                   DataVisitor::AtomCacheType & cache );

    /**
     * @brief Convert a ::fwAtoms::Object to a ::fwData::Object (which represent a ::fwMedData::AttachmentSeries).
     * @param atom atom to convert
     * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times.
     * @param uuidPolicy AtomVisitor policy
     */
    FWATOMCONVERSION_API virtual SPTR(::fwData::Object) convert ( SPTR(::fwAtoms::Object) atom,
                                                                  AtomVisitor::DataCacheType & cache,
                                                                  const AtomVisitor::IReadPolicy &uuidPolicy
                                                                  );

};

} // namespace mapper
} // namespace fwAtomConversion

#endif /*  __FWATOMCONVERSION_MAPPER_ATTACHMENTSERIES_HPP__*/
