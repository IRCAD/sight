/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_RECONSTRUCTION_HPP_
#define _FWDATA_RECONSTRUCTION_HPP_

#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/TriangularMesh.hpp"
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
 * @see ::fwData::Image, ::fwData::TriangularMesh, ::fwData::Material
 */

class FWDATA_CLASS_API Reconstruction : public Object
{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (Reconstruction)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Reconstruction >) ;

	/// Constructor
	FWDATA_API Reconstruction();
	/// Destructor
	FWDATA_API virtual ~Reconstruction();

	/// Clone method
	FWDATA_API Reconstruction::sptr clone() const;

	/// Copy method
	FWDATA_API Reconstruction &operator=( const Reconstruction & _reconstruction ) ;

	// Image -------------------------------------------------------------------
	/// @name Image accessor
	/// Field identifier for image
	static const Object::FieldID ID_IMAGE;

	/**
	 * @brief Set the image associated with the mesh
	 */
	FWDATA_API void setImage( ::fwData::Image::sptr _pImage );

	/**
	 * @{
	 * @brief Get the image associated with the acquisition
	 */
	FWDATA_API ::fwData::Image::csptr getImage() const;
	FWDATA_API ::fwData::Image::sptr getImage();
	//@}

	// Mesh -------------------------------------------------------------------
	/// @name Mesh accessor
	/// Field identifier for mesh
	static const Object::FieldID ID_MESH;
	//@{
	/// Get/Set triangular mesh
	FWDATA_API void setTriangularMesh( ::fwData::TriangularMesh::sptr _pTriangularMesh );
	FWDATA_API ::fwData::TriangularMesh::csptr getTriangularMesh() const;
	FWDATA_API ::fwData::TriangularMesh::sptr getTriangularMesh();
	//@}

	// Material -------------------------------------------------------------------
	/// @name Material accessor
	//@{
	/// Get/Set material
	FWDATA_API void setMaterial( ::fwData::Material::sptr _pMaterial );
	FWDATA_API ::fwData::Material::csptr getMaterial() const;
	FWDATA_API ::fwData::Material::sptr getMaterial();
	//@}

	// Generator result---------------------------------------------------------
	fwGettersSettersDocMacro(IsVisible, bIsVisible, bool, the visibility of the reconstruction (true if visible));

	fwGettersSettersDocMacro(ReconstructionFormat, sReconstructionFormat, std::string, the reconstruction format. (eg : TRIAN, TRIAN_GPG));

	fwGettersSettersDocMacro(OrganName, sOrganName, std::string, organ name);

	fwGettersSettersDocMacro(StructureType, sStructureType, std::string, structure type);

	fwGettersSettersDocMacro(IsClosed, bIsClosed, bool, if the reconstruction is closed);

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

	fwGettersSettersDocMacro(Path, fsPath, boost::filesystem::path, the reconstruction path);

	fwGettersSettersDocMacro(DbID, i32DbID, boost::int32_t, the database indentifier);

protected :


	//! true if this reconstruction is visible
	bool m_bIsVisible;

//	//! Color of the reconstructed organ. eg 1.0 0.52 0.3 0.0
//	double m_dRed;
//	double m_dGreen;
//	double m_dBlue;
//	double m_dAlpha;

	//! Material of reonstruction
	::fwData::Material::sptr m_pMaterial;

	//! Reconstruction format. eg : TRIAN, TRIAN_GPG
	std::string m_sReconstructionFormat;

	//! Organ name
	std::string m_sOrganName;

	//! Structure type
	std::string	m_sStructureType;

	//! true if the reconstruction is closed
	bool m_bIsClosed;

	//! true if the reconstruction is build automatically
	bool m_bIsAutomatic;

	//!the average volume
	double m_dAvgVolume;

	//! the volume's standard deviation
	double m_dVolStdDeviation;

	//! the volume's percent confidence
	double m_dVolPctConfidence;

	//! the reconstruction time
	std::string	m_sReconstructionTime;

	//! true if the mask is generated
	bool m_bMaskGenerated;

	//! The level ( is the organ skin, internal organ, a vessel or a pathology ? )
	boost::uint8_t  m_ui8Level;

	//! The label
	boost::uint16_t  m_ui16Label;

	//! MT;
	//bool m_bMT;

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
	boost::filesystem::path m_fsPath;

	// ::fwData::TriangularMesh::sptr m_pTriangularMesh;



	//! Database indentifier
	boost::int32_t  m_i32DbID;

	//! dictionary organ
	// ::boost::shared_ptr< ::fwData::DictionaryOrgan > m_pDictionaryOrgan;

	//! image (mask) corresponding to the organ
	// ::boost::shared_ptr< ::fwData::Image> m_pImage;

	//@}




};

} // namespace fwData

#endif // _FWDATA_RECONSTRUCTION_HPP_
