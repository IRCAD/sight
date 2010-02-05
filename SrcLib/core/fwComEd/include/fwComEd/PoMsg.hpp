/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PROCESSOBJECTMSG_HPP_
#define _FWCOMED_PROCESSOBJECTMSG_HPP_

#include <map>
#include <iostream>
#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwData
{
    class ProcessObject;
}

namespace fwComEd
{

///**
// * @brief Object event message specialized for processObject : store modification information
// */
//class FWCOMED_CLASS_API ProcessObjectMsg : public ::fwServices::ObjectMsg
//{
//public:
//  fwCoreClassDefinitionsWithFactoryMacro((ProcessObjectMsg)(::fwServices::ObjectMsg::Baseclass), ( ((::boost::shared_ptr< ::fwData::ProcessObject>)) ), new ProcessObjectMsg );
//  /**
//   * @brief   constructor
//   */
//  FWCOMED_API ProcessObjectMsg( ::boost::shared_ptr< ::fwData::ProcessObject> _obj ) throw();
//
//  /**
//   * @brief   destuctor
//   */
//  FWCOMED_API virtual ~ProcessObjectMsg() throw();
//
//  /**
//   * @brief search for the input or output modified, and invoke the appropriate method (i.e. either inputModified or output Modified)
//   * @note if does not correspond to input or output, considered as a traditionnal object field
//   */
//  FWCOMED_API void addIoModif( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _message, ModifProp _prop = ModifProp( ADDED , AFTER ) ) ;
//
//  /**
//   * @brief   get container with all messages concerning indifferently inputs and outputs
//   */
//  FWCOMED_API std::vector< ::boost::shared_ptr< const ::fwServices::ObjectMsg > > getIOMsg() const ;
//
//  /**
//   * @brief   get identifiers corresponding to modified inputs
//   */
//  FWCOMED_API std::vector< std::string > getInputModifIds() const ;
//
//  /**
//   * @brief   get modification message registered for specified input
//   */
//  FWCOMED_API ::boost::shared_ptr< const ::fwServices::ObjectMsg > getInputMsg(std::string _fieldId) const;
//
//  /**
//   * @brief return properties for specified id
//   */
//  FWCOMED_API ::fwServices::ObjectMsg::ModifProp getInputProp(std::string _fieldId ) ;
//
//  /**
//   * @brief   get casted modification message registered for specified input
//   */
//  template< class CONSTMSGTYPE >
//  ::boost::shared_ptr< const ::fwServices::ObjectMsg > getInputMsg(std::string _fieldId)
//  {
//      ::boost::shared_ptr< CONSTMSGTYPE > castedMsg = ::boost::dynamic_pointer_cast< CONSTMSGTYPE >( this->getInputMsg( _fieldId ) );
//      assert( castedMsg ) ;
//      return castedMsg ;
//  }
//
//  /**
//   * @brief   get identifiers corresponding to modified outputs
//   */
//  FWCOMED_API std::vector< std::string > getOutputModifIds() const;
//
//  /**
//   * @brief   get modification message registered for specified output
//   */
//  FWCOMED_API ::boost::shared_ptr< const ::fwServices::ObjectMsg > getOutputMsg(std::string _id) const;
//
//  /**
//   * @brief   get casted modification message registered for specified input
//   */
//  template< class CONSTMSGTYPE >
//  ::boost::shared_ptr< const ::fwServices::ObjectMsg > getOutputMsg(std::string _fieldId)
//  {
//      ::boost::shared_ptr< CONSTMSGTYPE > castedMsg = ::boost::dynamic_pointer_cast< CONSTMSGTYPE >( this->getOutputMsg( _fieldId ) );
//      assert( castedMsg ) ;
//      return castedMsg ;
//  }
//
//  /**
//   * @brief return properties for specified id
//   */
//  FWCOMED_API fwServices::ObjectMsg::ModifProp getOutputProp(std::string _fieldId ) ;
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
//  std::map< std::string , ::boost::shared_ptr< const ::fwServices::ObjectMsg > > m_inputModificationMap ;
//
//  /**
//   * @brief modification properties per input identifier
//   */
//  std::map< std::string , fwServices::ObjectMsg::ModifProp > m_inputModifProp ;
//
//  /**
//   * @brief mapping input identifiers and messages
//   */
//  std::map< std::string , ::boost::shared_ptr< const ::fwServices::ObjectMsg > > m_outputModificationMap ;
//
//  /**
//   * @brief modification properties per output identifier
//   */
//  std::map< std::string , fwServices::ObjectMsg::ModifProp > m_outputModifProp ;

//};

} // namespace fwComEd

#endif //_FWCOMED_PROCESSOBJECTMSG_HPP_

