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
#include <editeng/protitem.hxx>
#include <com/sun/star/i18n/WordType.hpp>
#include <com/sun/star/i18n/CharType.hpp>
#include <unotools/charclass.hxx>
#include <svl/ctloptions.hxx>
#include <swmodule.hxx>
#include <fmtcntnt.hxx>
#include <swtblfmt.hxx>
#include <swcrsr.hxx>
#include <unocrsr.hxx>
#include <doc.hxx>
#include <IDocumentUndoRedo.hxx>
#include <IDocumentRedlineAccess.hxx>
#include <IDocumentLayoutAccess.hxx>
#include <docary.hxx>
#include <ndtxt.hxx>
#include <section.hxx>
#include <swtable.hxx>
#include <cntfrm.hxx>
#include <rootfrm.hxx>
#include <txtfrm.hxx>
#include <scriptinfo.hxx>
#include <crstate.hxx>
#include <docsh.hxx>
#include <viewsh.hxx>
#include <frmatr.hxx>
#include <breakit.hxx>
#include <crsskip.hxx>
#include <vcl/msgbox.hxx>
#include <mdiexp.hxx>
#include <statstr.hrc>
#include <redline.hxx>
#include <txatbase.hxx>

using namespace ::com::sun::star::i18n;

static const sal_uInt16 coSrchRplcThreshold = 60000;

struct _PercentHdl
{
    SwDocShell* pDSh;
    sal_uLong nActPos;
    bool bBack, bNodeIdx;

    _PercentHdl( sal_uLong nStt, sal_uLong nEnd, SwDocShell* pSh )
        : pDSh(pSh), bBack(false), bNodeIdx(false)
    {
        nActPos = nStt;
        if( ( bBack = (nStt > nEnd )) )
        {
            sal_uLong n = nStt; nStt = nEnd; nEnd = n;
        }
        ::StartProgress( STR_STATSTR_SEARCH, nStt, nEnd, 0 );
    }

    _PercentHdl( const SwPaM& rPam )
        : pDSh( (SwDocShell*)rPam.GetDoc()->GetDocShell() )
    {
        sal_uLong nStt, nEnd;
        if( rPam.GetPoint()->nNode == rPam.GetMark()->nNode )
        {
            bNodeIdx = false;
            nStt = rPam.GetMark()->nContent.GetIndex();
            nEnd = rPam.GetPoint()->nContent.GetIndex();
        }
        else
        {
            bNodeIdx = true;
            nStt = rPam.GetMark()->nNode.GetIndex();
            nEnd = rPam.GetPoint()->nNode.GetIndex();
        }
        nActPos = nStt;
        if( ( bBack = (nStt > nEnd )) )
        {
            sal_uLong n = nStt; nStt = nEnd; nEnd = n;
        }
        ::StartProgress( STR_STATSTR_SEARCH, nStt, nEnd, pDSh );
    }

    ~_PercentHdl()                      { ::EndProgress( pDSh ); }

    void NextPos( sal_uLong nPos ) const
        { ::SetProgressState( bBack ? nActPos - nPos : nPos, pDSh ); }

    void NextPos( SwPosition& rPos ) const
        {
            sal_uLong nPos;
            if( bNodeIdx )
                nPos = rPos.nNode.GetIndex();
            else
                nPos = rPos.nContent.GetIndex();
            ::SetProgressState( bBack ? nActPos - nPos : nPos, pDSh );
        }
};

SwCursor::SwCursor( const SwPosition &rPos, SwPaM* pRing, bool bColumnSel )
    : SwPaM( rPos, pRing ), pSavePos( 0 ), mnRowSpanOffset( 0 ), nCursorBidiLevel( 0 ),
    mbColumnSelection( bColumnSel )
{
}

// @@@ semantic: no copy ctor.
SwCursor::SwCursor( SwCursor& rCpy )
    : SwPaM( rCpy ), pSavePos( 0 ), mnRowSpanOffset( rCpy.mnRowSpanOffset ),
    nCursorBidiLevel( rCpy.nCursorBidiLevel ), mbColumnSelection( rCpy.mbColumnSelection )
{
}

SwCursor::~SwCursor()
{
    while( pSavePos )
    {
        _SwCursor_SavePos* pNxt = pSavePos->pNext;
        delete pSavePos;
        pSavePos = pNxt;
    }
}

SwCursor* SwCursor::Create( SwPaM* pRing ) const
{
    return new SwCursor( *GetPoint(), pRing, false );
}

bool SwCursor::IsReadOnlyAvailable() const
{
    return false;
}

bool SwCursor::IsSkipOverHiddenSections() const
{
    return true;
}

bool SwCursor::IsSkipOverProtectSections() const
{
    return !IsReadOnlyAvailable();
}

// CreateNewSavePos is virtual so that derived classes of cursor can implement
// own SaveObjects if needed and validate them in the virtual check routines.
void SwCursor::SaveState()
{
    _SwCursor_SavePos* pNew = CreateNewSavePos();
    pNew->pNext = pSavePos;
    pSavePos = pNew;
}

void SwCursor::RestoreState()
{
    if( pSavePos ) // Robust
    {
        _SwCursor_SavePos* pDel = pSavePos;
        pSavePos = pSavePos->pNext;
        delete pDel;
    }
}

_SwCursor_SavePos* SwCursor::CreateNewSavePos() const
{
    return new _SwCursor_SavePos( *this );
}

/// determine if point is outside of the node-array's content area
bool SwCursor::IsNoCntnt() const
{
    return GetPoint()->nNode.GetIndex() <
            GetDoc()->GetNodes().GetEndOfExtras().GetIndex();
}

bool SwCursor::IsSelOvrCheck(int)
{
    return false;
}

// extracted from IsSelOvr()
bool SwTableCursor::IsSelOvrCheck(int eFlags)
{
    SwNodes& rNds = GetDoc()->GetNodes();
    // check sections of nodes array
    if( (nsSwCursorSelOverFlags::SELOVER_CHECKNODESSECTION & eFlags)
        && HasMark() )
    {
        SwNodeIndex aOldPos( rNds, GetSavePos()->nNode );
        if( !CheckNodesRange( aOldPos, GetPoint()->nNode, true ))
        {
            GetPoint()->nNode = aOldPos;
            GetPoint()->nContent.Assign( GetCntntNode(), GetSavePos()->nCntnt );
            return true;
        }
    }
    return SwCursor::IsSelOvrCheck(eFlags);
}

