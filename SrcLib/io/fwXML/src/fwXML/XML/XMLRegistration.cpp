/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Mesh.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <typeinfo>

#include "fwXML/config.hpp"

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Object.hpp"
#include "fwXML/boostSerializer/Acquisition.hpp"
#include "fwXML/boostSerializer/Array.hpp"
#include "fwXML/boostSerializer/Boolean.hpp"
#include "fwXML/boostSerializer/Color.hpp"
#include "fwXML/boostSerializer/Dictionary.hpp"
#include "fwXML/boostSerializer/DictionaryOrgan.hpp"
#include "fwXML/boostSerializer/Type.hpp"
#include "fwXML/boostSerializer/Float.hpp"
#include "fwXML/boostSerializer/Histogram.hpp"
#include "fwXML/boostSerializer/Image.hpp"
#include "fwXML/boostSerializer/Integer.hpp"
#include "fwXML/boostSerializer/Material.hpp"
#include "fwXML/boostSerializer/Patient.hpp"
#include "fwXML/boostSerializer/PatientDB.hpp"
#include "fwXML/boostSerializer/Point.hpp"
#include "fwXML/boostSerializer/ProcessObject.hpp"
#include "fwXML/boostSerializer/Reconstruction.hpp"
#include "fwXML/boostSerializer/ResectionDB.hpp"
#include "fwXML/boostSerializer/String.hpp"
#include "fwXML/boostSerializer/Study.hpp"
#include "fwXML/boostSerializer/TransformationMatrix3D.hpp"
#include "fwXML/boostSerializer/TransferFunction.hpp"

// translator generic
#include "fwXML/XML/GenericXMLTranslator.hpp"

// specific
#include "fwXML/XML/ArrayXMLTranslator.hpp"
#include "fwXML/XML/CompositeXMLTranslator.hpp"
#include "fwXML/XML/DictionaryOrganXMLTranslator.hpp"
#include "fwXML/XML/EdgeXMLTranslator.hpp"
#include "fwXML/XML/GraphXMLTranslator.hpp"
#include "fwXML/XML/ImageXMLTranslator.hpp"
#include "fwXML/XML/IntegerXMLTranslator.hpp"
#include "fwXML/XML/ListXMLTranslator.hpp"
#include "fwXML/XML/PatientDBXMLTranslator.hpp"
#include "fwXML/XML/PatientXMLTranslator.hpp"
#include "fwXML/XML/StudyXMLTranslator.hpp"
#include "fwXML/XML/MaterialXMLTranslator.hpp"
#include "fwXML/XML/MeshXMLTranslator.hpp"
#include "fwXML/XML/NodeXMLTranslator.hpp"
#include "fwXML/XML/PlaneListXMLTranslator.hpp"
#include "fwXML/XML/PlaneXMLTranslator.hpp"
#include "fwXML/XML/PointListXMLTranslator.hpp"
#include "fwXML/XML/PortXMLTranslator.hpp"
#include "fwXML/XML/ReconstructionXMLTranslator.hpp"
#include "fwXML/XML/AcquisitionXMLTranslator.hpp"
#include "fwXML/XML/ResectionXMLTranslator.hpp"
#include "fwXML/XML/SingleFileXMLTranslator.hpp"
#include "fwXML/XML/FolderXMLTranslator.hpp"
#include "fwXML/XML/StringXMLTranslator.hpp"
#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"
#include "fwXML/XML/VectorXMLTranslator.hpp"
#include "fwXML/XML/StructureTraitsXMLTranslator.hpp"
#include "fwXML/XML/ROITraitsXMLTranslator.hpp"
#include "fwXML/XML/ReconstructionTraitsXMLTranslator.hpp"
#include "fwXML/XML/StructureTraitsDictionaryXMLTranslator.hpp"
#include "fwXML/XML/ResectionDBXMLTranslator.hpp"
#include "fwXML/XML/DictionaryXMLTranslator.hpp"

