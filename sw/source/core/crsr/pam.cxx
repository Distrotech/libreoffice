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

#include <tools/gen.hxx>
#include <hintids.hxx>
#include <editeng/protitem.hxx>
#include <cntfrm.hxx>
#include <pagefrm.hxx>
#include <doc.hxx>
#include <IDocumentLayoutAccess.hxx>
#include <docary.hxx>
#include <pam.hxx>
#include <pamtyp.hxx>
#include <txtfrm.hxx>
#include <fmtcntnt.hxx>
#include <frmatr.hxx>
#include <swtable.hxx>
#include <crsskip.hxx>
#include <flyfrm.hxx>
#include <fmteiro.hxx>
#include <section.hxx>
#include <sectfrm.hxx>
#include <ndtxt.hxx>

#include <IMark.hxx>
#include <DocumentSettingManager.hxx>
#include <hints.hxx>
#include <xmloff/odffields.hxx>

// for the dump "MSC-" compiler
inline sal_Int32 GetSttOrEnd( bool bCondition, const SwCntntNode& rNd )
{
    return bCondition ? 0 : rNd.Len();
}

SwPosition::SwPosition( const SwNodeIndex & rNodeIndex, const SwIndex & rCntnt )
    : nNode( rNodeIndex ), nContent( rCntnt )
{
}

SwPosition::SwPosition( const SwNodeIndex & rNodeIndex )
    : nNode( rNodeIndex ), nContent( nNode.GetNode().GetCntntNode() )
{
}

SwPosition::SwPosition( const SwNode& rNode )
    : nNode( rNode ), nContent( nNode.GetNode().GetCntntNode() )
{
}

SwPosition::SwPosition( SwCntntNode & rNode, const sal_Int32 nOffset )
    : nNode( rNode ), nContent( &rNode, nOffset )
{
}

SwPosition::SwPosition( const SwPosition & rPos )
    : nNode( rPos.nNode ), nContent( rPos.nContent )
{
}

SwPosition &SwPosition::operator=(const SwPosition &rPos)
{
    nNode = rPos.nNode;
    nContent = rPos.nContent;
    return *this;
}

bool SwPosition::operator<(const SwPosition &rPos) const
{
    if( nNode < rPos.nNode )
        return true;
    if( nNode == rPos.nNode )
    {
        // note that positions with text node but no SwIndex registered are
        // created for text frames anchored at para (see SwXFrame::getAnchor())
        SwIndexReg const*const pThisReg(nContent.GetIdxReg());
        SwIndexReg const*const pOtherReg(rPos.nContent.GetIdxReg());
        if (pThisReg && pOtherReg)
        {
            return (nContent < rPos.nContent);
        }
        else // by convention position with no index is smaller
        {
            return (pOtherReg) ? true : false;
        }
    }
    return false;
}

bool SwPosition::operator>(const SwPosition &rPos) const
{
    if(nNode > rPos.nNode )
        return true;
    if( nNode == rPos.nNode )
    {
        // note that positions with text node but no SwIndex registered are
        // created for text frames anchored at para (see SwXFrame::getAnchor())
        SwIndexReg const*const pThisReg(nContent.GetIdxReg());
        SwIndexReg const*const pOtherReg(rPos.nContent.GetIdxReg());
        if (pThisReg && pOtherReg)
        {
            return (nContent > rPos.nContent);
        }
        else // by convention position with no index is smaller
        {
            return (pThisReg) ? true : false;
        }
    }
    return false;
}

bool SwPosition::operator<=(const SwPosition &rPos) const
{
    if(nNode < rPos.nNode )
        return true;
    if( nNode == rPos.nNode )
    {
        // note that positions with text node but no SwIndex registered are
        // created for text frames anchored at para (see SwXFrame::getAnchor())
        SwIndexReg const*const pThisReg(nContent.GetIdxReg());
        SwIndexReg const*const pOtherReg(rPos.nContent.GetIdxReg());
        if (pThisReg && pOtherReg)
        {
            return (nContent <= rPos.nContent);
        }
        else // by convention position with no index is smaller
        {
            return (pThisReg) ? false : true;
        }
    }
    return false;
}

bool SwPosition::operator>=(const SwPosition &rPos) const
{
    if(nNode > rPos.nNode )
        return true;
    if( nNode == rPos.nNode )
    {
        // note that positions with text node but no SwIndex registered are
        // created for text frames anchored at para (see SwXFrame::getAnchor())
        SwIndexReg const*const pThisReg(nContent.GetIdxReg());
        SwIndexReg const*const pOtherReg(rPos.nContent.GetIdxReg());
        if (pThisReg && pOtherReg)
        {
            return (nContent >= rPos.nContent);
        }
        else // by convention position with no index is smaller
        {
            return (pOtherReg) ? false : true;
        }
    }
    return false;
}