bool SwCursor::IsSelOvr( int eFlags )
{
    SwDoc* pDoc = GetDoc();
    SwNodes& rNds = pDoc->GetNodes();

    bool bSkipOverHiddenSections = IsSkipOverHiddenSections();
    bool bSkipOverProtectSections = IsSkipOverProtectSections();

    if ( IsSelOvrCheck( eFlags ) )
    {
        return true;
    }

    if( pSavePos->nNode != GetPoint()->nNode.GetIndex() &&
        // (1997) in UI-ReadOnly everything is allowed
        ( !pDoc->GetDocShell() || !pDoc->GetDocShell()->IsReadOnlyUI() ))
    {
        // check new sections
        SwNodeIndex& rPtIdx = GetPoint()->nNode;
        const SwSectionNode* pSectNd = rPtIdx.GetNode().FindSectionNode();
        if( pSectNd &&
            ((bSkipOverHiddenSections && pSectNd->GetSection().IsHiddenFlag() ) ||
            (bSkipOverProtectSections && pSectNd->GetSection().IsProtectFlag() )))
        {
            if( 0 == ( nsSwCursorSelOverFlags::SELOVER_CHANGEPOS & eFlags ) )
            {
                // then we're already done
                RestoreSavePos();
                return true;
            }

            // set cursor to new position:
            SwNodeIndex aIdx( rPtIdx );
            sal_Int32 nCntntPos = pSavePos->nCntnt;
            bool bGoNxt = pSavePos->nNode < rPtIdx.GetIndex();
            SwCntntNode* pCNd = bGoNxt
                ? rNds.GoNextSection( &rPtIdx, bSkipOverHiddenSections, bSkipOverProtectSections)
                : rNds.GoPrevSection( &rPtIdx, bSkipOverHiddenSections, bSkipOverProtectSections);
            if( !pCNd && ( nsSwCursorSelOverFlags::SELOVER_ENABLEREVDIREKTION & eFlags ))
            {
                bGoNxt = !bGoNxt;
                pCNd = bGoNxt ? rNds.GoNextSection( &rPtIdx, bSkipOverHiddenSections, bSkipOverProtectSections)
                    : rNds.GoPrevSection( &rPtIdx, bSkipOverHiddenSections, bSkipOverProtectSections);
            }

            bool bIsValidPos = 0 != pCNd;
            const bool bValidNodesRange = bIsValidPos &&
                ::CheckNodesRange( rPtIdx, aIdx, true );
            if( !bValidNodesRange )
            {
                rPtIdx = pSavePos->nNode;
                if( 0 == ( pCNd = rPtIdx.GetNode().GetCntntNode() ) )
                {
                    bIsValidPos = false;
                    nCntntPos = 0;
                    rPtIdx = aIdx;
                    if( 0 == ( pCNd = rPtIdx.GetNode().GetCntntNode() ) )
                    {
                        // then to the beginning of the document
                        rPtIdx = rNds.GetEndOfExtras();
                        pCNd = rNds.GoNext( &rPtIdx );
                    }
                }
            }

            // register ContentIndex:
            const sal_Int32 nTmpPos = bIsValidPos ? (bGoNxt ? 0 : pCNd->Len()) : nCntntPos;
            GetPoint()->nContent.Assign( pCNd, nTmpPos );
            if( !bIsValidPos || !bValidNodesRange ||
                IsInProtectTable( true ) )
                return true;
        }

        // is there a protected section in the section?
        if( HasMark() && bSkipOverProtectSections)
        {
            sal_uLong nSttIdx = GetMark()->nNode.GetIndex(),
                nEndIdx = GetPoint()->nNode.GetIndex();
            if( nEndIdx <= nSttIdx )
            {
                sal_uLong nTmp = nSttIdx;
                nSttIdx = nEndIdx;
                nEndIdx = nTmp;
            }

            const SwSectionFmts& rFmts = pDoc->GetSections();
            for( sal_uInt16 n = 0; n < rFmts.size(); ++n )
            {
                const SwSectionFmt* pFmt = rFmts[n];
                const SvxProtectItem& rProtect = pFmt->GetProtect();
                if( rProtect.IsCntntProtected() )
                {
                    const SwFmtCntnt& rCntnt = pFmt->GetCntnt(false);
                    OSL_ENSURE( rCntnt.GetCntntIdx(), "No SectionNode?" );
                    sal_uLong nIdx = rCntnt.GetCntntIdx()->GetIndex();
                    if( nSttIdx <= nIdx && nEndIdx >= nIdx )
                    {
                        // if it is no linked section then we cannot select it
                        const SwSection& rSect = *pFmt->GetSection();
                        if( CONTENT_SECTION == rSect.GetType() )
                        {
                            RestoreSavePos();
                            return true;
                        }
                    }
                }
            }
        }
    }

    const SwNode* pNd = &GetPoint()->nNode.GetNode();
    if( pNd->IsCntntNode() && !dynamic_cast<SwUnoCrsr*>(this) )
    {
        const SwCntntFrm* pFrm = static_cast<const SwCntntNode*>(pNd)->getLayoutFrm( pDoc->getIDocumentLayoutAccess().GetCurrentLayout() );
        if( pFrm && pFrm->IsValid()
            && 0 == pFrm->Frm().Height()
            && 0 != ( nsSwCursorSelOverFlags::SELOVER_CHANGEPOS & eFlags ) )
        {
            // skip to the next/prev valid paragraph with a layout
            SwNodeIndex& rPtIdx = GetPoint()->nNode;
            bool bGoNxt = pSavePos->nNode < rPtIdx.GetIndex();
            while( 0 != ( pFrm = ( bGoNxt ? pFrm->GetNextCntntFrm() : pFrm->GetPrevCntntFrm() ))
                   && 0 == pFrm->Frm().Height() )
                ;

            // #i72394# skip to prev/next valid paragraph with a layout in case
            // the first search did not succeed:
            if( !pFrm )
            {
                bGoNxt = !bGoNxt;
                pFrm = static_cast<const SwCntntNode*>(pNd)->getLayoutFrm( pDoc->getIDocumentLayoutAccess().GetCurrentLayout() );
                while ( pFrm && 0 == pFrm->Frm().Height() )
                {
                    pFrm = bGoNxt ? pFrm->GetNextCntntFrm()
                        :   pFrm->GetPrevCntntFrm();
                }
            }

            SwCntntNode* pCNd = (pFrm != NULL) ? (SwCntntNode*)pFrm->GetNode() : NULL;
            if ( pCNd != NULL )
            {
                // set this CntntNode as new position
                rPtIdx = *pCNd;
                pNd = pCNd;

                // assign corresponding ContentIndex
                const sal_Int32 nTmpPos = bGoNxt ? 0 : pCNd->Len();
                GetPoint()->nContent.Assign( pCNd, nTmpPos );

                if ( rPtIdx.GetIndex() == pSavePos->nNode
                     && nTmpPos == pSavePos->nCntnt )
                {
                    // new position equals saved one
                    // --> trigger restore of saved pos by setting <pFrm> to NULL - see below
                    pFrm = NULL;
                }

                if ( IsInProtectTable( true ) )
                {
                    // new position in protected table
                    // --> trigger restore of saved pos by setting <pFrm> to NULL - see below
                    pFrm = NULL;
                }
            }
        }

        if( !pFrm )
        {
            DeleteMark();
            RestoreSavePos();
            return true; // we need a frame
        }
    }

    // is the cursor allowed to be in a protected node?
    if( 0 == ( nsSwCursorSelOverFlags::SELOVER_CHANGEPOS & eFlags ) && !IsAtValidPos() )
    {
        DeleteMark();
        RestoreSavePos();
        return true;
    }

    if( !HasMark() )
        return false;

    // check for invalid sections
    if( !::CheckNodesRange( GetMark()->nNode, GetPoint()->nNode, true ))
    {
        DeleteMark();
        RestoreSavePos();
        return true; // we need a frame
    }

    if( (pNd = &GetMark()->nNode.GetNode())->IsCntntNode()
        && !static_cast<const SwCntntNode*>(pNd)->getLayoutFrm( pDoc->getIDocumentLayoutAccess().GetCurrentLayout() )
        && !dynamic_cast<SwUnoCrsr*>(this) )
    {
        DeleteMark();
        RestoreSavePos();
        return true; // we need a frame
    }

    // assure that selection is only inside an InputField or contains the InputField completely
    {
        const SwTxtAttr* pInputFldTxtAttrAtPoint = NULL;
        SwTxtNode* pTxtNdAtPoint = GetPoint()->nNode.GetNode().GetTxtNode();
        if ( pTxtNdAtPoint != NULL )
        {
            pInputFldTxtAttrAtPoint =
                pTxtNdAtPoint->GetTxtAttrAt( GetPoint()->nContent.GetIndex(), RES_TXTATR_INPUTFIELD, SwTxtNode::PARENT );
        }

        const SwTxtAttr* pInputFldTxtAttrAtMark = NULL;
        SwTxtNode* pTxtNdAtMark = GetMark()->nNode.GetNode().GetTxtNode();
        if ( pTxtNdAtMark != NULL )
        {
            pInputFldTxtAttrAtMark =
                pTxtNdAtMark->GetTxtAttrAt( GetMark()->nContent.GetIndex(), RES_TXTATR_INPUTFIELD, SwTxtNode::PARENT );
        }

        if ( pInputFldTxtAttrAtPoint != pInputFldTxtAttrAtMark )
        {
            const sal_uLong nRefNodeIdx =
                ( nsSwCursorSelOverFlags::SELOVER_TOGGLE & eFlags )
                ? pSavePos->nNode
                : GetMark()->nNode.GetIndex();
            const sal_Int32 nRefContentIdx =
                ( nsSwCursorSelOverFlags::SELOVER_TOGGLE & eFlags )
                ? pSavePos->nCntnt
                : GetMark()->nContent.GetIndex();
            const bool bIsForwardSelection =
                nRefNodeIdx < GetPoint()->nNode.GetIndex()
                || ( nRefNodeIdx == GetPoint()->nNode.GetIndex()
                     && nRefContentIdx < GetPoint()->nContent.GetIndex() );

            if ( pInputFldTxtAttrAtPoint != NULL )
            {
                const sal_Int32 nNewPointPos =
                    bIsForwardSelection ? *(pInputFldTxtAttrAtPoint->End()) : pInputFldTxtAttrAtPoint->GetStart();
                GetPoint()->nContent.Assign( pTxtNdAtPoint, nNewPointPos );
            }

            if ( pInputFldTxtAttrAtMark != NULL )
            {
                const sal_Int32 nNewMarkPos =
                    bIsForwardSelection ? pInputFldTxtAttrAtMark->GetStart() : *(pInputFldTxtAttrAtMark->End());
                GetMark()->nContent.Assign( pTxtNdAtMark, nNewMarkPos );
            }
        }
    }

    const SwTableNode* pPtNd = GetPoint()->nNode.GetNode().FindTableNode();
    const SwTableNode* pMrkNd = GetMark()->nNode.GetNode().FindTableNode();
    // both in no or in same table node
    if( ( !pMrkNd && !pPtNd ) || pPtNd == pMrkNd )
        return false;

    // in different tables or only mark in table
    if( pMrkNd )
    {
        // not allowed, so go back to old position
        RestoreSavePos();
        // Crsr stays at old position
        return true;
    }

    // Note: this cannot happen in TableMode
    // Only Point in Table then go behind/in front of table
    if (nsSwCursorSelOverFlags::SELOVER_CHANGEPOS & eFlags)
    {
        bool bSelTop = GetPoint()->nNode.GetIndex() <
            (( nsSwCursorSelOverFlags::SELOVER_TOGGLE & eFlags ) ? pSavePos->nNode
            : GetMark()->nNode.GetIndex());

        do { // loop for table after table
            sal_uLong nSEIdx = pPtNd->EndOfSectionIndex();
            sal_uLong nSttEndTbl = nSEIdx + 1;

            if( bSelTop )
                nSttEndTbl = rNds[ nSEIdx ]->StartOfSectionIndex() - 1;

            GetPoint()->nNode = nSttEndTbl;
            const SwNode* pMyNd = &(GetNode());

            if( pMyNd->IsSectionNode() || ( pMyNd->IsEndNode() &&
                pMyNd->StartOfSectionNode()->IsSectionNode() ) )
            {
                pMyNd = bSelTop
                    ? rNds.GoPrevSection( &GetPoint()->nNode,true,false )
                    : rNds.GoNextSection( &GetPoint()->nNode,true,false );

                /* #i12312# Handle failure of Go{Prev|Next}Section */
                if ( 0 == pMyNd)
                    break;

                if( 0 != ( pPtNd = pMyNd->FindTableNode() ))
                    continue;
            }

            // we permit these
            if( pMyNd->IsCntntNode() &&
                ::CheckNodesRange( GetMark()->nNode,
                GetPoint()->nNode, true ))
            {
                // table in table
                const SwTableNode* pOuterTableNd = pMyNd->FindTableNode();
                if ( pOuterTableNd )
                    pMyNd = pOuterTableNd;
                else
                {
                    SwCntntNode* pCNd = const_cast<SwCntntNode*>(static_cast<const SwCntntNode*>(pMyNd));
                    GetPoint()->nContent.Assign( pCNd, bSelTop ? pCNd->Len() : 0 );
                    return false;
                }
            }
            if( bSelTop
                ? ( !pMyNd->IsEndNode() || 0 == ( pPtNd = pMyNd->FindTableNode() ))
                : 0 == ( pPtNd = pMyNd->GetTableNode() ))
                break;
        } while( true );
    }

    // stay on old position
    RestoreSavePos();
    return true;
}

