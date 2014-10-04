/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <hintids.hxx>
#include <editeng/brushitem.hxx>
#include <flyfrm.hxx>
#include <rootfrm.hxx>
#include <txtfrm.hxx>
#include <sectfrm.hxx>
#include <section.hxx>
#include <viewsh.hxx>
#include <viewopt.hxx>
#include <doc.hxx>
#include <frmatr.hxx>
#include <pagefrm.hxx>
#include <pagedesc.hxx>
#include <fmtanchr.hxx>
#include <fldbas.hxx>
#include <dcontact.hxx>
#include <accmap.hxx>
#include <accfrmobjslist.hxx>
#include <accfrmobjmap.hxx>
#include <accframe.hxx>

using namespace sw::access;

// Regarding visibility (or in terms of accessibility: regarding the showing
// state): A frame is visible and therefore contained in the tree if its frame
// size overlaps with the visible area. The bounding box however is the
// frame's paint area.
sal_Int32 SwAccessibleFrame::GetChildCount( SwAccessibleMap& rAccMap,
                                            const SwRect& rVisArea,
                                            const SwFrm *pFrm,
                                            bool bInPagePreview )
{
    sal_Int32 nCount = 0;

    const SwAccessibleChildSList aVisList( rVisArea, *pFrm, rAccMap );

    SwAccessibleChildSList::const_iterator aIter( aVisList.begin() );
    while( aIter != aVisList.end() )
    {
        const SwAccessibleChild& rLower = *aIter;
        if( rLower.IsAccessible( bInPagePreview ) )
        {
            nCount++;
        }
        else if( rLower.GetSwFrm() )
        {
            // There are no unaccessible SdrObjects that count
            nCount += GetChildCount( rAccMap,
                                     rVisArea, rLower.GetSwFrm(),
                                     bInPagePreview );
        }
        ++aIter;
    }

    return nCount;
}

SwAccessibleChild SwAccessibleFrame::GetChild(
                                   SwAccessibleMap& rAccMap,
                                   const SwRect& rVisArea,
                                   const SwFrm& rFrm,
                                   sal_Int32& rPos,
                                   bool bInPagePreview )
{
    SwAccessibleChild aRet;

    if( rPos >= 0 )
    {
        if( SwAccessibleChildMap::IsSortingRequired( rFrm ) )
        {
            // We need a sorted list here
            const SwAccessibleChildMap aVisMap( rVisArea, rFrm, rAccMap );
            SwAccessibleChildMap::const_iterator aIter( aVisMap.begin() );
            while( aIter != aVisMap.end() && !aRet.IsValid() )
            {
                const SwAccessibleChild& rLower = (*aIter).second;
                if( rLower.IsAccessible( bInPagePreview ) )
                {
                    if( 0 == rPos )
                        aRet = rLower;
                    else
                        rPos--;
                }
                else if( rLower.GetSwFrm() )
                {
                    // There are no unaccessible SdrObjects that count
                    aRet = GetChild( rAccMap,
                                     rVisArea, *(rLower.GetSwFrm()), rPos,
                                     bInPagePreview );
                }
                ++aIter;
            }
        }
        else
        {
            // The unsorted list is sorted enough, because it returns lower
            // frames in the correct order.
            const SwAccessibleChildSList aVisList( rVisArea, rFrm, rAccMap );
            SwAccessibleChildSList::const_iterator aIter( aVisList.begin() );
            while( aIter != aVisList.end() && !aRet.IsValid() )
            {
                const SwAccessibleChild& rLower = *aIter;
                if( rLower.IsAccessible( bInPagePreview ) )
                {
                    if( 0 == rPos )
                        aRet = rLower;
                    else
                        rPos--;
                }
                else if( rLower.GetSwFrm() )
                {
                    // There are no unaccessible SdrObjects that count
                    aRet = GetChild( rAccMap,
                                     rVisArea, *(rLower.GetSwFrm()), rPos,
                                     bInPagePreview );
                }
                ++aIter;
            }
        }
    }

    return aRet;
}

