/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_LIGHTOBJECT_HPP_
#define _FWTOOLS_LIGHTOBJECT_HPP_

#include <string>
#include <vector>

#include <fwCore/base.hpp>
#include <fwCore/TimeStamp.hpp>
#include <fwCore/LogicStamp.hpp>

#include "fwTools/config.hpp"


namespace fwTools {

class Object;
class Field;

/**
 * @brief   Defines the abstract interface for deletion.
 * @class   IDeleter
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @note    To be specialized to extend object destruction procedure.
 * @note    Typical use: for service unregistration while keeping data independant with respect to fwServices library and keeping the benefits of smart pointers use.
 */
struct FWTOOLS_CLASS_API IDeleter : public ::fwCore::BaseObject
{
    fwCoreClassDefinitionsWithFactoryMacro((IDeleter), (()), new IDeleter );
    /**
     * @brief   Constructor : does nothing.
     */
    FWTOOLS_API IDeleter() {};
    /**
     * @brief   Destructor : does nothing.
     */
    FWTOOLS_API virtual ~IDeleter() {};

    /// To trigger specific procedure when obj destructor is invoked - does nothing by default
    FWTOOLS_API virtual void Delete( ::fwTools::Object *obj) {};

};


/**
 * @brief   Define Base class for FW4SPL objects and services
 * @class   Object
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
//class FWTOOLS_CLASS_API Object  : public ::boost::enable_shared_from_this<Object>
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Object), (()), new Object );
    fwCoreAllowSharedFromThis();



    typedef std::string FieldID;

    /// sub object contained by this one
    typedef std::vector< ::fwTools::Object::sptr > ChildContainer;

    typedef boost::shared_ptr< ::fwTools::Field > FieldSptr; // required du to a Field is declared afterward
    typedef boost::shared_ptr< const ::fwTools::Field > FieldCSptr; // required du to a Field is declared afterward



    /// return the sub class classname : an alias of this->getClassname
    FWTOOLS_API std::string className() const;

    /// return the simple version of UUID. If not set generate a new one (from UUID Class)
    FWTOOLS_API std::string getUUID() const;

    /// return true iff object have a UUID registred
    FWTOOLS_API bool hasUUID() const;

    /// set a new ID for object : assertion if object already have an ID or ID is already assigned
    FWTOOLS_API void setUUID(const std::string &newID);

    /// Usefull to externally define the way to delete an object
    FWTOOLS_API void setDeleter( ::fwTools::IDeleter::sptr _deleter ) ;

    /// Retrieve the object deleter
    FWTOOLS_API ::fwTools::IDeleter::sptr getDeleter() ;

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



    /**
     * @brief A shallow copy (also called "bitwise copy") simply copies chunks of memory from one location to another.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API virtual void shallowCopy( Object::csptr _source );

    /**
     * @brief A deep copy clone all source object parameters (sub-ojects are duplcated in memory). For a sptr on sub-object, method allocates a new object.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API virtual void deepCopy( Object::csptr _source );

protected :

    ChildContainer m_children;

    ::fwCore::TimeStamp::sptr  m_timeStamp;

    ::fwCore::LogicStamp::sptr m_logicStamp;

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    void shallowCopyOfChildren( Object::csptr _source );

    /**
     * @brief A deep copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    void deepCopyOfChildren( Object::csptr _source );

    /**
     * @brief A shallow copy of fields (objects in m_children), tests the classname and calls the specific method of the class.
     * @param[in] _source source of the copy.
     */
    template< typename FWDATATYPE >
    void shallowCopy( ::fwTools::Object::csptr _source );

    /**
     * @brief A deep copy of fields (objects in m_children), tests the classname and calls the specific method of the class.
     * @param[in] _source source of the copy.
     */
    template< typename FWDATATYPE >
    void deepCopy( ::fwTools::Object::csptr _source );


private :

    /// Standard copy operator, forbiden.
    Object &operator=(const Object &_obj);

    /// cache for storing uid
    mutable std::string m_cachedSimpleUuid;

    /// Abstract deleter for an object
    ::fwTools::IDeleter::sptr m_deleter ;

    /// Name identification for an object
    std::string m_strName;


};

/**
 * @brief   Class for objects fields.
 * @class   Field.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWTOOLS_CLASS_API Field :  public ::fwTools::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Field)(::fwTools::Object::Baseclass), (()) ( ((const std::string &)) ) , new Field );

    /// default constructor label is empty
    FWTOOLS_API Field();

    /// constructor with a given name
    FWTOOLS_API Field(const std::string &newlabel);

    /// retreive read only reference on label for the current class
    FWTOOLS_API const std::string &label() const;

    /// retreive r/w reference on label for the current class
    FWTOOLS_API std::string & label();

    FWTOOLS_API virtual ~Field();

    /**
     * @brief A shallow copy (also called "bitwise copy") simply copies chunks of memory from one location to another.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API void shallowCopy( ::fwTools::Field::csptr _source );
    FWTOOLS_API void shallowCopy( ::fwTools::Field::sptr _source );

    /**
     * @brief A deep copy clone all source object parameters (sub-ojects are duplcated in memory). For a sptr on sub-object, method allocates a new object.
     * @param[in] _source source of the copy.
     */
    FWTOOLS_API void deepCopy( ::fwTools::Field::csptr _source );
    FWTOOLS_API void deepCopy( ::fwTools::Field::sptr _source );

    FWTOOLS_API void shallowCopy( ::fwTools::Object::csptr _source );

    FWTOOLS_API void deepCopy( ::fwTools::Object::csptr _source );

protected :

    std::string m_label;

};

/**
 * @brief Return label (if any) from a ::fwTools::Object
 *
 * @pre ::fwTools::Object must be a Field
 */
FWTOOLS_API const std::string & getLabel(const ::fwTools::Object *obj);

/**
 * @brief Return Field smart_pointer (if possible) from a ::fwTools::Object
 *
 * @pre ::fwTools::Object must be a Field
 */
FWTOOLS_API ::fwTools::Field::sptr castToField(::fwTools::Object::sptr obj);



}

#include "fwTools/Object.hxx"

#endif /* LIGHTOBJECT_HPP_ */