bool SwPosition::operator==(const SwPosition &rPos) const
{
    return (nNode == rPos.nNode)
        // GetIndexReg may be null for FLY_AT_PARA frame anchor position
        && (nContent.GetIdxReg() == rPos.nContent.GetIdxReg())
        && (nContent == rPos.nContent);
}

bool SwPosition::operator!=(const SwPosition &rPos) const
{
    return (nNode != rPos.nNode)
        // GetIndexReg may be null for FLY_AT_PARA frame anchor position
        || (nContent.GetIdxReg() != rPos.nContent.GetIdxReg())
        || (nContent != rPos.nContent);
}

SwDoc * SwPosition::GetDoc() const
{
    return nNode.GetNode().GetDoc();
}

std::ostream &operator <<(std::ostream& s, const SwPosition& position)
{
    return s << "SwPosition (node " << position.nNode.GetIndex() << ", offset " << position.nContent.GetIndex() << ")";
}

enum CHKSECTION { Chk_Both, Chk_One, Chk_None };

static CHKSECTION lcl_TstIdx( sal_uLong nSttIdx, sal_uLong nEndIdx, const SwNode& rEndNd )
{
    sal_uLong nStt = rEndNd.StartOfSectionIndex(), nEnd = rEndNd.GetIndex();
    CHKSECTION eSec = nStt < nSttIdx && nEnd >= nSttIdx ? Chk_One : Chk_None;
    if( nStt < nEndIdx && nEnd >= nEndIdx )
        return( eSec == Chk_One ? Chk_Both : Chk_One );
    return eSec;
}

static bool lcl_ChkOneRange( CHKSECTION eSec, bool bChkSections,
                    const SwNode& rBaseEnd, sal_uLong nStt, sal_uLong nEnd )
{
    if( eSec != Chk_Both )
        return false;

    if( !bChkSections )
        return true;

    // search the surrounding section
    const SwNodes& rNds = rBaseEnd.GetNodes();
    const SwNode *pTmp, *pNd = rNds[ nStt ];
    if( !pNd->IsStartNode() )
        pNd = pNd->StartOfSectionNode();

    if( pNd == rNds[ nEnd ]->StartOfSectionNode() )
        return true; // same StartNode, same section

    // already on a base node => error
    if( !pNd->StartOfSectionIndex() )
        return false;

    while( ( pTmp = pNd->StartOfSectionNode())->EndOfSectionNode() !=
            &rBaseEnd )
        pNd = pTmp;

    sal_uLong nSttIdx = pNd->GetIndex(), nEndIdx = pNd->EndOfSectionIndex();
    return nSttIdx <= nStt && nStt <= nEndIdx &&
           nSttIdx <= nEnd && nEnd <= nEndIdx;
}

bool CheckNodesRange( const SwNodeIndex& rStt,
                      const SwNodeIndex& rEnd, bool bChkSection )
{
    const SwNodes& rNds = rStt.GetNodes();
    sal_uLong nStt = rStt.GetIndex(), nEnd = rEnd.GetIndex();
    CHKSECTION eSec = lcl_TstIdx( nStt, nEnd, rNds.GetEndOfContent() );
    if( Chk_None != eSec )
        return eSec == Chk_Both;

    eSec = lcl_TstIdx( nStt, nEnd, rNds.GetEndOfAutotext() );
    if( Chk_None != eSec )
        return lcl_ChkOneRange( eSec, bChkSection,
                            rNds.GetEndOfAutotext(), nStt, nEnd );

    eSec = lcl_TstIdx( nStt, nEnd, rNds.GetEndOfPostIts() );
    if( Chk_None != eSec )
        return lcl_ChkOneRange( eSec, bChkSection,
                            rNds.GetEndOfPostIts(), nStt, nEnd );

    eSec = lcl_TstIdx( nStt, nEnd, rNds.GetEndOfInserts() );
    if( Chk_None != eSec )
        return lcl_ChkOneRange( eSec, bChkSection,
                            rNds.GetEndOfInserts(), nStt, nEnd );

    eSec = lcl_TstIdx( nStt, nEnd, rNds.GetEndOfRedlines() );
    if( Chk_None != eSec )
        return lcl_ChkOneRange( eSec, bChkSection,
                            rNds.GetEndOfRedlines(), nStt, nEnd );

    return false; // somewhere in between => error
}

bool GoNext(SwNode* pNd, SwIndex * pIdx, sal_uInt16 nMode )
{
    if( pNd->IsCntntNode() )
        return static_cast<SwCntntNode*>(pNd)->GoNext( pIdx, nMode );
    return false;
}

bool GoPrevious( SwNode* pNd, SwIndex * pIdx, sal_uInt16 nMode )
{
    if( pNd->IsCntntNode() )
        return static_cast<SwCntntNode*>(pNd)->GoPrevious( pIdx, nMode );
    return false;
}

