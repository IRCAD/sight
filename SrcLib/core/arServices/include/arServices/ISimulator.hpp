/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSERVICES_ISIMULATOR_HPP__
#define __ARSERVICES_ISIMULATOR_HPP__

#include "arServices/config.hpp"

#include <fwServices/IService.hpp>

namespace arServices
{

/**
 * @brief   This interface defines simulator service API.
 *          Must be implemented for services that simulate.
 */
class ARSERVICES_CLASS_API ISimulator : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ISimulator)(::fwServices::IService) );

protected:

    /// ISimulator constructor. Do nothing.
    ARSERVICES_API ISimulator();

    /// ISimulator destructor. Do nothing.
    ARSERVICES_API virtual ~ISimulator();

};

}

#endif // __ARSERVICES_ISIMULATOR_HPP__
