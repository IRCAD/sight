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
 * @brief   Data holding a geometric structure composed of points, lines,
 * triangles, quads or polygons
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
        NO_CELL = 0,
        POINT,
        EDGE,
        TRIANGLE,
        QUAD,
        POLY
    } CellTypesEnum;

    typedef enum {
        RGB = 3,
        RGBA = 4
    } ColorArrayTypes;

    typedef float          PointValueType;
    typedef float          ColorValueType;
    typedef float          NormalValueType;
    typedef Id             CellValueType;
    typedef Id             CellDataOffsetType;
    typedef boost::uint8_t CellTypes;

    typedef boost::multi_array_ref<PointValueType    , 2> PointMultiArrayType;
    typedef boost::multi_array_ref<CellTypes         , 1> CellTypesMultiArrayType;
    typedef boost::multi_array_ref<CellValueType     , 1> CellDataMultiArrayType;
    typedef boost::multi_array_ref<CellDataOffsetType, 1> CellDataOffsetsMultiArrayType;
    typedef boost::multi_array_ref<ColorValueType    , 2> PointColorsMultiArrayType;
    typedef boost::multi_array_ref<ColorValueType    , 2> CellColorsMultiArrayType;
    typedef boost::multi_array_ref<NormalValueType   , 2> PointNormalsMultiArrayType;
    typedef boost::multi_array_ref<NormalValueType   , 2> CellNormalsMultiArrayType;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Mesh::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Mesh::csptr _source );


    /**
     * @brief Allocate Mesh memory
     *
     * Initializes points, cell-types, cell-data, and cell-data-offsets arrays
     * with informations given by the parameters. if nbCellData is zero, it
     * will be initialized to 3*nbCells
     * @param nbPts
     * @param nbCells
     * @param nbCellsData
     *
     * @return 
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API size_t allocate(size_t nbPts, size_t nbCells, size_t nbCellsData = 0)
        throw(::fwData::Exception);

    /**
     * @brief Allocates normals/colors array according to the number of points of the mesh.
     */
    FWDATA_API size_t allocatePointNormals() throw(::fwData::Exception);
    FWDATA_API size_t allocatePointColors(ColorArrayTypes t) throw(::fwData::Exception);

    /**
     * @brief Allocates normals/colors array according to the number of cells of the mesh.
     */
    FWDATA_API size_t allocateCellNormals() throw(::fwData::Exception);
    FWDATA_API size_t allocateCellColors(ColorArrayTypes t) throw(::fwData::Exception);


    /**
     * @brief Adjust mesh memory usage
     *
     * The array (points, cell-types, cell-data, cell-data-offsets, and if they
     * exists point-colors/normals and cell-colors/normals) will be resized
     * according to the number of points and cells of the mesh (they may have
     * been defined before adjusting by setNumberOfPoints or setNumberOfCells).
     * Arrays in array-map are not impacted by this method.
     *
     * @return true if memory usage changed
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API bool adjustAllocatedMemory() throw(::fwData::Exception);

    /**
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @param p[3] point coordinates
     *
     * @return The id of the new point
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API Id insertNextPoint(const PointValueType p[3]) throw(::fwData::Exception);
    FWDATA_API Id insertNextPoint(PointValueType x, PointValueType y, PointValueType z)
        throw(::fwData::Exception);

    /**
     * @brief Set a point coordinates.
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p[3] point coordinates
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API void setPoint(Id id, const PointValueType p[3]);
    FWDATA_API void setPoint(Id id, PointValueType x, PointValueType y, PointValueType z);

    /**
     * @brief Set a point color.
     *
     * Depending on the type of allocated point-colors array (RGB or RGBA), 3
     * or 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id point index
     * @param c[3] color
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API void setPointColor(Id id, const ColorValueType c[4]);

    /**
     * @brief Set a cell color.
     *
     * Depending on the type of allocated cell-colors array (RGB or RGBA), 3 or
     * 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id cell index
     * @param c[3] color
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API void setCellColor(Id id, const ColorValueType c[4]);

    /**
     * @brief Set a point normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id point index
     * @param n[3] normal
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API void setPointNormal(Id id, const NormalValueType n[3]);
    
    /**
     * @brief Set a cell normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id cell index
     * @param n[3] normal
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API void setCellNormal(Id id, const NormalValueType n[3]);

    /**
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API Id insertNextCell(CellTypesEnum type, const CellValueType *cell, size_t nb)
        throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p)
        throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2)
        throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3)
        throw(::fwData::Exception);
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3, CellValueType p4)
        throw(::fwData::Exception);
    template <typename T> Id insertNextCell(CellTypesEnum type, const T &pointsBegin, const T &pointsEnd)
        throw(::fwData::Exception);

    //FWDATA_API void cleanCells();


    /**
     * @brief Returns the internal corresponding array as a boost::multi_array_ref
     *
     * @return requested array
     */
    FWDATA_API PointMultiArrayType           getPoints() const;
    FWDATA_API CellTypesMultiArrayType       getCellTypes() const;
    FWDATA_API CellDataMultiArrayType        getCellData() const;
    FWDATA_API CellDataOffsetsMultiArrayType getCellDataOffsets() const;
    FWDATA_API PointColorsMultiArrayType     getPointColors() const;
    FWDATA_API CellColorsMultiArrayType      getCellColors() const;
    FWDATA_API PointNormalsMultiArrayType    getPointNormals() const;
    FWDATA_API CellNormalsMultiArrayType     getCellNormals() const;

    /**
     * @brief Sets the internal corresponding array
     *
     * @param array new array
     */
    FWDATA_API void setPointArray            (::fwData::Array::sptr array);
    FWDATA_API void setCellTypesArray        (::fwData::Array::sptr array);
    FWDATA_API void setCellDataArray         (::fwData::Array::sptr array);
    FWDATA_API void setCellDataOffsetsArray  (::fwData::Array::sptr array);
    FWDATA_API void setPointColorsArray      (::fwData::Array::sptr array);
    FWDATA_API void setCellColorsArray       (::fwData::Array::sptr array);
    FWDATA_API void setPointNormalsArray     (::fwData::Array::sptr array);
    FWDATA_API void setCellNormalsArray      (::fwData::Array::sptr array);

    /**
     * @brief Returns the internal corresponding array
     *
     * @return requested array
     */
    FWDATA_API ::fwData::Array::sptr getPointsArray           () const;
    FWDATA_API ::fwData::Array::sptr getCellTypesArray        () const;
    FWDATA_API ::fwData::Array::sptr getCellDataArray         () const;
    FWDATA_API ::fwData::Array::sptr getCellDataOffsetsArray  () const;
    FWDATA_API ::fwData::Array::sptr getPointColorsArray      () const;
    FWDATA_API ::fwData::Array::sptr getCellColorsArray       () const;
    FWDATA_API ::fwData::Array::sptr getPointNormalsArray     () const;
    FWDATA_API ::fwData::Array::sptr getCellNormalsArray      () const;



    /**
     * @brief Clear mesh points.
     * Calling this method don't impact memory allocation.
     *
     * @return 
     */
    FWDATA_API void clearPoints();

    /**
     * @brief Clear mesh cells.
     * Calling this method don't impact memory allocation.
     *
     * @return 
     */
    FWDATA_API void clearCells();

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    FWDATA_API void clear();

    /**
     * @brief Remove corresponding array.
     * Memory is freed.
     */
    FWDATA_API void clearPointNormals();
    FWDATA_API void clearPointColors();
    FWDATA_API void clearCellNormals();
    FWDATA_API void clearCellColors();


    /**
     * @brief Set/get number of points.
     */
    FWDATA_API void setNumberOfPoints(Id nb);
    FWDATA_API Id getNumberOfPoints() const;

    /**
     * @brief Set/get number of cells.
     */
    FWDATA_API void setNumberOfCells(Id nb);
    FWDATA_API Id getNumberOfCells() const;

    /**
     * @brief Set/get cell data size.
     */
    FWDATA_API void setCellDataSize(Id nb);
    FWDATA_API Id getCellDataSize() const;


    /**
     * @brief Get the mesh data size in bytes.
     *
     * @return Mesh data size in bytes
     */
    FWDATA_API size_t getDataSizeInBytes() const;

    /**
     * @brief Get the amount of memory allocated in this mesh. Mey be bigger
     * than getDataSizeInBytes().
     *
     * @return Mesh data size in bytes
     */
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @brief Add/get/remove an array in the mesh array-map.
     */
    FWDATA_API void addDataArray(const std::string &name, ::fwData::Array::sptr array);
    FWDATA_API ::fwData::Array::sptr getDataArray(const std::string &name) const;
    FWDATA_API void removeDataArray(const std::string &name);


