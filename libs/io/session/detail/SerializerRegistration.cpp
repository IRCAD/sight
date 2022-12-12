/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "Activity.hxx"
#include "ActivitySet.hxx"
#include "Array.hxx"
#include "CalibrationInfo.hxx"
#include "Camera.hxx"
#include "CameraSet.hxx"
#include "Color.hxx"
#include "Composite.hxx"
#include "DicomSeries.hxx"
#include "Image.hxx"
#include "ImageSeries.hxx"
#include "Landmarks.hxx"
#include "Line.hxx"
#include "Material.hxx"
#include "Matrix4.hxx"
#include "Mesh.hxx"
#include "ModelSeries.hxx"
#include "Plane.hxx"
#include "PlaneList.hxx"
#include "Point.hxx"
#include "PointList.hxx"
#include "Reconstruction.hxx"
#include "Resection.hxx"
#include "ResectionDB.hxx"
#include "Series.hxx"
#include "SeriesSet.hxx"
#include "Set.hxx"
#include "String.hxx"
#include "StructureTraits.hxx"
#include "StructureTraitsDictionary.hxx"
#include "TransferFunction.hxx"
#include "Vector.hxx"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <io/session/Helper.hpp>
#include <io/session/macros.hpp>

SIGHT_REGISTER_SERIALIZER(
    sight::data::Boolean,
    &sight::io::session::Helper::serialize<sight::data::Boolean>,
    &sight::io::session::Helper::deserialize<sight::data::Boolean>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::Float,
    &sight::io::session::Helper::serialize<sight::data::Float>,
    &sight::io::session::Helper::deserialize<sight::data::Float>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::Integer,
    &sight::io::session::Helper::serialize<sight::data::Integer>,
    &sight::io::session::Helper::deserialize<sight::data::Integer>
);
