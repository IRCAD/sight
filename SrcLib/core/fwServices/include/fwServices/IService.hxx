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
    OSLM_ASSERT("Expired object in service " << this->getID(), !m_associatedObject.expired() );
    SPTR(DATATYPE) casteDdata = ::boost::dynamic_pointer_cast<DATATYPE>( m_associatedObject.lock() );
    OSLM_ASSERT("DynamicCast "<< ::fwCore::TypeDemangler<DATATYPE>().getFullClassname()<<" failed", casteDdata);

    return casteDdata;
}

} // namespace fwServices

#endif // _FWSERVICES_ISERVICE_HXX_
