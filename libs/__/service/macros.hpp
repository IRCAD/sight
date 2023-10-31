/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "service/base.hpp"
#include "service/service_factory_registry.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

namespace sight::service
{

/**
 * @name Macros for declaring service to object association
 */

#define FWSERVICE_REGISTER_MACRO(service_t, ServiceImpl) \
    static sight::service::service_factory_registry<ServiceImpl> \
    BOOST_PP_CAT(serviceRegistry, __LINE__)( #ServiceImpl, #service_t);

#define FWSERVICE_REGISTER_OBJECT_MACRO(ServiceImpl, ServiceObject) \
    static sight::service::service_object_factory_registry \
    BOOST_PP_CAT(serviceObjectRegistry, __LINE__)( #ServiceImpl, #ServiceObject);

//@{

#define FWSERVICE_REGISTER_MACRO_2(ServiceImpl, ServiceObject) \
    FWSERVICE_REGISTER_MACRO(ServiceImpl, ServiceObject)

#define FWSERVICE_REGISTER_MACRO_3(service_t, ServiceImpl, ServiceObject) \
    FWSERVICE_REGISTER_MACRO(service_t, ServiceImpl) \
    FWSERVICE_REGISTER_OBJECT_MACRO(ServiceImpl, ServiceObject)

/**
 * @brief Service object type association including all string-based registration
 * Associations concern Service-object_t are object_t-Service. Keys are typeid.
 */
#if !BOOST_PP_VARIADICS_MSVC
#define SIGHT_REGISTER_SERVICE(...) BOOST_PP_OVERLOAD(FWSERVICE_REGISTER_MACRO_, __VA_ARGS__)(__VA_ARGS__)
#else
#define SIGHT_REGISTER_SERVICE(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(FWSERVICE_REGISTER_MACRO_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

#define SIGHT_REGISTER_SERVICE_OBJECT(ServiceImpl, ServiceObject) \
    FWSERVICE_REGISTER_OBJECT_MACRO(ServiceImpl, ServiceObject)

//@}

} // namespace sight::service
