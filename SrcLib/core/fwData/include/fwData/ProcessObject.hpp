/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PROCESSOBJECT_HPP_
#define _FWDATA_PROCESSOBJECT_HPP_

#include <map>
#include <vector>

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

fwCorePredeclare( (fwData)(Field) );

namespace fwData
{
/**
 * @class   ProcessObject
 * @brief   Provides the notion of Process Object: object having inputs and outputs
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API ProcessObject : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ProcessObject)(::fwData::Object), (()), ::fwData::Factory::New< ProcessObject >) ;

    typedef std::string ParamNameType;
    typedef std::vector<std::string> ParamNameVectorType;
    typedef std::map< ParamNameType, ::fwData::Object::sptr > ProcessObjectMapType;

    /**
     * @brief Retrieves the input data associated with specified name (null if non exist).
     * @param[in] name Input name
     * @return null sptr if input is not found
     */
    FWDATA_API ::fwData::Object::sptr getInput(const ParamNameType& name);

    /**
     * @brief Retrieves the input data associated with specified name type OBJECTTYPE (null if non exist).
     * @param[in] name Input name
     * @return null sptr if input is not found
     */
    template< class OBJECTTYPE >
    typename OBJECTTYPE::sptr getInput(const ParamNameType& name)
    {
        return OBJECTTYPE::dynamicCast( this->getInput( name ) ) ;
    }

    /**
     * @brief Retrieves the output data associated with specified name (null if non exist).
     * @param[in] name Output name
     * @return null sptr if output is not found
     */
    FWDATA_API ::fwData::Object::sptr getOutput(const ParamNameType& name);

    /**
     * @brief Retrieves the output data associated with specified name type OBJECTTYPE (null if non exist).
     * @param[in] name Output name
     * @return null sptr if output is not found
     */
    template< class OBJECTTYPE >
    typename OBJECTTYPE::sptr getOutput(const ParamNameType& name)
    {
        return OBJECTTYPE::dynamicCast( this->getOutput( name ) ) ;
    }

    /// Retrieve the input data
    fwDataGetSetCRefMacro(Inputs, ProcessObjectMapType);

    /// Retrieve the output data
    fwDataGetSetCRefMacro(Outputs, ProcessObjectMapType);

    /**
     * @brief Register input value with specified name.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Input name
     * @param[in] object  Input value
     */
    FWDATA_API void setInputValue(const ParamNameType& name, ::fwData::Object::sptr object);

    /**
     * @brief Register output value with specified name.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Output name
     * @param[in] object  Output value
     */
    FWDATA_API void setOutputValue(const ParamNameType& name, ::fwData::Object::sptr object);

    /**
     * @brief Returns vector of input parameters names.
     */
    FWDATA_API ParamNameVectorType getInputsParamNames() const;

    /**
     * @brief Returns vector of output parameters names.
     */
    FWDATA_API ParamNameVectorType getOutputsParamNames() const;

    /**
     * @brief Unregister all inputs parameters.
     */
    FWDATA_API void clearInputs();

    /**
     * @brief Unregister all output parameters.
     */
    FWDATA_API void clearOutputs();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( ProcessObject::csptr source );

    /// Defines deep copy
    FWDATA_API void deepCopy( ProcessObject::csptr source );

protected:

    /**
     * @brief   Constructor
     */
    FWDATA_API ProcessObject() ;

    /**
     * @brief   Destructor
     */
    FWDATA_API virtual ~ProcessObject();

    /**
     * @brief Returns vector of parameters names from params map.
     */
    FWDATA_API ParamNameVectorType getParamNames(const ProcessObjectMapType& params) const;

    /**
     * @brief Register value with specified name in params map.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Param name
     * @param[in] object  Param
     */
    FWDATA_API void setValue(const ParamNameType& name, ::fwData::Object::sptr object, ProcessObjectMapType& params);

    /**
     * @brief Retrieves data associated with specified name in params map (null if non exist).
     * @param[in] name Param name
     * @return null sptr if param is not found
     */
    FWDATA_API ::fwData::Object::sptr getValue(const ParamNameType& name, const ProcessObjectMapType& params);

    /**
     * @brief Unregister all parameters in params map.
     */
    FWDATA_API void clearParams(ProcessObjectMapType& params);

    /// Inputs values map
    ProcessObjectMapType m_attrInputs;

    /// Outputs values map
    ProcessObjectMapType m_attrOutputs;
};

} // namespace fwData


#endif //_FWDATA_PROCESSOBJECT_HPP_
