#ifndef __FWDATACAMP_MATERIAL_HPP__
#define __FWDATACAMP_MATERIAL_HPP__

#include <fwCamp/macros.hpp>
#include <fwData/Material.hpp>

#include "fwDataCamp/config.hpp"

// fwCampAutoDeclareDataMacro((fwData)(Material), FWDATACAMP_API)
fwCampAutoDeclareEnumMacro((fwData)(Material)(SHADING_MODE));
fwCampAutoDeclareEnumMacro((fwData)(Material)(REPRESENTATION_MODE));
fwCampAutoDeclareEnumMacro((fwData)(Material)(OPTIONS_MODE));

fwCampDeclareAccessor((fwData)(Material), (fwData)(Color));
#endif /* __FWDATACAMP_MATERIAL_HPP__ */
