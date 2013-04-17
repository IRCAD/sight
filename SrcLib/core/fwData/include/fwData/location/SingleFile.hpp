/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_LOCATION_SINGLEFILE_HPP_
#define _FWDATA_LOCATION_SINGLEFILE_HPP_


#include <boost/filesystem.hpp>

#include "fwData/config.hpp"
#include "fwData/location/ILocation.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(location)(SingleFile), FWDATA_API);


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
            ((::fwData::factory::New< SingleFile > ,() ))
            ((SingleFileFactory ,((::boost::filesystem::path)) ))
    );


    /// Constructor
    FWDATA_API SingleFile( ::fwData::Object::Key key );

    /// Destructor
    FWDATA_API virtual ~SingleFile();


    fwCampMakeFriendDataMacro((fwData)(location)(SingleFile));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Set file system path
    FWDATA_API void setPath( PathType path);

    /// Get file system path
    FWDATA_API PathType getPath() const;

protected :

    FWDATA_API static sptr SingleFileFactory(PathType path);

    /// file system path
    PathType m_path;

};

/**
 * @struct enableSingleFile
 * @brief This class is derived by reader/writer.
 *
 * Reader/Writer classes should only need to implement get/setLocation
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
    enableSingleFile(RW *rw) : m_rw(rw)
    {
        SLM_ASSERT("m_rw not instanced", m_rw);
    }

    /// Set file system path
    void setFile(ILocation::PathType path)
    {
        getLocation<SingleFile>(m_rw)->setPath(path);
    }

    /// Get file system path
    ILocation::PathType getFile()
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

#endif /* _FWDATA_LOCATION_SINGLEFILE_HPP_ */
