/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_MODEL_HPP__
#define __FWDATA_MODEL_HPP__

#include <map>
#include <boost/shared_ptr.hpp>

#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/TriangularMesh.hpp"
#include "fwData/Material.hpp"

fwCampAutoDeclareDataMacro((fwData)(Model), FWDATA_API);

namespace fwData
{

/**
 * @brief   This model class focusses on 3D model such as meshes.
 *
 * A model is represented by a triangular mesh and a material
 *
 * @see     ::fwData::TriangularMesh, ::fwData::Material
 */
class FWDATA_CLASS_API Model : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Model)(::fwData::Object), (()), ::fwData::factory::New< Model >) ;

    fwCampMakeFriendDataMacro((fwData)(Model));

    /// mesh and material container
    typedef std::map< ::fwData::TriangularMesh::sptr , ::fwData::Material::sptr  > Container ;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Model(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Model() throw();

    /**
     * @brief returns editable mesh container
     */
    FWDATA_API Container &getRefMap() ;

    /**
     * @brief returns read-only mesh container
     */
    FWDATA_API const Container &getCRefMap() const ;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected:

    Container m_map;
};

} // namespace fwData

#endif // __FWDATA_MODEL_HPP__

