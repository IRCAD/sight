/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "activity.hxx"
#include "activity_set.hxx"
#include "array.hxx"
#include "calibration_info.hxx"
#include "camera.hxx"
#include "camera_set.hxx"
#include "color.hxx"
#include "dicom_series.hxx"
#include "fiducials_series.hxx"
#include "image.hxx"
#include "image_series.hxx"
#include "landmarks.hxx"
#include "line.hxx"
#include "map.hxx"
#include "material.hxx"
#include "matrix4.hxx"
#include "mesh.hxx"
#include "model_series.hxx"
#include "plane.hxx"
#include "plane_list.hxx"
#include "point.hxx"
#include "point_list.hxx"
#include "reconstruction.hxx"
#include "resection.hxx"
#include "resection_db.hxx"
#include "series.hxx"
#include "series_set.hxx"
#include "set.hxx"
#include "string.hxx"
#include "structure_traits.hxx"
#include "structure_traits_dictionary.hxx"
#include "transfer_function.hxx"
#include "vector.hxx"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>

#include <io/session/helper.hpp>
#include <io/session/macros.hpp>

SIGHT_REGISTER_SERIALIZER(
    sight::data::boolean,
    &sight::io::session::helper::serialize<sight::data::boolean>,
    &sight::io::session::helper::deserialize<sight::data::boolean>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::real,
    &sight::io::session::helper::serialize<sight::data::real>,
    &sight::io::session::helper::deserialize<sight::data::real>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::integer,
    &sight::io::session::helper::serialize<sight::data::integer>,
    &sight::io::session::helper::deserialize<sight::data::integer>
);
