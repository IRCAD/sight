/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_MESH_HPP_
#define _FWDATA_MESH_HPP_

#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include "fwData/Array.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Mesh.hpp"

#include "fwData/config.hpp"

namespace fwData
{

/**
 * @class   Mesh
 * @brief   
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class FWDATA_CLASS_API Mesh : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Mesh)(::fwData::Object), (()), ::fwTools::Factory::New< Mesh >) ;

    fwDataObjectMacro();

    typedef std::map< std::string, ::fwData::Array::sptr > ArrayMapType;
    typedef boost::uint64_t Id;

    typedef enum {
        NONE = 0,
        POINT,
        EDGE,
        TRIANGLE,
        QUAD,
        POLY
    } CellTypesEnum;

    typedef float PointValueType;
    typedef float ColorValueType;
    typedef float NormalValueType;
    typedef Id    CellValueType;
    typedef char  CellTypes;

    typedef boost::multi_array_ref<PointValueType, 2>  PointMultiArrayType;
    typedef boost::multi_array_ref<CellTypes, 1>       CellTypesMultiArrayType;
    typedef boost::multi_array_ref<CellValueType, 1>   CellDataMultiArrayType;
    typedef boost::multi_array_ref<Id, 1>              CellDataOffsetsMultiArrayType;
    typedef boost::multi_array_ref<ColorValueType, 2>  PointColorsMultiArrayType;
    typedef boost::multi_array_ref<ColorValueType, 2>  CellColorsMultiArrayType;
    typedef boost::multi_array_ref<NormalValueType, 2> PointNormalsMultiArrayType;
    typedef boost::multi_array_ref<NormalValueType, 2> CellNormalsMultiArrayType;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Mesh::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Mesh::csptr _source );


    FWDATA_API size_t allocate(size_t nbPts, size_t nbCells, size_t nbCellsData = 0) throw(::fwData::Exception);

    FWDATA_API size_t allocatePointNormals() throw(::fwData::Exception);
    FWDATA_API size_t allocatePointColors() throw(::fwData::Exception);
    FWDATA_API size_t allocateCellNormals() throw(::fwData::Exception);
    FWDATA_API size_t allocateCellColors() throw(::fwData::Exception);

    FWDATA_API void stripAllocatedMemory() throw(::fwData::Exception);

    FWDATA_API Id insertNextPoint(const PointValueType p[3]) throw(::fwData::Exception);
    FWDATA_API Id insertNextPoint(PointValueType x, PointValueType y, PointValueType z) throw(::fwData::Exception);
    FWDATA_API void setPoint(Id id, PointValueType x, PointValueType y, PointValueType z);


    FWDATA_API Id insertNextCell(CellTypesEnum type, const CellValueType *cell, size_t nb) throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p) throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2) throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3) throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3, CellValueType p4) throw(::fwData::Exception);

    template <typename T> Id insertNextCell(CellTypesEnum type, const T &pointsBegin, const T &pointsEnd) throw(::fwData::Exception);

    FWDATA_API void cleanCells();


    FWDATA_API PointMultiArrayType           getPoints() const;
    FWDATA_API CellTypesMultiArrayType       getCellTypes() const;
    FWDATA_API CellDataMultiArrayType        getCellData() const;
    FWDATA_API CellDataOffsetsMultiArrayType getCellDataOffsets() const;
    FWDATA_API PointColorsMultiArrayType     getPointColors() const;
    FWDATA_API CellColorsMultiArrayType      getCellColors() const;
    FWDATA_API PointNormalsMultiArrayType    getPointNormals() const;
    FWDATA_API CellNormalsMultiArrayType     getCellNormals() const;

    FWDATA_API void setPointArray            (::fwData::Array::sptr array);
    FWDATA_API void setCellTypesArray        (::fwData::Array::sptr array);
    FWDATA_API void setCellDataArray         (::fwData::Array::sptr array);
    FWDATA_API void setCellDataOffsetsArray  (::fwData::Array::sptr array);
    FWDATA_API void setPointColorsArray      (::fwData::Array::sptr array);
    FWDATA_API void setCellColorsArray       (::fwData::Array::sptr array);
    FWDATA_API void setPointNormalsArray     (::fwData::Array::sptr array);
    FWDATA_API void setCellNormalsArray      (::fwData::Array::sptr array);

    FWDATA_API ::fwData::Array::sptr getPointsArray           () const;
    FWDATA_API ::fwData::Array::sptr getCellTypesArray        () const;
    FWDATA_API ::fwData::Array::sptr getCellDataArray         () const;
    FWDATA_API ::fwData::Array::sptr getCellDataOffsetsArray  () const;
    //FWDATA_API ::fwData::Array::sptr getPointColorsArray      () const;
    //FWDATA_API ::fwData::Array::sptr getCellColorsArray       () const;
    //FWDATA_API ::fwData::Array::sptr getPointNormalsArray     () const;
    //FWDATA_API ::fwData::Array::sptr getCellNormalsArray      () const;



    FWDATA_API void clearPoints();
    FWDATA_API void clearCells();
    FWDATA_API void clear();

    FWDATA_API void clearPointNormals();
    FWDATA_API void clearPointColors();
    FWDATA_API void clearCellNormals();
    FWDATA_API void clearCellColors();


    FWDATA_API size_t getNumberOfPoints() const;
    FWDATA_API size_t getNumberOfCells() const;

    FWDATA_API size_t getDataSizeInBytes() const;
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    FWDATA_API void addDataArray(const std::string &name, ::fwData::Array::sptr array);
    FWDATA_API ::fwData::Array::sptr getDataArray(const std::string &name) const;
    FWDATA_API void removeDataArray(const std::string &name);


protected:

    FWDATA_API Mesh();
    FWDATA_API virtual ~Mesh() ;


    Id m_nbPoints;
    Id m_nbCells;
    Id m_cellsDataSize;

    ::fwData::Array::sptr m_points;
    ::fwData::Array::sptr m_cellTypes;
    ::fwData::Array::sptr m_cellData;
    ::fwData::Array::sptr m_cellDataOffsets;
    ::fwData::Array::sptr m_pointColors;
    ::fwData::Array::sptr m_cellColors;
    ::fwData::Array::sptr m_pointNormals;
    ::fwData::Array::sptr m_cellNormals;

    ArrayMapType m_arrayMap;


};

} // namespace fwData

#endif // _FWDATA_MESH_HPP_
