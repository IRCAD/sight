#ifndef __FWDATACAMP_CAMERA_HPP__
#define __FWDATACAMP_CAMERA_HPP__

#include <fwCamp/macros.hpp>
#include <fwData/Camera.hpp>

#include "fwDataCamp/config.hpp"

// fwCampAutoDeclareDataMacro((fwData)(Camera), FWDATACAMP_API)

fwCampDeclareAccessor((fwData)(Camera), (fwData)(TransformationMatrix3D));
#endif /* __FWDATACAMP_CAMERA_HPP__ */
