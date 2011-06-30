/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATA_LOCATION_MULTIFILES_HPP_
#define _DATA_LOCATION_MULTIFILES_HPP_

#include <vector>
#include <boost/filesystem/path.hpp>

#include "fwData/config.hpp"
#include "fwData/location/ILocation.hpp"


namespace fwData
{
namespace location
{
/**
 * @class MultiFiles
 * @brief This class defines a multi files %location.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API MultiFiles  : public ILocation
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MultiFiles)(ILocation), (()), new MultiFiles ) ;

    /// Set file system paths
    FWDATA_API void setPaths( std::vector< ::boost::filesystem::path>  paths );

    /// Set file system paths
    FWDATA_API std::vector< ::boost::filesystem::path> getPaths();

protected :

    /// Constructor
    FWDATA_API MultiFiles();

    /// Destructor
    FWDATA_API virtual ~MultiFiles();

    /// file system paths
    std::vector< ::boost::filesystem::path> m_paths;

};

/**
 * @struct enableMultiFiles
 * @brief This class is derivated by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 *
 * @author   IRCAD (Research and Development Team).
 * @date 2007-2009.
 */
template<class RW> // reader or writer class should only need to implement get/setLocation
struct enableMultiFiles
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableMultiFiles(RW *rw) : m_rw(rw) {SLM_ASSERT("m_rw not instanced", m_rw);}

    /// Set file system paths
    void setFiles( std::vector< ::boost::filesystem::path>  paths )
    {
        getLocation<MultiFiles>(m_rw)->setPaths(paths);
    }

    /// Get file system paths
    std::vector< ::boost::filesystem::path> getFiles()
                     {
        return getLocation<MultiFiles>(m_rw)->getPaths();
                     }

private :
    /// Not implemented must use constructor with one parameter
    enableMultiFiles();
    RW *m_rw;

};

}
}

#endif /* _DATA_LOCATION_MULTIFILES_HPP_ */
