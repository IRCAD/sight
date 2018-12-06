/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

