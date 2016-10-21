/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_HELPER_COMPOSITE_HPP__
#define __FWDATATOOLS_HELPER_COMPOSITE_HPP__

#include "fwDataTools/config.hpp"

#include "fwData/Composite.hpp"

namespace fwDataTools
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Composite and create in parallel the message to announce this modification.
 */
class FWDATATOOLS_CLASS_API Composite
{

public:

    /// Constructor. Initialize parameters.
    FWDATATOOLS_API Composite( ::fwData::Composite::wptr _composite );

    /// Destrucotr. Do nothing.
    FWDATATOOLS_API ~Composite();

    /**
     * @brief Add an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    object to add in the composite
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void add( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    /**
     * @brief Remove an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void remove( std::string _compositeKey );

    /**
     * @brief Clear all objects in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void clear();

    /**
     * @brief Replace an object in the composite.
     * @param[in] _compositeKey key of the object in the composite.
     * @param[in] _newObject    the new object to add in the composite.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void swap( std::string _compositeKey, ::fwData::Object::sptr _newObject );

    /**
     * @brief Send the message of modification
     * @param _serviceSource service used to send the message
     * @param _allowLoops if true, the _serviceSource will receive the message
     */
    FWDATATOOLS_API void notify();

private:

    /// Map of added objects, send on notify
    ::fwData::Composite::ContainerType m_addedObjects;
    /// Map of new changed objects, send on notify
    ::fwData::Composite::ContainerType m_newChangedObjects;
    /// Map of old changed objects, send on notify
    ::fwData::Composite::ContainerType m_oldChangedObjects;
    /// Map of removed objects, send on notify
    ::fwData::Composite::ContainerType m_removedObjects;
    /// Composite to add/remove/change objects
    ::fwData::Composite::wptr m_composite;
};

} // namespace helper
} // namespace fwDataTools

#endif // __FWDATATOOLS_HELPER_COMPOSITE_HPP__
