/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_BASE_HPP__
#define __FWATOMCONVERSION_MAPPER_BASE_HPP__

#include <iostream>
#include <map>
#include <string>

#include <camp/class.hpp>

#include <fwData/Object.hpp>

#include <fwAtoms/Object.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/mapper/factory/new.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

namespace fwAtomConversion
{
namespace mapper
{

/// Base class of mapper mapper between fwData and MetaData
class FWATOMCONVERSION_CLASS_API Base
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
            ::fwAtomConversion::mapper::registry::get()->addFactory(
                    functorKey,
                    &::fwAtomConversion::mapper::factory::New<T>
                    );
        }
    };

    virtual ~Base(){};

    /// ???
    FWATOMCONVERSION_API virtual ::fwAtoms::Object::sptr convert
        ( ::fwData::Object::sptr object,
          DataVisitor::AtomCacheType & cache ) = 0;

    /// ???
    FWATOMCONVERSION_API virtual ::fwData::Object::sptr convert
    ( ::fwAtoms::Object::sptr atom,
      AtomVisitor::DataCacheType & cache ) = 0;

};

}
}

#endif /* __FWATOMCONVERSION_MAPPER_BASE_HPP__ */