SwCntntNode* GoNextNds( SwNodeIndex* pIdx, bool bChk )
{
    SwNodeIndex aIdx( *pIdx );
    SwCntntNode* pNd = aIdx.GetNodes().GoNext( &aIdx );
    if( pNd )
    {
        if( bChk && 1 != aIdx.GetIndex() - pIdx->GetIndex() &&
            !CheckNodesRange( *pIdx, aIdx, true ) )
                pNd = 0;
        else
            *pIdx = aIdx;
    }
    return pNd;
}

SwCntntNode* GoPreviousNds( SwNodeIndex * pIdx, bool bChk )
{
    SwNodeIndex aIdx( *pIdx );
    SwCntntNode* pNd = aIdx.GetNodes().GoPrevious( &aIdx );
    if( pNd )
    {
        if( bChk && 1 != pIdx->GetIndex() - aIdx.GetIndex() &&
            !CheckNodesRange( *pIdx, aIdx, true ) )
                pNd = 0;
        else
            *pIdx = aIdx;
    }
    return pNd;
}

SwPaM::SwPaM( const SwPosition& rPos, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rPos )
    , m_Bound2( rPos.nNode.GetNode().GetNodes() ) // default initialize
    , m_pPoint( &m_Bound1 )
    , m_pMark( m_pPoint )
    , m_bIsInFrontOfLabel( false )
{
}

SwPaM::SwPaM( const SwPosition& rMark, const SwPosition& rPoint, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rMark )
    , m_Bound2( rPoint )
    , m_pPoint( &m_Bound2 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
}

SwPaM::SwPaM( const SwNodeIndex& rMark, const SwNodeIndex& rPoint,
              long nMarkOffset, long nPointOffset, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rMark )
    , m_Bound2( rPoint )
    , m_pPoint( &m_Bound2 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    if ( nMarkOffset )
    {
        m_pMark->nNode += nMarkOffset;
    }
    if ( nPointOffset )
    {
        m_pPoint->nNode += nPointOffset;
    }
    m_Bound1.nContent.Assign( m_Bound1.nNode.GetNode().GetCntntNode(), 0 );
    m_Bound2.nContent.Assign( m_Bound2.nNode.GetNode().GetCntntNode(), 0 );
}

SwPaM::SwPaM( const SwNode& rMark, const SwNode& rPoint,
              long nMarkOffset, long nPointOffset, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rMark )
    , m_Bound2( rPoint )
    , m_pPoint( &m_Bound2 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    if ( nMarkOffset )
    {
        m_pMark->nNode += nMarkOffset;
    }
    if ( nPointOffset )
    {
        m_pPoint->nNode += nPointOffset;
    }
    m_Bound1.nContent.Assign( m_Bound1.nNode.GetNode().GetCntntNode(), 0 );
    m_Bound2.nContent.Assign( m_Bound2.nNode.GetNode().GetCntntNode(), 0 );
}

SwPaM::SwPaM( const SwNodeIndex& rMark, sal_Int32 nMarkCntnt,
              const SwNodeIndex& rPoint, sal_Int32 nPointCntnt, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rMark )
    , m_Bound2( rPoint )
    , m_pPoint( &m_Bound2 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    m_pPoint->nContent.Assign( rPoint.GetNode().GetCntntNode(), nPointCntnt);
    m_pMark ->nContent.Assign( rMark .GetNode().GetCntntNode(), nMarkCntnt );
}

SwPaM::SwPaM( const SwNode& rMark, sal_Int32 nMarkCntnt,
              const SwNode& rPoint, sal_Int32 nPointCntnt, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rMark )
    , m_Bound2( rPoint )
    , m_pPoint( &m_Bound2 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    m_pPoint->nContent.Assign( m_pPoint->nNode.GetNode().GetCntntNode(),
        nPointCntnt);
    m_pMark ->nContent.Assign( m_pMark ->nNode.GetNode().GetCntntNode(),
        nMarkCntnt );
}

SwPaM::SwPaM( const SwNode& rNode, sal_Int32 nCntnt, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rNode )
    , m_Bound2( m_Bound1.nNode.GetNode().GetNodes() ) // default initialize
    , m_pPoint( &m_Bound1 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    m_pPoint->nContent.Assign( m_pPoint->nNode.GetNode().GetCntntNode(),
        nCntnt );
}

SwPaM::SwPaM( const SwNodeIndex& rNodeIdx, sal_Int32 nCntnt, SwPaM* pRing )
    : Ring( pRing )
    , m_Bound1( rNodeIdx )
    , m_Bound2( rNodeIdx.GetNode().GetNodes() ) // default initialize
    , m_pPoint( &m_Bound1 )
    , m_pMark( &m_Bound1 )
    , m_bIsInFrontOfLabel( false )
{
    m_pPoint->nContent.Assign( rNodeIdx.GetNode().GetCntntNode(), nCntnt );
}

SwPaM::~SwPaM() {}

// @@@ semantic: no copy ctor.
SwPaM::SwPaM( SwPaM &rPam )
    : Ring( &rPam )
    , m_Bound1( *(rPam.m_pPoint) )
    , m_Bound2( *(rPam.m_pMark)  )
    , m_pPoint( &m_Bound1 ), m_pMark( rPam.HasMark() ? &m_Bound2 : m_pPoint )
    , m_bIsInFrontOfLabel( false )
{
}

