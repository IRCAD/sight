/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FILTERVRRENDER_MEDICALDATAV2_HPP__
#define __FILTERVRRENDER_MEDICALDATAV2_HPP__

#include <fwAtomsFilter/IFilter.hpp>

#include <fwCore/macros.hpp>

#include "filterVRRender/config.hpp"

namespace fwAtoms
{
class Object;
}

namespace filterVRRender
{

/**
 * @class MedicalDataV2
 * @brief Filter associated to context "MedicalData" version "V2" in VR-Render.
 *
 */
class FILTERVRRENDER_CLASS_API MedicalDataV2 : public ::fwAtomsFilter::IFilter
{

public:
    /// Constructor.
    MedicalDataV2(::fwAtomsFilter::IFilter::Key key);

    /// Destructor.
    ~MedicalDataV2();

    /// Apply filter onto given atom object.
    FILTERVRRENDER_API virtual void apply(const SPTR(::fwAtoms::Object)& atom);
};

} // namespace filterVRRender

#endif // __FILTERVRRENDER_MEDICALDATAV2_HPP__

