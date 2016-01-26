/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSFILTER_IFILTER_HPP__
#define __FWATOMSFILTER_IFILTER_HPP__

#include <string>
#include <map>

#include <fwCore/base.hpp>
#include <fwAtoms/Object.hpp>

#include "fwAtomsFilter/registry/detail.hpp"
#include "fwAtomsFilter/factory/new.hpp"
#include "fwAtomsFilter/config.hpp"

namespace fwAtoms
{
class Object;
}

namespace fwAtomsFilter
{

/**
 * @class IFilter
 * @brief Removes atom attributes which are not managed by a context.
 */
class FWATOMSFILTER_CLASS_API IFilter : public ::fwCore::BaseObject
{

public:
    /// Factory key used by IFilter implementations
    typedef ::fwAtomsFilter::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwAtomsFilter::registry::get()->addFactory(functorKey, &::fwAtomsFilter::factory::New<T>);
        }
    };


    fwCoreNonInstanciableClassDefinitionsMacro((IFilter));
    fwCoreAllowSharedFromThis();

    /// Constructor.
    FWATOMSFILTER_API IFilter();

    /// Destructor.
    FWATOMSFILTER_API virtual ~IFilter();

    /// Applies the filter onto the specified object
    FWATOMSFILTER_API virtual void apply(const SPTR(::fwAtoms::Object)& object) = 0;

};


} //fwAtomsFilter

#endif /* __FWATOMSFILTER_IFILTER_HPP__ */

