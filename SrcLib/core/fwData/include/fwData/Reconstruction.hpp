/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_RECONSTRUCTION_HPP__
#define __FWDATA_RECONSTRUCTION_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

#include "fwData/config.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (Reconstruction)(::fwData::Object), (()), ::fwData::factory::New< Reconstruction >) ;

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
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Constant to inform that mask volume has not been computed yet.
    FWDATA_API static const double s_NO_COMPUTED_MASK_VOLUME;

    // Generator result---------------------------------------------------------
    fwGettersSettersDocMacro(IsVisible, bIsVisible, bool, the visibility of the reconstruction (true if visible));

    fwGettersSettersDocMacro(OrganName, sOrganName, std::string, organ name);

    fwGettersSettersDocMacro(StructureType, sStructureType, std::string, structure type);

    /**
     * @brief Get/Set the image associated with the acquisition
     */
    fwDataGetSetSptrMacro(Image, SPTR(::fwData::Image));

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    fwDataGetSetSptrMacro(Mesh, SPTR(::fwData::Mesh));

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    fwDataGetSetSptrMacro(Material, SPTR(::fwData::Material));

    /**
     * @brief Get/Set the volume from mask
     */
    fwDataGetSetMacro(ComputedMaskVolume, double);

protected :

    //! true if this reconstruction is visible
    bool m_bIsVisible;

    //! Organ name
    std::string m_sOrganName;

    //! Structure type
    std::string m_sStructureType;

    //--------------------------------------------------------------------------


    //! Reconstruction's material
    SPTR(::fwData::Material) m_attrMaterial;

    //! Reconstruction's mask
    SPTR(::fwData::Image) m_attrImage;

    //! Reconstruction's mesh
    SPTR(::fwData::Mesh) m_attrMesh;

    //! Reconstruction's mask volume
    double m_attrComputedMaskVolume;

};

} // namespace fwData

#endif // __FWDATA_RECONSTRUCTION_HPP__

