/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_COMPOSITEMSG_HPP_
#define _FWCOMED_COMPOSITEMSG_HPP_

//#include <map>
//#include <iostream>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

//namespace fwData
//{
//  class Composite;
//}

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

    fwCoreClassDefinitionsWithFactoryMacro((CompositeMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new CompositeMsg );

    /// Event identifier used to inform that fields were modified
    FWCOMED_API static std::string MODIFIED_FIELDS;

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


//  fwCoreClassDefinitionsWithFactoryMacro((CompositeMsg)(::fwServices::ObjectMsg::Baseclass), ( ((::boost::shared_ptr< ::fwData::Composite>)) ), new CompositeMsg );
//
//  /**
//   * @brief   constructor
//   */
//  FWCOMED_API CompositeMsg( ::boost::shared_ptr< ::fwData::Composite> _obj ) throw();
//
//  /**
//   * @brief   destuctor
//   */
//  FWCOMED_API virtual ~CompositeMsg() throw();
//
//  /**
//   * @brief search for the input or output modified, and invoke the appropriate method (i.e. either inputModified or output Modified)
//   * @note if does not correspond to input or output, considered as a traditionnal object field
//   */
//  FWCOMED_API void addModif( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _message, ModifProp _prop = ModifProp( ADDED , AFTER ) ) ;
//
//  /**
//   * @brief   get container with all messages concerning indifferently inputs and outputs
//   */
//  FWCOMED_API std::vector< ::boost::shared_ptr< const ::fwServices::ObjectMsg > > getMsg() const ;
//
//  /**
//   * @brief   get identifiers corresponding to modified inputs
//   */
//  FWCOMED_API std::vector< std::string > getModifIds() const ;
//
//  /**
//   * @brief   get modification message registered for specified input
//   */
//  FWCOMED_API ::boost::shared_ptr< const ::fwServices::ObjectMsg > getMsg(std::string _fieldId) const;
//
//  /**
//   * @brief return properties for specified id
//   */
//  FWCOMED_API ::fwServices::ObjectMsg::ModifProp getProp(std::string _fieldId ) ;
//
//  /**
//   * @brief   get casted modification message registered for specified input
//   */
//  template< class CONSTMSGTYPE >
//  ::boost::shared_ptr< const ::fwServices::ObjectMsg > getMsg(std::string _fieldId)
//  {
//      ::boost::shared_ptr< CONSTMSGTYPE > castedMsg = ::boost::dynamic_pointer_cast< CONSTMSGTYPE >( this->getMsg( _fieldId ) );
//      assert( castedMsg ) ;
//      return castedMsg ;
//  }
//
//
//protected:
//
//  /**
//   * @brief used by operator<<(std::ostream & _sstream, IService& _service)

//   */
//  FWCOMED_API virtual void info(std::ostream &_sstream ) const ;
//
//  /**
//   * @brief mapping input identifiers and messages
//   */
//  std::map< std::string , ::boost::shared_ptr< const ::fwServices::ObjectMsg > > m_ModificationMap ;
//
//  /**
//   * @brief modification properties per input identifier
//   */
//  std::map< std::string , fwServices::ObjectMsg::ModifProp > m_ModifProp ;

};

} // namespace fwComEd

#endif //_FWCOMED_COMPOSITEMSG_HPP_

