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
 *
 * It is the new structure that represent mesh in fw4spl. For the moment, this new structure is available
 * to register mesh with triangle cells, with quad cells or with triangle and quad cells. Peharps, in future
 * work other cell type will be available (ex: POLYGON)
 *
 * This structure contains some information stock in ::fwData::Array \n
 *
 * An array (m_points) which contains point coord (x,y,z) \n
 * An array (m_cellTypes) which contains cell type (TRIAN or QUAD for the moment) \n
 * An array (m_cellData) which contains point indexes in m_points used to create cells, 3 indexes are necessary to create a triangle cell, 4 for quad cell. \n
 * An array (m_cellDataOffsets) which contains indexes relative to m_cellData, to retrieve the first point necessary to the cell creation. \n
 *
 * Example : \n
 * m_points.size = number of mesh points  * 3 \n
 * m_points = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ] \n
 * m_cellTypes.size = number of mesh cells \n
 * m_cellTypes = [TRIANGLE, TRIANGLE, QUAD, QUAD, TRIANGLE ... ] \n
 * m_cellDataOffsets.size = number of mesh cells \n
 * m_cellDataOffsets = [0, 3, 6, 10, 14, ... ] ( offset shifting in  m_cellData = +3 if triangle cell rr +4 if quad cell ) \n
 * number of mesh cells * 3 (if only triangle cell) < m_cellData.size < number of mesh cells * 4 (if only quad cell) \n
 * m_cellData = [0, 1, 2, 0, 1, 3, 0, 1, 3, 5... ] ( correspond to point id ) \n
 *
 * Get the points coordinates of the third cell \n
 * m_cellTypes[2] => cell type = QUAD \n
 * m_cellDataOffsets[2] => index in m_cellData of cell definition = 6 \n
 * index of p1 = m_cellData[6] = 0 \n
 * index of p2 = m_cellData[6+1] = 1 \n
 * index of p3 = m_cellData[6+2] = 3 \n
 * index of p4 = m_cellData[6+3] = 5 \n
 * p1 = [ x0=m_points[0]  y0 z0 ] ( 0 * 3 = 0 ) \n
 * p2 = [ x1=m_points[3]  y1 z1 ] ( 1 * 3 = 3 ) \n
 * p3 = [ x3=m_points[9]  y3 z3 ] ( 3 * 3 = 9 ) \n
 * p4 = [ x5=m_points[15] y5 z5 ] ( 5 * 3 = 15 ) \n
 *
 * We have another array to stock normal by points, normal by edges, color by points or color by cells, to short : \n
 * Normal arrays contains normal vector (x,y,z) \n
 * normals.size = number of mesh points (respc cells)  * 3 \n
 * normals = [ x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, ... ] \n
 * Color arrays contains RGB or RGBA colors ( check the number of components 3 or 4 of array ) \n
 * colors.size = number of mesh points (respc cells) * 3 (respc 4) \n
 * colors = [ r0, g0, b0, r1, g1, b1, ... ] \n
 * or colors = [ r0, g0, b0, a0, r1, g1, b1, a1, ... ] \n
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
    typedef boost::uint8_t ColorValueType;
    typedef float          NormalValueType;
    typedef Id             CellValueType;
    typedef Id             CellDataOffsetType;
    typedef boost::uint8_t CellTypes;

    typedef boost::multi_array_ref<PointValueType    , 2> PointsMultiArrayType;
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

    /// Allocates normals array according to the number of points of the mesh.
    FWDATA_API size_t allocatePointNormals() throw(::fwData::Exception);

    /// Allocates colors array according to the number of points of the mesh.
    FWDATA_API size_t allocatePointColors(ColorArrayTypes t) throw(::fwData::Exception);

    /// Allocates normals array according to the number of cells of the mesh.
    FWDATA_API size_t allocateCellNormals() throw(::fwData::Exception);

    /// Allocates colors array according to the number of cells of the mesh.
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

    /**
     * @brief Insert a point into the mesh.
     * @see insertNextPoint
     */
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

    /**
     * @brief Set a point coordinates.
     * @see setPoint
     */
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
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATA_API Id insertNextCell(CellValueType p)
    throw(::fwData::Exception);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2)
    throw(::fwData::Exception);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3)
    throw(::fwData::Exception);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATA_API Id insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3, CellValueType p4)
    throw(::fwData::Exception);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    template <typename T> Id insertNextCell(CellTypesEnum type, const T &pointsBegin, const T &pointsEnd)
    throw(::fwData::Exception);


    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API PointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API CellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API CellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API CellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API PointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API CellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API PointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATA_API CellNormalsMultiArrayType     getCellNormals() const;

    /// Sets the internal corresponding array
    FWDATA_API void setPointsArray           (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setCellTypesArray        (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setCellDataArray         (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setCellDataOffsetsArray  (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setPointColorsArray      (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setCellColorsArray       (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setPointNormalsArray     (::fwData::Array::sptr array);
    /// Sets the internal corresponding array
    FWDATA_API void setCellNormalsArray      (::fwData::Array::sptr array);

    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getPointsArray           () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getCellTypesArray        () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getCellDataArray         () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getCellDataOffsetsArray  () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getPointColorsArray      () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getCellColorsArray       () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getPointNormalsArray     () const;
    /// Returns the internal corresponding array
    FWDATA_API ::fwData::Array::sptr getCellNormalsArray      () const;

    /**
     * @brief Clear mesh points.
     * Calling this method don't impact memory allocation.
     */
    FWDATA_API void clearPoints();

    /**
     * @brief Clear mesh cells.
     * Calling this method don't impact memory allocation.
     */
    FWDATA_API void clearCells();

    /**
     * @brief Remove all data contained in the mesh. Memory is freed.
     */
    FWDATA_API void clear();

    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearPointNormals();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearPointColors();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearCellNormals();
    /// Remove corresponding array, memory is freed.
    FWDATA_API void clearCellColors();


    /// Set number of points.
    FWDATA_API void setNumberOfPoints(Id nb);

    /// Get number of points.
    FWDATA_API Id getNumberOfPoints() const;

    /// Set number of cells.
    FWDATA_API void setNumberOfCells(Id nb);

    /// Get number of cells.
    FWDATA_API Id getNumberOfCells() const;

    /// Set cell data size.
    FWDATA_API void setCellDataSize(Id nb);

    /// Get cell data size.
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

    /// Add an array in the mesh array-map
    FWDATA_API void addDataArray(const std::string &name, ::fwData::Array::sptr array);

    /// Get an array in the mesh array-map
    FWDATA_API ::fwData::Array::sptr getDataArray(const std::string &name) const;

    /// Remove an array in the mesh array-map
    FWDATA_API void removeDataArray(const std::string &name);

    /// Return all array names stock in the mesh array-map
    FWDATA_API std::vector<std::string> getDataArrayNames() const;

protected:

    /**
     * @brief Initializes points, cell-types, cell-data, and cell-data-offsets arrays.
     */
    FWDATA_API void initArrays();

    /// Constructor, cannot be called directly, use Mesh::New()
    FWDATA_API Mesh();

    /// Destructor
    FWDATA_API virtual ~Mesh() ;

    /// Number of points defined for the mesh
    Id m_nbPoints;

    /// Number of cells defined for the mesh
    Id m_nbCells;

    /// Number of point index defined for mesh (size of m_cellData)
    Id m_cellsDataSize;

    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbPoints x 3.
     *
     * This array contains points : [ x1 y1 z1 x2 y2 z2 ... xn yn zn ]
     */
    ::fwData::Array::sptr m_points;
    /**
     * @brief Cell types array : 1-components 1-dimension uint8 array, size = m_nbCells.
     *
     * This array each cell type : [ TRIANGLE QUAD QUAD ... TRIANGLE TRIANGLE QUAD ]
     */
    ::fwData::Array::sptr m_cellTypes;
    /**
     * @brief Cell data array : 1-components 1-dimension uint64 array, size = m_cellsDataSize.
     *
     * Contains cell data : cell points ids are contiguously stored regardless
     * of the cell type. Size depends of cell type. If we have only TRIANGLE type, size = m_nbCells x 3.
     *
     * This array contains point indexes (index in m_points) : [ TRIAN_ID1, TRIAN_ID2, TRIAN_ID3, QUAD_ID1, QUAD_ID2, QUAD_ID3, QUAD_ID4, ... ]
     */
    ::fwData::Array::sptr m_cellData;
    /**
     * @brief Cell data offsets array : 1-components 1-dimension uint64 array, size = m_nbCells.
     *
     * Contains cell data offsets : for each cell, gives the offset of
     * the first item of this cell in m_cellData.
     *
     * This array contains cell indexes m_cellData : [ INDEX_TRIAN_1, INDEX_QUAD_1, INDEX_QUAD_2 ...  ]
     */
    ::fwData::Array::sptr m_cellDataOffsets;
    /**
     * @brief point colors array : 3 or 4-components 1-dimension float array, size = m_nbPoints.
     *
     * This array contains point colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    ::fwData::Array::sptr m_pointColors;
    /**
     * @brief Mesh point array : 3 or 4-components 1-dimension uint8_t array, size = m_nbCells.
     *
     * This array contains cell colors : [ R1 G1 B1 R2 G2 B2 ... ] or [ R1 G1 B1 A1 R2 G2 B2 A2 ... ]
     */
    ::fwData::Array::sptr m_cellColors;
    /**
     * @brief Mesh point array : 3-components 1-dimension uint8_t array, size = m_nbPoints.
     *
     * This array contains point normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    ::fwData::Array::sptr m_pointNormals;
    /**
     * @brief Mesh point array : 3-components 1-dimension float array, size = m_nbCells.
     *
     * This array contains cell normals : [ nx1 ny1 nz1 nx2 ny2 nz2 ... ]
     */
    ::fwData::Array::sptr m_cellNormals;

    /// Array map where you can add few additional arrays registered thanks to a key to perform/conserve some specific analysis.
    ArrayMapType m_arrayMap;


};

} // namespace fwData

#endif // _FWDATA_MESH_HPP_