bool SwCursor::IsInProtectTable( bool bMove, bool bChgCrsr )
{
    SwCntntNode* pCNd = GetCntntNode();
    if( !pCNd )
        return false;

    // No table, no protected cell:
    const SwTableNode* pTableNode = pCNd->FindTableNode();
    if ( !pTableNode )
        return false;

    // Current position == last save position?
    if ( pSavePos->nNode == GetPoint()->nNode.GetIndex() )
        return false;

    // Check for convered cell:
    bool bInCoveredCell = false;
    const SwStartNode* pTmpSttNode = pCNd->FindTableBoxStartNode();
    OSL_ENSURE( pTmpSttNode, "In table, therefore I expect to get a SwTableBoxStartNode" );
    const SwTableBox* pBox = pTmpSttNode ? pTableNode->GetTable().GetTblBox( pTmpSttNode->GetIndex() ) : 0; //Robust #151355
    if ( pBox && pBox->getRowSpan() < 1 ) // Robust #151270
        bInCoveredCell = true;

    // Positions of covered cells are not acceptable:
    if ( !bInCoveredCell )
    {
        // Position not protected?
        if ( !pCNd->IsProtect() )
            return false;

        // Cursor in protected cells allowed?
        if ( IsReadOnlyAvailable() )
            return false;
    }

    // If we reach this point, we are in a protected or covered table cell!

    if( !bMove )
    {
        if( bChgCrsr )
            // restore the last save position
            RestoreSavePos();

        return true; // Crsr stays at old position
    }

    // We are in a protected table cell. Traverse top to bottom?
    if( pSavePos->nNode < GetPoint()->nNode.GetIndex() )
    {
        // search next valid box
        // if there is another StartNode after the EndNode of a cell then
        // there is another cell
        SwNodeIndex aCellStt( *GetNode().FindTableBoxStartNode()->EndOfSectionNode(), 1 );
        bool bProt = true;
GoNextCell:
        do {
            if( !aCellStt.GetNode().IsStartNode() )
                break;
            ++aCellStt;
            if( 0 == ( pCNd = aCellStt.GetNode().GetCntntNode() ))
                pCNd = aCellStt.GetNodes().GoNext( &aCellStt );
            if( !( bProt = pCNd->IsProtect() ))
                break;
            aCellStt.Assign( *pCNd->FindTableBoxStartNode()->EndOfSectionNode(), 1 );
        } while( bProt );

SetNextCrsr:
        if( !bProt ) // found free cell
        {
            GetPoint()->nNode = aCellStt;
            SwCntntNode* pTmpCNd = GetCntntNode();
            if( pTmpCNd )
            {
                GetPoint()->nContent.Assign( pTmpCNd, 0 );
                return false;
            }
            return IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                             nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
        }
        // end of table, so go to next node
        ++aCellStt;
        SwNode* pNd;
        if( ( pNd = &aCellStt.GetNode())->IsEndNode() || HasMark())
        {
            // if only table in FlyFrame or SSelection then stay on old position
            if( bChgCrsr )
                RestoreSavePos();
            return true;
        }
        else if( pNd->IsTableNode() && aCellStt++ )
            goto GoNextCell;

        bProt = false; // index is now on a content node
        goto SetNextCrsr;
    }

    // search for the previous valid box
    {
        // if there is another EndNode in front of the StartNode than there
        // exists a previous cell
        SwNodeIndex aCellStt( *GetNode().FindTableBoxStartNode(), -1 );
        SwNode* pNd;
        bool bProt = true;
GoPrevCell:
        do {
            if( !( pNd = &aCellStt.GetNode())->IsEndNode() )
                break;
            aCellStt.Assign( *pNd->StartOfSectionNode(), +1 );
            if( 0 == ( pCNd = aCellStt.GetNode().GetCntntNode() ))
                pCNd = pNd->GetNodes().GoNext( &aCellStt );
            if( !( bProt = pCNd->IsProtect() ))
                break;
            aCellStt.Assign( *pNd->FindTableBoxStartNode(), -1 );
        } while( bProt );

SetPrevCrsr:
        if( !bProt ) // found free cell
        {
            GetPoint()->nNode = aCellStt;
            SwCntntNode* pTmpCNd = GetCntntNode();
            if( pTmpCNd )
            {
                GetPoint()->nContent.Assign( pTmpCNd, 0 );
                return false;
            }
            return IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                             nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
        }
        // at the beginning of a table, so go to next node
        aCellStt--;
        if( ( pNd = &aCellStt.GetNode())->IsStartNode() || HasMark() )
        {
            // if only table in FlyFrame or SSelection then stay on old position
            if( bChgCrsr )
                RestoreSavePos();
            return true;
        }
        else if( pNd->StartOfSectionNode()->IsTableNode() && aCellStt-- )
            goto GoPrevCell;

        bProt = false; // index is now on a content node
        goto SetPrevCrsr;
    }
}

/// Return <true> if cursor can be set to this position
bool SwCursor::IsAtValidPos( bool bPoint ) const
{
    const SwDoc* pDoc = GetDoc();
    const SwPosition* pPos = bPoint ? GetPoint() : GetMark();
    const SwNode* pNd = &pPos->nNode.GetNode();

    if( pNd->IsCntntNode() && !static_cast<const SwCntntNode*>(pNd)->getLayoutFrm( pDoc->getIDocumentLayoutAccess().GetCurrentLayout() ) &&
        !dynamic_cast<const SwUnoCrsr*>(this) )
    {
        return false;
    }

    // #i45129# - in UI-ReadOnly everything is allowed
    if( !pDoc->GetDocShell() || !pDoc->GetDocShell()->IsReadOnlyUI() )
        return true;

    const bool bCrsrInReadOnly = IsReadOnlyAvailable();
    if( !bCrsrInReadOnly && pNd->IsProtect() )
        return false;

    const SwSectionNode* pSectNd = pNd->FindSectionNode();
    if( pSectNd && (pSectNd->GetSection().IsHiddenFlag() ||
                    ( !bCrsrInReadOnly && pSectNd->GetSection().IsProtectFlag() )))
        return false;

    return true;
}

void SwCursor::SaveTblBoxCntnt( const SwPosition* ) {}

/// set range for search in document
SwMoveFnCollection* SwCursor::MakeFindRange( SwDocPositions nStart,
                                SwDocPositions nEnd, SwPaM* pRange ) const
{
    pRange->SetMark();
    FillFindPos( nStart, *pRange->GetMark() );
    FillFindPos( nEnd, *pRange->GetPoint() );

    // determine direction of search
    return ( DOCPOS_START == nStart || DOCPOS_OTHERSTART == nStart ||
              (DOCPOS_CURR == nStart &&
                (DOCPOS_END == nEnd || DOCPOS_OTHEREND == nEnd ) ))
                ? fnMoveForward : fnMoveBackward;
}

static sal_uLong lcl_FindSelection( SwFindParas& rParas, SwCursor* pCurCrsr,
                        SwMoveFn fnMove, SwCursor*& pFndRing,
                        SwPaM& aRegion, FindRanges eFndRngs,
                        bool bInReadOnly, bool& bCancel )
{
    SwDoc* pDoc = pCurCrsr->GetDoc();
    bool const bDoesUndo = pDoc->GetIDocumentUndoRedo().DoesUndo();
    int nFndRet = 0;
    sal_uLong nFound = 0;
    const bool bSrchBkwrd = fnMove == fnMoveBackward;
    SwPaM *pTmpCrsr = pCurCrsr, *pSaveCrsr = pCurCrsr;

    // only create progress bar for ShellCrsr
    bool bIsUnoCrsr = 0 != dynamic_cast<SwUnoCrsr*>(pCurCrsr);
    _PercentHdl* pPHdl = 0;
    sal_uInt16 nCrsrCnt = 0;
    if( FND_IN_SEL & eFndRngs )
    {
        while( pCurCrsr != ( pTmpCrsr = static_cast<SwPaM*>(pTmpCrsr->GetNext()) ))
            ++nCrsrCnt;
        if( nCrsrCnt && !bIsUnoCrsr )
            pPHdl = new _PercentHdl( 0, nCrsrCnt, pDoc->GetDocShell() );
    }
    else
        pSaveCrsr = static_cast<SwPaM*>(pSaveCrsr->GetPrev());

    bool bEnd = false;
    do {
        aRegion.SetMark();
        // independent from search direction: SPoint is always bigger than mark
        // if the search area is valid
        SwPosition *pSttPos = aRegion.GetMark(),
                        *pEndPos = aRegion.GetPoint();
        *pSttPos = *pTmpCrsr->Start();
        *pEndPos = *pTmpCrsr->End();
        if( bSrchBkwrd )
            aRegion.Exchange();

        if( !nCrsrCnt && !pPHdl && !bIsUnoCrsr )
            pPHdl = new _PercentHdl( aRegion );

        // as long as found and not at same position
        while(  *pSttPos <= *pEndPos &&
                0 != ( nFndRet = rParas.Find( pCurCrsr, fnMove,
                                            &aRegion, bInReadOnly )) &&
                ( !pFndRing ||
                    *pFndRing->GetPoint() != *pCurCrsr->GetPoint() ||
                    *pFndRing->GetMark() != *pCurCrsr->GetMark() ))
        {
            if( !( FIND_NO_RING & nFndRet ))
            {
                // #i24084# - create ring similar to the one in CreateCrsr
                SwCursor* pNew = pCurCrsr->Create( pFndRing );
                if( !pFndRing )
                    pFndRing = pNew;

                pNew->SetMark();
                *pNew->GetMark() = *pCurCrsr->GetMark();
            }

            ++nFound;

            if( !( eFndRngs & FND_IN_SELALL) )
            {
                bEnd = true;
                break;
            }

            if ((coSrchRplcThreshold == nFound)
                && pDoc->GetIDocumentUndoRedo().DoesUndo()
                && rParas.IsReplaceMode())
            {
                short nRet = pCurCrsr->MaxReplaceArived();
                if( RET_YES == nRet )
                {
                    pDoc->GetIDocumentUndoRedo().DelAllUndoObj();
                    pDoc->GetIDocumentUndoRedo().DoUndo(false);
                }
                else
                {
                    bEnd = true;
                    if(RET_CANCEL == nRet)
                    {
                        bCancel = true;
                    }
                    break;
                }
            }

            if( bSrchBkwrd )
                // move pEndPos in front of the found area
                *pEndPos = *pCurCrsr->Start();
            else
                // move pSttPos behind the found area
                *pSttPos = *pCurCrsr->End();

            if( *pSttPos == *pEndPos )
                // in area but at the end => done
                break;

            if( !nCrsrCnt && pPHdl )
            {
                pPHdl->NextPos( *aRegion.GetMark() );
            }
        }

        if( bEnd || !( eFndRngs & ( FND_IN_SELALL | FND_IN_SEL )) )
            break;

        pTmpCrsr = static_cast<SwPaM*>(pTmpCrsr->GetNext());
        if( nCrsrCnt && pPHdl )
        {
            pPHdl->NextPos( ++pPHdl->nActPos );
        }

    } while( pTmpCrsr != pSaveCrsr );

    if( nFound && !pFndRing ) // if no ring should be created
        pFndRing = pCurCrsr->Create();

    delete pPHdl;
    pDoc->GetIDocumentUndoRedo().DoUndo(bDoesUndo);
    return nFound;
}

