/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/PlaneList.hpp"
#include "data/Reconstruction.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <vector>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Resection));
namespace sight::data
{

/**
 * @brief This class defines a resection.
 */
class DATA_CLASS_API Resection : public data::Object
{
public:

    SIGHT_DECLARE_CLASS(Resection, data::Object, data::factory::New<Resection>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Resection));

    typedef std::vector<data::Reconstruction::sptr> ResectionInputs;
    typedef std::vector<data::Reconstruction::sptr> ResectionOutputs;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Resection(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~Resection();

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @{
     * @brief Get/Set value of the planeList.
     */
    data::PlaneList::sptr& getPlaneList();
    const data::PlaneList::sptr& getPlaneList() const;
    void setPlaneList(const data::PlaneList::sptr& _planeList);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the inputs.
     */
    ResectionInputs& getInputs();
    const ResectionInputs& getInputs() const;
    void setInputs(const ResectionInputs& _vInputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the outputs.
     */
    ResectionOutputs& getOutputs();
    const ResectionOutputs& getOutputs() const;
    void setOutputs(const ResectionOutputs& _vOutputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsSafePart.
     */
    bool getIsSafePart() const;
    void setIsSafePart(const bool _isSafePart);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the name.
     */
    std::string& getName();
    const std::string& getName() const;
    void setName(const std::string& _name);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsVisible.
     */
    bool getIsVisible() const;
    void setIsVisible(const bool _isVisible);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsValid.
     */
    bool getIsValid() const;
    void setIsValid(const bool _isValid);

    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when a reconstruction is added
    typedef core::com::Signal<void ()> ReconstructionAddedSignalType;

    /// Key in m_signals map of signal m_sigReconstructionAdded
    DATA_API static const core::com::Signals::SignalKeyType s_RECONSTRUCTION_ADDED_SIG;

    /// Type of signal when the resection visibility is modified
    typedef core::com::Signal<void ()> VisibilityModifiedSignalType;

    /// Key in m_signals map of signal m_sigVisibilityModified
    DATA_API static const core::com::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;

/**
 * @}
 */

protected:

    //! Resection name
    std::string m_name;

    //! Planes list
    data::PlaneList::sptr m_planeList;

    //! Inputs (reconstructions)
    ResectionInputs m_vInputs;

    //! Outputs (reconstructions)
    ResectionOutputs m_vOutputs;

    //! flag if the part is safe
    bool m_isSafePart;

    //! flag if the resection is valid
    bool m_isValid;

    //! flag if the resection is visible
    bool m_isVisible;

private:

    /**
     * @name Signals attributes
     * @{
     */
    /// Signal emitted when a reconstruction is added
    ReconstructionAddedSignalType::sptr m_sigReconstructionAdded;

    /// Signal emitted when the resection visibility is modified
    VisibilityModifiedSignalType::sptr m_sigVisibilityModified;
    /**
     * @}
     */
}; // end class Resection

//-----------------------------------------------------------------------------

inline data::PlaneList::sptr& Resection::getPlaneList()
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline const data::PlaneList::sptr& Resection::getPlaneList() const
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline void Resection::setPlaneList(const data::PlaneList::sptr& _planeList)
{
    m_planeList = _planeList;
}

//-----------------------------------------------------------------------------

inline Resection::ResectionInputs& Resection::getInputs()
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline const Resection::ResectionInputs& Resection::getInputs() const
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline void Resection::setInputs(const Resection::ResectionInputs& _vInputs)
{
    m_vInputs = _vInputs;
}

//-----------------------------------------------------------------------------

inline Resection::ResectionOutputs& Resection::getOutputs()
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline const Resection::ResectionOutputs& Resection::getOutputs() const
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline void Resection::setOutputs(const Resection::ResectionOutputs& _vOutputs)
{
    m_vOutputs = _vOutputs;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsSafePart() const
{
    return m_isSafePart;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsSafePart(const bool _isSafePart)
{
    m_isSafePart = _isSafePart;
}

//-----------------------------------------------------------------------------

inline std::string& Resection::getName()
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline const std::string& Resection::getName() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void Resection::setName(const std::string& _name)
{
    m_name = _name;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsVisible() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsVisible(const bool _isVisible)
{
    m_isVisible = _isVisible;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsValid() const
{
    return m_isValid;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsValid(const bool _isValid)
{
    m_isValid = _isValid;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
