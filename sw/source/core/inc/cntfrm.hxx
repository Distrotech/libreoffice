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

#ifndef INCLUDED_SW_SOURCE_CORE_INC_CNTFRM_HXX
#define INCLUDED_SW_SOURCE_CORE_INC_CNTFRM_HXX

#include "frame.hxx"
#include "flowfrm.hxx"
#include "cshtyp.hxx"
#include "node.hxx"

class SwLayoutFrm;
class SwCntntNode;
class SwBorderAttrs;
class SwAttrSetChg;
class SwTxtFrm;

// implemented in cntfrm.cxx, used in cntfrm.cxx and crsrsh.cxx
extern bool GetFrmInPage( const SwCntntFrm*, SwWhichPage, SwPosPage, SwPaM* );

class SwCntntFrm: public SwFrm, public SwFlowFrm
{
    friend void MakeNxt( SwFrm *pFrm, SwFrm *pNxt );    // calls MakePrtArea

    // parameter <bObjsInNewUpper>  indicates that objects exist in remaining
    // area of new upper
    bool _WouldFit( SwTwips nSpace,
                    SwLayoutFrm *pNewUpper,
                    bool bTstMove,
                    const bool bObjsInNewUpper );

    virtual void MakeAll() SAL_OVERRIDE;

    void _UpdateAttr( const SfxPoolItem*, const SfxPoolItem*, sal_uInt8 &,
                      SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );

    virtual bool ShouldBwdMoved( SwLayoutFrm *pNewUpper, bool, bool& ) SAL_OVERRIDE;

    const SwCntntFrm* ImplGetNextCntntFrm( bool bFwd ) const;

protected:
    bool MakePrtArea( const SwBorderAttrs & );

    virtual void Modify( const SfxPoolItem*, const SfxPoolItem* ) SAL_OVERRIDE;
    virtual SwTwips ShrinkFrm( SwTwips, bool bTst = false, bool bInfo = false ) SAL_OVERRIDE;
    virtual SwTwips GrowFrm  ( SwTwips, bool bTst = false, bool bInfo = false ) SAL_OVERRIDE;

    SwCntntFrm( SwCntntNode * const, SwFrm* );

public:
    virtual ~SwCntntFrm();
    TYPEINFO_OVERRIDE(); // already in base class

    virtual void Cut() SAL_OVERRIDE;
    virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 ) SAL_OVERRIDE;

    inline const SwCntntNode *GetNode() const;
    inline       SwCntntNode *GetNode();
    sal_uInt16 GetSectionLevel();

    inline const SwCntntFrm *GetFollow() const;
    inline       SwCntntFrm *GetFollow();
    inline const SwCntntFrm *GetPrecede() const;
    inline       SwCntntFrm *GetPrecede();
    SwTxtFrm* FindMaster() const;

    // layout dependent cursor travelling
    virtual bool LeftMargin( SwPaM * ) const;
    virtual bool RightMargin( SwPaM *, bool bAPI = false ) const;
    virtual bool UnitUp( SwPaM *, const SwTwips nOffset = 0,
                         bool bSetInReadOnly = false ) const;
    virtual bool UnitDown( SwPaM *, const SwTwips nOffset = 0,
                           bool bSetInReadOnly = false ) const;
    inline  bool StartNextPage( SwPaM * ) const;
    inline  bool StartPrevPage( SwPaM * ) const;
    inline  bool StartCurrPage( SwPaM * ) const;
    inline  bool EndCurrPage( SwPaM * ) const;
    inline  bool EndNextPage( SwPaM * ) const;
    inline  bool EndPrevPage( SwPaM * ) const;

    // nMaxHeight is the required height
    // bSplit indicates that the paragraph has to be split
    // bTst indicates that we are currently doing a test formatting
    virtual bool WouldFit( SwTwips &nMaxHeight, bool &bSplit, bool bTst );

    bool MoveFtnCntFwd( bool, SwFtnBossFrm* ); // called by MoveFwd if content

    inline  SwCntntFrm* GetNextCntntFrm() const;
    inline  SwCntntFrm* GetPrevCntntFrm() const;
    static bool CalcLowers( SwLayoutFrm* pLay, const SwLayoutFrm* pDontLeave, long nBottom, bool bSkipRowSpanCells );
    void RegisterToNode( SwCntntNode& );
};

inline SwCntntFrm* SwCntntFrm::GetNextCntntFrm() const
{
    if ( GetNext() && GetNext()->IsCntntFrm() )
        return const_cast<SwCntntFrm*>(static_cast<const SwCntntFrm*>(GetNext()));
    else
        return (SwCntntFrm*)ImplGetNextCntntFrm( true );
}

inline SwCntntFrm* SwCntntFrm::GetPrevCntntFrm() const
{
    if ( GetPrev() && GetPrev()->IsCntntFrm() )
        return const_cast<SwCntntFrm*>(static_cast<const SwCntntFrm*>(GetPrev()));
    else
        return (SwCntntFrm*)ImplGetNextCntntFrm( false );
}

inline SwCntntNode *SwCntntFrm::GetNode()
{
    return static_cast< SwCntntNode* >( GetDep() );
}
inline const SwCntntNode *SwCntntFrm::GetNode() const
{
    return static_cast< const SwCntntNode* >( GetDep() );
}

inline const SwCntntFrm *SwCntntFrm::GetFollow() const
{
    return static_cast<const SwCntntFrm*>(SwFlowFrm::GetFollow());
}
inline SwCntntFrm *SwCntntFrm::GetFollow()
{
    return static_cast<SwCntntFrm*>(SwFlowFrm::GetFollow());
}

inline const SwCntntFrm *SwCntntFrm::GetPrecede() const
{
    return static_cast<const SwCntntFrm*>(SwFlowFrm::GetPrecede());
}
inline SwCntntFrm *SwCntntFrm::GetPrecede()
{
    return static_cast<SwCntntFrm*>(SwFlowFrm::GetPrecede());
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