static bool lcl_MakeSelFwrd( const SwNode& rSttNd, const SwNode& rEndNd,
                        SwPaM& rPam, bool bFirst )
{
    if( rSttNd.GetIndex() + 1 == rEndNd.GetIndex() )
        return false;

    SwNodes& rNds = rPam.GetDoc()->GetNodes();
    rPam.DeleteMark();
    SwCntntNode* pCNd;
    if( !bFirst )
    {
        rPam.GetPoint()->nNode = rSttNd;
        pCNd = rNds.GoNext( &rPam.GetPoint()->nNode );
        if( !pCNd )
            return false;
        pCNd->MakeStartIndex( &rPam.GetPoint()->nContent );
    }
    else if( rSttNd.GetIndex() > rPam.GetPoint()->nNode.GetIndex() ||
             rPam.GetPoint()->nNode.GetIndex() >= rEndNd.GetIndex() )
        // not in this section
        return false;

    rPam.SetMark();
    rPam.GetPoint()->nNode = rEndNd;
    pCNd = rNds.GoPrevious( &rPam.GetPoint()->nNode );
    if( !pCNd )
        return false;
    pCNd->MakeEndIndex( &rPam.GetPoint()->nContent );

    return *rPam.GetMark() < *rPam.GetPoint();
}

static bool lcl_MakeSelBkwrd( const SwNode& rSttNd, const SwNode& rEndNd,
                        SwPaM& rPam, bool bFirst )
{
    if( rEndNd.GetIndex() + 1 == rSttNd.GetIndex() )
        return false;

    SwNodes& rNds = rPam.GetDoc()->GetNodes();
    rPam.DeleteMark();
    SwCntntNode* pCNd;
    if( !bFirst )
    {
        rPam.GetPoint()->nNode = rSttNd;
        pCNd = rNds.GoPrevious( &rPam.GetPoint()->nNode );
        if( !pCNd )
            return false;
        pCNd->MakeEndIndex( &rPam.GetPoint()->nContent );
    }
    else if( rEndNd.GetIndex() > rPam.GetPoint()->nNode.GetIndex() ||
             rPam.GetPoint()->nNode.GetIndex() >= rSttNd.GetIndex() )
        return false;       // not in this section

    rPam.SetMark();
    rPam.GetPoint()->nNode = rEndNd;
    pCNd = rNds.GoNext( &rPam.GetPoint()->nNode );
    if( !pCNd )
        return false;
    pCNd->MakeStartIndex( &rPam.GetPoint()->nContent );

    return *rPam.GetPoint() < *rPam.GetMark();
}

// this method "searches" for all use cases because in SwFindParas is always the
// correct parameters and respective search method
sal_uLong SwCursor::FindAll( SwFindParas& rParas,
                            SwDocPositions nStart, SwDocPositions nEnde,
                            FindRanges eFndRngs, bool& bCancel )
{
    bCancel = false;
    SwCrsrSaveState aSaveState( *this );

    // create region without adding it to the ring
    SwPaM aRegion( *GetPoint() );
    SwMoveFn fnMove = MakeFindRange( nStart, nEnde, &aRegion );

    sal_uLong nFound = 0;
    const bool bMvBkwrd = fnMove == fnMoveBackward;
    bool bInReadOnly = IsReadOnlyAvailable();

    SwCursor* pFndRing = 0;
    SwNodes& rNds = GetDoc()->GetNodes();

    // search in sections?
    if( FND_IN_SEL & eFndRngs )
    {
        // if string was not found in region then get all sections (cursors
        // stays unchanged)
        if( 0 == ( nFound = lcl_FindSelection( rParas, this, fnMove,
                                                pFndRing, aRegion, eFndRngs,
                                                bInReadOnly, bCancel ) ))
            return nFound;

        // found string at least once; it's all in new Crsr ring thus delete old one
        while( GetNext() != this )
            delete GetNext();

        *GetPoint() = *pFndRing->GetPoint();
        SetMark();
        *GetMark() = *pFndRing->GetMark();
        pFndRing->MoveRingTo( this );
        delete pFndRing;
    }
    else if( FND_IN_OTHER & eFndRngs )
    {
        // put cursor as copy of current into ring
        // chaining points always to first created, so forward
        std::unique_ptr< SwCursor > pSav( Create( this ) ); // save the current cursor

        // if already outside of body text search from this position or start at
        // 1. base section
        if( bMvBkwrd
            ? lcl_MakeSelBkwrd( rNds.GetEndOfExtras(),
                    *rNds.GetEndOfPostIts().StartOfSectionNode(),
                     *this, rNds.GetEndOfExtras().GetIndex() >=
                    GetPoint()->nNode.GetIndex() )
            : lcl_MakeSelFwrd( *rNds.GetEndOfPostIts().StartOfSectionNode(),
                    rNds.GetEndOfExtras(), *this,
                    rNds.GetEndOfExtras().GetIndex() >=
                    GetPoint()->nNode.GetIndex() ))
        {
            nFound = lcl_FindSelection( rParas, this, fnMove, pFndRing,
                                        aRegion, eFndRngs, bInReadOnly, bCancel );
        }

        if( !nFound )
        {
            // put back the old one
            *GetPoint() = *pSav->GetPoint();
            if( pSav->HasMark() )
            {
                SetMark();
                *GetMark() = *pSav->GetMark();
            }
            else
                DeleteMark();
            return 0;
        }
        pSav.release();

        if( !( FND_IN_SELALL & eFndRngs ))
        {
            // there should only be a single one, thus add it
            // independent from search direction: SPoint is always bigger than
            // mark if the search area is valid
            *GetPoint() = *pFndRing->GetPoint();
            SetMark();
            *GetMark() = *pFndRing->GetMark();
        }
        else
        {
            // found string at least once; it's all in new Crsr ring thus delete old one
            while( GetNext() != this )
                delete GetNext();

            *GetPoint() = *pFndRing->GetPoint();
            SetMark();
            *GetMark() = *pFndRing->GetMark();
            pFndRing->MoveRingTo( this );
        }
        delete pFndRing;
    }
    else if( FND_IN_SELALL & eFndRngs )
    {
        ::std::unique_ptr< SwCursor> pSav( Create( this ) );  // save the current cursor

        const SwNode* pSttNd = ( FND_IN_BODYONLY & eFndRngs )
                            ? rNds.GetEndOfContent().StartOfSectionNode()
                            : rNds.GetEndOfPostIts().StartOfSectionNode();

        if( bMvBkwrd
            ? lcl_MakeSelBkwrd( rNds.GetEndOfContent(), *pSttNd, *this, false )
            : lcl_MakeSelFwrd( *pSttNd, rNds.GetEndOfContent(), *this, false ))
        {
            nFound = lcl_FindSelection( rParas, this, fnMove, pFndRing,
                                        aRegion, eFndRngs, bInReadOnly, bCancel );
        }

        if( !nFound )
        {
            // put back the old one
            *GetPoint() = *pSav->GetPoint();
            if( pSav->HasMark() )
            {
                SetMark();
                *GetMark() = *pSav->GetMark();
            }
            else
                DeleteMark();
            return 0;
        }
        pSav.release();
        while( GetNext() != this )
            delete GetNext();

        *GetPoint() = *pFndRing->GetPoint();
        SetMark();
        *GetMark() = *pFndRing->GetMark();
        pFndRing->MoveRingTo( this );
        delete pFndRing;
    }
    else
    {
        // if a GetMark is set then keep the GetMark of the found object
        // This allows spanning an area with this search.
        SwPosition aMarkPos( *GetMark() );
        const bool bMarkPos = HasMark() && !eFndRngs;

        if( 0 != (nFound = rParas.Find( this, fnMove,
                                        &aRegion, bInReadOnly ) ? 1 : 0)
            && bMarkPos )
            *GetMark() = aMarkPos;
    }

    if( nFound && SwCursor::IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE ) )
        nFound = 0;
    return nFound;
}

void SwCursor::FillFindPos( SwDocPositions ePos, SwPosition& rPos ) const
{
    bool bIsStart = true;
    SwCntntNode* pCNd = 0;
    SwNodes& rNds = GetDoc()->GetNodes();

    switch( ePos )
    {
    case DOCPOS_START:
        rPos.nNode = *rNds.GetEndOfContent().StartOfSectionNode();
        pCNd = rNds.GoNext( &rPos.nNode );
        break;
    case DOCPOS_END:
        rPos.nNode = rNds.GetEndOfContent();
        pCNd = rNds.GoPrevious( &rPos.nNode );
        bIsStart = false;
        break;
    case DOCPOS_OTHERSTART:
        rPos.nNode = *rNds[ sal_uLong(0) ];
        pCNd = rNds.GoNext( &rPos.nNode );
        break;
    case DOCPOS_OTHEREND:
        rPos.nNode = *rNds.GetEndOfContent().StartOfSectionNode();
        pCNd = rNds.GoPrevious( &rPos.nNode );
        bIsStart = false;
        break;
    default:
        rPos = *GetPoint();
    }

    if( pCNd )
    {
        rPos.nContent.Assign( pCNd, bIsStart ? 0 : pCNd->Len() );
    }
}

short SwCursor::MaxReplaceArived()
{
    return RET_YES;
}

bool SwCursor::IsStartWord( sal_Int16 nWordType ) const
{
    return IsStartWordWT( nWordType );
}

bool SwCursor::IsEndWord( sal_Int16 nWordType ) const
{
    return IsEndWordWT( nWordType );
}

bool SwCursor::IsInWord( sal_Int16 nWordType ) const
{
    return IsInWordWT( nWordType );
}

bool SwCursor::GoStartWord()
{
    return GoStartWordWT( WordType::ANYWORD_IGNOREWHITESPACES );
}

bool SwCursor::GoEndWord()
{
    return GoEndWordWT( WordType::ANYWORD_IGNOREWHITESPACES );
}

bool SwCursor::GoNextWord()
{
    return GoNextWordWT( WordType::ANYWORD_IGNOREWHITESPACES );
}

bool SwCursor::GoPrevWord()
{
    return GoPrevWordWT( WordType::ANYWORD_IGNOREWHITESPACES );
}

bool SwCursor::SelectWord( SwViewShell* pViewShell, const Point* pPt )
{
    return SelectWordWT( pViewShell, WordType::ANYWORD_IGNOREWHITESPACES, pPt );
}

bool SwCursor::IsStartWordWT( sal_Int16 nWordType ) const
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        const sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        bRet = g_pBreakIt->GetBreakIter()->isBeginWord(
                            pTxtNd->GetTxt(), nPtPos,
                            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos )),
                            nWordType );
    }
    return bRet;
}

bool SwCursor::IsEndWordWT( sal_Int16 nWordType ) const
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        const sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        bRet = g_pBreakIt->GetBreakIter()->isEndWord(
                            pTxtNd->GetTxt(), nPtPos,
                            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos ) ),
                            nWordType );

    }
    return bRet;
}

