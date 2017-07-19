/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ECHOEDSIMU_SPROBEMESH_HPP__
#define __ECHOEDSIMU_SPROBEMESH_HPP__

#include <arServices/ISimulator.hpp>

#include "echoEdSimu/config.hpp"

namespace echoEdSimu
{

/**
 * @brief   Generates a mesh that can be used to display an echographic image.
 *
 */
class ECHOEDSIMU_CLASS_API SProbeMesh : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SProbeMesh)(::arServices::ISimulator) );

    /// Constructor. Do nothing.
    ECHOEDSIMU_API SProbeMesh() noexcept;

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SProbeMesh() noexcept;

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void starting();

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping();

    /// Generate the mesh
    ECHOEDSIMU_API virtual void updating();

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping();

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring();

};

} // namespace echoEdSimu

#endif /*__ECHOEDSIMU_SPROBEMESH_HPP__*/


