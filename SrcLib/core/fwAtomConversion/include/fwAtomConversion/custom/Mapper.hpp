/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CUSTOM_MAPPER_HPP__
#define __FWATOMCONVERSION_CUSTOM_MAPPER_HPP__

#include <iostream>
#include <map>
#include <string>

#include <camp/class.hpp>

#include <fwData/Object.hpp>
#include <fwAtomConversion/AtomHelper.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/custom/factory/new.hpp"
#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace custom
{

/**
 * Base class of custom mapper between fwData and MetaData
 */
class FWATOMCONVERSION_CLASS_API Mapper
{
public:

    /**
     * @brief Class used to register a class factory in factory registry.
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwAtomConversion::custom::registry::get()->addFactory(
                    functorKey,
                    &::fwAtomConversion::custom::factory::New<T>
                    );
        }
    };

    virtual ~Mapper(){};

    /**
     * Convert fwData to MetaData
     * @param object the origin fwData
     * @param metaHelper this metaHelper is used to compute internal
     * fwDataObject to metaData
     * @return the mapped metaData
     */
    FWATOMCONVERSION_API virtual ::fwAtoms::Object::sptr
                                  toMeta( ::fwData::Object::sptr object,
                                          ::fwAtomConversion::AtomHelper& metaHelper) = 0;

    /**
     * Convert MetaData to fwData
     * @param meta the origin metaData
     * @param metaHelper this metaHelper is used to compute internal
     * metaData
     * @return the new fwData
     **/
    FWATOMCONVERSION_API virtual ::fwData::Object::sptr
                                 fromMeta( ::fwAtoms::Object::sptr meta,
                                           ::fwAtomConversion::AtomHelper& metaHelper) = 0;

protected:

    ::fwAtoms::Object::sptr computeMinimalInfos( ::fwData::Object::sptr object,
                                                    ::fwAtomConversion::AtomHelper& metaHelper);

    ::fwData::Object::sptr computeMinimalInfos( ::fwAtoms::Object::sptr object,
                                                ::fwAtomConversion::AtomHelper& metaHelper);

}; // Value Object

}
}

#endif /* __FWATOMCONVERSION_CUSTOM_MAPPER_HPP__ */
