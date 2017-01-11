/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_ICAMP_HPP__
#define __CTRLCAMP_ICAMP_HPP__

#include "ctrlCamp/config.hpp"

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>



namespace ctrlCamp
{
/**
 * @brief This interface defines service API. It can be considered as a default type for services using fwCamp.
 */
class CTRLCAMP_CLASS_API ICamp : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (ICamp)(::fwServices::IService) );

    /// Constructor
    CTRLCAMP_API ICamp();

    /// Destructor
    CTRLCAMP_API ~ICamp();

};

}// namespace ctrlCamp


#endif /* __CTRLCAMP_ICAMP_HPP__ */
