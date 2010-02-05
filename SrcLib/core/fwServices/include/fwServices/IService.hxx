/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ISERVICE_HXX_
#define _FWSERVICES_ISERVICE_HXX_

namespace fwServices
{

template< class DATATYPE >
 SPTR(DATATYPE) IService::getObject()
{
	assert( m_associatedObject.use_count() );
	assert( m_associatedObject.expired() == false );

	SPTR(DATATYPE) casteDdata = ::boost::dynamic_pointer_cast<DATATYPE>( m_associatedObject.lock() );
	assert(casteDdata);

	return casteDdata;
}

} // namespace fwServices

#endif // _FWSERVICES_ISERVICE_HXX_
