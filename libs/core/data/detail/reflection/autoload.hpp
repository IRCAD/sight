/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/Array.hpp"
#include "data/Boolean.hpp"
#include "data/CalibrationInfo.hpp"
#include "data/Camera.hpp"
#include "data/CameraSeries.hpp"
#include "data/Color.hpp"
#include "data/Composite.hpp"
#include "data/config.hpp"
#include "data/detail/reflection/ActivitySeries.hpp"
#include "data/detail/reflection/Camera.hpp"
#include "data/detail/reflection/CameraSeries.hpp"
#include "data/detail/reflection/DicomSeries.hpp"
#include "data/detail/reflection/Image.hpp"
#include "data/detail/reflection/ImageSeries.hpp"
#include "data/detail/reflection/Material.hpp"
#include "data/detail/reflection/Series.hpp"
#include "data/detail/reflection/StructureTraits.hpp"
#include "data/detail/reflection/TransferFunction.hpp"
#include "data/DicomSeries.hpp"
#include "data/Edge.hpp"
#include "data/Equipment.hpp"
#include "data/Float.hpp"
#include "data/Graph.hpp"
#include "data/Histogram.hpp"
#include "data/Image.hpp"
#include "data/ImageSeries.hpp"
#include "data/Integer.hpp"
#include "data/Landmarks.hpp"
#include "data/Line.hpp"
#include "data/List.hpp"
#include "data/Material.hpp"
#include "data/Matrix4.hpp"
#include "data/Mesh.hpp"
#include "data/ModelSeries.hpp"
#include "data/Patient.hpp"
#include "data/Plane.hpp"
#include "data/PlaneList.hpp"
#include "data/Point.hpp"
#include "data/PointList.hpp"
#include "data/Port.hpp"
#include "data/ProcessObject.hpp"
#include "data/Reconstruction.hpp"
#include "data/ReconstructionTraits.hpp"
#include "data/Resection.hpp"
#include "data/ResectionDB.hpp"
#include "data/ROITraits.hpp"
#include "data/Series.hpp"
#include "data/SeriesDB.hpp"
#include "data/String.hpp"
#include "data/StructureTraits.hpp"
#include "data/StructureTraitsDictionary.hpp"
#include "data/Study.hpp"
#include "data/Tag.hpp"
#include "data/Vector.hpp"

#include <core/reflection/macros.hpp>

namespace sight::data
{

namespace detail
{

namespace reflection
{

struct runner
{
    runner()
    {
        localDeclaresightdataObject();
        localDeclaresightdataFloat();
        localDeclaresightdataInteger();
        localDeclaresightdataBoolean();
        localDeclaresightdataString();
        localDeclaresightdataVector();
        localDeclaresightdataColor();
        localDeclaresightdataArray();
        localDeclaresightdataImagePixelFormat();
        localDeclaresightdataImage();
        localDeclaresightdataMesh();
        localDeclaresightdataMaterialOptionsType();
        localDeclaresightdataMaterialRepresentationType();
        localDeclaresightdataMaterialShadingType();
        localDeclaresightdataMaterialFilteringType();
        localDeclaresightdataMaterialWrappingType();
        localDeclaresightdataMaterial();
        localDeclaresightdataReconstruction();
        localDeclaresightdataComposite();
        localDeclaresightdataPoint();
        localDeclaresightdataPointList();
        localDeclaresightdataMatrix4();
        localDeclaresightdataTransferFunctionInterpolationMode();
        localDeclaresightdataTransferFunction();
        localDeclaresightdataPort();
        localDeclaresightdataEdge();
        localDeclaresightdataNode();
        localDeclaresightdataGraph();
        localDeclaresightdataStructureTraitsCategory();
        localDeclaresightdataStructureTraitsStructureClass();
        localDeclaresightdataStructureTraits();
        localDeclaresightdataStructureTraitsDictionary();
        localDeclaresightdataReconstructionTraits();
        localDeclaresightdataROITraits();
        localDeclaresightdataPlane();
        localDeclaresightdataPlaneList();
        localDeclaresightdataResection();
        localDeclaresightdataResectionDB();
        localDeclaresightdataHistogram();
        localDeclaresightdataLine();
        localDeclaresightdataList();
        localDeclaresightdataProcessObject();
        localDeclaresightdataTag();
        localDeclaresightdataLandmarks();
        localDeclaresightdataEquipment();
        localDeclaresightdataStudy();
        localDeclaresightdataPatient();
        localDeclaresightdataSeries();
        localDeclaresightdataSeriesDB();
        localDeclaresightdataDicomSeries();
        localDeclaresightdataImageSeries();
        localDeclaresightdataModelSeries();
        localDeclaresightdataActivitySeries();
        localDeclaresightdataCameraSourceType();
        localDeclaresightdataCameraPixelFormat();
        localDeclaresightdataCamera();
        localDeclaresightdataCameraSeries();
        localDeclaresightdataCalibrationInfo();
    }

    static runner r;
};

} // namespace reflection

} // namespace detail

} // namespace sight::data
