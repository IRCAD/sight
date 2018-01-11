/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VALIDATORS_MODELSERIES_CONTAINONESKIN_HPP__
#define __VALIDATORS_MODELSERIES_CONTAINONESKIN_HPP__

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace ModelSeries
{
/**
 * @brief Defines a validator which checks that the ModelSeries contains one and only one Skin.
 * @note If the given Obect is a container (Vector or Composite), it validates that each sub-object is a ModelSeries
 *       with one and only one Skin.
 */
class VALIDATORS_CLASS_API ContainOneSkin : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ContainOneSkin)(::fwActivities::IObjectValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< ContainOneSkin > );

    /// Constructor. Do nothing.
    VALIDATORS_API ContainOneSkin(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~ContainOneSkin();

    /**
     * @brief Checks that the ModelSeries contains one only one Skin.
     * @note Given object should be a single ModelSeries or a Vector or a Composite of ModelSeries.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object)& currentObject ) const
    override;
};

} // namespace ModelSeries
} // namespace validators

#endif // __VALIDATORS_MODELSERIES_CONTAINONESKIN_HPP__

