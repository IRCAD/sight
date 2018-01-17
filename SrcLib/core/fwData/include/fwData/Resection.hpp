/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/PlaneList.hpp"
#include "fwData/Reconstruction.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Resection), FWDATA_API);
namespace fwData
{
/**
 * @brief This class defines a resection.
 */
class FWDATA_CLASS_API Resection : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Resection)(::fwData::Object),
                                            (()), ::fwData::factory::New< Resection >);

    fwCampMakeFriendDataMacro((fwData)(Resection));

    typedef std::vector< ::fwData::Reconstruction::sptr > ResectionInputs;
    typedef std::vector< ::fwData::Reconstruction::sptr > ResectionOutputs;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Resection(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Resection();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @{
     * @brief Get/Set value of the planeList.
     */
    ::fwData::PlaneList::sptr& getPlaneList();
    const ::fwData::PlaneList::sptr& getPlaneList() const;
    void setPlaneList (const ::fwData::PlaneList::sptr& _planeList);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the inputs.
     */
    ResectionInputs& getInputs ();
    const ResectionInputs& getInputs () const;
    void setInputs (const ResectionInputs& _vInputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the outputs.
     */
    ResectionOutputs& getOutputs ();
    const ResectionOutputs& getOutputs () const;
    void setOutputs (const ResectionOutputs& _vOutputs);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsSafePart.
     */
    bool  getIsSafePart () const;
    void setIsSafePart (const bool _isSafePart);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the name.
     */
    std::string& getName ();
    const std::string& getName () const;
    void setName (const std::string& _name);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsVisible.
     */
    bool  getIsVisible () const;
    void setIsVisible (const bool _isVisible);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the IsValid.
     */
    bool  getIsValid () const;
    void setIsValid (const bool _isValid);

    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when a reconstruction is added
    typedef ::fwCom::Signal< void () > ReconstructionAddedSignalType;

    /// Key in m_signals map of signal m_sigReconstructionAdded
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_RECONSTRUCTION_ADDED_SIG;

    /// Type of signal when the resection visibility is modified
    typedef ::fwCom::Signal< void () > VisibilityModifiedSignalType;

    /// Key in m_signals map of signal m_sigVisibilityModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;

    /**
     * @}
     */

protected:

    //! Resection name
    std::string m_name;

    //! Planes list
    ::fwData::PlaneList::sptr m_planeList;

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

inline ::fwData::PlaneList::sptr& Resection::getPlaneList()
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline const ::fwData::PlaneList::sptr& Resection::getPlaneList() const
{
    return m_planeList;
}

//-----------------------------------------------------------------------------

inline void Resection::setPlaneList (const ::fwData::PlaneList::sptr& _planeList)
{
    m_planeList = _planeList;
}

//-----------------------------------------------------------------------------

inline Resection::ResectionInputs& Resection::getInputs ()
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline const Resection::ResectionInputs& Resection::getInputs () const
{
    return m_vInputs;
}

//-----------------------------------------------------------------------------

inline void Resection::setInputs (const Resection::ResectionInputs& _vInputs)
{
    m_vInputs = _vInputs;
}

//-----------------------------------------------------------------------------

inline Resection::ResectionOutputs& Resection::getOutputs ()
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline const Resection::ResectionOutputs& Resection::getOutputs () const
{
    return m_vOutputs;
}

//-----------------------------------------------------------------------------

inline void Resection::setOutputs (const Resection::ResectionOutputs& _vOutputs)
{
    m_vOutputs = _vOutputs;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsSafePart () const
{
    return m_isSafePart;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsSafePart (const bool _isSafePart)
{
    m_isSafePart = _isSafePart;
}

//-----------------------------------------------------------------------------

inline std::string& Resection::getName ()
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline const std::string& Resection::getName () const
{
    return m_name;
}

//-----------------------------------------------------------------------------

inline void Resection::setName (const std::string& _name)
{
    m_name = _name;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsVisible () const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsVisible (const bool _isVisible)
{
    m_isVisible = _isVisible;
}

//-----------------------------------------------------------------------------

inline bool Resection::getIsValid () const
{
    return m_isValid;
}

//-----------------------------------------------------------------------------

inline void Resection::setIsValid (const bool _isValid)
{
    m_isValid = _isValid;
}

//-----------------------------------------------------------------------------

} // end namespace fwData
