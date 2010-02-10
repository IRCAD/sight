/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_HELPER_COMPOSITE_HPP_
#define _FWCOMED_HELPER_COMPOSITE_HPP_

#include <fwServices/IService.hpp>

#include "fwComEd/export.hpp"
#include "fwComEd/CompositeMsg.hpp"
#include "fwData/Composite.hpp"



namespace fwComEd
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Composite and create in parallel the message to annouce this modification.
 * @struct  Composite
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWCOMED_CLASS_API Composite
{

public :

    FWCOMED_API Composite( ::fwData::Composite::wptr _composite );

    FWCOMED_API ~Composite();

    FWCOMED_API void add( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    FWCOMED_API void remove( std::string _compositeKey );

    FWCOMED_API void swap( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    FWCOMED_API void notify( ::fwServices::IService::sptr _serviceSource );

private :

    ::fwComEd::CompositeMsg::sptr m_compositeMsg;

    ::fwData::Composite::wptr m_composite;
};

} // namespace helper
} // namespace fwComEd

#endif // _FWCOMED_HELPER_COMPOSITE_HPP_
