/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWACTIVITIES_BUILDER_ACTIVITYSERIES_HPP_
#define _FWACTIVITIES_BUILDER_ACTIVITYSERIES_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwActivities/IBuilder.hpp"
#include "fwActivities/config.hpp"

namespace fwActivities
{
namespace builder
{

/**
 * @brief   Defines the default builder for the data ActivitySeries.
 * @class   ActivitySeries.
 * @date    2013.
 */
class FWACTIVITIES_CLASS_API ActivitySeries : public ::fwActivities::IBuilder
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ActivitySeries)(::fwActivities::IBuilder),
                                            (()),
                                            ::fwActivities::builder::factory::New< ActivitySeries > );


    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    FWACTIVITIES_API ActivitySeries(::fwActivities::IBuilder::Key key);

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~ActivitySeries();

    /**
     * @brief Build an ActivitySeries with required data present in currentSelection and defined in configuration.
     * @param[in] currentSelection a composite which contains current selected data.
     * @param[in] activityConfigId an activity configuration identifier associated with the ActivitySeries data instanced.
     * @return specific data ActivitySeries for the specified Activity.
     */
    FWACTIVITIES_API virtual ::fwActivities::ActivitySeries::sptr buildData(
            SPTR(::fwData::Composite) currentSelection,
            const ConfigIdType& activityConfigId ) const;
};

} // namespace builder
} // namespace fwActivities

#endif /*_FWACTIVITIES_BUILDER_ACTIVITYSERIES_HPP_*/