protected:

    /**
     * @brief Initializes points, cell-types, cell-data, and cell-data-offsets arrays.
     */
    FWDATA_API void initArrays();

    FWDATA_API Mesh();
    FWDATA_API virtual ~Mesh() ;


    Id m_nbPoints;
    Id m_nbCells;
    Id m_cellsDataSize;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array .
     */
    ::fwData::Array::sptr m_points;
    /**
     * @brief Cell types array : 1-components 1-dimension uint8 array .
     *
     * Identifies each cell type
     */
    ::fwData::Array::sptr m_cellTypes;
    /**
     * @brief Cell data array : 1-components 1-dimension uint64 array .
     *
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type
     */
    ::fwData::Array::sptr m_cellData;
    /**
     * @brief Cell data offsets array : 1-components 1-dimension uint64 array.
     *
     * Contains cell data offsets : for each cell, gives the offset of
     * the first item of this cell in m_cellData
     */
    ::fwData::Array::sptr m_cellDataOffsets;
    /**
     * @brief point colors array : 3 or 4-components 1-dimension float array
     *
     */
    ::fwData::Array::sptr m_pointColors;
    /**
     * @brief Mesh point array : 3 or 4-components 1-dimension float array
     */
    ::fwData::Array::sptr m_cellColors;
    /**
     * @brief Mesh point array : 3-components 1-dimension float array
     */
    ::fwData::Array::sptr m_pointNormals;
    /**
     * @brief Mesh point array : 3-components 1-dimension float array
     */
    ::fwData::Array::sptr m_cellNormals;


    ArrayMapType m_arrayMap;


};

} // namespace fwData

#endif // _FWDATA_MESH_HPP_
