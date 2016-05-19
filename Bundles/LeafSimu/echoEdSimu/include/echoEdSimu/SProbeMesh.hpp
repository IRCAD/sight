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
    ECHOEDSIMU_API SProbeMesh() throw();

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SProbeMesh() throw();

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void starting() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() throw(::fwTools::Failed);

    /// Generate the mesh
    ECHOEDSIMU_API virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring() throw(fwTools::Failed);

};

} // namespace echoEdSimu

#endif /*__ECHOEDSIMU_SPROBEMESH_HPP__*/


