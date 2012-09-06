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
#include "fwData/Boolean.hpp"
#include "fwDataCamp/Camera.hpp"
#include "fwData/Color.hpp"
#include "fwData/Composite.hpp"
#include "fwData/Dictionary.hpp"
#include "fwDataCamp/DictionaryOrgan.hpp"
#include "fwData/Edge.hpp"
#include "fwData/Float.hpp"
#include "fwData/Graph.hpp"
#include "fwData/Histogram.hpp"
#include "fwDataCamp/Image.hpp"
#include "fwData/Integer.hpp"
#include "fwDataCamp/Line.hpp"
#include "fwData/List.hpp"
#include "fwDataCamp/Material.hpp"
#include "fwDataCamp/Mesh.hpp"
#include "fwData/Model.hpp"
#include "fwDataCamp/Node.hpp"
#include "fwData/None.hpp"
#include "fwData/Patient.hpp"
#include "fwData/PatientDB.hpp"
#include "fwData/Plane.hpp"
#include "fwData/PlaneList.hpp"
#include "fwData/Point.hpp"
#include "fwData/PointList.hpp"
#include "fwData/Port.hpp"
#include "fwData/ProcessObject.hpp"
#include "fwDataCamp/ROITraits.hpp"
#include "fwDataCamp/Reconstruction.hpp"
#include "fwDataCamp/ReconstructionTraits.hpp"
#include "fwDataCamp/Resection.hpp"
#include "fwDataCamp/ResectionDB.hpp"
#include "fwData/String.hpp"
#include "fwDataCamp/StructureTraits.hpp"
#include "fwData/StructureTraitsDictionary.hpp"
#include "fwData/Study.hpp"
#include "fwDataCamp/Tag.hpp"
#include "fwDataCamp/TransferFunction.hpp"
#include "fwData/TransformationMatrix3D.hpp"
#include "fwData/Vector.hpp"
#include "fwDataCamp/Video.hpp"
#include "fwData/location/Folder.hpp"
#include "fwData/location/SingleFile.hpp"
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
