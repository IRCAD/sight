/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <service/base.hpp>

namespace sight::module::data
{

/**
 * @brief The purpose of this service is to wrap messages incoming to the object to new messages.
 */

class i_wrapper_srv : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(i_wrapper_srv, service::base);

    ///@brief IWrapperSrv constructor. Do nothing.
    i_wrapper_srv();

    ///@brief IWrapperSrv destructor. Do nothing.
    ~i_wrapper_srv() override;
};

} // namespace sight::module::data
