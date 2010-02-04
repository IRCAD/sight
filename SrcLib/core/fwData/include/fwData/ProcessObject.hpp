/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PROCESSOBJECT_HPP_
#define _FWDATA_PROCESSOBJECT_HPP_

#include <map>

#include "fwData/Object.hpp"

fwCorePredeclare( (data)(Field) );

namespace fwData
{
/**
 * @class 	ProcessObject
 * @brief 	Provides the notion of Process Object: object having inputs and outputs
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
class FWDATA_CLASS_API ProcessObject : public Object
{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (ProcessObject)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< ProcessObject >) ;

	/// IO Container type
	static const std::string InputKey  ;
	/// IO Container type
	static const std::string OutputKey ;

	/**
	 * @brief	Constructor
	 */
	FWDATA_API ProcessObject() ;

	/**
	 * @brief	Destructor
	 */
	FWDATA_API virtual ~ProcessObject();

	/// retrieve the input data associated with _id
	FWDATA_API ::fwData::Object::sptr  getInput( std::string _id  );

	/// retrieve the input data associated with _id and type OBJECTTYPE
	template< class OBJECTTYPE >
	typename OBJECTTYPE::sptr  getInput( std::string _id  )
	{
		return OBJECTTYPE::dynamicCast( this->getInput( _id ) ) ;
	}

	/// retrieve the output data associated with _id
	FWDATA_API ::fwData::Object::sptr getOutput( std::string _id  );

	/// retrieve the output data associated with _id and type OBJECTTYPE
	template< class OBJECTTYPE >
	typename OBJECTTYPE::sptr getOutput( std::string _id  )
	{
		return OBJECTTYPE::dynamicCast( this->getOutput( _id ) ) ;
	}

	/// @{
	/// retrieve the input data
	FWDATA_API fwTools::Object::sptr  getInputs(  );
	FWDATA_API fwTools::Object::csptr  getInputs(  ) const ;
	///@}

	///@{
	/// retrieve the output data
	FWDATA_API fwTools::Object::sptr getOutputs(  );
	FWDATA_API fwTools::Object::csptr getOutputs(  ) const ;
	/// @}

	/// Affect value to input
	FWDATA_API void setInputValue(std::string _id, ::fwData::Object::sptr _object);
	/// Affect value to output
	FWDATA_API void setOutputValue(std::string _id, ::fwData::Object::sptr _object);

    typedef std::string MapKeyType;
    typedef ::fwData::Object::wptr MapValueType;
    typedef std::map< MapKeyType, MapValueType > MapType;
	/**
	 * @brief return a map where key is input id and value a weak pointer on associated object input
	 * @note this is not a reference: take care not to build iterator on return
	 */
	FWDATA_API MapType getInputMap() ;

	/**
	 * @brief return a map where key is output id and value a weak pointer on associated object output
	 * @note this is not a reference: take care not to build iterator on return
	 */
	FWDATA_API MapType getOutputMap() ;

	/**
	 * @brief build and return a container containing all inputs and ouputs (are Fields)
	 * @note required to scan all inputs and outputs without indifferently
	 * @note this is not a reference: take care not to build iterator on return
	 */
	FWDATA_API    Object::ChildContainer getIO();

};

} // namespace fwData


#endif //_FWDATA_PROCESSOBJECT_HPP_
