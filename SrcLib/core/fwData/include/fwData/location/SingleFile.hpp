/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATA_LOCATION_SIMPLEFILE_HPP_
#define _DATA_LOCATION_SIMPLEFILE_HPP_


#include <boost/filesystem.hpp>

#include "fwData/config.hpp"
#include "fwData/location/ILocation.hpp"


namespace fwData
{
namespace location
{
/**
 * @class SingleFile
 * @brief This class defines a single file %location.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API SingleFile  : public ILocation
{
public:
    fwCoreClassDefinitionsWithNFactoriesMacro(
            (SingleFile)(ILocation),
            ((::fwTools::Factory::New< SingleFile > ,() ))
            ((SingleFileFactory ,((::boost::filesystem::path)) ))
    );

    FWDATA_API static sptr SingleFileFactory(::boost::filesystem::path _path);

    /// Constructor
    FWDATA_API SingleFile();

    /// Destructor
    FWDATA_API virtual ~SingleFile();

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( SingleFile::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( SingleFile::csptr _source );

    /// Set file system path
    FWDATA_API void setPath( ::boost::filesystem::path path);

    /// Get file system path
    FWDATA_API ::boost::filesystem::path getPath() const;

protected :

    /// file system path
    ::boost::filesystem::path m_path;

};

/**
 * @struct enableSingleFile
 * @brief This class is derivated by reader/writer.
 *
 * Reader/Writter classes should only need to implement get/setLocation
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
template<typename RW> // reader or writer class should only need to implement get/setLocation
struct enableSingleFile
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableSingleFile(RW *rw) : m_rw(rw) { assert(m_rw);}

    /// Set file system path
    void setFile(::boost::filesystem::path path)
    {
        getLocation<SingleFile>(m_rw)->setPath(path);
    }

    /// Get file system path
    ::boost::filesystem::path getFile()
    {
        return (getLocation<SingleFile>(m_rw))->getPath();
    }

private :
    /// Not implemented must use constructor with one parameter
    enableSingleFile();
    RW *m_rw;

};



}
}

#endif /* _DATA_LOCATION_SIMPLEFILE_HPP_ */
