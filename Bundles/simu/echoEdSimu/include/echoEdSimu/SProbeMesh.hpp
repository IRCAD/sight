/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "echoEdSimu/config.hpp"

#include <arServices/ISimulator.hpp>

namespace echoEdSimu
{

/**
 * @brief   Generates a mesh that can be used to display an echographic image.
 * @deprecated This service will be removed in 19.0
 */
class ECHOEDSIMU_CLASS_API SProbeMesh : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SProbeMesh)(::arServices::ISimulator) );

    /// Constructor. Do nothing.
    ECHOEDSIMU_API SProbeMesh() noexcept;

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SProbeMesh() noexcept;

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void starting() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() override;

    /// Generate the mesh
    ECHOEDSIMU_API virtual void updating() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring() override;

};

} // namespace echoEdSimu
