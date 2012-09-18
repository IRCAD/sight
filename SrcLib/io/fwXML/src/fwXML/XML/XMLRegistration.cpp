/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Mesh.hpp>

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
#include "fwXML/XML/ProcessObjectXMLTranslator.hpp"

#include "fwXML/registry/macros.hpp"

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

fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Object>                 ,  ::fwData::Object                   );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Boolean>                ,  ::fwData::Boolean                  );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Float>                  ,  ::fwData::Float                    );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Histogram>              ,  ::fwData::Histogram                );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::TransformationMatrix3D> ,  ::fwData::TransformationMatrix3D   );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Color>                  ,  ::fwData::Color                    );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::Point >                 ,  ::fwData::Point                    );
fwXMLRegisterMacro( GenericXMLTranslator< ::fwData::TransferFunction >      ,  ::fwData::TransferFunction         );
fwXMLRegisterMacro( IntegerXMLTranslator                                    ,  ::fwData::Integer                  );
fwXMLRegisterMacro( ImageXMLTranslator                                      ,  ::fwData::Image                    );
fwXMLRegisterMacro( StringXMLTranslator                                     ,  ::fwData::String                   );
fwXMLRegisterMacro( PatientDBXMLTranslator                                  ,  ::fwData::PatientDB                );
fwXMLRegisterMacro( PatientXMLTranslator                                    ,  ::fwData::Patient                  );
fwXMLRegisterMacro( StudyXMLTranslator                                      ,  ::fwData::Study                    );
fwXMLRegisterMacro( NodeXMLTranslator                                       ,  ::fwData::Node                     );
fwXMLRegisterMacro( GraphXMLTranslator                                      ,  ::fwData::Graph                    );
fwXMLRegisterMacro( PortXMLTranslator                                       ,  ::fwData::Port                     );
fwXMLRegisterMacro( EdgeXMLTranslator                                       ,  ::fwData::Edge                     );
fwXMLRegisterMacro( TriangularMeshXMLTranslator                             ,  ::fwData::TriangularMesh           );
fwXMLRegisterMacro( CompositeXMLTranslator                                  ,  ::fwData::Composite                );
fwXMLRegisterMacro( VectorXMLTranslator                                     ,  ::fwData::Vector                   );
fwXMLRegisterMacro( ListXMLTranslator                                       ,  ::fwData::List                     );
fwXMLRegisterMacro( PointListXMLTranslator                                  ,  ::fwData::PointList                );
fwXMLRegisterMacro( ResectionDBXMLTranslator                                ,  ::fwData::ResectionDB              );
fwXMLRegisterMacro( ResectionXMLTranslator                                  ,  ::fwData::Resection                );
fwXMLRegisterMacro( PlaneXMLTranslator                                      ,  ::fwData::Plane                    );
fwXMLRegisterMacro( PlaneListXMLTranslator                                  ,  ::fwData::PlaneList                );
fwXMLRegisterMacro( SingleFileXMLTranslator                                 ,  ::fwData::location::SingleFile     );
fwXMLRegisterMacro( FolderXMLTranslator                                     ,  ::fwData::location::Folder         );
fwXMLRegisterMacro( DictionaryXMLTranslator                                 ,  ::fwData::Dictionary               );
fwXMLRegisterMacro( DictionaryOrganXMLTranslator                            ,  ::fwData::DictionaryOrgan          );
fwXMLRegisterMacro( ArrayXMLTranslator                                      ,  ::fwData::Array                    );
fwXMLRegisterMacro( MeshXMLTranslator                                       ,  ::fwData::Mesh                     );
fwXMLRegisterMacro( MaterialXMLTranslator                                   ,  ::fwData::Material                 );
fwXMLRegisterMacro( ReconstructionXMLTranslator                             ,  ::fwData::Reconstruction           );
fwXMLRegisterMacro( AcquisitionXMLTranslator                                ,  ::fwData::Acquisition              );
fwXMLRegisterMacro( StructureTraitsXMLTranslator                            ,  ::fwData::StructureTraits          );
fwXMLRegisterMacro( ROITraitsXMLTranslator                                  ,  ::fwData::ROITraits                );
fwXMLRegisterMacro( ReconstructionTraitsXMLTranslator                       ,  ::fwData::ReconstructionTraits     );
fwXMLRegisterMacro( StructureTraitsDictionaryXMLTranslator                  ,  ::fwData::StructureTraitsDictionary);
fwXMLRegisterMacro( ProcessObjectXMLTranslator                              ,  ::fwData::ProcessObject            );

}



