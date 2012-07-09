#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/TransformationMatrix3D.hpp"

fwCampImplementDataMacro((fwData)(TransformationMatrix3D))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("coefficient", &::fwData::TransformationMatrix3D::m_vCoefficients)
        ;
}
