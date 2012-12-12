/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2010.
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef __ACCTABLEEVENTLISTENER_HXX
#define __ACCTABLEEVENTLISTENER_HXX

#include <stdio.h>
#include "AccDescendantManagerEventListener.hxx"
#include <com/sun/star/accessibility/XAccessibleEventListener.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>

using namespace ::com::sun::star::uno;

/**
 * AccTableEventListener is inherited from AccDescendantManagerEventListener. It handles 
 * the evnets generated by tree controls. The accessible role is: TABLE.
 * It defines the procedure of specific event handling related with table components 
 * and provides the detailed support for some related methods. 
 */
class AccTableEventListener: public AccDescendantManagerEventListener
{
public:
    AccTableEventListener(com::sun::star::accessibility::XAccessible* pAcc, AccObjectManagerAgent* Agent);
    virtual ~AccTableEventListener();

    //AccessibleEventListener
    virtual void SAL_CALL notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException);
    //for active descendant changed event
    virtual void SAL_CALL handleActiveDescendantChangedEvent(Any oldValue, Any newValue);

	void SAL_CALL handleTableModelChangeEvent(Any newValue);

};

#endif