bool SwAccessibleFrame::GetChildIndex(
                                   SwAccessibleMap& rAccMap,
                                   const SwRect& rVisArea,
                                   const SwFrm& rFrm,
                                   const SwAccessibleChild& rChild,
                                   sal_Int32& rPos,
                                   bool bInPagePreview )
{
    bool bFound = false;

    if( SwAccessibleChildMap::IsSortingRequired( rFrm ) )
    {
        // We need a sorted list here
        const SwAccessibleChildMap aVisMap( rVisArea, rFrm, rAccMap );
        SwAccessibleChildMap::const_iterator aIter( aVisMap.begin() );
        while( aIter != aVisMap.end() && !bFound )
        {
            const SwAccessibleChild& rLower = (*aIter).second;
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                if( rChild == rLower )
                    bFound = true;
                else
                    rPos++;
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                bFound = GetChildIndex( rAccMap,
                                        rVisArea, *(rLower.GetSwFrm()), rChild,
                                        rPos, bInPagePreview );
            }
            ++aIter;
        }
    }
    else
    {
        // The unsorted list is sorted enough, because it returns lower
        // frames in the correct order.

        const SwAccessibleChildSList aVisList( rVisArea, rFrm, rAccMap );
        SwAccessibleChildSList::const_iterator aIter( aVisList.begin() );
        while( aIter != aVisList.end() && !bFound )
        {
            const SwAccessibleChild& rLower = *aIter;
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                if( rChild == rLower )
                    bFound = true;
                else
                    rPos++;
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                bFound = GetChildIndex( rAccMap,
                                        rVisArea, *(rLower.GetSwFrm()), rChild,
                                        rPos, bInPagePreview );
            }
            ++aIter;
        }
    }

    return bFound;
}

SwAccessibleChild SwAccessibleFrame::GetChildAtPixel( const SwRect& rVisArea,
                                          const SwFrm& rFrm,
                                          const Point& rPixPos,
                                          bool bInPagePreview,
                                          SwAccessibleMap& rAccMap )
{
    SwAccessibleChild aRet;

    if( SwAccessibleChildMap::IsSortingRequired( rFrm ) )
    {
        // We need a sorted list here, and we have to reverse iterate,
        // because objects in front should be returned.
        const SwAccessibleChildMap aVisMap( rVisArea, rFrm, rAccMap );
        SwAccessibleChildMap::const_reverse_iterator aRIter( aVisMap.rbegin() );
        while( aRIter != aVisMap.rend() && !aRet.IsValid() )
        {
            const SwAccessibleChild& rLower = (*aRIter).second;
            // A frame is returned if it's frame size is inside the visarea
            // and the position is inside the frame's paint area.
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                SwRect aLogBounds( rLower.GetBounds( rAccMap ) );
                if( !aLogBounds.IsEmpty() )
                {
                    Rectangle aPixBounds( rAccMap.CoreToPixel( aLogBounds.SVRect() ) );
                    if( aPixBounds.IsInside( rPixPos ) )
                        aRet = rLower;
                }
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                aRet = GetChildAtPixel( rVisArea, *(rLower.GetSwFrm()), rPixPos,
                                        bInPagePreview, rAccMap );
            }
            ++aRIter;
        }
    }
    else
    {
        // The unsorted list is sorted enough, because it returns lower
        // frames in the correct order. Morover, we can iterate forward,
        // because the lowers don't overlap!
        const SwAccessibleChildSList aVisList( rVisArea, rFrm, rAccMap );
        SwAccessibleChildSList::const_iterator aIter( aVisList.begin() );
        while( aIter != aVisList.end() && !aRet.IsValid() )
        {
            const SwAccessibleChild& rLower = *aIter;
            // A frame is returned if it's frame size is inside the visarea
            // and the position is inside the frame's paint area.
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                SwRect aLogBounds( rLower.GetBounds( rAccMap ) );
                if( !aLogBounds.IsEmpty() )
                {
                    Rectangle aPixBounds( rAccMap.CoreToPixel( aLogBounds.SVRect() ) );
                    if( aPixBounds.IsInside( rPixPos ) )
                        aRet = rLower;
                }
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                aRet = GetChildAtPixel( rVisArea, *(rLower.GetSwFrm()), rPixPos,
                                   bInPagePreview, rAccMap );
            }
            ++aIter;
        }
    }

    return aRet;
}

