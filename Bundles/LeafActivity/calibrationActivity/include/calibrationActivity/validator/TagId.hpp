/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATIONACTIVITY_VALIDATOR_TAGID_HPP__
#define __CALIBRATIONACTIVITY_VALIDATOR_TAGID_HPP__

#include "calibrationActivity/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

#include <fwCore/base.hpp>

namespace calibrationActivity
{
namespace validator
{

/**
 * @brief Defines a validator which checks that the current data String contains a list of tag identifiers separated by
 *        a ',' (for example: "101, 102, 103").
 *
 */
class CALIBRATIONACTIVITY_CLASS_API TagId : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagId)(::fwActivities::IValidator),
                                            (()),
                                            ::fwActivities::validator::factory::New< TagId > );

    /// Constructor. Do nothing.
    CALIBRATIONACTIVITY_API TagId(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    CALIBRATIONACTIVITY_API virtual ~TagId();

    FWACTIVITIES_API virtual ValidationType validate(const CSPTR(::fwData::Object) &currentData ) const;
};

} // namespace validator
} // namespace calibrationActivity

#endif /*__CALIBRATIONACTIVITY_VALIDATOR_TAGID_HPP__*/


