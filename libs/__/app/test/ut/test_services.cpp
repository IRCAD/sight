/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "test_services.hpp"

#include <data/image.hpp>

SIGHT_REGISTER_SERVICE(
    sight::app::ut::TestConfigService,
    sight::app::ut::test1_image,
    sight::data::image
);

SIGHT_REGISTER_SERVICE(sight::service::base, sight::app::ut::test_order_srv);
SIGHT_REGISTER_SERVICE(sight::service::base, sight::app::ut::test_reset_order_srv);

namespace sight::app::ut
{

unsigned int test_order_srv::s_ORDER;

} // namespace sight::app::ut
