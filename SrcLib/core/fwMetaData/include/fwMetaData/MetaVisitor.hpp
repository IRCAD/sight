#ifndef __FWMETADATA_METAVISITOR_HPP_
#define __FWMETADATA_METAVISITOR_HPP_

#include <fwCamp/camp/ExtendedClassVisitor.hpp>
#include "fwMetaData/Object.hpp"
#include "fwMetaData/Policy.hpp"

#include "fwMetaData/config.hpp"

namespace fwMetaData {


/**
 * @brief This class offer the possibility to visit a MetaData and manage his execution with a Policy
 */
class FWMETADATA_CLASS_API MetaVisitor: public ::camp::ExtendedClassVisitor
{
public:
    /**
     * @brief Construct a new MetaVisitor.
     * @param policy the policy to manage visitation @see Policy.
     * @param metaObject the visited metaObject.
     */
    FWMETADATA_API MetaVisitor(Policy& policy, Object::sptr metaObject);

    /**
     * @brief Meta visitor destructor
     */
    FWMETADATA_API ~MetaVisitor();


    /**
     * @brief visit MapProperty(the map of Object attributes).
     * Call Policy::processAttributes method
     */
    FWMETADATA_API virtual void visit(const camp::MapProperty& property);

private:
    Policy& m_policy;
    camp::UserObject m_userObj;

};



}  // namespace fwMetaData

#endif /* __FWMETADATA_METAVISITOR_HPP_ */
