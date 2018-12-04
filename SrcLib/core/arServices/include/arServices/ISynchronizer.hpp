/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