bool SwCursor::IsInWordWT( sal_Int16 nWordType ) const
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        const sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        Boundary aBoundary = g_pBreakIt->GetBreakIter()->getWordBoundary(
                            pTxtNd->GetTxt(), nPtPos,
                            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos ) ),
                            nWordType,
                            true );

        bRet = aBoundary.startPos != aBoundary.endPos &&
                aBoundary.startPos <= nPtPos &&
                    nPtPos <= aBoundary.endPos;
        if(bRet)
        {
            const CharClass& rCC = GetAppCharClass();
            bRet = rCC.isLetterNumeric( pTxtNd->GetTxt(), aBoundary.startPos );
        }
    }
    return bRet;
}

bool SwCursor::IsStartEndSentence( bool bEnd ) const
{
    bool bRet = bEnd ?
                    GetCntntNode() && GetPoint()->nContent == GetCntntNode()->Len() :
                    GetPoint()->nContent.GetIndex() == 0;

    if( !bRet )
    {
        SwCursor aCrsr(*GetPoint(), 0, false);
        SwPosition aOrigPos = *aCrsr.GetPoint();
        aCrsr.GoSentence( bEnd ? SwCursor::END_SENT : SwCursor::START_SENT );
        bRet = aOrigPos == *aCrsr.GetPoint();
    }
    return bRet;
}

bool SwCursor::GoStartWordWT( sal_Int16 nWordType )
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        SwCrsrSaveState aSave( *this );
        sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        nPtPos = g_pBreakIt->GetBreakIter()->getWordBoundary(
                            pTxtNd->GetTxt(), nPtPos,
                            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos ) ),
                            nWordType,
                            false ).startPos;

        if (nPtPos < pTxtNd->GetTxt().getLength() && nPtPos >= 0)
        {
            GetPoint()->nContent = nPtPos;
            if( !IsSelOvr() )
                bRet = true;
        }
    }
    return bRet;
}

bool SwCursor::GoEndWordWT( sal_Int16 nWordType )
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        SwCrsrSaveState aSave( *this );
        sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        nPtPos = g_pBreakIt->GetBreakIter()->getWordBoundary(
                            pTxtNd->GetTxt(), nPtPos,
                            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos ) ),
                            nWordType,
                            true ).endPos;

        if (nPtPos <= pTxtNd->GetTxt().getLength() && nPtPos >= 0 &&
            GetPoint()->nContent.GetIndex() != nPtPos )
        {
            GetPoint()->nContent = nPtPos;
            if( !IsSelOvr() )
                bRet = true;
        }
    }
    return bRet;
}

bool SwCursor::GoNextWordWT( sal_Int16 nWordType )
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        SwCrsrSaveState aSave( *this );
        sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();

        nPtPos = g_pBreakIt->GetBreakIter()->nextWord(
                                pTxtNd->GetTxt(), nPtPos,
            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos, 1 ) ),
                    nWordType ).startPos;

        if (nPtPos < pTxtNd->GetTxt().getLength() && nPtPos >= 0)
        {
            GetPoint()->nContent = nPtPos;
            if( !IsSelOvr() )
                bRet = true;
        }
    }
    return bRet;
}

bool SwCursor::GoPrevWordWT( sal_Int16 nWordType )
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        SwCrsrSaveState aSave( *this );
        sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        const sal_Int32 nPtStart = nPtPos;

        if( nPtPos )
            --nPtPos;
        nPtPos = g_pBreakIt->GetBreakIter()->previousWord(
                                pTxtNd->GetTxt(), nPtStart,
            g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos, 1 ) ),
                    nWordType ).startPos;

        if (nPtPos < pTxtNd->GetTxt().getLength() && nPtPos >= 0)
        {
            GetPoint()->nContent = nPtPos;
            if( !IsSelOvr() )
                bRet = true;
        }
    }
    return bRet;
}

bool SwCursor::SelectWordWT( SwViewShell* pViewShell, sal_Int16 nWordType, const Point* pPt )
{
    SwCrsrSaveState aSave( *this );

    bool bRet = false;
    bool bForward = true;
    DeleteMark();
    const SwRootFrm* pLayout = pViewShell->GetLayout();
    if( pPt && 0 != pLayout )
    {
        // set the cursor to the layout position
        Point aPt( *pPt );
        pLayout->GetCrsrOfst( GetPoint(), aPt );
    }

    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        // Should we select the whole fieldmark?
        const IDocumentMarkAccess* pMarksAccess = GetDoc()->getIDocumentMarkAccess( );
        sw::mark::IMark* pMark = GetPoint() ? pMarksAccess->getFieldmarkFor( *GetPoint( ) ) : NULL;
        if ( pMark )
        {
            const SwPosition rStart = pMark->GetMarkStart();
            GetPoint()->nNode = rStart.nNode;
            GetPoint()->nContent = rStart.nContent;
            GetPoint()->nContent++; // Don't select the start delimiter

            const SwPosition rEnd = pMark->GetMarkEnd();

            if ( rStart != rEnd )
            {
                SetMark();
                GetMark()->nNode = rEnd.nNode;
                GetMark()->nContent = rEnd.nContent;
                GetMark()->nContent--; //Don't select the end delimiter
            }
            bRet = true;
        }
        else
        {
            const sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
            Boundary aBndry( g_pBreakIt->GetBreakIter()->getWordBoundary(
                                pTxtNd->GetTxt(), nPtPos,
                                g_pBreakIt->GetLocale( pTxtNd->GetLang( nPtPos ) ),
                                nWordType,
                                bForward ));

            if( aBndry.startPos != aBndry.endPos )
            {
                GetPoint()->nContent = aBndry.endPos;
                if( !IsSelOvr() )
                {
                    SetMark();
                    GetMark()->nContent = aBndry.startPos;
                    if (sw::mark::IMark* pAnnotationMark = pMarksAccess->getAnnotationMarkFor(*GetPoint()))
                    {
                        // An annotation mark covers the selected word. Check
                        // if it covers only the word: in that case we select
                        // the comment anchor as well.
                        bool bStartMatch = GetMark()->nNode == pAnnotationMark->GetMarkStart().nNode &&
                            GetMark()->nContent == pAnnotationMark->GetMarkStart().nContent;
                        bool bEndMatch = GetPoint()->nNode == pAnnotationMark->GetMarkEnd().nNode &&
                            GetPoint()->nContent.GetIndex() + 1 == pAnnotationMark->GetMarkEnd().nContent.GetIndex();
                        if (bStartMatch && bEndMatch)
                            GetPoint()->nContent++;
                    }
                    if( !IsSelOvr() )
                        bRet = true;
                }
            }
        }
    }

    if( !bRet )
    {
        DeleteMark();
        RestoreSavePos();
    }
    return bRet;
}

static OUString lcl_MaskDeletedRedlines( const SwTxtNode* pTxtNd )
{
    OUString aRes;
    if (pTxtNd)
    {
        //mask deleted redlines
        OUString sNodeText(pTxtNd->GetTxt());
        const SwDoc& rDoc = *pTxtNd->GetDoc();
        const bool nShowChg = IDocumentRedlineAccess::IsShowChanges( rDoc.getIDocumentRedlineAccess().GetRedlineMode() );
        if ( nShowChg )
        {
            sal_uInt16 nAct = rDoc.getIDocumentRedlineAccess().GetRedlinePos( *pTxtNd, USHRT_MAX );
            for ( ; nAct < rDoc.getIDocumentRedlineAccess().GetRedlineTbl().size(); nAct++ )
            {
                const SwRangeRedline* pRed = rDoc.getIDocumentRedlineAccess().GetRedlineTbl()[ nAct ];
                if ( pRed->Start()->nNode > pTxtNd->GetIndex() )
                    break;

                if( nsRedlineType_t::REDLINE_DELETE == pRed->GetType() )
                {
                    sal_Int32 nStart, nEnd;
                    pRed->CalcStartEnd( pTxtNd->GetIndex(), nStart, nEnd );

                    while ( nStart < nEnd && nStart < sNodeText.getLength() )
                        sNodeText = sNodeText.replaceAt( nStart++, 1, OUString(CH_TXTATR_INWORD) );
                }
            }
        }
        aRes = sNodeText;
    }
    return aRes;
}

bool SwCursor::GoSentence( SentenceMoveType eMoveType )
{
    bool bRet = false;
    const SwTxtNode* pTxtNd = GetNode().GetTxtNode();
    if( pTxtNd && g_pBreakIt->GetBreakIter().is() )
    {
        OUString sNodeText( lcl_MaskDeletedRedlines( pTxtNd ) );

        SwCrsrSaveState aSave( *this );
        sal_Int32 nPtPos = GetPoint()->nContent.GetIndex();
        switch ( eMoveType )
        {
        case START_SENT: /* when modifying: see also ExpandToSentenceBorders below! */
            nPtPos = g_pBreakIt->GetBreakIter()->beginOfSentence(
                                    sNodeText,
                                    nPtPos, g_pBreakIt->GetLocale(
                                            pTxtNd->GetLang( nPtPos ) ));
            break;
        case END_SENT: /* when modifying: see also ExpandToSentenceBorders below! */
            nPtPos = g_pBreakIt->GetBreakIter()->endOfSentence(
                                    sNodeText,
                                    nPtPos, g_pBreakIt->GetLocale(
                                                pTxtNd->GetLang( nPtPos ) ));
            break;
        case NEXT_SENT:
            {
                nPtPos = g_pBreakIt->GetBreakIter()->endOfSentence(
                                        sNodeText,
                                        nPtPos, g_pBreakIt->GetLocale(
                                                    pTxtNd->GetLang( nPtPos ) ));
                while (nPtPos>=0 && ++nPtPos < sNodeText.getLength()
                       && sNodeText[nPtPos] == ' ' /*isWhiteSpace( aTxt.GetChar(nPtPos)*/ )
                    ;
                break;
            }
        case PREV_SENT:
            nPtPos = g_pBreakIt->GetBreakIter()->beginOfSentence(
                                    sNodeText,
                                    nPtPos, g_pBreakIt->GetLocale(
                                                pTxtNd->GetLang( nPtPos ) ));
            if (nPtPos == 0)
                return false;   // the previous sentence is not in this paragraph
            if (nPtPos > 0)
                nPtPos = g_pBreakIt->GetBreakIter()->beginOfSentence(
                                    sNodeText,
                                    nPtPos - 1, g_pBreakIt->GetLocale(
                                                pTxtNd->GetLang( nPtPos ) ));
            break;
        }

        // it is allowed to place the PaM just behind the last
        // character in the text thus <= ...Len
        if (nPtPos <= pTxtNd->GetTxt().getLength() && nPtPos >= 0)
        {
            GetPoint()->nContent = nPtPos;
            if( !IsSelOvr() )
                bRet = true;
        }
    }
    return bRet;
}

