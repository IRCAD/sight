/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_CALIBRATIONINFO_HPP__
#define __ARDATA_CALIBRATIONINFO_HPP__

#include "arData/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwCore/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/Object.hpp>
#include <fwData/PointList.hpp>

#include <list>

fwCampAutoDeclareDataMacro((arData)(CalibrationInfo), ARDATA_API);

namespace arData
{
/**
 * @brief class for handle calibration information , this class handle two lists the first one contains images
 * and the second one contains pointList
 *
 */
class ARDATA_CLASS_API CalibrationInfo : public ::fwData::Object
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((CalibrationInfo)( ::fwData::Object), (()),
                                           ::fwData::factory::New< CalibrationInfo >);

    fwCampMakeFriendDataMacro((arData)(CalibrationInfo));

    ///typedefs
    typedef std::list< ::fwData::Image::sptr > ImageContainerType;
    typedef std::list< ::fwData::PointList::sptr > PointListContainerType;

    /**
     * @name Signals API
     * @{
     */
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_RECORD_SIG;
    typedef ::fwCom::Signal<void ()> AddedRecordSignalType;

    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_REMOVED_RECORD_SIG;
    typedef ::fwCom::Signal<void ()> RemovedRecordSignalType;

    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_RESET_RECORD_SIG;
    typedef ::fwCom::Signal<void ()> ResetRecordSignalType;

    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_GET_RECORD_SIG;
    typedef ::fwCom::Signal<void (size_t)> GetRecordSignalType;

    /**
     * @}
     */

    ///Constructor
    ARDATA_API CalibrationInfo(::fwData::Object::Key key);
    ///Destructor
    ARDATA_API ~CalibrationInfo();

    /// Defines shallow copy
    ARDATA_API void shallowCopy(const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    ARDATA_API void cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief add an Image and a pointList into CalibrationInfo
     * @param[in] img the new image
     * @param[in] pl the corresponding point list
     */
    ARDATA_API void addRecord( const ::fwData::Image::sptr& img, const ::fwData::PointList::sptr& pl );

    /**
     * @brief remove the desired record (both image and pointList)
     * @param[in] idx index of the record to be removed
     */
    ARDATA_API void removeRecord(size_t idx);

    /// Reset all records
    ARDATA_API void resetRecords();

    ///Get all the images
    ARDATA_API ImageContainerType getImageContainer() const;

    ///Get all the pointLists
    ARDATA_API PointListContainerType getPointListContainer() const;

    /**
     * @brief Get the pointList corresponding to the specified image, if the specified image is not found
     * a null pointer is returned
     * @param[in] img image
     * @return pointList corresponding to the image
     */
    ARDATA_API ::fwData::PointList::sptr getPointList( const ::fwData::Image::sptr& img) const;

    /**
     * @brief Get the image corresponding to the specified pointList, if the specified pointList is not found
     *  a null pointer is returned
     * @param[in] pl the pointList
     * @return image corresponding to the pointList
     */
    ARDATA_API ::fwData::Image::sptr getImage( const ::fwData::PointList::sptr& pl) const;

    /**
     * @brief Get the image corresponding to the specified index, if the index is not found
     *  a null pointer is returned
     * @param[in] idx index of image
     * @return image corresponding to the index
     */
    ARDATA_API ::fwData::Image::sptr getImage(size_t idx) const;

protected:
    ///List of Image
    ImageContainerType m_imageContainer;
    ///List of pointList
    PointListContainerType m_pointListContainer;
    ///added record signal
    AddedRecordSignalType::sptr m_sigAddedRecord;
    /// removed record signal
    RemovedRecordSignalType::sptr m_sigRemovedRecord;
    ///reset records signal
    ResetRecordSignalType::sptr m_sigResetRecord;
    ///get record signal
    GetRecordSignalType::sptr m_sigGetRecord;

};

}// namespace arData

#endif /* __ARDATA_CALIBRATIONINFO_HPP__ */
