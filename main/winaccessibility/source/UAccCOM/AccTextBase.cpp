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

//////////////////////////////////////////////////////////////////////
// AccTextBase.cpp: implementation of the CAccTextBase class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string>
#define WNT

#include <com/sun/star/accessibility/AccessibleTextType.hpp>
#include "AccTextBase.h"
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleTextSelection.hpp>
#include "MAccessible.h"

using namespace com::sun::star::accessibility;
using namespace com::sun::star::uno;
using namespace rtl;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OUString ReplaceFourChar(OUString oldOUString);

CAccTextBase::CAccTextBase()
{}

CAccTextBase::~CAccTextBase()
{}


/**
   * Get special selection.
   * @param startOffset Start selection offset.
   * @param endOffset   End selection offset.
   * @param success     Variant to accept the result of if the method call is successful.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_addSelection(long startOffset, long endOffset)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK XInterface#
    if(pUNOInterface == NULL)
        return E_FAIL;

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();

    Reference< XAccessibleTextSelection > pRExtension(pRContext,UNO_QUERY);

    if( pRExtension.is() )
    {
        pRExtension->addSelection(0, startOffset, endOffset);
        return S_OK;
    }
    else
    {
        GetXInterface()->setSelection(startOffset, endOffset);
        return S_OK;
    }

    return E_FAIL;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get special attributes.
   * @param offset Offset.
   * @param startOffset Variant to accept start offset.
   * @param endOffset   Variant to accept end offset.
   * @param textAttributes     Variant to accept attributes.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_attributes(long offset, long * startOffset, long * endOffset, BSTR * textAttributes)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (startOffset == NULL || endOffset == NULL || textAttributes == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
    {
        return E_FAIL;
    }

    if( offset < 0 || offset > GetXInterface()->getCharacterCount() )
        return E_FAIL;

	std::wstring strAttrs;  

    strAttrs += L"Version:1;";

    Sequence< ::com::sun::star::beans::PropertyValue > pValues = GetXInterface()->getCharacterAttributes(offset, Sequence< rtl::OUString >());
    int nCount = pValues.getLength();

    short numberingLevel = 0;
    OUString numberingPrefix;
	Any anyNumRule;
    bool bHaveNumberingPrefixAttr = false;
	bool bHaveNumberingLevel = false;
	bool bHaveNumberingRules = false;
    for(int i =0; i<nCount; i++)
    {

        ::com::sun::star::beans::PropertyValue &pValue = pValues[i];
        if(pValue.Name.compareTo(OUString::createFromAscii("NumberingLevel"))==0)
        {
			if (pValue.Value != Any())
				pValue.Value >>= numberingLevel;
			else
				numberingLevel = -1;
			bHaveNumberingLevel = true;
            continue;
        }
        if(pValue.Name.compareTo(OUString::createFromAscii("NumberingPrefix"))==0)
        {
            pValue.Value >>=numberingPrefix;
            bHaveNumberingPrefixAttr = true;
            continue;
        }
        if(pValue.Name.compareTo(OUString::createFromAscii("NumberingRules"))==0)
        {
			bHaveNumberingRules = true;
			anyNumRule = pValue.Value;
			continue;
        }
		if (bHaveNumberingLevel && bHaveNumberingRules && bHaveNumberingPrefixAttr)
		{
			OLECHAR numProps[512] = {0};
			strAttrs+=L";";
			numberingPrefix = ReplaceFourChar(numberingPrefix);
			CMAccessible::get_OLECHAR4Numbering(anyNumRule,numberingLevel,numberingPrefix,numProps);
			strAttrs += numProps;
			bHaveNumberingLevel = 0;
			bHaveNumberingRules = 0;
		}
		if( (bHaveNumberingPrefixAttr && i > 1 ) ||
			(!bHaveNumberingPrefixAttr && i > 0 ) ) //element 0 is NumberingPrefix, not write alone
		{
			strAttrs+=L";";
		}
        strAttrs += pValue.Name.getStr();
        strAttrs += L":";

        OLECHAR pTemp[2048] = {0};

        if (pValue.Name.compareTo(OUString::createFromAscii("CharBackColor"))==0 ||
                pValue.Name.compareTo(OUString::createFromAscii("CharColor"))==0 ||
                pValue.Name.compareTo(OUString::createFromAscii("CharUnderlineColor"))==0 )
        {
            unsigned long nColor;
            pValue.Value >>= nColor;
            OLECHAR pBuf[64];
            swprintf( pBuf, L"%08X", nColor );
            pTemp[0]=L'#';
            wcscat( pTemp, pBuf );

        }
        else
        {
            CMAccessible::get_OLECHARFromAny(pValue.Value,pTemp);
        }

        strAttrs +=pTemp;
    }
	strAttrs +=L";";
    // #CHECK#
    if(*textAttributes)
        SysFreeString(*textAttributes);
    *textAttributes = SysAllocString(strAttrs.c_str());

    if( offset < GetXInterface()->getCharacterCount() )
    {
        TextSegment textSeg = GetXInterface()->getTextAtIndex(offset, AccessibleTextType::ATTRIBUTE_RUN);
        *startOffset = textSeg.SegmentStart;
        *endOffset = textSeg.SegmentEnd;
    }
    else
    {
        *startOffset = offset;
        *endOffset = offset;
    }

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get caret position.
   * @param offset     Variant to accept caret offset.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_caretOffset(long * offset)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (offset == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
    {
        *offset = 0;
        return S_OK;
    }

    *offset = GetXInterface()->getCaretPosition();
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get character count.
   * @param nCharacters  Variant to accept character count.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_characterCount(long * nCharacters)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (nCharacters == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
    {
        *nCharacters = 0;
        return S_OK;
    }

    *nCharacters = GetXInterface()->getCharacterCount();
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get character extents.
   * @param offset  Offset.
   * @param x Variant to accept x position.
   * @param y Variant to accept y position.
   * @param width Variant to accept width.
   * @param Height Variant to accept height.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_characterExtents(long offset, IA2CoordinateType coordType, long * x, long * y, long * width, long * height)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (x == NULL || height == NULL || y == NULL || width == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    if(offset < 0 || offset > GetXInterface()->getCharacterCount() )
        return E_FAIL;

    com::sun::star::awt::Rectangle rectangle;
    rectangle = GetXInterface()->getCharacterBounds(offset);

    //IA2Point aPoint;
    com::sun::star::awt::Point aPoint;

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();
    if( !pRContext.is() )
    {
        return E_FAIL;
    }
    Reference<XAccessibleComponent> pRComp(pRContext,UNO_QUERY);
    if( pRComp.is() )
    {
        if(coordType == IA2_COORDTYPE_SCREEN_RELATIVE)
        {
            ::com::sun::star::awt::Point pt = pRComp->getLocationOnScreen();
            aPoint.X = pt.X;
            aPoint.Y = pt.Y;
        }
        else if(coordType == IA2_COORDTYPE_PARENT_RELATIVE)
        {
            ::com::sun::star::awt::Point pt = pRComp->getLocation();
            aPoint.X = pt.X;
            aPoint.Y = pt.Y;
        }
    }
    rectangle.X = rectangle.X + aPoint.X;
    rectangle.Y = rectangle.Y + aPoint.Y;

    *x = rectangle.X;
    *y = rectangle.Y;

    // GetXInterface()->getCharacterBounds() have different implement in different acc component
    // But we need return the width/height == 1 for every component when offset == text length.
    // So we ignore the return result of GetXInterface()->getCharacterBounds() when offset == text length.
    if( offset == GetXInterface()->getCharacterCount() )
    {
        *width = 1;
        *height = 1;
    }
    else
    {
        *width = rectangle.Width;
        *height = rectangle.Height;
    }

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get selections count.
   * @param nSelections Variant to accept selections count.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_nSelections(long * nSelections)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (nSelections == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(pUNOInterface == NULL)
    {
        *nSelections = 0;
        return S_OK;
    }

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();

    Reference< XAccessibleTextSelection > pRExtension(pRContext,UNO_QUERY);

    if( pRExtension.is() )
    {
        *nSelections = pRExtension->getSelectedPortionCount();
        return S_OK;
    }

    long iLength = GetXInterface()->getSelectedText().getLength();
    if( iLength> 0)
    {
        *nSelections = 1;
        return S_OK;
    }

    *nSelections = 0;
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get offset of some special point.
   * @param x X position of one point.
   * @param x Y position of one point.
   * @param coordType Type.
   * @param offset Variant to accept offset.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_offsetAtPoint(long x, long y, IA2CoordinateType, long * offset)
{
	CHECK_ENABLE_INF
    ENTER_PROTECTED_BLOCK

    if (offset == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    com::sun::star::awt::Point point;
    point.X = x;
    point.Y = y;
    *offset = GetXInterface()->getIndexAtPoint(point);
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get selection range.
   * @param selection selection count.
   * @param startOffset Variant to accept the start offset of special selection.
   * @param endOffset Variant to accept the end offset of special selection.
   * @return Result.
*/

