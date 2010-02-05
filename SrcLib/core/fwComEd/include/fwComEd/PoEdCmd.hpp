/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

//#ifndef FWCOMED_POEDCMD_HPP_
//#define FWCOMED_POEDCMD_HPP_
//#include <fwServices/IEditionService.hpp>
//
//#include <map>
//#include "fwComEd/export.hpp"
//
//namespace fwComEd
//{
//
///**
// * @brief This edition command specialized for process object
// */
//class FWCOMED_CLASS_API POEditCmd : public fwServices::EdCmd
//{
//public:
//
//	/**
//	 * @brief	constructor
//	 */
//	FWCOMED_API POEditCmd() ;
//
//	/**
//	 * @brief	destructor
//	 */
//	FWCOMED_API virtual ~POEditCmd() ;
//
//	/// Overrides
//	FWCOMED_API virtual const boost::int32_t  getSize() const {return 0;};
//	/// Overrides
//	FWCOMED_API virtual	void apply() ;
//	/// Overrides
//	FWCOMED_API virtual	void unapply() ;
//	/// Overrides
//	FWCOMED_API virtual	const std::string getDescription( void ) const {return std::string("ProcessObjectEditAction command description");};
//
//	/// Values to be affected to the input of the algorithm object
//	FWCOMED_API void setInputValue( std::string id , ::boost::shared_ptr< ::fwData::Object > _object) ;
//	/// Values to be affected to the output of the algorithm object
//	FWCOMED_API void setOutputValue( std::string id , ::boost::shared_ptr< ::fwData::Object > _object) ;
//
//protected:
//
//	/**
//	 * @brief map of algorithm inputs
//	 * the first element stands for the id, related the role of the input (e.g. gradient image, seeds, parameter)
//	 * the second element hands the object being for example an image, or a point...
//	 */
//	std::map< std::string , ::boost::shared_ptr< ::fwData::Object > >		m_inputs ;
//
//	/**
//	 * @brief map of algorithm inputs
//	 * the first element stands for the id, related the role of the input (e.g. gradient image, seeds, parameter)
//	 * the second element hands the object being for example an image, or a point...
//	 */
//	std::map< std::string , ::boost::shared_ptr< ::fwData::Object > >		m_outputs ;
//
//};
//
//
//}
//
//#endif /*HELPER_HPP_*/
