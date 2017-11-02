/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_PROCESSOBJECT_HPP__
#define __FWDATA_PROCESSOBJECT_HPP__

#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <map>
#include <vector>

namespace fwData
{
class Field;
}

fwCampAutoDeclareDataMacro((fwData)(ProcessObject), FWDATA_API);

namespace fwData
{

/**
 * @brief Provides the notion of Process Object: object having inputs and outputs.
 */
class FWDATA_CLASS_API ProcessObject : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ProcessObject)(::fwData::Object), (()),
                                            ::fwData::factory::New< ProcessObject >);

    fwCampMakeFriendDataMacro((fwData)(ProcessObject));

    typedef std::string ParamNameType;
    typedef std::vector<std::string> ParamNameVectorType;
    typedef std::map< ParamNameType, ::fwData::Object::sptr > ProcessObjectMapType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ProcessObject(::fwData::Object::Key key);

    /**
     * @brief   Destructor
     */
    FWDATA_API virtual ~ProcessObject();

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
    typename OBJECTTYPE::sptr getInput(const ParamNameType& name);

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
    typename OBJECTTYPE::sptr getOutput(const ParamNameType& name);

    /**
     * @{
     * @brief Retrieve the input data.
     */
    const ProcessObjectMapType& getInputs () const;

    void setInputs (const ProcessObjectMapType& val);
    /// @}

    /**
     * @{
     * @briefRetrieve the output data.
     */
    const ProcessObjectMapType& getOutputs () const;

    void setOutputs (const ProcessObjectMapType& val);
    /// @}

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
    FWDATA_API void shallowCopy( const Object::csptr& source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) override;

protected:

    /**
     * @brief Returns vector of parameters names from params map.
     */
    FWDATA_API ParamNameVectorType getParamNames(const ProcessObjectMapType& params) const;

    /**
     * @brief Register value with specified name in params map.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Param name
     * @param[in] object  Param
     * @param params parameters map to insert object
     */
    FWDATA_API void setValue(const ParamNameType& name, ::fwData::Object::sptr object, ProcessObjectMapType& params);

    /**
     * @brief Retrieves data associated with specified name in params map (null if non exist).
     * @param[in] name Param name
     * @param params parameters map containing the data
     * @return null sptr if param is not found
     */
    FWDATA_API ::fwData::Object::sptr getValue(const ParamNameType& name, const ProcessObjectMapType& params);

    /**
     * @brief Unregister all parameters in params map.
     */
    FWDATA_API void clearParams(ProcessObjectMapType& params);

    /// Inputs values map
    ProcessObjectMapType m_inputs;

    /// Outputs values map
    ProcessObjectMapType m_outputs;
};

//-----------------------------------------------------------------------------

template< class OBJECTTYPE >
inline typename OBJECTTYPE::sptr ProcessObject::getInput(const ProcessObject::ParamNameType& name)
{
    return OBJECTTYPE::dynamicCast( this->getInput( name ) );
}

//-----------------------------------------------------------------------------

template< class OBJECTTYPE >
inline typename OBJECTTYPE::sptr ProcessObject::getOutput(const ProcessObject::ParamNameType& name)
{
    return OBJECTTYPE::dynamicCast( this->getOutput( name ) );
}

//-----------------------------------------------------------------------------

inline const ProcessObject::ProcessObjectMapType& ProcessObject::getInputs () const
{
    return m_inputs;
}

//-----------------------------------------------------------------------------

inline void ProcessObject::setInputs (const ProcessObject::ProcessObjectMapType& val)
{
    m_inputs = val;
}

//-----------------------------------------------------------------------------

inline const ProcessObject::ProcessObjectMapType& ProcessObject::getOutputs () const
{
    return m_outputs;
}

//-----------------------------------------------------------------------------

inline void ProcessObject::setOutputs (const ProcessObject::ProcessObjectMapType& val)
{
    m_outputs = val;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_PROCESSOBJECT_HPP__

