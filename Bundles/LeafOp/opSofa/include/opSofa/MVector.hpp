#ifndef OPSOFA_MVECTOR_H
#define OPSOFA_MVECTOR_H

#include <vector>

/**
 * @brief Class derivated from the vector STL template class
 * and permitting to point a new data with the vector
 */
template<class T> class MVector : public std::vector<T> 
{

public :

   /** 
    * @brief Method permitting to point a new data
    *
    * @param ptr : new pointer
    * @param nbElements : number of elements of the new data, generally 3 for the X, Y, Z coordinates
    */
    void setFront(T *ptr, int nbElements) {
        /*this->_Myfirst = ptr;
        this->_Mylast = ptr+nbElements;
        this->_Myend = ptr+nbElements;
        */
        // Use this if your compiler make error
        this->_M_impl._M_start = ptr;
        this->_M_impl._M_finish = ptr+nbElements;
        this->_M_impl._M_end_of_storage = ptr+nbElements;
    }

   /** 
    * @brief Method permitting to cancel the link of the pointers
    */
    void clear() {
        /*this->_Myfirst = NULL;
        this->_Mylast = NULL;
        */
        // Use this if your compiler make error
        this->_M_impl._M_start = NULL;
        this->_M_impl._M_finish = NULL;
        this->_M_impl._M_end_of_storage = NULL;
    }

};

#endif // OPSOFA_MVECTOR_H