void SwAccessibleFrame::GetChildren( SwAccessibleMap& rAccMap,
                                     const SwRect& rVisArea,
                                     const SwFrm& rFrm,
                                     ::std::list< SwAccessibleChild >& rChildren,
                                     bool bInPagePreview )
{
    if( SwAccessibleChildMap::IsSortingRequired( rFrm ) )
    {
        // We need a sorted list here
        const SwAccessibleChildMap aVisMap( rVisArea, rFrm, rAccMap );
        SwAccessibleChildMap::const_iterator aIter( aVisMap.begin() );
        while( aIter != aVisMap.end() )
        {
            const SwAccessibleChild& rLower = (*aIter).second;
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                rChildren.push_back( rLower );
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                GetChildren( rAccMap, rVisArea, *(rLower.GetSwFrm()),
                             rChildren, bInPagePreview );
            }
            ++aIter;
        }
    }
    else
    {
        // The unsorted list is sorted enough, because it returns lower
        // frames in the correct order.
        const SwAccessibleChildSList aVisList( rVisArea, rFrm, rAccMap );
        SwAccessibleChildSList::const_iterator aIter( aVisList.begin() );
        while( aIter != aVisList.end() )
        {
            const SwAccessibleChild& rLower = *aIter;
            if( rLower.IsAccessible( bInPagePreview ) )
            {
                rChildren.push_back( rLower );
            }
            else if( rLower.GetSwFrm() )
            {
                // There are no unaccessible SdrObjects that count
                GetChildren( rAccMap, rVisArea, *(rLower.GetSwFrm()),
                             rChildren, bInPagePreview );
            }
            ++aIter;
        }
    }
}

SwRect SwAccessibleFrame::GetBounds( const SwAccessibleMap& rAccMap,
                                     const SwFrm *pFrm )
{
    if( !pFrm )
        pFrm = GetFrm();

    SwAccessibleChild aFrm( pFrm );
    SwRect aBounds( aFrm.GetBounds( rAccMap ).Intersection( maVisArea ) );
    return aBounds;
}

bool SwAccessibleFrame::IsEditable( SwViewShell *pVSh ) const
{
    const SwFrm *pFrm = GetFrm();
    if( !pFrm )
        return false;

    OSL_ENSURE( pVSh, "no view shell" );
    if( pVSh && (pVSh->GetViewOptions()->IsReadonly() ||
                 pVSh->IsPreview()) )
        return false;

    if( !pFrm->IsRootFrm() && pFrm->IsProtected() )
        return false;

    return true;
}