STDMETHODIMP CAccTextBase::get_selection(long selectionIndex, long * startOffset, long * endOffset)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (startOffset == NULL || endOffset == NULL )
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(pUNOInterface == NULL )
        return E_FAIL;

    long nSelection = 0;
    get_nSelections(&nSelection);

    if(selectionIndex >= nSelection || selectionIndex < 0 )
        return E_FAIL;

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();

    Reference< XAccessibleTextSelection > pRExtension(pRContext,UNO_QUERY);

    if( pRExtension.is() )
    {
        *startOffset = pRExtension->getSeletedPositionStart(selectionIndex);
        *endOffset = pRExtension->getSeletedPositionEnd(selectionIndex);
        return S_OK;
    }
    else if(GetXInterface()->getSelectionEnd() > -1)
    {
        *startOffset = GetXInterface()->getSelectionStart();
        *endOffset = GetXInterface()->getSelectionEnd();
        return S_OK;
    }

    *startOffset = 0;
    *endOffset = 0;
    return E_FAIL;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get special text.
   * @param startOffset Start position of special range.
   * @param endOffset   End position of special range.
   * @param text        Variant to accept the text of special range.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_text(long startOffset, long endOffset, BSTR * text)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (text == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    if (endOffset < -1 || endOffset < startOffset )
    {
        return E_FAIL;
    }

    ::rtl::OUString ouStr;
    if (endOffset == -1 )
    {
        long nLen=0;
        if(SUCCEEDED(get_characterCount(&nLen)))
        {
            ouStr = GetXInterface()->getTextRange( 0, nLen );
        }
    }
    else
    {
        ouStr = GetXInterface()->getTextRange( startOffset, endOffset );
    }

    SysFreeString(*text);
    *text = SysAllocString((OLECHAR*)ouStr.getStr());
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get special text before some position.
   * @param offset Special position.
   * @param boundaryType Boundary type.
   * @param startOffset Variant to accept the start offset.
   * @param endOffset   Variant to accept the end offset.
   * @param text        Variant to accept the special text.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_textBeforeOffset(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK#
    if (startOffset == NULL || endOffset == NULL || text == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    // In New UNO IAccessibleText.idl these constant values are defined as follows:
    //
    //  const long TEXT_BOUNDARY_CHAR = -1;
    //  const long TEXT_BOUNDARY_TO_CURSOR_POS = -2;
    //  const long TEXT_BOUNDARY_START_OF_WORD = -3;
    //  const long TEXT_BOUNDARY_END_OF_WORD = -4;
    //  const long TEXT_BOUNDARY_START_OF_SENTENCE = -5;
    //  const long TEXT_BOUNDARY_END_OF_SENTENCE = -6;
    //  const long TEXT_BOUNDARY_START_OF_LINE = -7;
    //  const long TEXT_BOUNDARY_END_OF_LINE = -8;
    //
    // In UNO, the corresponding values are as follows:
    //
    //  const short CHARACTER = 1;
    //  const short WORD = 2;
    //  const short SENTENCE = 3;
    //  const short PARAGRAPH = 4;
    //  const short LINE = 5;
    //  const short GLYPH = 6;
    //  const short ATTRIBUTE_RUN = 7;
    //

    long			lUnoBoundaryType;

    switch(boundaryType)
    {
    case IA2_TEXT_BOUNDARY_CHAR:
        lUnoBoundaryType = 1; // CHARACTER;
        break;
    case IA2_TEXT_BOUNDARY_WORD:
        lUnoBoundaryType = 2; // WORD;
        break;
    case IA2_TEXT_BOUNDARY_SENTENCE:
        lUnoBoundaryType = 3; // SENTENCE;
        break;
    case IA2_TEXT_BOUNDARY_LINE:
        lUnoBoundaryType = 5; // LINE;
        break;
    case IA2_TEXT_BOUNDARY_PARAGRAPH:
        lUnoBoundaryType = 4;
        break;
    case IA2_TEXT_BOUNDARY_ALL:
        {
            long nChar;
            get_nCharacters( &nChar );
            *startOffset = 0;
            *endOffset = nChar;
            return get_text(0, nChar, text);
        }
        break;
    default:
        return E_FAIL;
    }

    TextSegment segment = GetXInterface()->getTextBeforeIndex( offset, sal_Int16(lUnoBoundaryType));
    ::rtl::OUString ouStr = segment.SegmentText;
    SysFreeString(*text);
    *text = SysAllocString((OLECHAR*)ouStr.getStr());
    *startOffset = segment.SegmentStart;
    *endOffset = segment.SegmentEnd;

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get special text after some position.
   * @param offset Special position.
   * @param boundaryType Boundary type.
   * @param startOffset Variant to accept the start offset.
   * @param endOffset   Variant to accept the end offset.
   * @param text        Variant to accept the special text.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_textAfterOffset(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (startOffset == NULL || endOffset == NULL || text == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    // In New UNO IAccessibleText.idl these constant values are defined as follows:
    //
    //  const long TEXT_BOUNDARY_CHAR = -1;
    //  const long TEXT_BOUNDARY_TO_CURSOR_POS = -2;
    //  const long TEXT_BOUNDARY_START_OF_WORD = -3;
    //  const long TEXT_BOUNDARY_END_OF_WORD = -4;
    //  const long TEXT_BOUNDARY_START_OF_SENTENCE = -5;
    //  const long TEXT_BOUNDARY_END_OF_SENTENCE = -6;
    //  const long TEXT_BOUNDARY_START_OF_LINE = -7;
    //  const long TEXT_BOUNDARY_END_OF_LINE = -8;
    //
    // In UNO, the corresponding values are as follows:
    //
    //  const short CHARACTER = 1;
    //  const short WORD = 2;
    //  const short SENTENCE = 3;
    //  const short PARAGRAPH = 4;
    //  const short LINE = 5;
    //  const short GLYPH = 6;
    //  const short ATTRIBUTE_RUN = 7;
    //

    long			lUnoBoundaryType;
    switch(boundaryType)
    {
    case IA2_TEXT_BOUNDARY_CHAR:
        lUnoBoundaryType = 1; // CHARACTER;
        break;
    case IA2_TEXT_BOUNDARY_WORD:
        lUnoBoundaryType = 2; // WORD;
        break;
    case IA2_TEXT_BOUNDARY_SENTENCE:
        lUnoBoundaryType = 3; // SENTENCE;
        break;
    case IA2_TEXT_BOUNDARY_LINE:
        lUnoBoundaryType = 5; // LINE;
        break;
    case IA2_TEXT_BOUNDARY_PARAGRAPH:
        lUnoBoundaryType = 4;
        break;
    case IA2_TEXT_BOUNDARY_ALL:
        {
            long nChar;
            get_nCharacters( &nChar );
            *startOffset = 0;
            *endOffset = nChar;
            return get_text(0, nChar, text);
        }
        break;
    default:
        return E_FAIL;
    }

    TextSegment segment = GetXInterface()->getTextBehindIndex( offset, sal_Int16(lUnoBoundaryType));
    ::rtl::OUString ouStr = segment.SegmentText;
    SysFreeString(*text);
    *text = SysAllocString((OLECHAR*)ouStr.getStr());
    *startOffset = segment.SegmentStart;
    *endOffset = segment.SegmentEnd;

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get special text at some position.
   * @param offset Special position.
   * @param boundaryType Boundary type.
   * @param startOffset Variant to accept the start offset.
   * @param endOffset   Variant to accept the end offset.
   * @param text        Variant to accept the special text.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_textAtOffset(long offset, IA2TextBoundaryType boundaryType, long * startOffset, long * endOffset, BSTR * text)
{
    

	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (startOffset == NULL || text == NULL ||endOffset == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    // In New UNO IAccessibleText.idl these constant values are defined as follows:
    //
    //  const long TEXT_BOUNDARY_CHAR = -1;
    //  const long TEXT_BOUNDARY_TO_CURSOR_POS = -2;
    //  const long TEXT_BOUNDARY_START_OF_WORD = -3;
    //  const long TEXT_BOUNDARY_END_OF_WORD = -4;
    //  const long TEXT_BOUNDARY_START_OF_SENTENCE = -5;
    //  const long TEXT_BOUNDARY_END_OF_SENTENCE = -6;
    //  const long TEXT_BOUNDARY_START_OF_LINE = -7;
    //  const long TEXT_BOUNDARY_END_OF_LINE = -8;
    //
    // In UNO, the corresponding values are as follows:
    //
    //  const short CHARACTER = 1;
    //  const short WORD = 2;
    //  const short SENTENCE = 3;
    //  const short PARAGRAPH = 4;
    //  const short LINE = 5;
    //  const short GLYPH = 6;
    //  const short ATTRIBUTE_RUN = 7;
    //

    long			lUnoBoundaryType;

    switch(boundaryType)
    {
    case IA2_TEXT_BOUNDARY_CHAR:
        lUnoBoundaryType = 1; // CHARACTER;
        break;
    case IA2_TEXT_BOUNDARY_WORD:
        lUnoBoundaryType = 2; // WORD;
        break;
    case IA2_TEXT_BOUNDARY_SENTENCE:
        lUnoBoundaryType = 3; // SENTENCE;
        break;
    case IA2_TEXT_BOUNDARY_LINE:
        lUnoBoundaryType = 5; // LINE;
        break;
    case IA2_TEXT_BOUNDARY_PARAGRAPH:
        lUnoBoundaryType = 4;
        break;
    case IA2_TEXT_BOUNDARY_ALL:
        {
            long nChar;
            get_nCharacters( &nChar );
            *startOffset = 0;
            *endOffset = nChar;
            return get_text(0, nChar, text);
        }
        break;
    default:
        return E_FAIL;
    }

    TextSegment segment = GetXInterface()->getTextAtIndex( offset, sal_Int16(lUnoBoundaryType));
    ::rtl::OUString ouStr = segment.SegmentText;
    SysFreeString(*text);
    *text = SysAllocString((OLECHAR*)ouStr.getStr());
    *startOffset = segment.SegmentStart;
    *endOffset = segment.SegmentEnd;

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Remove selection.
   * @param selectionIndex Special selection index
   * @param success Variant to accept the memthod called result.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::removeSelection(long selectionIndex)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK XInterface#
    if(pUNOInterface == NULL)
    {
        return E_FAIL;
    }

    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();

    Reference< XAccessibleTextSelection > pRExtension(pRContext,UNO_QUERY);

    if( pRExtension.is() )
    {
        pRExtension->removeSelection(selectionIndex);
        return S_OK;
    }
	else
    {
        GetXInterface()->setSelection(0, 0);
        return S_OK;
    }

    return E_FAIL;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Set caret position.
   * @param offset Special position.
   * @param success Variant to accept the memthod called result.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::setCaretOffset(long offset)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK XInterface#
    if(!pRXText.is())
        return E_FAIL;

    GetXInterface()->setCaretPosition( offset);

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Set special selection.
   * @param selectionIndex Special selection index.
   * @param startOffset start position.
   * @param endOffset end position.
   * @param success Variant to accept the memthod called result.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::setSelection(long, long startOffset, long endOffset)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    // #CHECK XInterface#
    if(!pRXText.is())
    {
        return E_FAIL;
    }

    GetXInterface()->setSelection( startOffset,	endOffset );

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Get characters count.
   * @param nCharacters Variant to accept the characters count.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::get_nCharacters(long * nCharacters)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    if (nCharacters == NULL)
        return E_INVALIDARG;
    // #CHECK XInterface#
    if(!pRXText.is())
    {
        *nCharacters = 0;
        return S_OK;
    }

    *nCharacters = GetXInterface()->getCharacterCount();

    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

// added by qiuhd, 2006/07/03, for direver 07/11
STDMETHODIMP CAccTextBase::get_newText( IA2TextSegment *)
{
    return E_NOTIMPL;
}

STDMETHODIMP CAccTextBase::get_oldText( IA2TextSegment *)
{
    return E_NOTIMPL;
}

/**
   * Scroll to special sub-string .
   * @param startIndex Start index of sub string.
   * @param endIndex   End index of sub string.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::scrollSubstringToPoint(long, long, IA2CoordinateType, long, long )
{
    

    ENTER_PROTECTED_BLOCK

    return E_NOTIMPL;

    LEAVE_PROTECTED_BLOCK
}

STDMETHODIMP CAccTextBase::scrollSubstringTo(long, long, IA2ScrollType)
{
    

    ENTER_PROTECTED_BLOCK

    return E_NOTIMPL;

    LEAVE_PROTECTED_BLOCK
}

/**
   * Put UNO interface.
   * @param pXInterface UNO interface.
   * @return Result.
*/
STDMETHODIMP CAccTextBase::put_XInterface(long pXInterface)
{
    
	CHECK_ENABLE_INF

    ENTER_PROTECTED_BLOCK

    CUNOXWrapper::put_XInterface(pXInterface);
    //special query.
    if(pUNOInterface == NULL)
        return E_FAIL;
    Reference<XAccessibleContext> pRContext = pUNOInterface->getAccessibleContext();
    if( !pRContext.is() )
    {
        return E_FAIL;
    }
    Reference<XAccessibleText> pRXI(pRContext,UNO_QUERY);
    if( !pRXI.is() )
        pRXText = NULL;
    else
        pRXText = pRXI;
    return S_OK;

    LEAVE_PROTECTED_BLOCK
}

OUString ReplaceOneChar(OUString oldOUString, OUString replacedChar, OUString replaceStr)
{
    int iReplace = -1;
    iReplace = oldOUString.lastIndexOf(replacedChar);
    if (iReplace > -1)
    {
        for(;iReplace>-1;)
        {
            oldOUString = oldOUString.replaceAt(iReplace,1, replaceStr);
            iReplace=oldOUString.lastIndexOf(replacedChar,iReplace);
        }
    }
    return oldOUString;

}
OUString ReplaceFourChar(OUString oldOUString)
{
    oldOUString = ReplaceOneChar(oldOUString,OUString::createFromAscii("\\"),OUString::createFromAscii("\\\\"));
    oldOUString = ReplaceOneChar(oldOUString,OUString::createFromAscii(";"),OUString::createFromAscii("\\;"));
    oldOUString = ReplaceOneChar(oldOUString,OUString::createFromAscii("="),OUString::createFromAscii("\\="));
    oldOUString = ReplaceOneChar(oldOUString,OUString::createFromAscii(","),OUString::createFromAscii("\\,"));
    oldOUString = ReplaceOneChar(oldOUString,OUString::createFromAscii(":"),OUString::createFromAscii("\\:"));
    return oldOUString;
}
