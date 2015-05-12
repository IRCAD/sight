/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_COMPOSITE_HPP__
#define __FWCOMED_HELPER_COMPOSITE_HPP__

#include <fwServices/IService.hpp>

#include "fwComEd/config.hpp"
#include "fwComEd/CompositeMsg.hpp"
#include "fwData/Composite.hpp"



namespace fwComEd
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Composite and create in parallel the message to announce this modification.
 * @struct  Composite
 *
 * @date    2007-2009.
 */
class FWCOMED_CLASS_API Composite
{

public:

    /// Constructor. Initialize parameters.
    FWCOMED_API Composite( ::fwData::Composite::wptr _composite );

    /// Destrucotr. Do nothing.
    FWCOMED_API ~Composite();

    /**
     * @brief Add an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    object to add in the composite
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void add( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    /**
     * @brief Remove an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void remove( std::string _compositeKey );

    /**
     * @brief Clear all objects in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void clear();

    /**
     * @brief Replace an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    the new object to add in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWCOMED_API void swap( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    /**
     * @brief Send the message of modification
     * @param _serviceSource service used to send the message
     * @param _allowLoops if true, the _serviceSource will receive the message
     */
    FWCOMED_API void notify( ::fwServices::IService::sptr _serviceSource, bool _allowLoops = true );

private:

    ::fwComEd::CompositeMsg::sptr m_compositeMsg;
    ::fwData::Composite::wptr m_composite;
};

} // namespace helper
} // namespace fwComEd

#endif // __FWCOMED_HELPER_COMPOSITE_HPP__
