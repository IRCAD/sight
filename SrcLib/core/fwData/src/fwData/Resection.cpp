/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Resection.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Resection, ::fwData::Resection );

namespace fwData
{

//------------------------------------------------------------------------------

Resection::Resection ()
: m_isSafePart(true),
  m_isVisible(true),
  m_isValid(false)
{
	SLM_TRACE_FUNC();
	m_planeList = ::fwData::PlaneList::New();
}

//------------------------------------------------------------------------------

Resection::~Resection ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Resection::sptr Resection::clone() const
{
	SLM_TRACE_FUNC();

	Resection::NewSptr pNewResection;

	// Copy encoding
	pNewResection->m_planeList = this->m_planeList->clone();

	for ( 	Resection::ResectionInputs::const_iterator iter = this->m_vInputs.begin();
			iter != this->m_vInputs.end();
			++iter )
	{
		pNewResection->m_vInputs.push_back( (*iter)->clone() );
	}

	for ( 	Resection::ResectionOutputs::const_iterator iter = this->m_vOutputs.begin();
			iter != this->m_vOutputs.end();
			++iter )
	{
		pNewResection->m_vOutputs.push_back( (*iter)->clone() );
	}

	return pNewResection;
}

//------------------------------------------------------------------------------

Resection & Resection::operator=( const Resection & _resection )
{
	// Copy encoding
	m_planeList = _resection.m_planeList;
	m_vInputs = _resection.m_vInputs;
	m_vOutputs = _resection.m_vOutputs;

	return *this;
}

//------------------------------------------------------------------------------

} // namespace fwData