bool SwCursor::ExpandToSentenceBorders()
{
    bool bRes = false;
    const SwTxtNode* pStartNd = Start()->nNode.GetNode().GetTxtNode();
    const SwTxtNode* pEndNd   = End()->nNode.GetNode().GetTxtNode();
    if (pStartNd && pEndNd && g_pBreakIt->GetBreakIter().is())
    {
        if (!HasMark())
            SetMark();

        OUString sStartText( lcl_MaskDeletedRedlines( pStartNd ) );
        OUString sEndText( pStartNd == pEndNd? sStartText : lcl_MaskDeletedRedlines( pEndNd ) );

        SwCrsrSaveState aSave( *this );
        sal_Int32 nStartPos = Start()->nContent.GetIndex();
        sal_Int32 nEndPos   = End()->nContent.GetIndex();

        nStartPos = g_pBreakIt->GetBreakIter()->beginOfSentence(
                                sStartText, nStartPos,
                                g_pBreakIt->GetLocale( pStartNd->GetLang( nStartPos ) ) );
        nEndPos   = g_pBreakIt->GetBreakIter()->endOfSentence(
                                sEndText, nEndPos,
                                g_pBreakIt->GetLocale( pEndNd->GetLang( nEndPos ) ) );

        // it is allowed to place the PaM just behind the last
        // character in the text thus <= ...Len
        bool bChanged = false;
        if (nStartPos <= pStartNd->GetTxt().getLength() && nStartPos >= 0)
        {
            GetMark()->nContent = nStartPos;
            bChanged = true;
        }
        if (nEndPos <= pEndNd->GetTxt().getLength() && nEndPos >= 0)
        {
            GetPoint()->nContent = nEndPos;
            bChanged = true;
        }
        if (bChanged && !IsSelOvr())
            bRes = true;
    }
    return bRes;
}

bool SwTableCursor::LeftRight( bool bLeft, sal_uInt16 nCnt, sal_uInt16 /*nMode*/,
    bool /*bVisualAllowed*/, bool /*bSkipHidden*/, bool /*bInsertCrsr*/ )
{
    return bLeft ? GoPrevCell( nCnt )
                 : GoNextCell( nCnt );
}

// calculate cursor bidi level: extracted from LeftRight()
const SwCntntFrm*
SwCursor::DoSetBidiLevelLeftRight(
    bool & io_rbLeft, bool bVisualAllowed, bool bInsertCrsr)
{
    // calculate cursor bidi level
    const SwCntntFrm* pSttFrm = NULL;
    SwNode& rNode = GetPoint()->nNode.GetNode();

    if( rNode.IsTxtNode() )
    {
        const SwTxtNode& rTNd = *rNode.GetTxtNode();
        SwIndex& rIdx = GetPoint()->nContent;
        sal_Int32 nPos = rIdx.GetIndex();

        const SvtCTLOptions& rCTLOptions = SW_MOD()->GetCTLOptions();
        if ( bVisualAllowed && rCTLOptions.IsCTLFontEnabled() &&
             SvtCTLOptions::MOVEMENT_VISUAL ==
             rCTLOptions.GetCTLCursorMovement() )
        {
            // for visual cursor travelling (used in bidi layout)
            // we first have to convert the logic to a visual position
            Point aPt;
            pSttFrm = rTNd.getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
            if( pSttFrm )
            {
                sal_uInt8 nCrsrLevel = GetCrsrBidiLevel();
                bool bForward = ! io_rbLeft;
                const_cast<SwTxtFrm*>(static_cast<const SwTxtFrm*>(pSttFrm))->PrepareVisualMove( nPos, nCrsrLevel,
                                                         bForward, bInsertCrsr );
                rIdx = nPos;
                SetCrsrBidiLevel( nCrsrLevel );
                io_rbLeft = ! bForward;
            }
        }
        else
        {
            const SwScriptInfo* pSI = SwScriptInfo::GetScriptInfo( rTNd );
            if ( pSI )
            {
                const sal_Int32 nMoveOverPos = io_rbLeft ?
                                               ( nPos ? nPos - 1 : 0 ) :
                                                nPos;
                SetCrsrBidiLevel( pSI->DirType( nMoveOverPos ) );
            }
        }
    }
    return pSttFrm;
}

bool SwCursor::LeftRight( bool bLeft, sal_uInt16 nCnt, sal_uInt16 nMode,
                          bool bVisualAllowed,bool bSkipHidden, bool bInsertCrsr )
{
    // calculate cursor bidi level
    SwNode& rNode = GetPoint()->nNode.GetNode();
    const SwCntntFrm* pSttFrm = // may side-effect bLeft!
        DoSetBidiLevelLeftRight(bLeft, bVisualAllowed, bInsertCrsr);

    // can the cursor be moved n times?
    SwCrsrSaveState aSave( *this );
    SwMoveFn fnMove = bLeft ? fnMoveBackward : fnMoveForward;

    SwGoInDoc fnGo;
    if ( bSkipHidden )
        fnGo = CRSR_SKIP_CELLS == nMode ? fnGoCntntCellsSkipHidden : fnGoCntntSkipHidden;
    else
        fnGo = CRSR_SKIP_CELLS == nMode ? fnGoCntntCells : fnGoCntnt;

    while( nCnt )
    {
        SwNodeIndex aOldNodeIdx( GetPoint()->nNode );

        if ( !Move( fnMove, fnGo ) )
            break;

        // If we were located inside a covered cell but our position has been
        // corrected, we check if the last move has moved the cursor to a
        // different table cell. In this case we set the cursor to the stored
        // covered position and redo the move:
        if ( mnRowSpanOffset )
        {
            const SwNode* pOldTabBoxSttNode = aOldNodeIdx.GetNode().FindTableBoxStartNode();
            const SwTableNode* pOldTabSttNode = pOldTabBoxSttNode ? pOldTabBoxSttNode->FindTableNode() : 0;
            const SwNode* pNewTabBoxSttNode = GetPoint()->nNode.GetNode().FindTableBoxStartNode();
            const SwTableNode* pNewTabSttNode = pNewTabBoxSttNode ? pNewTabBoxSttNode->FindTableNode() : 0;

            const bool bCellChanged = pOldTabSttNode && pNewTabSttNode &&
                                      pOldTabSttNode == pNewTabSttNode &&
                                      pOldTabBoxSttNode && pNewTabBoxSttNode &&
                                      pOldTabBoxSttNode != pNewTabBoxSttNode;

            if ( bCellChanged )
            {
                // Set cursor to start/end of covered cell:
                SwTableBox* pTableBox = pOldTabBoxSttNode->GetTblBox();
                if ( pTableBox && pTableBox->getRowSpan() > 1 )
                {
                    pTableBox = & pTableBox->FindEndOfRowSpan( pOldTabSttNode->GetTable(), (sal_uInt16)(pTableBox->getRowSpan() + mnRowSpanOffset ) );
                    SwNodeIndex& rPtIdx = GetPoint()->nNode;
                    SwNodeIndex aNewIdx( *pTableBox->GetSttNd() );
                    rPtIdx = aNewIdx;

                    GetDoc()->GetNodes().GoNextSection( &rPtIdx, false, false );
                    SwCntntNode* pCntntNode = GetCntntNode();
                    if ( pCntntNode )
                    {
                        GetPoint()->nContent.Assign( pCntntNode, bLeft ? pCntntNode->Len() : 0 );

                        // Redo the move:
                        if ( !Move( fnMove, fnGo ) )
                            break;
                    }
                }
                mnRowSpanOffset = 0;
            }
        }

        // Check if I'm inside a covered cell. Correct cursor if necessary and
        // store covered cell:
        const SwNode* pTableBoxStartNode = GetPoint()->nNode.GetNode().FindTableBoxStartNode();
        if ( pTableBoxStartNode )
        {
            const SwTableBox* pTableBox = pTableBoxStartNode->GetTblBox();
            if ( pTableBox && pTableBox->getRowSpan() < 1 )
            {
                // Store the row span offset:
                mnRowSpanOffset = pTableBox->getRowSpan();

                // Move cursor to non-covered cell:
                const SwTableNode* pTblNd = pTableBoxStartNode->FindTableNode();
                pTableBox = & pTableBox->FindStartOfRowSpan( pTblNd->GetTable(), USHRT_MAX );
                 SwNodeIndex& rPtIdx = GetPoint()->nNode;
                SwNodeIndex aNewIdx( *pTableBox->GetSttNd() );
                rPtIdx = aNewIdx;

                GetDoc()->GetNodes().GoNextSection( &rPtIdx, false, false );
                SwCntntNode* pCntntNode = GetCntntNode();
                if ( pCntntNode )
                {
                    GetPoint()->nContent.Assign( pCntntNode, bLeft ? pCntntNode->Len() : 0 );
                }
            }
        }
        --nCnt;
    }

    // here come some special rules for visual cursor travelling
    if ( pSttFrm )
    {
        SwNode& rTmpNode = GetPoint()->nNode.GetNode();
        if ( &rTmpNode != &rNode && rTmpNode.IsTxtNode() )
        {
            Point aPt;
            const SwCntntFrm* pEndFrm = rTmpNode.GetTxtNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
            if ( pEndFrm )
            {
                if ( ! pEndFrm->IsRightToLeft() != ! pSttFrm->IsRightToLeft() )
                {
                    if ( ! bLeft )
                        pEndFrm->RightMargin( this );
                    else
                        pEndFrm->LeftMargin( this );
                }
            }
        }
    }

    return 0 == nCnt && !IsInProtectTable( true ) &&
            !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                       nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
}

// calculate cursor bidi level: extracted from UpDown()
void SwCursor::DoSetBidiLevelUpDown()
{
    SwNode& rNode = GetPoint()->nNode.GetNode();
    if ( rNode.IsTxtNode() )
    {
        const SwScriptInfo* pSI =
            SwScriptInfo::GetScriptInfo( *rNode.GetTxtNode() );
        if ( pSI )
        {
            SwIndex& rIdx = GetPoint()->nContent;
            const sal_Int32 nPos = rIdx.GetIndex();

            if (nPos && nPos < rNode.GetTxtNode()->GetTxt().getLength())
            {
                const sal_uInt8 nCurrLevel = pSI->DirType( nPos );
                const sal_uInt8 nPrevLevel = pSI->DirType( nPos - 1 );

                if ( nCurrLevel % 2 != nPrevLevel % 2 )
                {
                    // set cursor level to the lower of the two levels
                    SetCrsrBidiLevel( std::min( nCurrLevel, nPrevLevel ) );
                }
                else
                    SetCrsrBidiLevel( nCurrLevel );
            }
        }
    }
}

