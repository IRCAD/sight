/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <typeinfo>

#include "fwXML/config.hpp"

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Object.hpp"
#include "fwXML/boostSerializer/Float.hpp"
#include "fwXML/boostSerializer/Boolean.hpp"

#include "fwXML/boostSerializer/Study.hpp"
#include "fwXML/boostSerializer/String.hpp"
#include "fwXML/boostSerializer/Patient.hpp"
#include "fwXML/boostSerializer/ProcessObject.hpp"
#include "fwXML/boostSerializer/Region.hpp"
#include "fwXML/boostSerializer/PatientDB.hpp"
#include "fwXML/boostSerializer/Acquisition.hpp"
#include "fwXML/boostSerializer/Reconstruction.hpp"
//#include "fwXML/boostSerializer/DictionaryOrgan.hpp"
#include "fwXML/boostSerializer/Dictionary.hpp"
#include "fwXML/boostSerializer/TransformationMatrix3D.hpp"
#include "fwXML/boostSerializer/Color.hpp"
#include "fwXML/boostSerializer/Histogram.hpp"
#include "fwXML/boostSerializer/TransfertFunctionPoint.hpp"
#include "fwXML/boostSerializer/TransfertFunction.hpp"
#include "fwXML/boostSerializer/Point.hpp"
#include "fwXML/boostSerializer/ResectionDB.hpp"

//#include "fwXML/boostSerializer/Graph.hpp"


// translator generic
#include "fwXML/XML/GenericXMLTranslator.hpp"

// specific
#include "fwXML/XML/ImageXMLTranslator.hpp"
#include "fwXML/XML/FieldXMLTranslator.hpp"
#include "fwXML/XML/IntegerXMLTranslator.hpp"
#include "fwXML/XML/StringXMLTranslator.hpp"
#include "fwXML/XML/PortXMLTranslator.hpp"
#include "fwXML/XML/EdgeXMLTranslator.hpp"
#include "fwXML/XML/NodeXMLTranslator.hpp"
#include "fwXML/XML/GraphXMLTranslator.hpp"
#include "fwXML/XML/TriangularMeshXMLTranslator.hpp"
#include "fwXML/XML/CompositeXMLTranslator.hpp"
#include "fwXML/XML/ListXMLTranslator.hpp"
#include "fwXML/XML/VectorXMLTranslator.hpp"
#include "fwXML/XML/PointListXMLTranslator.hpp"
#include "fwXML/XML/ResectionXMLTranslator.hpp"
#include "fwXML/XML/PlaneXMLTranslator.hpp"
#include "fwXML/XML/PlaneListXMLTranslator.hpp"
#include "fwXML/XML/SingleFileXMLTranslator.hpp"
#include "fwXML/XML/DictionaryOrganXMLTranslator.hpp"

namespace fwXML
{

REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator<fwTools::Object>                   ,  fwTools::Object                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Boolean>                ,  ::fwData::Boolean                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , IntegerXMLTranslator                                    ,  ::fwData::Integer                );
//REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Integer>                 ,  ::fwData::Integer                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , FieldXMLTranslator                                      ,  ::fwTools::Field                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::ProcessObject>          ,  ::fwData::ProcessObject          );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ImageXMLTranslator                                      ,  ::fwData::Image                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Float>                  ,  ::fwData::Float                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Region>                 ,  ::fwData::Region                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , StringXMLTranslator                                     ,  ::fwData::String                 );
//REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::String>                 ,  ::fwData::String                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::PatientDB>              ,  ::fwData::PatientDB              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Patient>                ,  ::fwData::Patient                );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Study>                  ,  ::fwData::Study                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Histogram>              ,  ::fwData::Histogram                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Acquisition>            ,  ::fwData::Acquisition            );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Reconstruction>         ,  ::fwData::Reconstruction         );
//REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::DictionaryOrgan>        ,  ::fwData::DictionaryOrgan        );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Dictionary>             ,  ::fwData::Dictionary             );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::TransformationMatrix3D> ,  ::fwData::TransformationMatrix3D );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::TransfertFunctionPoint> ,  ::fwData::TransfertFunctionPoint );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::TransfertFunction>      ,  ::fwData::TransfertFunction      );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Color>                  ,  ::fwData::Color                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::ResectionDB>            ,  ::fwData::ResectionDB            );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , NodeXMLTranslator                                       ,  ::fwData::Node                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GraphXMLTranslator                                      ,  ::fwData::Graph                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PortXMLTranslator                                       ,  ::fwData::Port                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , EdgeXMLTranslator                                       ,  ::fwData::Edge                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , TriangularMeshXMLTranslator                             ,  ::fwData::TriangularMesh         );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , CompositeXMLTranslator                                  ,  ::fwData::Composite              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , VectorXMLTranslator                                     ,  ::fwData::Vector                 );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ListXMLTranslator                                       ,  ::fwData::List                   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , GenericXMLTranslator< ::fwData::Point >                 ,  ::fwData::Point                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PointListXMLTranslator                                  ,  ::fwData::PointList              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , ResectionXMLTranslator                                  ,  ::fwData::Resection              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PlaneXMLTranslator                                      ,  ::fwData::Plane                  );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , PlaneListXMLTranslator                                  ,  ::fwData::PlaneList              );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , SingleFileXMLTranslator                                 ,  ::fwData::location::SingleFile   );
REGISTER_BINDING_BYCLASSNAME( XMLTranslator , DictionaryOrganXMLTranslator                            ,  ::fwData::DictionaryOrgan        );
}



