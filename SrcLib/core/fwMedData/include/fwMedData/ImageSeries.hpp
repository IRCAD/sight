/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_IMAGESERIES_HPP__
#define __FWMEDDATA_IMAGESERIES_HPP__

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include "fwMedData/types.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(ImageSeries), FWMEDDATA_API);

namespace fwData
{
    class Image;
}

namespace fwMedData
{


/**
 * @class ImageSeries
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API ImageSeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageSeries)(::fwData::Object), (()), ::fwData::factory::New< ImageSeries >) ;

    fwCampMakeFriendDataMacro((fwMedData)(ImageSeries));

    fwDataObjectMacro();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ImageSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ImageSeries();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( ImageSeries::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( ImageSeries::csptr _source );

    fwDataGetSetSptrMacro(Image, SPTR(::fwData::Image));

protected:
    SPTR(::fwData::Image) m_attrImage;

};

}   //end namespace fwMedData

#endif // __FWMEDDATA_IMAGESERIES_HPP__


