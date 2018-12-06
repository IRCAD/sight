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

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include <fwAtomsFilter/registry/macros.hpp>

#include "filterVRRender/MedicalDataV1.hpp"

namespace filterVRRender
{

fwAtomsFilterRegisterMacro( ::filterVRRender::MedicalDataV1, "VRRenderMedicalDataV1" );

MedicalDataV1::MedicalDataV1(::fwAtomsFilter::IFilter::Key key)
{
}

MedicalDataV1::~MedicalDataV1()
{
}

void clearMapAttribute(const std::string& key, const ::fwAtoms::Map::sptr& values)
{
    if(values->find(key) != values->end())
    {
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast( (*values)[key] );
        SLM_ASSERT("Failed to retrieve key '" + key + "' from atom", obj);

        ::fwAtoms::Map::sptr objValues = obj->getAttribute< ::fwAtoms::Map >("values");
        SLM_ASSERT("Failed to retrieve 'values' attribute as ::fwAtoms::Map", objValues);
        objValues->clear();
    }
}

void MedicalDataV1::apply(const SPTR(::fwAtoms::Object)& atom)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Unable to filter atom : invalid object", atom);

    const std::string& classname = ::fwAtomsPatch::helper::getClassname(atom);
    FW_RAISE_IF("Unable to filter atom of class '" << classname << "'. Expected class is '::fwData::Composite'",
                classname != "::fwData::Composite");

    ::fwAtoms::Map::sptr values = atom->getAttribute< ::fwAtoms::Map >("values");
    SLM_ASSERT("Failed to retrieve 'values' attribute as ::fwAtoms::Map", values);

    clearMapAttribute("processingDB", values);
    clearMapAttribute("planningDB", values);
}

}

