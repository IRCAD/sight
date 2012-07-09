#ifndef __FWDATACAMP_RECONSTRUCTION_HPP__
#define __FWDATACAMP_RECONSTRUCTION_HPP__

#include <fwCamp/macros.hpp>
#include <fwData/Reconstruction.hpp>

#include "fwDataCamp/config.hpp"

// fwCampAutoDeclareDataMacro((fwData)(Reconstruction), FWDATACAMP_API)
fwCampDeclareAccessor((fwData)(Reconstruction), (fwData)(Mesh));
fwCampDeclareAccessor((fwData)(Reconstruction), (fwData)(Image));
fwCampDeclareAccessor((fwData)(Reconstruction), (fwData)(Material));
#endif /* __FWDATACAMP_RECONSTRUCTION_HPP__ */
