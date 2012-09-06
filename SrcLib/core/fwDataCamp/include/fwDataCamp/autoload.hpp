/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATACAMP_AUTOLOAD_HPP__
#define __FWDATACAMP_AUTOLOAD_HPP__

#include <fwCamp/macros.hpp>


#include "fwDataCamp/Acquisition.hpp"
#include "fwDataCamp/Array.hpp"
#include "fwDataCamp/Boolean.hpp"
#include "fwDataCamp/Camera.hpp"
#include "fwDataCamp/Color.hpp"
#include "fwDataCamp/Composite.hpp"
#include "fwDataCamp/Dictionary.hpp"
#include "fwDataCamp/DictionaryOrgan.hpp"
#include "fwDataCamp/Edge.hpp"
#include "fwDataCamp/Float.hpp"
#include "fwDataCamp/Graph.hpp"
#include "fwDataCamp/Histogram.hpp"
#include "fwDataCamp/Image.hpp"
#include "fwDataCamp/Integer.hpp"
#include "fwDataCamp/Line.hpp"
#include "fwDataCamp/List.hpp"
#include "fwDataCamp/Material.hpp"
#include "fwDataCamp/Mesh.hpp"
#include "fwDataCamp/Model.hpp"
#include "fwDataCamp/Node.hpp"
#include "fwDataCamp/None.hpp"
#include "fwDataCamp/Patient.hpp"
#include "fwDataCamp/PatientDB.hpp"
#include "fwDataCamp/Plane.hpp"
#include "fwDataCamp/PlaneList.hpp"
#include "fwDataCamp/Point.hpp"
#include "fwDataCamp/PointList.hpp"
#include "fwDataCamp/Port.hpp"
#include "fwDataCamp/ProcessObject.hpp"
#include "fwDataCamp/ROITraits.hpp"
#include "fwDataCamp/Reconstruction.hpp"
#include "fwDataCamp/ReconstructionTraits.hpp"
#include "fwDataCamp/Resection.hpp"
#include "fwDataCamp/ResectionDB.hpp"
#include "fwDataCamp/String.hpp"
#include "fwDataCamp/StructureTraits.hpp"
#include "fwDataCamp/StructureTraitsDictionary.hpp"
#include "fwDataCamp/Study.hpp"
#include "fwDataCamp/Tag.hpp"
#include "fwDataCamp/TransferFunction.hpp"
#include "fwDataCamp/TransformationMatrix3D.hpp"
#include "fwDataCamp/Vector.hpp"
#include "fwDataCamp/Video.hpp"
#include "fwDataCamp/location/Folder.hpp"
#include "fwDataCamp/location/SingleFile.hpp"
#include "fwDataCamp/config.hpp"


namespace fwDataCamp {

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
        localDeclarefwDataMaterialOPTIONS_MODE();
        localDeclarefwDataMaterialREPRESENTATION_MODE();
        localDeclarefwDataMaterialSHADING_MODE();
        localDeclarefwDataMaterial();
        localDeclarefwDataReconstruction();
        localDeclarefwDataAcquisition();
        localDeclarefwDataStudy();
        localDeclarefwDataComposite();
        localDeclarefwDataPatient();
        localDeclarefwDataPatientDB();
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
        localDeclarefwDataCamera();
        localDeclarefwDataDictionary();
        localDeclarefwDataDictionaryOrgan();
        localDeclarefwDataHistogram();
        localDeclarefwDataLine();
        localDeclarefwDataList();
        localDeclarefwDatalocationSingleFile();
        localDeclarefwDataModel();
        localDeclarefwDataNone();
        localDeclarefwDataProcessObject();
        localDeclarefwDataTag();
        localDeclarefwDataVideo();
    }

    static runner r;
};
}



#endif //__FWDATACAMP_AUTOLOAD_HPP__
