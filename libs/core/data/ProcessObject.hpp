/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "data/factory/new.hpp"
#include "data/Object.hpp"

#include <map>
#include <vector>

namespace sight::data
{

class Field;

}

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (ProcessObject));

namespace sight::data
{

/**
 * @brief Provides the notion of Process Object: object having inputs and outputs.
 */
class DATA_CLASS_API ProcessObject : public Object
{
public:

    SIGHT_DECLARE_CLASS(ProcessObject, Object, factory::New<ProcessObject>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (ProcessObject));

    typedef std::string ParamNameType;
    typedef std::vector<std::string> ParamNameVectorType;
    typedef std::map<ParamNameType, Object::sptr> ProcessObjectMapType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ProcessObject(Object::Key key);

    /**
     * @brief   Destructor
     */
    DATA_API virtual ~ProcessObject();

    /**
     * @brief Retrieves the input data associated with specified name (null if non exist).
     * @param[in] name Input name
     * @return null sptr if input is not found
     */
    DATA_API Object::sptr getInput(const ParamNameType& name);
    DATA_API Object::csptr getInput(const ParamNameType& name) const;

    /**
     * @brief Retrieves the input data associated with specified name type OBJECTTYPE (null if non exist).
     * @param[in] name Input name
     * @return null sptr if input is not found
     */
    template<class OBJECTTYPE>
    typename OBJECTTYPE::sptr getInput(const ParamNameType& name);

    template<class OBJECTTYPE>
    typename OBJECTTYPE::csptr getInput(const ParamNameType& name) const;

    /**
     * @brief Retrieves the output data associated with specified name (null if non exist).
     * @param[in] name Output name
     * @return null sptr if output is not found
     */
    DATA_API Object::sptr getOutput(const ParamNameType& name);
    DATA_API Object::csptr getOutput(const ParamNameType& name) const;

    /**
     * @brief Retrieves the output data associated with specified name type OBJECTTYPE (null if non exist).
     * @param[in] name Output name
     * @return null sptr if output is not found
     */
    template<class OBJECTTYPE>
    typename OBJECTTYPE::sptr getOutput(const ParamNameType& name);

    template<class OBJECTTYPE>
    typename OBJECTTYPE::csptr getOutput(const ParamNameType& name) const;

    /**
     * @{
     * @brief Retrieve the input data.
     */
    const ProcessObjectMapType& getInputs() const;

    void setInputs(const ProcessObjectMapType& val);
    /// @}

    /**
     * @{
     * @briefRetrieve the output data.
     */
    const ProcessObjectMapType& getOutputs() const;

    void setOutputs(const ProcessObjectMapType& val);
    /// @}

    /**
     * @brief Register input value with specified name.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Input name
     * @param[in] object  Input value
     */
    DATA_API void setInputValue(const ParamNameType& name, Object::sptr object);

    /**
     * @brief Register output value with specified name.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Output name
     * @param[in] object  Output value
     */
    DATA_API void setOutputValue(const ParamNameType& name, Object::sptr object);

    /**
     * @brief Returns vector of input parameters names.
     */
    DATA_API ParamNameVectorType getInputsParamNames() const;

    /**
     * @brief Returns vector of output parameters names.
     */
    DATA_API ParamNameVectorType getOutputsParamNames() const;

    /**
     * @brief Unregister all inputs parameters.
     */
    DATA_API void clearInputs();

    /**
     * @brief Unregister all output parameters.
     */
    DATA_API void clearOutputs();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ProcessObject& other) const noexcept;
    DATA_API bool operator!=(const ProcessObject& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) override;

    /**
     * @brief Returns vector of parameters names from params map.
     */
    DATA_API ParamNameVectorType getParamNames(const ProcessObjectMapType& params) const;

    /**
     * @brief Register value with specified name in params map.
     * If the name does already exist, the matching value will be replaced.
     * @param[in] name Param name
     * @param[in] object  Param
     * @param params parameters map to insert object
     */
    DATA_API void setValue(const ParamNameType& name, Object::sptr object, ProcessObjectMapType& params);

    /**
     * @brief Retrieves data associated with specified name in params map (null if non exist).
     * @param[in] name Param name
     * @param params parameters map containing the data
     * @return null sptr if param is not found
     */
    DATA_API Object::sptr getValue(const ParamNameType& name, const ProcessObjectMapType& params);
    DATA_API Object::csptr getValue(const ParamNameType& name, const ProcessObjectMapType& params) const;

    /**
     * @brief Unregister all parameters in params map.
     */
    DATA_API void clearParams(ProcessObjectMapType& params);

    /// Inputs values map
    ProcessObjectMapType m_inputs;

    /// Outputs values map
    ProcessObjectMapType m_outputs;
};

//-----------------------------------------------------------------------------

template<class OBJECTTYPE>
inline typename OBJECTTYPE::sptr ProcessObject::getInput(const ProcessObject::ParamNameType& name)
{
    return OBJECTTYPE::dynamicCast(this->getInput(name));
}

//------------------------------------------------------------------------------

template<class OBJECTTYPE>
inline typename OBJECTTYPE::csptr ProcessObject::getInput(const ProcessObject::ParamNameType& name) const
{
    return OBJECTTYPE::dynamicCast(this->getInput(name));
}

//-----------------------------------------------------------------------------

template<class OBJECTTYPE>
inline typename OBJECTTYPE::sptr ProcessObject::getOutput(const ProcessObject::ParamNameType& name)
{
    return OBJECTTYPE::dynamicCast(this->getOutput(name));
}

//------------------------------------------------------------------------------

template<class OBJECTTYPE>
inline typename OBJECTTYPE::csptr ProcessObject::getOutput(const ProcessObject::ParamNameType& name) const
{
    return OBJECTTYPE::dynamicCast(this->getOutput(name));
}

//-----------------------------------------------------------------------------

inline const ProcessObject::ProcessObjectMapType& ProcessObject::getInputs() const
{
    return m_inputs;
}

//-----------------------------------------------------------------------------

inline void ProcessObject::setInputs(const ProcessObject::ProcessObjectMapType& val)
{
    m_inputs = val;
}

//-----------------------------------------------------------------------------

inline const ProcessObject::ProcessObjectMapType& ProcessObject::getOutputs() const
{
    return m_outputs;
}

//-----------------------------------------------------------------------------

inline void ProcessObject::setOutputs(const ProcessObject::ProcessObjectMapType& val)
{
    m_outputs = val;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