bool SwCursor::UpDown( bool bUp, sal_uInt16 nCnt,
                            Point* pPt, long nUpDownX )
{
    SwTableCursor* pTblCrsr = dynamic_cast<SwTableCursor*>(this);
    bool bAdjustTableCrsr = false;

    // If the point/mark of the table cursor in the same box then set cursor to
    // beginning of the box
    if( pTblCrsr && GetNode( true ).StartOfSectionNode() ==
                    GetNode( false ).StartOfSectionNode() )
    {
        if ( End() != GetPoint() )
            Exchange();
        bAdjustTableCrsr = true;
    }

    bool bRet = false;
    Point aPt;
    if( pPt )
        aPt = *pPt;
    SwCntntFrm* pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );

    if( pFrm )
    {
        SwCrsrSaveState aSave( *this );

        if( !pPt )
        {
            SwRect aTmpRect;
            pFrm->GetCharRect( aTmpRect, *GetPoint() );
            aPt = aTmpRect.Pos();

            nUpDownX = pFrm->IsVertical() ?
                aPt.getY() - pFrm->Frm().Top() :
                aPt.getX() - pFrm->Frm().Left();
        }

        // It is allowed to move footnotes in other footnotes but not sections
        const bool bChkRange = !pFrm->IsInFtn() || HasMark();
        const SwPosition aOldPos( *GetPoint() );
        const bool bInReadOnly = IsReadOnlyAvailable();

        if ( bAdjustTableCrsr && !bUp )
        {
            // Special case: We have a table cursor but the start box has more
            // than one paragraph. If we want to go down, we have to set the
            // point to the last frame in the table box. This is only necessary
            // if we do not already have a table selection
            const SwStartNode* pTblNd = GetNode( true ).FindTableBoxStartNode();
            OSL_ENSURE( pTblNd, "pTblCrsr without SwTableNode?" );

            if ( pTblNd ) // safety first
            {
                const SwNode* pEndNd = pTblNd->EndOfSectionNode();
                GetPoint()->nNode = *pEndNd;
                pTblCrsr->Move( fnMoveBackward, fnGoNode );
                   pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
            }
        }

        while( nCnt &&
               (bUp ? pFrm->UnitUp( this, nUpDownX, bInReadOnly )
                    : pFrm->UnitDown( this, nUpDownX, bInReadOnly ) ) &&
                CheckNodesRange( aOldPos.nNode, GetPoint()->nNode, bChkRange ))
        {
               pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
            --nCnt;
        }

        // iterate over whole number of items?
        if( !nCnt && !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                                nsSwCursorSelOverFlags::SELOVER_CHANGEPOS ) )
        {
            if( !pTblCrsr )
            {
                // try to position the cursor at half of the char-rect's height
                pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
                SwCrsrMoveState eTmpState( MV_UPDOWN );
                eTmpState.bSetInReadOnly = bInReadOnly;
                SwRect aTmpRect;
                pFrm->GetCharRect( aTmpRect, *GetPoint(), &eTmpState );
                if ( pFrm->IsVertical() )
                {
                    aPt.setX(aTmpRect.Center().getX());
                    pFrm->Calc();
                    aPt.setY(pFrm->Frm().Top() + nUpDownX);
                }
                else
                {
                    aPt.setY(aTmpRect.Center().getY());
                    pFrm->Calc();
                    aPt.setX(pFrm->Frm().Left() + nUpDownX);
                }
                pFrm->GetCrsrOfst( GetPoint(), aPt, &eTmpState );
            }
            bRet = !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE | nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
        }
        else
            *GetPoint() = aOldPos;

        DoSetBidiLevelUpDown(); // calculate cursor bidi level
    }
    return bRet;
}

bool SwCursor::LeftRightMargin( bool bLeft, bool bAPI )
{
    Point aPt;
    SwCntntFrm * pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );

    // calculate cursor bidi level
    if ( pFrm )
        SetCrsrBidiLevel( pFrm->IsRightToLeft() ? 1 : 0 );

    SwCrsrSaveState aSave( *this );
    return pFrm
           && (bLeft ? pFrm->LeftMargin( this ) : pFrm->RightMargin( this, bAPI ) )
           && !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE | nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
}

bool SwCursor::IsAtLeftRightMargin( bool bLeft, bool bAPI ) const
{
    bool bRet = false;
    Point aPt;
    SwCntntFrm * pFrm = GetCntntNode()->getLayoutFrm( GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aPt, GetPoint() );
    if( pFrm )
    {
        SwPaM aPam( *GetPoint() );
        if( !bLeft && aPam.GetPoint()->nContent.GetIndex() )
            aPam.GetPoint()->nContent--;
        bRet = (bLeft ? pFrm->LeftMargin( &aPam )
                      : pFrm->RightMargin( &aPam, bAPI ))
                && *aPam.GetPoint() == *GetPoint();
    }
    return bRet;
}

bool SwCursor::SttEndDoc( bool bStt )
{
    SwCrsrSaveState aSave( *this );
    // Never jump over section boundaries during selection!
    // Can the cursor still moved on?
    SwMoveFn fnMove = bStt ? fnMoveBackward : fnMoveForward;
    bool bRet = (!HasMark() || !IsNoCntnt() ) &&
                    Move( fnMove, fnGoDoc ) &&
                    !IsInProtectTable( true ) &&
                    !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                               nsSwCursorSelOverFlags::SELOVER_CHANGEPOS |
                               nsSwCursorSelOverFlags::SELOVER_ENABLEREVDIREKTION );
    return bRet;
}

bool SwCursor::GoPrevNextCell( bool bNext, sal_uInt16 nCnt )
{
    const SwTableNode* pTblNd = GetPoint()->nNode.GetNode().FindTableNode();
    if( !pTblNd )
        return false;

    // If there is another EndNode in front of the cell's StartNode then there
    // exists a previous cell
    SwCrsrSaveState aSave( *this );
    SwNodeIndex& rPtIdx = GetPoint()->nNode;

    while( nCnt-- )
    {
        const SwNode* pTableBoxStartNode = rPtIdx.GetNode().FindTableBoxStartNode();
        const SwTableBox* pTableBox = pTableBoxStartNode->GetTblBox();

        // Check if we have to move the cursor to a covered cell before
        // proceeding:
        if ( mnRowSpanOffset )
        {
            if ( pTableBox && pTableBox->getRowSpan() > 1 )
            {
                pTableBox = & pTableBox->FindEndOfRowSpan( pTblNd->GetTable(), (sal_uInt16)(pTableBox->getRowSpan() + mnRowSpanOffset) );
                SwNodeIndex aNewIdx( *pTableBox->GetSttNd() );
                rPtIdx = aNewIdx;
                pTableBoxStartNode = rPtIdx.GetNode().FindTableBoxStartNode();
            }
            mnRowSpanOffset = 0;
        }

        const SwNode* pTmpNode = bNext ?
                                 pTableBoxStartNode->EndOfSectionNode() :
                                 pTableBoxStartNode;

        SwNodeIndex aCellIdx( *pTmpNode, bNext ? 1 : -1 );
        if(  (bNext && !aCellIdx.GetNode().IsStartNode()) ||
            (!bNext && !aCellIdx.GetNode().IsEndNode()) )
            return false;

        rPtIdx = bNext ? aCellIdx : SwNodeIndex(*aCellIdx.GetNode().StartOfSectionNode());

        pTableBoxStartNode = rPtIdx.GetNode().FindTableBoxStartNode();
        pTableBox = pTableBoxStartNode->GetTblBox();
        if ( pTableBox && pTableBox->getRowSpan() < 1 )
        {
            mnRowSpanOffset = pTableBox->getRowSpan();
            // move cursor to non-covered cell:
            pTableBox = & pTableBox->FindStartOfRowSpan( pTblNd->GetTable(), USHRT_MAX );
            SwNodeIndex aNewIdx( *pTableBox->GetSttNd() );
            rPtIdx = aNewIdx;
        }
    }

    ++rPtIdx;
    if( !rPtIdx.GetNode().IsCntntNode() )
        GetDoc()->GetNodes().GoNextSection( &rPtIdx, true, false );
    GetPoint()->nContent.Assign( GetCntntNode(), 0 );

    return !IsInProtectTable( true );
}

bool SwTableCursor::GotoTable( const OUString& )
{
    return false; // invalid action
}

bool SwCursor::GotoTable( const OUString& rName )
{
    bool bRet = false;
    if ( !HasMark() )
    {
        SwTable* pTmpTbl = SwTable::FindTable( GetDoc()->FindTblFmtByName( rName ) );
        if( pTmpTbl )
        {
            // a table in a normal nodes array
            SwCrsrSaveState aSave( *this );
            GetPoint()->nNode = *pTmpTbl->GetTabSortBoxes()[ 0 ]->
                                GetSttNd()->FindTableNode();
            Move( fnMoveForward, fnGoCntnt );
            bRet = !IsSelOvr();
        }
    }
    return bRet;
}

bool SwCursor::GotoTblBox( const OUString& rName )
{
    bool bRet = false;
    const SwTableNode* pTblNd = GetPoint()->nNode.GetNode().FindTableNode();
    if( pTblNd )
    {
        // retrieve box by name
        const SwTableBox* pTblBox = pTblNd->GetTable().GetTblBox( rName );
        if( pTblBox && pTblBox->GetSttNd() &&
            ( !pTblBox->GetFrmFmt()->GetProtect().IsCntntProtected() ||
              IsReadOnlyAvailable() ) )
        {
            SwCrsrSaveState aSave( *this );
            GetPoint()->nNode = *pTblBox->GetSttNd();
            Move( fnMoveForward, fnGoCntnt );
            bRet = !IsSelOvr();
        }
    }
    return bRet;
}

bool SwCursor::MovePara(SwWhichPara fnWhichPara, SwPosPara fnPosPara )
{
    // for optimization test something before
    const SwNode* pNd = &GetPoint()->nNode.GetNode();
    bool bShortCut = false;
    if ( fnWhichPara == fnParaCurr )
    {
        // #i41048#
        // If fnWhichPara == fnParaCurr then (*fnWhichPara)( *this, fnPosPara )
        // can already move the cursor to a different text node. In this case
        // we better check if IsSelOvr().
        const SwCntntNode* pCntntNd = pNd->GetCntntNode();
        if ( pCntntNd )
        {
            const sal_Int32 nSttEnd = fnPosPara == fnMoveForward ? 0 : pCntntNd->Len();
            if ( GetPoint()->nContent.GetIndex() != nSttEnd )
                bShortCut = true;
        }
    }
    else
    {
        if ( pNd->IsTxtNode() &&
             pNd->GetNodes()[ pNd->GetIndex() +
                    (fnWhichPara == fnParaNext ? 1 : -1 ) ]->IsTxtNode() )
            bShortCut = true;
    }

    if ( bShortCut )
        return (*fnWhichPara)( *this, fnPosPara );

    // else we must use the SaveStructure, because the next/prev is not
    // a same node type.
    SwCrsrSaveState aSave( *this );
    return (*fnWhichPara)( *this, fnPosPara ) &&
            !IsInProtectTable( true ) &&
            !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                       nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
}

