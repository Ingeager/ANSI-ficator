//Jan 2014

#ifndef DEF_INGE_TYPES
#define DEF_INGE_TYPES

#include "string.h"

typedef signed long int si32;
typedef unsigned long int ui32;
typedef signed short int si16;
typedef unsigned short int ui16;
typedef signed char si8;
typedef unsigned char ui8;

inline void gotoDummy() {}

template<class T> class tScopeNew {
    private:
        T *mPointer;
        ui32 mByteSize;
    public:
        tScopeNew() {
            mPointer = 0;
            mByteSize = 0;
        }
        tScopeNew(int aBlocks) {
            allocate(aBlocks);
        }

        virtual ~tScopeNew() {
            if (mPointer) {
                delete[] mPointer;
            }
        }
        
        T *allocate(int aBlocks) {
            mPointer = new T[aBlocks];
            mByteSize = (sizeof(T)*aBlocks);
            return ( mPointer );
        }
        
        operator T* () {
            return mPointer;
        }
        
        T& operator[] (int vIndex)  {
            return mPointer[vIndex];
        }
        
        ui32 getByteSize() {
            return mByteSize;
        }
};

class tScopeString {
    private:
        char *mPointer;
    public:
        void deleteString() {
            if (mPointer) {
                delete[] mPointer;
            }
        }
        void setString(char *aString) {
            if (mPointer) {
                deleteString();
            }
            mPointer = new char[strlen(aString) + 1];
            strcpy(mPointer, aString);
        }
        
        tScopeString() {
            mPointer = 0;  
        }
        tScopeString(char *aString) {
            setString(aString);
        }
        virtual ~tScopeString() {
            deleteString();
        }

        operator char* () {
            return mPointer;
        }
};

#define C_BRACECASE(CA_CASE, CA_ACTION) \
    case ( CA_CASE ): \
        { \
            CA_ACTION \
        } \
        break;

#define C_LOOP(CA_VARNAME, CA_UNTIL, CA_ACTION) \
    int CA_VARNAME = 0; \
    while (CA_VARNAME CA_UNTIL) { \
        CA_ACTION \
        CA_VARNAME++; \
    }

#endif


