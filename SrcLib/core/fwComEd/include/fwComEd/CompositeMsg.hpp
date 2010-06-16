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

    /// Event identifier used to inform that fields were modified
    FWCOMED_API static std::string MODIFIED_FIELDS;

    /// Event identifier used to inform that fields were added
    FWCOMED_API static std::string ADDED_FIELDS;

    /// Event identifier used to inform that fields were removed
    FWCOMED_API static std::string REMOVED_FIELDS;

    /// Event identifier used to inform that fields were swapped
    FWCOMED_API static std::string SWAPPED_FIELDS;


    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API CompositeMsg() throw();

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~CompositeMsg() throw();

    /**
     * @brief       This method add an event to the message from its fields identifiers.
     * @param[in]   _modifiedFields : list of the keys of the composite object which were modified.
     *
     * This method create a MODIFIED_FIELDS event.
     */
    FWCOMED_API void addEventModifiedFields( const std::vector< std::string > & _modifiedFields );

    /**
     * @brief       This method add an event to the message from its fields identifiers.
     * @param[in]   _modifiedFields : list of the keys of the objects which were modified.
     * @param[in]   _oldObjects list : of objects which were modified.
     *
     * This method create a MODIFIED_FIELDS event.
     */
    FWCOMED_API void addEventModifiedFields( const std::vector< std::string > & _modifiedFields, std::vector< ::fwData::Object::sptr > _oldObjects );

    /**
     * @brief   Return the list modified fields.
     * @return  The list of the modified fields identifiers.
     * @pre     The message must contain a MODIFIED_FIELDS event.
     */
    FWCOMED_API std::vector< std::string > getEventModifiedFields() const;

    /**
     * @brief       This method add an object in the modified fields list.
     * @param[in]   _modifiedField : the key of the object which was modified.
     *
     * This method create a MODIFIED_FIELDS event with an additional data information containing the fields identifiers.
     */
    FWCOMED_API  void addEventModifiedField( std::string _modifiedField );


    /**
     * @brief       This method add an object in the composite of added fields.
     * @param[in]   _compositeKey : the key of the added object.
     * @param[in]   _pNewObject   : the added object.
     */
    FWCOMED_API void addAddedField( std::string _compositeKey, ::fwData::Object::sptr _pNewObject );

    /**
     * @brief   Return the composite of the added fields.
     */
    FWCOMED_API ::fwData::Composite::sptr getAddedFields() const;


    /**
     * @brief       This method add an object in the composite of removed fields.
     * @param[in]   _compositeKey : the key of the removed object.
     * @param[in]   _pOldObject   : the object which will be removed.
     */
    FWCOMED_API void addRemovedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject );

    /**
     * @brief   Return the composite of the removed fields.
     */
    FWCOMED_API ::fwData::Composite::sptr getRemovedFields() const;

    /**
     * @brief       This method add an object in the composites of swapped fields.
     * @param[in]   _compositeKey : the key of the swapped object.
     * @param[in]   _pOldObject   : the old object which will be replaced.
     * @param[in]   _pNewObject   : the new object.
     */
    FWCOMED_API void addSwappedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject );

    /**
     * @brief   Return the composite of the old swapped fields (field which will be replaced).
     */
    FWCOMED_API ::fwData::Composite::sptr  getSwappedOldFields() const;

    /**
    * @brief   Return the composite of the new swapped fields.
    */
    FWCOMED_API ::fwData::Composite::sptr  getSwappedNewFields() const;

private :

    std::vector< std::string > m_modifiedFields;
    std::vector< ::fwData::Object::sptr > m_modifiedObjects;

    ::fwData::Composite::sptr m_removedFields;
    ::fwData::Composite::sptr m_addedFields;
    ::fwData::Composite::sptr m_swappedOldFields;
    ::fwData::Composite::sptr m_swappedNewFields;
};

} // namespace fwComEd

#endif //_FWCOMED_COMPOSITEMSG_HPP_

