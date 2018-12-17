/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDATACAMP_AUTOLOAD_HPP__
#define __FWDATACAMP_AUTOLOAD_HPP__

#include "fwDataCamp/Material.hpp"
#include "fwDataCamp/StructureTraits.hpp"
#include "fwDataCamp/TransferFunction.hpp"

#include <fwCamp/macros.hpp>

#include <fwData/Array.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Edge.hpp>
#include <fwData/Float.hpp>
#include <fwData/Graph.hpp>
#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Landmarks.hpp>
#include <fwData/Line.hpp>
#include <fwData/List.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Port.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/String.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Tag.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

namespace fwDataCamp
{

struct runner
{
    runner()
    {
        localDeclarefwDataObject();
        localDeclarefwDataFloat();
        localDeclarefwDataInteger();
        localDeclarefwDataBoolean();
        localDeclarefwDataString();
        localDeclarefwDataVector();
        localDeclarefwDataColor();
        localDeclarefwDataArray();
        localDeclarefwDataImage();
        localDeclarefwDataMesh();
        localDeclarefwDataMaterialOptionsType();
        localDeclarefwDataMaterialRepresentationType();
        localDeclarefwDataMaterialShadingType();
        localDeclarefwDataMaterialFilteringType();
        localDeclarefwDataMaterialWrappingType();
        localDeclarefwDataMaterial();
        localDeclarefwDataReconstruction();
        localDeclarefwDataComposite();
        localDeclarefwDataPoint();
        localDeclarefwDataPointList();
        localDeclarefwDataTransformationMatrix3D();
        localDeclarefwDataTransferFunctionInterpolationMode();
        localDeclarefwDataTransferFunction();
        localDeclarefwDataPort();
        localDeclarefwDataEdge();
        localDeclarefwDataNode();
        localDeclarefwDataGraph();
        localDeclarefwDataStructureTraitsCategory();
        localDeclarefwDataStructureTraitsStructureClass();
        localDeclarefwDataStructureTraits();
        localDeclarefwDataStructureTraitsDictionary();
        localDeclarefwDataReconstructionTraits();
        localDeclarefwDataROITraits();
        localDeclarefwDatalocationFolder();
        localDeclarefwDataPlane();
        localDeclarefwDataPlaneList();
        localDeclarefwDataResection();
        localDeclarefwDataResectionDB();
        localDeclarefwDataHistogram();
        localDeclarefwDataLine();
        localDeclarefwDataList();
        localDeclarefwDatalocationSingleFile();
        localDeclarefwDataProcessObject();
        localDeclarefwDataTag();
        localDeclarefwDataLandmarks();
    }

    static runner r;
};
} //end namespace fwDataCamp

#endif //__FWDATACAMP_AUTOLOAD_HPP__
