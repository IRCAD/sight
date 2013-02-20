/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_IBUILDER_HPP__
#define __FWACTIVITIES_IBUILDER_HPP__

#include <boost/enable_shared_from_this.hpp>

#include <fwCore/base.hpp>

#include "fwActivities/builder/factory/new.hpp"
#include "fwActivities/builder/registry/detail.hpp"
#include "fwActivities/ActivitySeries.hpp"
#include "fwActivities/config.hpp"

namespace fwRuntime
{
struct ConfigurationElement;
}
namespace fwData
{
class Composite;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities builder's classes
 */
class FWACTIVITIES_CLASS_API IBuilder : public ::fwCore::BaseObject
{

public :

    typedef ::fwActivities::builder::factory::Key Key;
    typedef ::fwActivities::ActivitySeries::ConfigIdType ConfigIdType;

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
     * @param[in] currentSelection a composite which contains current selected data.
     * @param[in] activityConfigId an activity configuration identifier associated with the ActivitySeries data instanced.
     * @return specific data ActivitySeries for the specified Activity.
     */
    FWACTIVITIES_API virtual ::fwActivities::ActivitySeries::sptr buildData(
            SPTR(::fwData::Composite) currentSelection,
            const ConfigIdType& activityConfigId ) const = 0;

    /**
     * @brief Affect the configuration, using a generic XML like structure.
     * @param[in] configuration a structure which represents the xml configuration
     *
     * Example of configuration
     * @verbatim
      <requirements>
            <param name="imageSeries" type="::fwMedData::ImageSeries" minOccurs="0" maxOccurs="2" />
            <param name="modelSeries" type="::fwMedData::ModelSeries" minOccurs="1" maxOccurs="1" />
        </requirements>
       @endverbatim
     */
    FWACTIVITIES_API virtual void setConfiguration( SPTR(::fwRuntime::ConfigurationElement) configuration);

protected :

    /**
     * @name Constructor/Destructor
     * @{ */

    IBuilder(){};

    virtual ~IBuilder(){};

    /**  @} */

    SPTR(::fwRuntime::ConfigurationElement) m_configuration;
};

} // namespace fwActivities

#endif // __FWACTIVITIES_IBUILDER_HPP__

