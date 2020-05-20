/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

fwCampAutoDeclareDataMacro((fwData)(Reconstruction), FWDATA_API);

namespace fwData
{

class Image;
class Material;
class Mesh;

/**
 * @brief This class defines a reconstruction object.
 *
 * A reconstruction is represented by a mesh, a material and an image (mask).
 */
class FWDATA_CLASS_API Reconstruction : public Object
{
public:
    fwCoreClassMacro(Reconstruction, ::fwData::Object, ::fwData::factory::New< Reconstruction >);

    fwCoreAllowSharedFromThis()

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Reconstruction(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Reconstruction();

    fwCampMakeFriendDataMacro((fwData)(Reconstruction));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Constant to inform that mask volume has not been computed yet.
    FWDATA_API static const double s_NO_COMPUTED_MASK_VOLUME;

    /**
     * @{
     * @brief Get/Set value of the bIsVisible.
     */
    bool  getIsVisible () const;
    void setIsVisible (const bool _bIsVisible);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the organName.
     */
    std::string& getOrganName ();
    const std::string& getOrganName () const;
    void setOrganName (const std::string& _sOrganName);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the structureType.
     */
    std::string& getStructureType ();
    const std::string& getStructureType () const;
    void setStructureType (const std::string& _sStructureType);
    /// @}

    /**
     * @brief Get/Set the image associated with the acquisition
     */
    SPTR(::fwData::Image)  getImage() const;
    void setImage (const SPTR(::fwData::Image)& val);

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    SPTR(::fwData::Mesh) getMesh() const;
    void setMesh(const SPTR(::fwData::Mesh)& val);
    /// @}

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    SPTR(::fwData::Material) getMaterial() const;
    void setMaterial (const SPTR(::fwData::Material)& val);
    /// @}

    /**
     * @brief Get/Set the volume from mask
     */
    double  getComputedMaskVolume () const;
    void setComputedMaskVolume (double val);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the mesh pointer has changed, mesh parameter is used to store old mesh
    typedef ::fwCom::Signal< void (SPTR( ::fwData::Mesh ) ) > MeshChangedSignalType;

    /// Key in m_signals map of signal m_sigMeshModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_MESH_CHANGED_SIG;

    /// Type of signal when mesh is modified
    typedef ::fwCom::Signal< void (bool) > VisibilityModifiedSignalType;

    /// Key in m_signals map of signal m_sigVisibilityModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;
    /**
     * @}
     */

protected:

    //! true if this reconstruction is visible
    bool m_bIsVisible;

    //! Organ name
    std::string m_sOrganName;

    //! Structure type
    std::string m_sStructureType;

    //--------------------------------------------------------------------------

    //! Reconstruction's material
    SPTR(::fwData::Material) m_material;

    //! Reconstruction's mask
    SPTR(::fwData::Image) m_image;

    //! Reconstruction's mesh
    SPTR(::fwData::Mesh) m_mesh;

    //! Reconstruction's mask volume
    double m_computedMaskVolume;

    /**
     * @name Signals
     * @{
     */

    /// Signal emitted when mesh changed, mesh parameter is used to store old mesh
    MeshChangedSignalType::sptr m_sigMeshChanged;

    /// Signal emitted  when visibility is modified
    VisibilityModifiedSignalType::sptr m_sigVisibilityModified;
    /**
     * @}
     */

};

//-----------------------------------------------------------------------------

inline bool Reconstruction::getIsVisible () const
{
    return m_bIsVisible;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setIsVisible (const bool _bIsVisible)
{
    m_bIsVisible = _bIsVisible;
}

//-----------------------------------------------------------------------------

inline std::string& Reconstruction::getOrganName ()
{
    return m_sOrganName;
}

//-----------------------------------------------------------------------------

inline const std::string& Reconstruction::getOrganName () const
{
    return m_sOrganName;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setOrganName (const std::string& _sOrganName)
{
    m_sOrganName = _sOrganName;
}

//-----------------------------------------------------------------------------

inline std::string& Reconstruction::getStructureType ()
{
    return this->m_sStructureType;
}

//-----------------------------------------------------------------------------

inline const std::string& Reconstruction::getStructureType () const
{
    return this->m_sStructureType;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setStructureType (const std::string& _sStructureType)
{
    this->m_sStructureType = _sStructureType;
}

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Image)  Reconstruction::getImage() const
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setImage (const SPTR(::fwData::Image)& val)
{
    m_image = val;
}

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Mesh) Reconstruction::getMesh() const
{
    return m_mesh;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setMesh(const SPTR(::fwData::Mesh)& val)
{
    m_mesh = val;
}

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Material) Reconstruction::getMaterial() const
{
    return m_material;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setMaterial (const SPTR(::fwData::Material)& val)
{
    m_material = val;
}

//-----------------------------------------------------------------------------

inline double Reconstruction::getComputedMaskVolume () const
{
    return m_computedMaskVolume;
}

//-----------------------------------------------------------------------------

inline void Reconstruction::setComputedMaskVolume (double val)
{
    m_computedMaskVolume = val;
}

//-----------------------------------------------------------------------------

} // namespace fwData