INSTANTIATE_SERIALIZER(::fwData::Object);
INSTANTIATE_SERIALIZER(::fwData::Acquisition);
INSTANTIATE_SERIALIZER(::fwData::Array);
INSTANTIATE_SERIALIZER(::fwData::Boolean);
INSTANTIATE_SERIALIZER(::fwData::Color);
INSTANTIATE_SERIALIZER(::fwData::Dictionary);
INSTANTIATE_SERIALIZER(::fwData::DictionaryOrgan);
INSTANTIATE_SERIALIZER(::fwData::Float);
INSTANTIATE_SERIALIZER(::fwData::Histogram)
INSTANTIATE_SERIALIZER(::fwData::Image);
INSTANTIATE_SERIALIZER(::fwData::Integer);
INSTANTIATE_SERIALIZER(::fwData::Material);
INSTANTIATE_SERIALIZER(::fwData::Patient);
INSTANTIATE_SERIALIZER(::fwData::PatientDB);
INSTANTIATE_SERIALIZER(::fwData::Point);
INSTANTIATE_SERIALIZER(::fwData::ProcessObject);
INSTANTIATE_SERIALIZER(::fwData::Reconstruction);
INSTANTIATE_SERIALIZER(::fwData::ResectionDB);
INSTANTIATE_SERIALIZER(::fwData::String);
INSTANTIATE_SERIALIZER(::fwData::Study);
INSTANTIATE_SERIALIZER(::fwData::TransformationMatrix3D);
INSTANTIATE_SERIALIZER(::fwTools::Type);


namespace fwXML
{

REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Object>                 ,  ::fwData::Object                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Boolean>                ,  ::fwData::Boolean                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::ProcessObject>          ,  ::fwData::ProcessObject          );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Float>                  ,  ::fwData::Float                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Histogram>              ,  ::fwData::Histogram              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::TransformationMatrix3D> ,  ::fwData::TransformationMatrix3D );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Color>                  ,  ::fwData::Color                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Point >                 ,  ::fwData::Point                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::TransfertFunction_VERSION_II >,  ::fwData::TransfertFunction_VERSION_II );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , IntegerXMLTranslator                                    ,  ::fwData::Integer                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ImageXMLTranslator                                      ,  ::fwData::Image                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , StringXMLTranslator                                     ,  ::fwData::String                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PatientDBXMLTranslator                                  ,  ::fwData::PatientDB              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PatientXMLTranslator                                    ,  ::fwData::Patient                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , StudyXMLTranslator                                      ,  ::fwData::Study                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , NodeXMLTranslator                                       ,  ::fwData::Node                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GraphXMLTranslator                                      ,  ::fwData::Graph                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PortXMLTranslator                                       ,  ::fwData::Port                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , EdgeXMLTranslator                                       ,  ::fwData::Edge                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , TriangularMeshXMLTranslator                             ,  ::fwData::TriangularMesh         );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , CompositeXMLTranslator                                  ,  ::fwData::Composite              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , VectorXMLTranslator                                     ,  ::fwData::Vector                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ListXMLTranslator                                       ,  ::fwData::List                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PointListXMLTranslator                                  ,  ::fwData::PointList              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ResectionDBXMLTranslator                                ,  ::fwData::ResectionDB            );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ResectionXMLTranslator                                  ,  ::fwData::Resection              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PlaneXMLTranslator                                      ,  ::fwData::Plane                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PlaneListXMLTranslator                                  ,  ::fwData::PlaneList              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , SingleFileXMLTranslator                                 ,  ::fwData::location::SingleFile   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , FolderXMLTranslator                                     ,  ::fwData::location::Folder       );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , DictionaryXMLTranslator                                 ,  ::fwData::Dictionary             );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , DictionaryOrganXMLTranslator                            ,  ::fwData::DictionaryOrgan        );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ArrayXMLTranslator                                      ,  ::fwData::Array                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , MeshXMLTranslator                                       ,  ::fwData::Mesh                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , MaterialXMLTranslator                                   ,  ::fwData::Material               );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ReconstructionXMLTranslator                             ,  ::fwData::Reconstruction         );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , AcquisitionXMLTranslator                                ,  ::fwData::Acquisition            );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , StructureTraitsXMLTranslator                            ,  ::fwData::StructureTraits        );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ROITraitsXMLTranslator                                  ,  ::fwData::ROITraits              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ReconstructionTraitsXMLTranslator                       ,  ::fwData::ReconstructionTraits   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , StructureTraitsDictionaryXMLTranslator                  ,  ::fwData::StructureTraitsDictionary);

}



