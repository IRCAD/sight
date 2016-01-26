/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_IBUILDER_HPP__
#define __FWACTIVITIES_IBUILDER_HPP__

#include <fwCore/base.hpp>
#include <fwMedData/ActivitySeries.hpp>

#include "fwActivities/builder/factory/new.hpp"
#include "fwActivities/builder/registry/detail.hpp"
#include "fwActivities/registry/Activities.hpp"

#include "fwActivities/config.hpp"

namespace fwData
{
class Vector;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities builder's classes
 */
class FWACTIVITIES_CLASS_API IBuilder : public ::fwCore::BaseObject
{

public:

    typedef ::fwActivities::builder::factory::Key Key;
    typedef ::fwMedData::ActivitySeries::ConfigIdType ConfigIdType;

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
            ::fwActivities::builder::registry::get()->addFactory( functorKey,
                                                                  &::fwActivities::builder::factory::New<T> );
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (IBuilder)(::fwCore::BaseObject) );


    /**
     * @brief Build an ActivitySeries with required data present in currentSelection and defined in configuration.
     * @param[in] activityInfo a structure which contains all the Activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return specific data ActivitySeries for the specified Activity.
     */
    FWACTIVITIES_API virtual ::fwMedData::ActivitySeries::sptr buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        SPTR(::fwData::Vector) currentSelection ) const = 0;

protected:

    FWACTIVITIES_API virtual SPTR(::fwData::Vector) getType( SPTR(::fwData::Vector) currentSelection,
                                                             std::string type ) const;
};

} // namespace fwActivities

#endif // __FWACTIVITIES_IBUILDER_HPP__