bool SwCursor::MoveSection( SwWhichSection fnWhichSect,
                                SwPosSection fnPosSect)
{
    SwCrsrSaveState aSave( *this );
    return (*fnWhichSect)( *this, fnPosSect ) &&
            !IsInProtectTable( true ) &&
            !IsSelOvr( nsSwCursorSelOverFlags::SELOVER_TOGGLE |
                       nsSwCursorSelOverFlags::SELOVER_CHANGEPOS );
}

void SwCursor::RestoreSavePos()
{
    // This method is not supposed to be used in cases when nodes may be
    // deleted; detect such cases, but do not crash (example: fdo#40831).
    sal_uLong uNodeCount = GetPoint()->nNode.GetNodes().Count();
    OSL_ENSURE(!pSavePos || pSavePos->nNode < uNodeCount,
        "SwCursor::RestoreSavePos: invalid node: "
        "probably something was deleted; consider using SwUnoCrsr instead");
    if( pSavePos && pSavePos->nNode < uNodeCount )
    {
        GetPoint()->nNode = pSavePos->nNode;

        sal_Int32 nIdx = 0;
        if ( GetCntntNode() )
        {
            if ( pSavePos->nCntnt <= GetCntntNode()->Len() )
                nIdx = pSavePos->nCntnt;
            else
            {
                nIdx = GetCntntNode()->Len();
                OSL_FAIL("SwCursor::RestoreSavePos: invalid content index");
            }
        }
        GetPoint()->nContent.Assign( GetCntntNode(), nIdx );
    }
}

SwTableCursor::SwTableCursor( const SwPosition &rPos, SwPaM* pRing )
    : SwCursor( rPos, pRing, false )
{
    bParked = false;
    bChg = false;
    nTblPtNd = 0, nTblMkNd = 0;
    nTblPtCnt = 0, nTblMkCnt = 0;
}

SwTableCursor::~SwTableCursor() {}

static bool
lcl_SeekEntry(const SwSelBoxes& rTmp, SwStartNode const*const pSrch,
        size_t & o_rFndPos)
{
    sal_uLong nIdx = pSrch->GetIndex();

    size_t nO = rTmp.size();
    if( nO > 0 )
    {
        nO--;
        size_t nU = 0;
        while( nU <= nO )
        {
            size_t nM = nU + ( nO - nU ) / 2;
            if( rTmp[ nM ]->GetSttNd() == pSrch )
            {
                o_rFndPos = nM;
                return true;
            }
            else if( rTmp[ nM ]->GetSttIdx() < nIdx )
                nU = nM + 1;
            else if( nM == 0 )
                return false;
            else
                nO = nM - 1;
        }
    }
    return false;
}

SwCursor* SwTableCursor::MakeBoxSels( SwCursor* pAktCrsr )
{
    if( bChg )
    {
        if( bParked )
        {
            // move back into content
            Exchange();
            Move( fnMoveForward );
            Exchange();
            Move( fnMoveForward );
            bParked = false;
        }

        bChg = false;

        // create temporary copies so that all boxes that
        // have already cursors can be removed
        SwSelBoxes aTmp(m_SelectedBoxes);

        // compare old and new ones
        SwNodes& rNds = pAktCrsr->GetDoc()->GetNodes();
        const SwStartNode* pSttNd;
        SwPaM* pCur = pAktCrsr;
        do {
            size_t nPos;
            bool bDel = false;
            pSttNd = pCur->GetPoint()->nNode.GetNode().FindTableBoxStartNode();
            if( !pCur->HasMark() || !pSttNd ||
                pSttNd != pCur->GetMark()->nNode.GetNode().FindTableBoxStartNode() )
                bDel = true;

            else if( lcl_SeekEntry( aTmp, pSttNd, nPos ))
            {
                SwNodeIndex aIdx( *pSttNd, 1 );
                const SwNode* pNd = &aIdx.GetNode();
                if( !pNd->IsCntntNode() )
                    pNd = rNds.GoNextSection( &aIdx, true, false );

                SwPosition* pPos = pCur->GetMark();
                if( pNd != &pPos->nNode.GetNode() )
                    pPos->nNode = *pNd;
                pPos->nContent.Assign( const_cast<SwCntntNode*>(static_cast<const SwCntntNode*>(pNd)), 0 );

                aIdx.Assign( *pSttNd->EndOfSectionNode(), - 1 );
                if( !( pNd = &aIdx.GetNode())->IsCntntNode() )
                    pNd = rNds.GoPrevSection( &aIdx, true, false );

                pPos = pCur->GetPoint();
                if (pNd && pNd != &pPos->nNode.GetNode())
                    pPos->nNode = *pNd;
                pPos->nContent.Assign(const_cast<SwCntntNode*>(static_cast<const SwCntntNode*>(pNd)), pNd ? static_cast<const SwCntntNode*>(pNd)->Len() : 0);

                aTmp.erase( aTmp.begin() + nPos );
            }
            else
                bDel = true;

            pCur = static_cast<SwPaM*>(pCur->GetNext());
            if( bDel )
            {
                SwPaM* pDel = static_cast<SwPaM*>(pCur->GetPrev());

                if( pDel == pAktCrsr )
                    pAktCrsr->DeleteMark();
                else
                    delete pDel;
            }
        } while ( pAktCrsr != pCur );

        for (size_t nPos = 0; nPos < aTmp.size(); ++nPos)
        {
            pSttNd = aTmp[ nPos ]->GetSttNd();

            SwNodeIndex aIdx( *pSttNd, 1 );
            if( &aIdx.GetNodes() != &rNds )
                break;
            SwNode* pNd = &aIdx.GetNode();
            if( !pNd->IsCntntNode() )
                pNd = rNds.GoNextSection( &aIdx, true, false );

            SwPaM *const pNew = (pAktCrsr->GetNext() == pAktCrsr && !pAktCrsr->HasMark())
                ? pAktCrsr
                : pAktCrsr->Create( pAktCrsr );
            pNew->GetPoint()->nNode = *pNd;
            pNew->GetPoint()->nContent.Assign( static_cast<SwCntntNode*>(pNd), 0 );
            pNew->SetMark();

            SwPosition* pPos = pNew->GetPoint();
            pPos->nNode.Assign( *pSttNd->EndOfSectionNode(), - 1 );
            if( !( pNd = &pPos->nNode.GetNode())->IsCntntNode() )
                pNd = rNds.GoPrevSection( &pPos->nNode, true, false );

            pPos->nContent.Assign(static_cast<SwCntntNode*>(pNd), pNd ? static_cast<SwCntntNode*>(pNd)->Len() : 0);
        }
    }
    return pAktCrsr;
}

void SwTableCursor::InsertBox( const SwTableBox& rTblBox )
{
    SwTableBox* pBox = (SwTableBox*)&rTblBox;
    m_SelectedBoxes.insert(pBox);
    bChg = true;
}

void SwTableCursor::DeleteBox(size_t const nPos)
{
    m_SelectedBoxes.erase(m_SelectedBoxes.begin() + nPos);
    bChg = true;
}

bool SwTableCursor::NewTableSelection()
{
    bool bRet = false;
    const SwNode *pStart = GetCntntNode()->FindTableBoxStartNode();
    const SwNode *pEnd = GetCntntNode(false)->FindTableBoxStartNode();
    if( pStart && pEnd )
    {
        const SwTableNode *pTableNode = pStart->FindTableNode();
        if( pTableNode == pEnd->FindTableNode() &&
            pTableNode->GetTable().IsNewModel() )
        {
            bRet = true;
            SwSelBoxes aNew(m_SelectedBoxes);
            pTableNode->GetTable().CreateSelection( pStart, pEnd, aNew,
                SwTable::SEARCH_NONE, false );
            ActualizeSelection( aNew );
        }
    }
    return bRet;
}

void SwTableCursor::ActualizeSelection( const SwSelBoxes &rNew )
{
    size_t nOld = 0, nNew = 0;
    while (nOld < m_SelectedBoxes.size() && nNew < rNew.size())
    {
        SwTableBox const*const pPOld = m_SelectedBoxes[ nOld ];
        const SwTableBox* pPNew = rNew[ nNew ];
        if( pPOld == pPNew )
        {   // this box will stay
            ++nOld;
            ++nNew;
        }
        else if( pPOld->GetSttIdx() < pPNew->GetSttIdx() )
        {
            DeleteBox( nOld ); // this box has to go
        }
        else
        {
            InsertBox( *pPNew ); // this is a new one
            ++nOld;
            ++nNew;
        }
    }

    while (nOld < m_SelectedBoxes.size())
    {
        DeleteBox( nOld ); // some more to delete
    }

    for ( ; nNew < rNew.size(); ++nNew ) // some more to insert
    {
        InsertBox( *rNew[ nNew ] );
    }
}

bool SwTableCursor::IsCrsrMovedUpdt()
{
    if( !IsCrsrMoved() )
        return false;

    nTblMkNd = GetMark()->nNode.GetIndex();
    nTblPtNd = GetPoint()->nNode.GetIndex();
    nTblMkCnt = GetMark()->nContent.GetIndex();
    nTblPtCnt = GetPoint()->nContent.GetIndex();
    return true;
}

/// park table cursor on the boxes' start node
void SwTableCursor::ParkCrsr()
{
    // de-register index from text node
    SwNode* pNd = &GetPoint()->nNode.GetNode();
    if( !pNd->IsStartNode() )
        pNd = pNd->StartOfSectionNode();
    GetPoint()->nNode = *pNd;
    GetPoint()->nContent.Assign( 0, 0 );

    pNd = &GetMark()->nNode.GetNode();
    if( !pNd->IsStartNode() )
        pNd = pNd->StartOfSectionNode();
    GetMark()->nNode = *pNd;
    GetMark()->nContent.Assign( 0, 0 );

    bChg = true;
    bParked = true;
}

bool SwTableCursor::HasReadOnlyBoxSel() const
{
    bool bRet = false;
    for (size_t n = m_SelectedBoxes.size(); n; )
    {
        if (m_SelectedBoxes[--n]->GetFrmFmt()->GetProtect().IsCntntProtected())
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
