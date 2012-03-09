/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RECONSTRUCTION_HPP_
#define _FWDATA_RECONSTRUCTION_HPP_

#include <vector>

#include <boost/logic/tribool.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Material.hpp"
#include "fwData/Image.hpp"

namespace fwData
{
/**
 * @class Reconstruction
 * @brief This class defines a reconstruction object
 *
 * A reconstruction is represented by a triangular mesh, a material and an image.
 *
 * @see ::fwData::Image, ::fwData::Mesh, ::fwData::Material
 */

class FWDATA_CLASS_API Reconstruction : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Reconstruction)(::fwData::Object), (()), ::fwData::Factory::New< Reconstruction >) ;

    fwCoreAllowSharedFromThis()
    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Reconstruction::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Reconstruction::csptr _source );


    // Generator result---------------------------------------------------------
    fwGettersSettersDocMacro(IsVisible, bIsVisible, bool, the visibility of the reconstruction (true if visible));

    fwGettersSettersDocMacro(ReconstructionFormat, sReconstructionFormat, std::string, the reconstruction format. (eg : TRIAN, TRIAN_GPG));

    fwGettersSettersDocMacro(OrganName, sOrganName, std::string, organ name);

    fwGettersSettersDocMacro(StructureType, sStructureType, std::string, structure type);

    fwGettersSettersDocMacro(IsAutomatic, bIsAutomatic, bool, if the reconstruction is build automatically);

    fwGettersSettersDocMacro(AvgVolume, dAvgVolume, double, the average volume);

    fwGettersSettersDocMacro(VolStdDeviation, dVolStdDeviation, double, the volume standard deviation);

    fwGettersSettersDocMacro(VolPctConfidence, dVolPctConfidence, double, the volume percent confidence);

    fwGettersSettersDocMacro(ReconstructionTime, sReconstructionTime, std::string, the reconstruction time);

    fwGettersSettersDocMacro(MaskGenerated, bMaskGenerated, bool, if the mask is generated);

    fwGettersSettersDocMacro(Level, ui8Level, ::boost::uint8_t, the level ( is the organ skin, internal organ, a vessel or a pathology ? ));

    fwGettersSettersDocMacro(Label, ui16Label, ::boost::uint16_t, the label);

    fwGettersSettersDocMacro(Generated3D, bGenerated3D, bool, if the 3D reconstruction is generated);

    fwGettersSettersDocMacro(Type3D, sType3D, std::string, the 3D model type);

    fwGettersSettersDocMacro(Path, fsPath, ::boost::filesystem::path, the reconstruction path);

    fwGettersSettersDocMacro(DbID, i32DbID, ::boost::int32_t, the database indentifier);

    /*!
     * @brief Return true if the reconstruction is closed.
     * The result is computed if m_bIsClosed is undefined
     * This method will be removed, avoid using it.
     */
    FWDATA_API bool getIsClosed();

    /*!
     * @brief Return true if the reconstruction is closed.
     * The result is computed for each call.
     */
    static FWDATA_API bool isClosed(Reconstruction::csptr);

    //! Get/Set if the reconstruction is closed
    FWDATA_API ::boost::logic::tribool& getRefIsClosed();
    FWDATA_API const ::boost::logic::tribool& getCRefIsClosed() const;
    FWDATA_API void setIsClosed(::boost::logic::tribool isClosed);


    /**
     * @brief Get/Set the image associated with the acquisition
     */
    fwDataGetSetSptrMacro(Image, ::fwData::Image::sptr);

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    fwDataGetSetSptrMacro(Mesh, ::fwData::Mesh::sptr);

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    fwDataGetSetSptrMacro(Material, ::fwData::Material::sptr);

protected :

    /// Constructor
    FWDATA_API Reconstruction();
    /// Destructor
    FWDATA_API virtual ~Reconstruction();

    //! true if this reconstruction is visible
    bool m_bIsVisible;


    //! Reconstruction format. eg : TRIAN, TRIAN_GPG
    std::string m_sReconstructionFormat;

    //! Organ name
    std::string m_sOrganName;

    //! Structure type
    std::string m_sStructureType;

    //! true if the reconstruction is closed
    ::boost::logic::tribool m_bIsClosed;

    //! true if the reconstruction is build automatically
    bool m_bIsAutomatic;

    //!the average volume
    double m_dAvgVolume;

    //! the volume's standard deviation
    double m_dVolStdDeviation;

    //! the volume's percent confidence
    double m_dVolPctConfidence;

    //! the reconstruction time
    std::string m_sReconstructionTime;

    //! true if the mask is generated
    bool m_bMaskGenerated;

    //! The level ( is the organ skin, internal organ, a vessel or a pathology ? )
    ::boost::uint8_t  m_ui8Level;

    //! The label
    ::boost::uint16_t  m_ui16Label;

    //! true if the 3D reconstruction is generated
    bool m_bGenerated3D;

    //! The 3D model type
    std::string m_sType3D;


    //--------------------------------------------------------------------------

    /**
     *  Fields used in database
     */
    //@{

    //! Reconstruction path
    ::boost::filesystem::path m_fsPath;

    //! Database indentifier
    ::boost::int32_t  m_i32DbID;

    //@}


    //! Material of reconstruction
    ::fwData::Material::sptr m_attrMaterial;

    ::fwData::Image::sptr m_attrImage;
    ::fwData::Mesh::sptr m_attrMesh;

    ::fwData::Mesh::wptr m_meshForWhichISCompoutedIsClosed;
};

} // namespace fwData

#endif // _FWDATA_RECONSTRUCTION_HPP_
