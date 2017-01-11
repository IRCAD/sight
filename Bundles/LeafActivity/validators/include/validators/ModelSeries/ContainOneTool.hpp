/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VALIDATORS_MODELSERIES_CONTAINONETOOL_HPP__
#define __VALIDATORS_MODELSERIES_CONTAINONETOOL_HPP__

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace ModelSeries
{
/**
 * @brief Defines a validator which checks that the ModelSeries contains one and only one Tool.
 * @note If the given Obect is a container (Vector or Composite), it validates that each sub-object is a ModelSeries
 *       with one and only one Tool.
 */
class VALIDATORS_CLASS_API ContainOneTool : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ContainOneTool)(::fwActivities::IObjectValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< ContainOneTool > );


    /// Constructor. Do nothing.
    VALIDATORS_API ContainOneTool(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~ContainOneTool();

    /**
     * @brief Checks that the ModelSeries contains one only one Tool.
     * @note Given object should be a single ModelSeries or a Vector or a Composite of ModelSeries.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object) &currentObject ) const;
};

} // namespace ModelSeries
} // namespace validators



#endif // __VALIDATORS_MODELSERIES_CONTAINONETOOL_HPP__

