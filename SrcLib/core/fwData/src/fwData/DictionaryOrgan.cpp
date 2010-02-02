/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/DictionaryOrgan.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::DictionaryOrgan,  ::fwData::DictionaryOrgan);
namespace fwData
{
//------------------------------------------------------------------------------

DictionaryOrgan::DictionaryOrgan()
{
	SLM_WARN("::fwData::DictionaryOrgan() : (ToDo) field default value");
}

//------------------------------------------------------------------------------

DictionaryOrgan::~DictionaryOrgan()
{
	SLM_WARN("::fwData::~DictionaryOrgan() : (ToDo)");
}

//------------------------------------------------------------------------------

} // end namespace fwData
