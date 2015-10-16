/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_MODELSERIES_HPP__
#define __FWMEDDATA_MODELSERIES_HPP__

#include "fwMedData/config.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>

#include <fwData/Object.hpp>

#include <vector>


fwCampAutoDeclareDataMacro((fwMedData)(ModelSeries), FWMEDDATA_API);

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{


/**
 * @brief Holds models data
 */
class FWMEDDATA_CLASS_API ModelSeries : public ::fwMedData::Series
{

public:
    typedef std::vector< SPTR(::fwData::Reconstruction) > ReconstructionVectorType;

    fwCoreClassDefinitionsWithFactoryMacro( (ModelSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< ModelSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(ModelSeries));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ModelSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ModelSeries();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /**
     * @name Getters / Setters
     * @{ */
    const ReconstructionVectorType &getReconstructionDB() const;
    void setReconstructionDB(const ReconstructionVectorType &val);
    /**  @} */


protected:

    /// Model container
    ReconstructionVectorType m_reconstructionDB;

};

//-----------------------------------------------------------------------------

inline const ModelSeries::ReconstructionVectorType& ModelSeries::getReconstructionDB() const
{
    return m_reconstructionDB;
}

//-----------------------------------------------------------------------------

inline void ModelSeries::setReconstructionDB(const ModelSeries::ReconstructionVectorType &val)
{
    m_reconstructionDB = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_MODELSERIES_HPP__


