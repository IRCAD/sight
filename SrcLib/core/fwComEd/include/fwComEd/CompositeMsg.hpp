/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_COMPOSITEMSG_HPP_
#define _FWCOMED_COMPOSITEMSG_HPP_

#include <fwServices/ObjectMsg.hpp>
#include <fwData/Composite.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for Composite : store modification information
 * @class   CompositeMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2005-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API CompositeMsg : public ::fwServices::ObjectMsg
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((CompositeMsg)(::fwServices::ObjectMsg), ( () ), new CompositeMsg );

    /// Event identifier used to inform that keys were modified
    FWCOMED_API static std::string MODIFIED_KEYS;

    /// Event identifier used to inform that keys were added
    FWCOMED_API static std::string ADDED_KEYS;

    /// Event identifier used to inform that keys were removed
    FWCOMED_API static std::string REMOVED_KEYS;

    /// Event identifier used to inform that keys were changed
    FWCOMED_API static std::string CHANGED_KEYS;


    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API CompositeMsg() throw();

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~CompositeMsg() throw();

    /**
     * @brief       This method add an event to the message from its keys identifiers.
     * @param[in]   _modifiedkeys : list of the keys of the composite object which were modified.
     *
     * This method create a MODIFIED_KEYS event.
     */
    FWCOMED_API void addModifiedKeysEvent( const std::vector< std::string > & _modifiedkeys );

    /**
     * @brief       This method add an event to the message from its keys identifiers.
     * @param[in]   _modifiedkeys : list of the keys of the objects which were modified.
     * @param[in]   _oldObjects list : of objects which were modified.
     *
     * This method create a MODIFIED_KEYS event.
     */
    FWCOMED_API void addModifiedKeysEvent( const std::vector< std::string > & _modifiedkeys, std::vector< ::fwData::Object::sptr > _oldObjects );

    /**
     * @brief   Return the list modified keys.
     * @return  The list of the modified keys identifiers.
     * @pre     The message must contain a MODIFIED_KEYS event.
     */
    FWCOMED_API std::vector< std::string > getModifiedKeys() const;

    /**
     * @brief       This method add an object in the modified keys list.
     * @param[in]   _modifiedkey : the key of the object which was modified.
     *
     * This method create a MODIFIED_KEYS event with an additional data information containing the keys identifiers.
     */
    FWCOMED_API  void addModifiedKeyEvent( std::string _modifiedKey );


    /**
     * @brief       This method add an object in the composite of added keys.
     * @param[in]   _compositeKey : the key of the added object.
     * @param[in]   _pNewObject   : the added object.
     */
    FWCOMED_API void appendAddedKey( std::string _compositeKey, ::fwData::Object::sptr _pNewObject );

    /**
     * @brief   Return the composite of the added keys.
     */
    FWCOMED_API ::fwData::Composite::sptr getAddedKeys() const;


    /**
     * @brief       This method add an object in the composite of removed keys.
     * @param[in]   _compositeKey : the key of the removed object.
     * @param[in]   _pOldObject   : the object which will be removed.
     */
    FWCOMED_API void appendRemovedKey( std::string _compositeKey, ::fwData::Object::sptr _pOldObject );

    /**
     * @brief   Return the composite of the removed keys.
     */
    FWCOMED_API ::fwData::Composite::sptr getRemovedKeys() const;

    /**
     * @brief       This method add an object in the composites of changed keys.
     * @param[in]   _compositeKey : the key of the changed object.
     * @param[in]   _pOldObject   : the old object which will be replaced.
     * @param[in]   _pNewObject   : the new object.
     */
    FWCOMED_API void appendChangedKey( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject );

    /**
     * @brief   Return the composite of the old changed keys (key which will be replaced).
     */
    FWCOMED_API ::fwData::Composite::sptr  getOldChangedKeys() const;

    /**
    * @brief   Return the composite of the new changed keys.
    */
    FWCOMED_API ::fwData::Composite::sptr  getNewChangedKeys() const;

private :

    std::vector< std::string > m_modifiedKeys;
    std::vector< ::fwData::Object::sptr > m_modifiedObjects;

    ::fwData::Composite::sptr m_removedKeys;
    ::fwData::Composite::sptr m_addedKeys;
    ::fwData::Composite::sptr m_oldChangedKeys;
    ::fwData::Composite::sptr m_newChangedKeys;
};

} // namespace fwComEd

#endif //_FWCOMED_COMPOSITEMSG_HPP_