// @@@ semantic: no copy assignment for super class Ring.
SwPaM &SwPaM::operator=( const SwPaM &rPam )
{
    *m_pPoint = *( rPam.m_pPoint );
    if ( rPam.HasMark() )
    {
        SetMark();
        *m_pMark = *( rPam.m_pMark );
    }
    else
    {
        DeleteMark();
    }
    return *this;
}

void SwPaM::SetMark()
{
    if (m_pPoint == &m_Bound1)
    {
        m_pMark = &m_Bound2;
    }
    else
    {
        m_pMark = &m_Bound1;
    }
    (*m_pMark) = (*m_pPoint);
}

#ifdef DBG_UTIL
void SwPaM::Exchange()
{
    if (m_pPoint != m_pMark)
    {
        SwPosition *pTmp = m_pPoint;
        m_pPoint = m_pMark;
        m_pMark = pTmp;
    }
}
#endif

/// movement of cursor
bool SwPaM::Move( SwMoveFn fnMove, SwGoInDoc fnGo )
{
    const bool bRet = (*fnGo)( *this, fnMove );

    m_bIsInFrontOfLabel = false;
    return bRet;
}

/** make a new region

    Sets the first SwPaM onto the given SwPaM, or to the beginning or end of a
    document. SPoint stays at its position, GetMark will be changed respectively.

    @param fnMove  Contains information if beginning or end of document.
    @param pOrigRg The given region.

    @return Newly created area.
*/
SwPaM* SwPaM::MakeRegion( SwMoveFn fnMove, const SwPaM * pOrigRg )
{
    SwPaM* pPam;
    if( pOrigRg == 0 )
    {
        pPam = new SwPaM( *m_pPoint );
        pPam->SetMark(); // set beginning
        pPam->Move( fnMove, fnGoSection); // to beginning or end of a node

        // set SPoint onto its old position; set GetMark to the "end"
        pPam->Exchange();
    }
    else
    {
        pPam = new SwPaM( *(SwPaM*)pOrigRg ); // given search area
        // make sure that SPoint is on the "real" start position
        // FORWARD: SPoint always smaller than GetMark
        // BACKWARD: SPoint always bigger than GetMark
        if( (pPam->GetMark()->*fnMove->fnCmpOp)( *pPam->GetPoint() ) )
            pPam->Exchange();
    }
    return pPam;
}

SwPaM & SwPaM::Normalize(bool bPointFirst)
{
    if (HasMark())
        if ( ( bPointFirst && *m_pPoint > *m_pMark) ||
             (!bPointFirst && *m_pPoint < *m_pMark) )
        {
            Exchange();
        }
    return *this;
}

/// return page number at cursor (for reader and page bound frames)
sal_uInt16 SwPaM::GetPageNum( bool bAtPoint, const Point* pLayPos )
{
    const SwCntntFrm* pCFrm;
    const SwPageFrm *pPg;
    const SwCntntNode *pNd ;
    const SwPosition* pPos = bAtPoint ? m_pPoint : m_pMark;

    if( 0 != ( pNd = pPos->nNode.GetNode().GetCntntNode() ) &&
        0 != ( pCFrm = pNd->getLayoutFrm( pNd->GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), pLayPos, pPos, false )) &&
        0 != ( pPg = pCFrm->FindPageFrm() ))
        return pPg->GetPhyPageNum();
    return 0;
}

// Formular view - See also SwCrsrShell::IsCrsrReadonly()
static const SwFrm* lcl_FindEditInReadonlyFrm( const SwFrm& rFrm )
{
    const SwFrm* pRet = 0;

    const SwFlyFrm* pFly;
    const SwSectionFrm* pSectionFrm;

    if( rFrm.IsInFly() &&
       (pFly = rFrm.FindFlyFrm())->GetFmt()->GetEditInReadonly().GetValue() &&
        pFly->Lower() &&
       !pFly->Lower()->IsNoTxtFrm() )
    {
       pRet = pFly;
    }
    else if ( rFrm.IsInSct() &&
              0 != ( pSectionFrm = rFrm.FindSctFrm() )->GetSection() &&
              pSectionFrm->GetSection()->IsEditInReadonlyFlag() )
    {
        pRet = pSectionFrm;
    }

    return pRet;
}

