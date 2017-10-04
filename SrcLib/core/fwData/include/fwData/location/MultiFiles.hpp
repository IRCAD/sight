/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_LOCATION_MULTIFILES_HPP__
#define __FWDATA_LOCATION_MULTIFILES_HPP__

#include "fwData/config.hpp"
#include "fwData/location/ILocation.hpp"

#include <boost/filesystem/path.hpp>

#include <vector>

namespace fwData
{
namespace location
{
/**
 * @brief This class defines a multi files location.
 */
class FWDATA_CLASS_API MultiFiles  : public ILocation
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MultiFiles)(ILocation), (()),  ::fwData::factory::New< MultiFiles > );

    /// Constructor
    FWDATA_API MultiFiles( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~MultiFiles();

    /// Set file system paths
    FWDATA_API void setPaths( VectPathType paths );

    /// Set file system paths
    FWDATA_API VectPathType getPaths();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

protected:

    /// file system paths
    VectPathType m_paths;

};

/**
 * @struct enableMultiFiles
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
 */
template<class RW> // reader or writer class should only need to implement get/setLocation
struct enableMultiFiles
{
    /**
     * @brief constructor
     * @param[in] rw reader or writer
     */
    enableMultiFiles(RW* rw) :
        m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// Set file system paths
    void setFiles( ILocation::VectPathType paths )
    {
        getLocation<MultiFiles>(m_rw)->setPaths(paths);
    }

    /// Get file system paths
    ILocation::VectPathType getFiles()
    {
        return getLocation<MultiFiles>(m_rw)->getPaths();
    }

    private:
        /// Not implemented must use constructor with one parameter
        enableMultiFiles();
        RW* m_rw;
};

}
}

#endif /* __FWDATA_LOCATION_MULTIFILES_HPP__ */
