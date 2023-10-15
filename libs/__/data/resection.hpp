/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "data/plane_list.hpp"
#include "data/reconstruction.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief This class defines a resection.
 */
class DATA_CLASS_API resection final : public object
{
public:

    SIGHT_DECLARE_CLASS(resection, object);

    typedef std::vector<reconstruction::sptr> ResectionInputs;
    typedef std::vector<reconstruction::sptr> ResectionOutputs;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API resection();

    /// Destructor
    DATA_API ~resection() noexcept override = default;

    /**
     * @{
     * @brief Get/Set value of the planeList.
     */
    plane_list::sptr& getPlaneList();
    const plane_list::sptr& getPlaneList() const;
    void setPlaneList(const plane_list::sptr& _planeList);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the inputs.
     */
    ResectionInputs& getInputs();
    const ResectionInputs& getInputs() const;
    void SetInputs(const ResectionInputs& _vInputs);
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
    void setIsSafePart(bool _isSafePart);
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
    void setIsVisible(bool _isVisible);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsValid.
     */
    bool getIsValid() const;
    void setIsValid(bool _isValid);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when a reconstruction is added
    typedef core::com::signal<void ()> ReconstructionAddedSignalType;

    /// Key in m_signals map of signal m_sigReconstructionAdded
    DATA_API static const core::com::signals::key_t RECONSTRUCTION_ADDED_SIG;

    /// Type of signal when the resection visibility is modified
    typedef core::com::signal<void ()> VisibilityModifiedSignalType;

    /// Key in m_signals map of signal m_sigVisibilityModified
    DATA_API static const core::com::signals::key_t VISIBILITY_MODIFIED_SIG;
    /**
     * @}
     */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const resection& other) const noexcept;
    DATA_API bool operator!=(const resection& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! resection name
    std::string m_name;

    //! Planes list
    plane_list::sptr m_planeList;

    //! Inputs (reconstructions)
    ResectionInputs m_vInputs;

    //! Outputs (reconstructions)
    ResectionOutputs m_vOutputs;

    //! flag if the part is safe
    bool m_isSafePart {true};

    //! flag if the resection is valid
    bool m_isValid {false};

    //! flag if the resection is visible
    bool m_isVisible {true};

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
}; // end class resection

//-----------------------------------------------------------------------------

inline plane_list::sptr& resection::getPlaneList()
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline const plane_list::sptr& resection::getPlaneList() const
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline void resection::setPlaneList(const plane_list::sptr& _planeList)
{
    m_planeList = _planeList;
}

//-----------------------------------------------------------------------------

inline resection::ResectionInputs& resection::getInputs()
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline const resection::ResectionInputs& resection::getInputs() const
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline void resection::SetInputs(const resection::ResectionInputs& _vInputs)
{
    m_vInputs = _vInputs;
}

//-----------------------------------------------------------------------------

inline resection::ResectionOutputs& resection::getOutputs()
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline const resection::ResectionOutputs& resection::getOutputs() const
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline void resection::setOutputs(const resection::ResectionOutputs& _vOutputs)
{
    m_vOutputs = _vOutputs;
}

//-----------------------------------------------------------------------------

inline bool resection::getIsSafePart() const
{
    return m_isSafePart;
}

//-----------------------------------------------------------------------------

inline void resection::setIsSafePart(const bool _isSafePart)
{
    m_isSafePart = _isSafePart;
}

//-----------------------------------------------------------------------------

inline std::string& resection::getName()
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline const std::string& resection::getName() const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void resection::setName(const std::string& _name)
{
    m_name = _name;
}

//-----------------------------------------------------------------------------

inline bool resection::getIsVisible() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

inline void resection::setIsVisible(const bool _isVisible)
{
    m_isVisible = _isVisible;
}

//-----------------------------------------------------------------------------

inline bool resection::getIsValid() const
{
    return m_isValid;
}

//-----------------------------------------------------------------------------

inline void resection::setIsValid(const bool _isValid)
{
    m_isValid = _isValid;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
