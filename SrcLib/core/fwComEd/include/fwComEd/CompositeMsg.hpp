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
 * @todo    clean old API commented
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
     * @param[in]   _modifiedFields list of the composite field which were modified.
     *
     * This method create a MODIFIED_FIELDS event with an additional data information containing the fields identifiers.
     */
    FWCOMED_API void addEventModifiedFields( const std::vector< std::string > & _modifiedFields );

    FWCOMED_API void addEventModifiedFields( const std::vector< std::string > & _modifiedFields, std::vector< ::fwData::Object::sptr > _oldObjects );

    /**
     * @brief   Return the list modified fields.
     * @return  The list of the modified fieds identifiers.
     * @pre     The message must contain a MODIFIED_FIELDS event.
     */
    FWCOMED_API std::vector< std::string > getEventModifiedFields() const;

    FWCOMED_API  void addEventModifiedField( std::string _modifiedField );



    FWCOMED_API void addAddedField( std::string _compositeKey, ::fwData::Object::sptr _pNewObject );
    FWCOMED_API ::fwData::Composite::sptr getAddedFields() const;

    FWCOMED_API void addRemovedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject );
    FWCOMED_API ::fwData::Composite::sptr getRemovedFields() const;

    FWCOMED_API void addSwappedField( std::string _compositeKey, ::fwData::Object::sptr _pOldObject, ::fwData::Object::sptr _pNewObject );
    FWCOMED_API ::fwData::Composite::sptr  getSwappedOldFields() const;
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

