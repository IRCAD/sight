/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "filter/dicom/registry/detail.hpp"

#include <core/lazy_instantiator.hpp>

namespace sight::filter::dicom::registry
{

struct dicom_io_filter_registry_instantiator_tag {};

SPTR(type) get()
{
    using instantiator_t = core::lazy_instantiator<type, dicom_io_filter_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

} // namespace sight::filter::dicom::registry
