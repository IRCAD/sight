/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_OBJECT_HPP_
#define _FWTOOLS_OBJECT_HPP_

#include <string>
#include <vector>

#include <fwCore/base.hpp>
#include <fwCore/TimeStamp.hpp>
#include <fwCore/LogicStamp.hpp>

#include "fwTools/config.hpp"
#include "fwTools/fwID.hpp"

namespace fwTools
{

class Field;
class UUID;

/**
 * @brief   Define Base class for FW4SPL objects and services
 * @class   Object
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject , protected ::fwTools::fwID
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Object), (()), new Object );
    fwCoreAllowSharedFromThis();

    friend class ::fwTools::UUID;

    typedef std::string FieldID;

    /// sub object contained by this one
    typedef std::vector< ::fwTools::Object::sptr > ChildContainer;

    typedef ::boost::shared_ptr< ::fwTools::Field > FieldSptr; // required du to a Field is declared afterward
    typedef ::boost::shared_ptr< const ::fwTools::Field > FieldCSptr; // required du to a Field is declared afterward


    // expose API for ID management
    FWTOOLS_API using  ::fwTools::fwID::hasID;
    FWTOOLS_API using  ::fwTools::fwID::getID;
    FWTOOLS_API using  ::fwTools::fwID::setID;
    FWTOOLS_API using  ::fwTools::fwID::resetID;
    FWTOOLS_API using  ::fwTools::fwID::swapID;


    /// return the sub class classname : an alias of this->getClassname
    FWTOOLS_API std::string className() const;

    FWTOOLS_API Object();

    FWTOOLS_API virtual ~Object();



/** @name Field
 * creation and destruction : Field are designed to *be named* and contain sub Element (children)
 */
//@{


    /**
     * @brief set the Field with fieldId with no Element ( create empty Field or erase oldest children )
     *
     * Behavior :
     * \li if Field doesn't exist create a Field with fieldId and no children.
     * \li if Field already exist : The corresponding children Field ( with label==FieldID ) is removed from
     *  children container ( smart_ptr meca ) then the new one is inserted like previous described action.
     *
     * Create (if no previous one) or replace the Field with fieldId as label and containing newSubObject.
     * @return the created Field
     **/
    FWTOOLS_API FieldSptr setField( const FieldID &fieldId );

    /**
     * @brief   Return a smart_ptr of corresponding field ( empty if non exist )
     * @note    Can be used to test whatever if the field exist
     */
    FWTOOLS_API FieldSptr getField( const FieldID &fieldId );

    /**
     * @brief   Return a smart_ptr of corresponding field ( empty if non exist ) (const version)
     */
    FWTOOLS_API FieldCSptr getField( const FieldID &fieldId ) const;

    /**
     * @brief   Return a smart_ptr of corresponding field ( empty if non exist ), in _path (subfields search)
     */
    FWTOOLS_API FieldSptr getField( std::vector< std::string > _path , const FieldID &fieldId );

    /// lazy remove field ( perform nothing if non-exist )
    FWTOOLS_API void removeField( const FieldID &fieldId );

//@} // end group Field


/** @name FieldElement
 * add,remove Element in Field
 */
//@{

    /**
     * @brief   Set a *unique* newSubObject if the Field fieldId (created if necessary, other children are dropped)
     *
     * Behavior :
     * \li if Field doesn't exist create a Field with fieldId as label and containing the newSubObject.
     *  Then this labeledObject is insert in children container of the current Objecy
     * \li if Field already exist : The corresponding children Field ( with label==FieldID ) are removed from
     *  children container ( smart_ptr meca ) then the new one is inserted like previous described action
     * Create (if no previous one) or replace the Field with fieldId as label and containing newSubObject.
     * @return the created Field
     **/
    FWTOOLS_API FieldSptr setFieldSingleElement( const FieldID &fieldId, ::fwTools::Object::sptr newSubObject );

