/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSERVICES_ISYNCHRONIZER_HPP__
#define __ARSERVICES_ISYNCHRONIZER_HPP__

#include "arServices/config.hpp"

#include <fwServices/IService.hpp>

namespace arServices
{
/**
 * @brief   ISynchronizer service synchronize objects from timelines
 * @class   ISynchronizer
 */
class ARSERVICES_CLASS_API ISynchronizer : public fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro((ISynchronizer)(fwServices::IService));

    /**
     * @brief Constructor.
     */
    ARSERVICES_API ISynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    ARSERVICES_API virtual ~ISynchronizer() noexcept;

};

} //namespace arServices
#endif  // __ARSERVICES_ISYNCHRONIZER_HPP__
