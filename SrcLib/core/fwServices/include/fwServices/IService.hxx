/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwServices/IService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IService::getObject()
{
    FW_DEPRECATED("getObject()", "getInput() or getInOut()");

    SPTR(DATATYPE) castData = std::dynamic_pointer_cast<DATATYPE>( m_associatedObject.lock() );
    OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", castData);

    return castData;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
CSPTR(DATATYPE) IService::getInput(const KeyType& key) const
{
    CSPTR(DATATYPE) input;

    auto iterator = m_inputsMap.find(key);
    if(iterator != m_inputsMap.end())
    {
        input = std::dynamic_pointer_cast<const DATATYPE>( iterator->second.lock() );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", input);
    }

    return input;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IService::getInOut(const KeyType& key) const
{
    SPTR(DATATYPE) inout;
    auto iterator = m_inOutsMap.find(key);
    if(iterator != m_inOutsMap.end())
    {
        inout = std::dynamic_pointer_cast<DATATYPE>( iterator->second.lock() );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", inout);
    }

    return inout;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IService::getOutput(const KeyType& key) const
{
    SPTR(DATATYPE) output;
    auto iterator = m_outputsMap.find(key);
    if(iterator != m_outputsMap.end())
    {
        output = std::dynamic_pointer_cast<DATATYPE>( iterator->second );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", output);
    }

    return output;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
CSPTR(DATATYPE) IService::getInput(const KeyType& keybase, size_t index) const
{
# ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getInput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IService::getInOut(const KeyType& keybase, size_t index) const
{
# ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getInOut< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}
//------------------------------------------------------------------------------

template< class DATATYPE >
SPTR(DATATYPE) IService::getOutput(const KeyType& keybase, size_t index) const
{
# ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getOutput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

inline size_t IService::getKeyGroupSize(const IService::KeyType& keybase) const
{
    auto it = m_keyGroupSize.find(keybase);
    if(it != m_keyGroupSize.end())
    {
        return it->second;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------

inline const IService::InputMapType& IService::getInputs() const
{
    return m_inputsMap;
}

//------------------------------------------------------------------------------

inline const IService::InOutMapType& IService::getInOuts() const
{
    return m_inOutsMap;
}

//------------------------------------------------------------------------------

inline const IService::OutputMapType& IService::getOutputs() const
{
    return m_outputsMap;
}

//------------------------------------------------------------------------------

inline std::vector< ::fwData::Object::csptr > IService::getObjects() const
{
    std::vector< ::fwData::Object::csptr > objectsVector;
    if(!m_inputsMap.empty() || !m_inOutsMap.empty() ||  !m_outputsMap.empty())
    {
        for(auto itObj : m_inputsMap)
        {
            objectsVector.push_back(itObj.second.lock());
        }
        for(auto itObj : m_inOutsMap)
        {
            objectsVector.push_back(itObj.second.lock());
        }
        for(auto itObj : m_outputsMap)
        {
            objectsVector.push_back(itObj.second);
        }
    }
    else
    {
        objectsVector.push_back(m_associatedObject.lock());
    }

    return objectsVector;
}

//------------------------------------------------------------------------------

} // namespace fwServices