    /**
     * @brief add into the field fieldId the newSubObject.
     *
     * Behavior :
     * \li If the field doesn't exist use setField behavior
     * \li If the field exist get the specific object then append newSubObject to specificObject children container
     */
    FWTOOLS_API FieldSptr addFieldElement( const FieldID &fieldId, ::fwTools::Object::sptr newSubObject );

    /// remove the subObject if present *in* the givenField ( perform nothing if non-exist )
    FWTOOLS_API void removeFieldElement( const FieldID &fieldId, ::fwTools::Object::sptr subObjectToRemove );

    /**
     * @brief get the number of element in then given field
     * @return the number of element in the field, return 0 if the field does not exist
     */
    FWTOOLS_API int getFieldSize( const FieldID& id ) const throw();


    /**
     * @brief       Retrieve *the single* element (casted) in fieldID.
     * @param[in]   id The field ID.
     * @note        An assert raised if attempting to get a singleElement in a multiElement Field
     * @return      The corresponding typed field, null if nonexistent (or cast Failed).
     */
    template< typename FWDATATYPE >
    typename FWDATATYPE::sptr getFieldSingleElement( const FieldID& id ) const
        throw();


    /**
     * @brief       Retrieve the ith element element (casted) in fieldID.
     * @param[in]   id The field ID.
     * @return      The corresponding typed field, null if nonexistent (or cast Failed).
     */
    template< typename FWDATATYPE >
    typename FWDATATYPE::sptr getFieldElement( const FieldID& id , unsigned int _index ) const
        throw();


    /// get all Fields Element FOR EXPERT ONLY
    ChildContainer &children() { return m_children; }

    /// get all Fields Element FOR EXPERT ONLY
    const ChildContainer &children() const { return m_children; }

//@} // GroupFieldElement


    /**
     * @name    ***DEPRECATED*** All concerning universal unique identifier (UUID) ***DEPRECATED***
     */
    //@{
    /**
     * @brief Set a name for an object
     * @deprecated (should use UUID)
     */
    FWTOOLS_API void setName ( const std::string _strName ) { m_strName = _strName; }

    /**
     * @brief Retrieve the name object
     * @deprecated (should use UUID)
     */
    FWTOOLS_API const std::string getName() const { return m_strName; }

    //@}

    FWTOOLS_API void modified()
    {
        m_logicStamp->modified();
    };

    FWTOOLS_API void timeModified()
    {
        m_timeStamp->modified();
    };

    FWTOOLS_API void setLifePeriod(::fwCore::TimeStamp::TimeStampType p)
    {
        m_timeStamp->setLifePeriod(p);
    };

    FWTOOLS_API ::fwCore::TimeStamp::TimeStampType getLifePeriod()
    {
        return m_timeStamp->getLifePeriod();
    };

    FWTOOLS_API bool periodExpired()
    {
        return m_timeStamp->periodExpired();
    };

    FWTOOLS_API ::fwCore::TimeStamp::sptr  getTimeStamp()  const { return m_timeStamp; }
    FWTOOLS_API ::fwCore::LogicStamp::sptr getLogicStamp() const { return m_logicStamp; }

    FWTOOLS_API ::fwCore::LogicStamp::csptr getOSRKey() const { return m_OSRKey; }

protected :

    ChildContainer m_children;

    ::fwCore::TimeStamp::sptr  m_timeStamp;

    ::fwCore::LogicStamp::sptr m_logicStamp;

    SPTR(::fwTools::UUID) m_uuid;

private :

    /// Standard copy operator, forbidden.
    Object &operator=(const Object &_obj);

    /// Name identification for an object
    std::string m_strName;

    /// key used to register object in OSR
    ::fwCore::LogicStamp::sptr m_OSRKey;
};

}

#include "fwTools/Field.hpp"
#include "fwTools/Object.hxx"

#endif /* _FWTOOLS_OBJECT_HPP_ */
