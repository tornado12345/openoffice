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

#if !defined(AFX_ACCTEXT_H__F06C5496_9959_4C7C_873E_A8D50CFB290D__INCLUDED_)
#define AFX_ACCTEXT_H__F06C5496_9959_4C7C_873E_A8D50CFB290D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

#include "AccTextBase.h"

/**
 * CAccText implements IAccessibleText interface.
 */
class ATL_NO_VTABLE CAccText :
            public CComObjectRoot,
            public CComCoClass<CAccText,&CLSID_AccText>,
            public IAccessibleText,
            public CAccTextBase
{
public:
    CAccText()
    {
            }
    ~CAccText()
    {
            }

    BEGIN_COM_MAP(CAccText)
    COM_INTERFACE_ENTRY(IAccessibleText)
    COM_INTERFACE_ENTRY(IUNOXWrapper)
    COM_INTERFACE_ENTRY_FUNC_BLIND(NULL,_SmartQI)
    END_COM_MAP()

    static HRESULT WINAPI _SmartQI(void* pv,
                                   REFIID iid, void** ppvObject, DWORD)
    {
        return ((CAccText*)pv)->SmartQI(iid,ppvObject);
    }

    HRESULT SmartQI(REFIID iid, void** ppvObject)
    {
        if( m_pOuterUnknown )
            return OuterQueryInterface(iid,ppvObject);
        return E_FAIL;
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_AccText)

    // IAccessibleText
public:
    // IAccessibleText

    // Adds a text selection.
    STDMETHOD(addSelection)(long startOffset, long endOffset);//, unsigned char * success);

    // Gets text attributes.
    STDMETHOD(get_attributes)(long offset, long * startOffset, long * endOffset, BSTR * textAttributes);

    // Gets caret offset.
    STDMETHOD(get_caretOffset)(long * offset);

    // Gets total number of characters.
    STDMETHOD(get_characterCount)(long * nCharacters);

    // Gets bounding rect containing the glyph(s) representing the character
    // at the specified text offset
    STDMETHOD(get_characterExtents)(long offset, IA2CoordinateType coordType, long * x, long * y, long * width, long * height);

    // Gets number of active non-contiguous selections.
    STDMETHOD(get_nSelections)(long * nSelections);

    // Gets bounding rect for the glyph at a certain point.
    STDMETHOD(get_offsetAtPoint)(long x, long y, IA2CoordinateType coordType, long * offset);

    // Gets character offsets of N-th active text selection.
    STDMETHOD(get_selection)(long selection, long * startOffset, long * endOffset);

    // Gets a range of text by offset NOTE: returned string may be longer
    // than endOffset-startOffset bytes if text contains multi-byte characters.
    STDMETHOD(get_text)(long startOffset, long endOffset, BSTR * text);

    // Gets a specified amount of text that ends before a specified offset.
    STDMETHOD(get_textBeforeOffset)(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text);

    // Gets a specified amount of text that spans the specified offset.
    STDMETHOD(get_textAfterOffset)(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text);

    // Gets a specified amount of text that starts after a specified offset.
    STDMETHOD(get_textAtOffset)(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text);

    // Unselects a range of text.
    STDMETHOD(removeSelection)(long selectionIndex);//, unsigned char * success);

    // Moves text caret.
    STDMETHOD(setCaretOffset)(long offset);//, unsigned char * success);

    // Changes the bounds of an existing selection.
    STDMETHOD(setSelection)(long selectionIndex, long startOffset, long endOffset);//, unsigned char * success);

    // Gets total number of characters.
    // NOTE: this may be different than the total number of bytes required
    // to store the text, if the text contains multi-byte characters.
    STDMETHOD(get_nCharacters)(long * nCharacters);

    // Makes specific part of string visible on screen.
    STDMETHOD(scrollSubstringTo)(long startIndex, long endIndex,enum IA2ScrollType scrollType);
    STDMETHOD(scrollSubstringToPoint)(long startIndex, long endIndex,enum IA2CoordinateType coordinateType, long x, long y );

    STDMETHOD(get_newText)( IA2TextSegment *newText);

    STDMETHOD(get_oldText)( IA2TextSegment *oldText);

    // Overide of IUNOXWrapper.
    STDMETHOD(put_XInterface)(long pXInterface);

};

#endif // !defined(AFX_ACCTEXT_H__F06C5496_9959_4C7C_873E_A8D50CFB290D__INCLUDED_)