/// is in protected section or selection surrounds something protected
bool SwPaM::HasReadonlySel( bool bFormView, bool bAnnotationMode ) const
{
    bool bRet = false;

    const SwCntntNode* pNd = GetPoint()->nNode.GetNode().GetCntntNode();
    const SwCntntFrm *pFrm = NULL;
    if ( pNd != NULL )
    {
        Point aTmpPt;
        pFrm = pNd->getLayoutFrm( pNd->GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aTmpPt, GetPoint(), false );
    }

    // Will be set if point are inside edit-in-readonly environment
    const SwFrm* pPointEditInReadonlyFrm = NULL;
    if ( pFrm != NULL
         && ( pFrm->IsProtected()
              || ( bFormView
                   && 0 == ( pPointEditInReadonlyFrm = lcl_FindEditInReadonlyFrm( *pFrm ) ) ) ) )
    {
        bRet = true;
    }
    else if( pNd != NULL )
    {
        const SwSectionNode* pSNd = pNd->GetSectionNode();
        if ( pSNd != NULL
             && ( pSNd->GetSection().IsProtectFlag()
                  || ( bFormView
                       && !pSNd->GetSection().IsEditInReadonlyFlag()) ) )
        {
            bRet = true;
        }
    }

    if ( !bRet
         && HasMark()
         && GetPoint()->nNode != GetMark()->nNode )
    {
        pNd = GetMark()->nNode.GetNode().GetCntntNode();
        pFrm = NULL;
        if ( pNd != NULL )
        {
            Point aTmpPt;
            pFrm = pNd->getLayoutFrm( pNd->GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout(), &aTmpPt, GetMark(), false );
        }

        const SwFrm* pMarkEditInReadonlyFrm = NULL;
        if ( pFrm != NULL
             && ( pFrm->IsProtected()
                  || ( bFormView
                       && 0 == ( pMarkEditInReadonlyFrm = lcl_FindEditInReadonlyFrm( *pFrm ) ) ) ) )
        {
            bRet = true;
        }
        else if( pNd != NULL )
        {
            const SwSectionNode* pSNd = pNd->GetSectionNode();
            if ( pSNd != NULL
                 && ( pSNd->GetSection().IsProtectFlag()
                      || ( bFormView
                           && !pSNd->GetSection().IsEditInReadonlyFlag()) ) )
            {
                bRet = true;
            }
        }

        if ( !bRet && bFormView )
        {
           // Check if start and end frame are inside the _same_
           // edit-in-readonly-environment. Otherwise we better return 'true'
           if ( pPointEditInReadonlyFrm != pMarkEditInReadonlyFrm )
                bRet = true;
        }

        // check for protected section inside the selection
        if( !bRet )
        {
            sal_uLong nSttIdx = GetMark()->nNode.GetIndex(),
                    nEndIdx = GetPoint()->nNode.GetIndex();
            if( nEndIdx <= nSttIdx )
            {
                sal_uLong nTmp = nSttIdx;
                nSttIdx = nEndIdx;
                nEndIdx = nTmp;
            }

            // If a protected section should be between nodes, then the
            // selection needs to contain already x nodes.
            // (TxtNd, SectNd, TxtNd, EndNd, TxtNd )
            if( nSttIdx + 3 < nEndIdx )
            {
                const SwSectionFmts& rFmts = GetDoc()->GetSections();
                for( sal_uInt16 n = rFmts.size(); n;  )
                {
                    const SwSectionFmt* pFmt = rFmts[ --n ];
                    if( pFmt->GetProtect().IsCntntProtected() )
                    {
                        const SwFmtCntnt& rCntnt = pFmt->GetCntnt(false);
                        OSL_ENSURE( rCntnt.GetCntntIdx(), "where is the SectionNode?" );
                        sal_uLong nIdx = rCntnt.GetCntntIdx()->GetIndex();
                        if( nSttIdx <= nIdx && nEndIdx >= nIdx &&
                            rCntnt.GetCntntIdx()->GetNode().GetNodes().IsDocNodes() )
                        {
                            bRet = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    //FIXME FieldBk
    // TODO: Form Protection when Enhanced Fields are enabled
    const SwDoc *pDoc = GetDoc();
    const IDocumentMarkAccess* pMarksAccess = pDoc->getIDocumentMarkAccess();
    sw::mark::IMark* pA = GetPoint() ? pMarksAccess->getFieldmarkFor( *GetPoint( ) ) : NULL;
    sw::mark::IMark* pB = GetMark( ) ? pMarksAccess->getFieldmarkFor( *GetMark( ) ) : pA;

    bool bUnhandledMark = false;
    sw::mark::IFieldmark* pFieldmark = pMarksAccess->getFieldmarkFor( *GetPoint() );
    if ( pFieldmark )
        bUnhandledMark = pFieldmark->GetFieldname( ) == ODF_UNHANDLED;

    if (!bRet)
    {
        // Unhandled fieldmarks case shouldn't be edited manually to avoid breaking anything
        if ( ( pA == pB ) && bUnhandledMark )
            bRet = true;
        else
        {
            // Form protection case
            bool bAtStartA = pA != NULL && pA->GetMarkStart() == *GetPoint();
            bool bAtStartB = pB != NULL && pB->GetMarkStart() == *GetMark();
            bRet = ( pA != pB ) || bAtStartA || bAtStartB;
            bool bProtectForm = pDoc->GetDocumentSettingManager().get( IDocumentSettingAccess::PROTECT_FORM );
            if ( bProtectForm )
                bRet |= ( pA == NULL || pB == NULL );
        }
    }
    else
    {
        bRet = !( pA == pB && pA != NULL );
    }

    // Don't allow inserting characters between the 'field mark end' and
    // the 'comment anchor', unless the cursor is inside the annotation.
    if (!bRet && !bAnnotationMode)
    {
        if (!pA && GetPoint() && GetPoint()->nNode.GetNode().IsTxtNode() && GetPoint()->nContent.GetIndex() > 0)
        {
            // getFieldmarkFor() searches for >= start and < end, so check for
            // the previous character, to also get the fieldmark, if we're
            // exactly at the end.
            SwPosition aPrevChar(*GetPoint());
            aPrevChar.nContent--;
            pFieldmark = pMarksAccess->getFieldmarkFor(aPrevChar);
            if (pFieldmark && pFieldmark->GetMarkEnd() == *GetPoint())
                bRet = true;
        }
    }

    return bRet;
}

/// This function returns the next node in direction of search. If there is no
/// left or the next is out of the area, then a null-pointer is returned.
/// @param rbFirst If <true> than first time request. If so than the position of
///        the PaM must not be changed!
SwCntntNode* GetNode( SwPaM & rPam, bool& rbFirst, SwMoveFn fnMove,
                      bool bInReadOnly )
{
    SwCntntNode * pNd = 0;
    SwCntntFrm* pFrm;
    if( ((*rPam.GetPoint()).*fnMove->fnCmpOp)( *rPam.GetMark() ) ||
        ( *rPam.GetPoint() == *rPam.GetMark() && rbFirst ) )
    {
        if( rbFirst )
        {
            rbFirst = false;
            pNd = rPam.GetCntntNode();
            if( pNd )
            {
                if(
                    (
                        0 == ( pFrm = pNd->getLayoutFrm( pNd->GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout() ) ) ||
                        ( !bInReadOnly && pFrm->IsProtected() ) ||
                        (pFrm->IsTxtFrm() && static_cast<SwTxtFrm*>(pFrm)->IsHiddenNow())
                    ) ||
                    ( !bInReadOnly && pNd->FindSectionNode() &&
                        pNd->FindSectionNode()->GetSection().IsProtect()
                    )
                  )
                    {
                        pNd = 0;
                    }
            }
        }

        if( !pNd ) // is the cursor not on a CntntNode?
        {
            SwPosition aPos( *rPam.GetPoint() );
            bool bSrchForward = fnMove == fnMoveForward;
            SwNodes& rNodes = aPos.nNode.GetNodes();

            // go to next/previous CntntNode
            while( true )
            {
                pNd = bSrchForward
                        ? rNodes.GoNextSection( &aPos.nNode, true, !bInReadOnly )
                        : rNodes.GoPrevSection( &aPos.nNode, true, !bInReadOnly );
                if( pNd )
                {
                    aPos.nContent.Assign( pNd, ::GetSttOrEnd( bSrchForward,*pNd ));
                    // is the position still in the area
                    if( (aPos.*fnMove->fnCmpOp)( *rPam.GetMark() ) )
                    {
                        // only in AutoTextSection can be nodes that are hidden
                        if( 0 == ( pFrm = pNd->getLayoutFrm( pNd->GetDoc()->getIDocumentLayoutAccess().GetCurrentLayout() ) ) ||
                            ( !bInReadOnly && pFrm->IsProtected() ) ||
                            ( pFrm->IsTxtFrm() &&
                                static_cast<SwTxtFrm*>(pFrm)->IsHiddenNow() ) )
                        {
                            pNd = 0;
                            continue;
                        }
                        *(SwPosition*)rPam.GetPoint() = aPos;
                    }
                    else
                        pNd = 0; // no valid node
                    break;
                }
                break;
            }
        }
    }
    return pNd;
}

void GoStartDoc( SwPosition * pPos )
{
    SwNodes& rNodes = pPos->nNode.GetNodes();
    pPos->nNode = *rNodes.GetEndOfContent().StartOfSectionNode();
    // we always need to find a ContentNode!
    SwCntntNode* pCNd = rNodes.GoNext( &pPos->nNode );
    if( pCNd )
        pCNd->MakeStartIndex( &pPos->nContent );
}

void GoEndDoc( SwPosition * pPos )
{
    SwNodes& rNodes = pPos->nNode.GetNodes();
    pPos->nNode = rNodes.GetEndOfContent();
    SwCntntNode* pCNd = GoPreviousNds( &pPos->nNode, true );
    if( pCNd )
        pCNd->MakeEndIndex( &pPos->nContent );
}

void GoStartSection( SwPosition * pPos )
{
    // jump to section's beginning
    SwNodes& rNodes = pPos->nNode.GetNodes();
    sal_uInt16 nLevel = rNodes.GetSectionLevel( pPos->nNode );
    if( pPos->nNode < rNodes.GetEndOfContent().StartOfSectionIndex() )
        nLevel--;
    do { rNodes.GoStartOfSection( &pPos->nNode ); } while( nLevel-- );

    // already on a CntntNode
    pPos->nNode.GetNode().GetCntntNode()->MakeStartIndex( &pPos->nContent );
}

/// go to the end of the current base section
void GoEndSection( SwPosition * pPos )
{
    // jump to section's beginning/end
    SwNodes& rNodes = pPos->nNode.GetNodes();
    sal_uInt16 nLevel = rNodes.GetSectionLevel( pPos->nNode );
    if( pPos->nNode < rNodes.GetEndOfContent().StartOfSectionIndex() )
        nLevel--;
    do { rNodes.GoEndOfSection( &pPos->nNode ); } while( nLevel-- );

    // now on a EndNode, thus to the previous CntntNode
    if( GoPreviousNds( &pPos->nNode, true ) )
        pPos->nNode.GetNode().GetCntntNode()->MakeEndIndex( &pPos->nContent );
}

bool GoInDoc( SwPaM & rPam, SwMoveFn fnMove )
{
    (*fnMove->fnDoc)( rPam.GetPoint() );
    return true;
}

bool GoInSection( SwPaM & rPam, SwMoveFn fnMove )
{
    (*fnMove->fnSections)( (SwPosition*)rPam.GetPoint() );
    return true;
}

bool GoInNode( SwPaM & rPam, SwMoveFn fnMove )
{
    SwCntntNode *pNd = (*fnMove->fnNds)( &rPam.GetPoint()->nNode, true );
    if( pNd )
        rPam.GetPoint()->nContent.Assign( pNd,
                        ::GetSttOrEnd( fnMove == fnMoveForward, *pNd ) );
    return pNd;
}

bool GoInCntnt( SwPaM & rPam, SwMoveFn fnMove )
{
    if( (*fnMove->fnNd)( &rPam.GetPoint()->nNode.GetNode(),
                        &rPam.GetPoint()->nContent, CRSR_SKIP_CHARS ))
        return true;
    return GoInNode( rPam, fnMove );
}

bool GoInCntntCells( SwPaM & rPam, SwMoveFn fnMove )
{
    if( (*fnMove->fnNd)( &rPam.GetPoint()->nNode.GetNode(),
                         &rPam.GetPoint()->nContent, CRSR_SKIP_CELLS ))
        return true;
    return GoInNode( rPam, fnMove );
}

bool GoInCntntSkipHidden( SwPaM & rPam, SwMoveFn fnMove )
{
    if( (*fnMove->fnNd)( &rPam.GetPoint()->nNode.GetNode(),
                        &rPam.GetPoint()->nContent, CRSR_SKIP_CHARS | CRSR_SKIP_HIDDEN ) )
        return true;
    return GoInNode( rPam, fnMove );
}

bool GoInCntntCellsSkipHidden( SwPaM & rPam, SwMoveFn fnMove )
{
    if( (*fnMove->fnNd)( &rPam.GetPoint()->nNode.GetNode(),
                         &rPam.GetPoint()->nContent, CRSR_SKIP_CELLS | CRSR_SKIP_HIDDEN ) )
        return true;
    return GoInNode( rPam, fnMove );
}

bool GoPrevPara( SwPaM & rPam, SwPosPara aPosPara )
{
    if( rPam.Move( fnMoveBackward, fnGoNode ) )
    {
        // always on a CntntNode
        SwPosition& rPos = *rPam.GetPoint();
        SwCntntNode * pNd = rPos.nNode.GetNode().GetCntntNode();
        rPos.nContent.Assign( pNd,
                            ::GetSttOrEnd( aPosPara == fnMoveForward, *pNd ) );
        return true;
    }
    return false;
}

bool GoCurrPara( SwPaM & rPam, SwPosPara aPosPara )
{
    SwPosition& rPos = *rPam.GetPoint();
    SwCntntNode * pNd = rPos.nNode.GetNode().GetCntntNode();
    if( pNd )
    {
        const sal_Int32 nOld = rPos.nContent.GetIndex();
        const sal_Int32 nNew = aPosPara == fnMoveForward ? 0 : pNd->Len();
        // if already at beginning/end then to the next/previous
        if( nOld != nNew )
        {
            rPos.nContent.Assign( pNd, nNew );
            return true;
        }
    }
    // move node to next/previous CntntNode
    if( ( aPosPara==fnParaStart && 0 != ( pNd =
            GoPreviousNds( &rPos.nNode, true ))) ||
        ( aPosPara==fnParaEnd && 0 != ( pNd =
            GoNextNds( &rPos.nNode, true ))) )
    {
        rPos.nContent.Assign( pNd,
                        ::GetSttOrEnd( aPosPara == fnMoveForward, *pNd ));
        return true;
    }
    return false;
}

bool GoNextPara( SwPaM & rPam, SwPosPara aPosPara )
{
    if( rPam.Move( fnMoveForward, fnGoNode ) )
    {
        // always on a CntntNode
        SwPosition& rPos = *rPam.GetPoint();
        SwCntntNode * pNd = rPos.nNode.GetNode().GetCntntNode();
        rPos.nContent.Assign( pNd,
                        ::GetSttOrEnd( aPosPara == fnMoveForward, *pNd ) );
        return true;
    }
    return false;
}

bool GoCurrSection( SwPaM & rPam, SwMoveFn fnMove )
{
    SwPosition& rPos = *rPam.GetPoint();
    SwPosition aSavePos( rPos ); // position for comparison
    SwNodes& rNds = aSavePos.nNode.GetNodes();
    (rNds.*fnMove->fnSection)( &rPos.nNode );
    SwCntntNode *pNd;
    if( 0 == ( pNd = rPos.nNode.GetNode().GetCntntNode()) &&
        0 == ( pNd = (*fnMove->fnNds)( &rPos.nNode, true )) )
    {
        rPos = aSavePos; // do not change cursor
        return false;
    }

    rPos.nContent.Assign( pNd,
                        ::GetSttOrEnd( fnMove == fnMoveForward, *pNd ) );
    return aSavePos != rPos;
}

bool GoNextSection( SwPaM & rPam, SwMoveFn fnMove )
{
    SwPosition& rPos = *rPam.GetPoint();
    SwPosition aSavePos( rPos ); // position for comparison
    SwNodes& rNds = aSavePos.nNode.GetNodes();
    rNds.GoEndOfSection( &rPos.nNode );

    // no other CntntNode existent?
    if( !GoInCntnt( rPam, fnMoveForward ) )
    {
        rPos = aSavePos; // do not change cursor
        return false;
    }
    (rNds.*fnMove->fnSection)( &rPos.nNode );
    SwCntntNode *pNd = rPos.nNode.GetNode().GetCntntNode();
    rPos.nContent.Assign( pNd,
                        ::GetSttOrEnd( fnMove == fnMoveForward, *pNd ) );
    return true;
}

bool GoPrevSection( SwPaM & rPam, SwMoveFn fnMove )
{
    SwPosition& rPos = *rPam.GetPoint();
    SwPosition aSavePos( rPos ); // position for comparison
    SwNodes& rNds = aSavePos.nNode.GetNodes();
    rNds.GoStartOfSection( &rPos.nNode );

    // no further CntntNode existent?
    if( !GoInCntnt( rPam, fnMoveBackward ))
    {
        rPos = aSavePos; // do not change cursor
        return false;
    }
    (rNds.*fnMove->fnSection)( &rPos.nNode );
    SwCntntNode *pNd = rPos.nNode.GetNode().GetCntntNode();
    rPos.nContent.Assign( pNd,
                            ::GetSttOrEnd( fnMove == fnMoveForward, *pNd ));
    return true;
}

OUString SwPaM::GetTxt() const
{
    OUString aResult;

    SwNodeIndex aNodeIndex = Start()->nNode;

    // The first node can be already the end node.
    // Use a "forever" loop with an exit condition in the middle
    // of its body, in order to correctly handle all cases.
    bool bIsStartNode = true;
    for (;;)
    {
        const bool bIsEndNode = aNodeIndex == End()->nNode;
        SwTxtNode * pTxtNode = aNodeIndex.GetNode().GetTxtNode();

        if (pTxtNode != NULL)
        {
            const OUString aTmpStr = pTxtNode->GetTxt();

            if (bIsStartNode || bIsEndNode)
            {
                // Handle corner cases of start/end node(s)
                const sal_Int32 nStart = bIsStartNode
                    ? Start()->nContent.GetIndex()
                    : 0;
                const sal_Int32 nEnd = bIsEndNode
                    ? End()->nContent.GetIndex()
                    : aTmpStr.getLength();

                aResult += aTmpStr.copy(nStart, nEnd-nStart);
            }
            else
            {
                aResult += aTmpStr;
            }
        }

        if (bIsEndNode)
        {
            break;
        }

        ++aNodeIndex;
        bIsStartNode = false;
    }

    return aResult;
}

void SwPaM::InvalidatePaM()
{
    const SwNode &_pNd = this->GetNode();
    const SwTxtNode *_pTxtNd = _pNd.GetTxtNode();
    if (_pTxtNd != NULL)
    {
        // pretend that the PaM marks inserted text to recalc the portion...
        SwInsTxt aHint( Start()->nContent.GetIndex(),
                        End()->nContent.GetIndex() - Start()->nContent.GetIndex() + 1 );
        SwModify *_pModify=(SwModify*)_pTxtNd;
        _pModify->ModifyNotification( 0, &aHint);
    }
}

std::ostream &operator <<(std::ostream& s, const SwPaM& pam)
{
    if( pam.HasMark())
        return s << "SwPaM (point " << *pam.GetPoint() << ", mark " << *pam.GetMark() << ")";
    else
        return s << "SwPaM (point " << *pam.GetPoint() << ")";
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