bool SwAccessibleFrame::IsOpaque( SwViewShell *pVSh ) const
{
    SwAccessibleChild aFrm( GetFrm() );
    if( !aFrm.GetSwFrm() )
        return false;

    OSL_ENSURE( pVSh, "no view shell" );
    if( !pVSh )
        return false;

    const SwViewOption *pVOpt = pVSh->GetViewOptions();
    do
    {
        const SwFrm *pFrm = aFrm.GetSwFrm();
        if( pFrm->IsRootFrm() )
            return true;

        if( pFrm->IsPageFrm() && !pVOpt->IsPageBack() )
            return false;

        const SvxBrushItem &rBack = pFrm->GetAttrSet()->GetBackground();
        if( !rBack.GetColor().GetTransparency() ||
             rBack.GetGraphicPos() != GPOS_NONE )
            return true;

        // If a fly frame has a transparent background color, we have to consider the background.
        // But a background color "no fill"/"auto fill" should *not* be considered.
        if( pFrm->IsFlyFrm() &&
            (rBack.GetColor().GetTransparency() != 0) &&
            (rBack.GetColor() != COL_TRANSPARENT)
          )
            return true;

        if( pFrm->IsSctFrm() )
        {
            const SwSection* pSection = ((SwSectionFrm*)pFrm)->GetSection();
            if( pSection && ( TOX_HEADER_SECTION == pSection->GetType() ||
                TOX_CONTENT_SECTION == pSection->GetType() ) &&
                !pVOpt->IsReadonly() &&
                SwViewOption::IsIndexShadings() )
                return true;
        }
        if( pFrm->IsFlyFrm() )
            aFrm = static_cast<const SwFlyFrm*>(pFrm)->GetAnchorFrm();
        else
            aFrm = pFrm->GetUpper();
    } while( aFrm.GetSwFrm() && !aFrm.IsAccessible( IsInPagePreview() ) );

    return false;
}

SwAccessibleFrame::SwAccessibleFrame( const SwRect& rVisArea,
                                      const SwFrm *pF,
                                      bool bIsPagePreview ) :
    maVisArea( rVisArea ),
    mpFrm( pF ),
    mbIsInPagePreview( bIsPagePreview ),
    bIsAccDocUse( false )
{
}

SwAccessibleFrame::~SwAccessibleFrame()
{
}

const SwFrm* SwAccessibleFrame::GetParent( const SwAccessibleChild& rFrmOrObj,
                                           bool bInPagePreview )
{
    return rFrmOrObj.GetParent( bInPagePreview );
}

OUString SwAccessibleFrame::GetFormattedPageNumber() const
{
    sal_uInt16 nPageNum = GetFrm()->GetVirtPageNum();
    sal_uInt32 nFmt = GetFrm()->FindPageFrm()->GetPageDesc()
                              ->GetNumType().GetNumberingType();
    if( SVX_NUM_NUMBER_NONE == nFmt )
        nFmt = SVX_NUM_ARABIC;

    OUString sRet( FormatNumber( nPageNum, nFmt ) );
    return sRet;
}

sal_Int32 SwAccessibleFrame::GetChildCount( SwAccessibleMap& rAccMap ) const
{
    return GetChildCount( rAccMap, maVisArea, mpFrm, IsInPagePreview() );
}

sw::access::SwAccessibleChild SwAccessibleFrame::GetChild(
                                                SwAccessibleMap& rAccMap,
                                                sal_Int32 nPos ) const
{
    return SwAccessibleFrame::GetChild( rAccMap, maVisArea, *mpFrm, nPos, IsInPagePreview() );
}

sal_Int32 SwAccessibleFrame::GetChildIndex( SwAccessibleMap& rAccMap,
                                            const sw::access::SwAccessibleChild& rChild ) const
{
    sal_Int32 nPos = 0;
    return GetChildIndex( rAccMap, maVisArea, *mpFrm, rChild, nPos, IsInPagePreview() )
           ? nPos
           : -1L;
}

sw::access::SwAccessibleChild SwAccessibleFrame::GetChildAtPixel(
                                                const Point& rPos,
                                                SwAccessibleMap& rAccMap ) const
{
    return GetChildAtPixel( maVisArea, *mpFrm, rPos, IsInPagePreview(), rAccMap );
}

void SwAccessibleFrame::GetChildren( SwAccessibleMap& rAccMap,
                                     ::std::list< sw::access::SwAccessibleChild >& rChildren ) const
{
    GetChildren( rAccMap, maVisArea, *mpFrm, rChildren, IsInPagePreview() );
}

bool SwAccessibleFrame::IsShowing( const SwAccessibleMap& rAccMap,
                                       const sw::access::SwAccessibleChild& rFrmOrObj ) const
{
    return IsShowing( rFrmOrObj.GetBox( rAccMap ) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
