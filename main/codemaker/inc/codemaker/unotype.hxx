/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef INCLUDED_CODEMAKER_UNOTYPE_HXX
#define INCLUDED_CODEMAKER_UNOTYPE_HXX

#include "sal/types.h"

#include <vector>

namespace rtl { class OString; }

namespace codemaker {

namespace UnoType {
    /**
       An enumeration of all the sorts of UNO types.

       All complex UNO types are subsumed under SORT_COMPLEX.
     */
    enum Sort {
        SORT_VOID,
        SORT_BOOLEAN,
        SORT_BYTE,
        SORT_SHORT,
        SORT_UNSIGNED_SHORT,
        SORT_LONG,
        SORT_UNSIGNED_LONG,
        SORT_HYPER,
        SORT_UNSIGNED_HYPER,
        SORT_FLOAT,
        SORT_DOUBLE,
        SORT_CHAR,
        SORT_STRING,
        SORT_TYPE,
        SORT_ANY,
        SORT_COMPLEX
    };

    /**
       Maps from a binary UNO type name or UNO type registry name to its type
       sort.

       @param type a binary UNO type name or UNO type registry name

       @return the sort of the UNO type denoted by the given type; the detection
       of the correct sort is purely syntactical (especially, if the given input
       is a UNO type registry name that denotes something other than a UNO type,
       SORT_COMPLEX is returned)
     */
    Sort getSort(rtl::OString const & type);

    /**
       Determines whether a UNO type name or UNO type registry name denotes a
       UNO sequence type.

       @param type a binary UNO type name or UNO type registry name

       @return true iff the given type denotes a UNO sequence type; the
       detection is purely syntactical
     */
    bool isSequenceType(rtl::OString const & type);

    /**
       Decomposes a UNO type name or UNO type registry name.

       @param type a binary UNO type name or UNO type registry name

       @param rank if non-null, returns the rank of the denoted UNO type (which
       is zero for any given type that does not denote a UNO sequence type)

       @param arguments if non-null, the type arguments are stripped from an
       instantiated polymorphic struct type and returned via this parameter (in
       the correct order); if null, type arguments are not stripped from
       instantiated polymorphic struct types

       @return the base part of the given type
     */
    rtl::OString decompose(
        rtl::OString const & type, sal_Int32 * rank = 0,
        std::vector< rtl::OString > * arguments = 0);
}

}

#endif
