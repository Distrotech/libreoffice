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

#ifndef INCLUDED_SVTOOLS_SOURCE_CONTNR_IMIVCTL_HXX
#define INCLUDED_SVTOOLS_SOURCE_CONTNR_IMIVCTL_HXX

#include <vcl/virdev.hxx>
#include <vcl/scrbar.hxx>
#include <vcl/timer.hxx>
#include <vcl/seleng.hxx>
#include <tools/debug.hxx>
#include "svtaccessiblefactory.hxx"

#include <limits.h>

#include <svtools/ivctrl.hxx>
#include <boost/ptr_container/ptr_map.hpp>

class IcnCursor_Impl;
class SvtIconChoiceCtrl;
class SvxIconChoiceCtrlEntry;
class IcnViewEdit_Impl;
class IcnGridMap_Impl;



// some defines

#define PAINTFLAG_HOR_CENTERED  0x0001
#define PAINTFLAG_VER_CENTERED  0x0002

#define F_VER_SBARSIZE_WITH_HBAR        0x0001
#define F_HOR_SBARSIZE_WITH_VBAR        0x0002
#define F_PAINTED                       0x0004  // true after first paint
#define F_ADD_MODE                      0x0008
#define F_SELECTING_RECT                0x0020
#define F_DOWN_CTRL                     0x0080
#define F_DOWN_DESELECT                 0x0100
#define F_START_EDITTIMER_IN_MOUSEUP    0x0400
#define F_MOVED_ENTRIES                 0x0800
#define F_ENTRYLISTPOS_VALID            0x1000
#define F_CLEARING_SELECTION            0x2000
#define F_ARRANGING                     0x4000

// unit = pixels
// distances from window borders
#define LROFFS_WINBORDER            4
#define TBOFFS_WINBORDER            4
// for the bounding rectangle
#define LROFFS_BOUND                2
#define TBOFFS_BOUND                2
// distance icon to text
#define HOR_DIST_BMP_STRING         3
#define VER_DIST_BMP_STRING         3
//  width offset of highlight rectangle for Text
#define LROFFS_TEXT                 2

#define DEFAULT_MAX_VIRT_WIDTH      200
#define DEFAULT_MAX_VIRT_HEIGHT     200

#define VIEWMODE_MASK   (WB_ICON | WB_SMALLICON | WB_DETAILS)





enum IcnViewFieldType
{
    IcnViewFieldTypeDontknow = 0,
    IcnViewFieldTypeImage = 1,
    IcnViewFieldTypeText = 2
};



// Data about the focus of entries

struct LocalFocus
{
    bool        bOn;
    Rectangle   aRect;
    Color       aPenColor;

    LocalFocus() { bOn = false; }
};



// Entry-List

typedef ::std::vector< SvxIconChoiceCtrlEntry* > SvxIconChoiceCtrlEntryList_impl;

class EntryList_Impl
{
private:
    SvxIconChoiceCtrlEntryList_impl maIconChoiceCtrlEntryList;
    SvxIconChoiceCtrl_Impl*         _pOwner;

public:
                            EntryList_Impl( SvxIconChoiceCtrl_Impl* );
                            ~EntryList_Impl();

    void                    clear();

    size_t                  size()
                            {
                                return maIconChoiceCtrlEntryList.size();
                            }
    size_t                  size() const
                            {
                                return maIconChoiceCtrlEntryList.size();
                            }

    SvxIconChoiceCtrlEntry* operator[]( size_t nPos )
                            {
                                return  ( nPos < maIconChoiceCtrlEntryList.size() )
                                        ? maIconChoiceCtrlEntryList[ nPos ]
                                        : NULL;
                            }
    SvxIconChoiceCtrlEntry* operator[]( size_t nPos ) const
                            {
                                return  ( nPos < maIconChoiceCtrlEntryList.size() )
                                        ? maIconChoiceCtrlEntryList[ nPos ]
                                        : NULL;
                            }
    void                    insert( size_t nPos, SvxIconChoiceCtrlEntry* pEntry );
};




// Implementation-class of IconChoiceCtrl


typedef boost::ptr_map<sal_uInt16, SvxIconChoiceCtrlColumnInfo> SvxIconChoiceCtrlColumnInfoMap;
typedef std::vector<SvxIconChoiceCtrlEntry*> SvxIconChoiceCtrlEntryPtrVec;

class SvxIconChoiceCtrl_Impl
{
    friend class IcnCursor_Impl;
    friend class EntryList_Impl;
    friend class IcnGridMap_Impl;

    bool                    bChooseWithCursor;
    EntryList_Impl          aEntries;
    ScrollBar               aVerSBar;
    ScrollBar               aHorSBar;
    ScrollBarBox            aScrBarBox;
    Rectangle               aCurSelectionRect;
    std::vector<Rectangle*> aSelectedRectList;
    Timer                   aEditTimer;                 // for editing in place
    Timer                   aAutoArrangeTimer;
    Timer                   aDocRectChangedTimer;
    Timer                   aVisRectChangedTimer;
    Timer                   aCallSelectHdlTimer;
    Size                    aVirtOutputSize;
    Size                    aImageSize;
    Size                    aDefaultTextSize;
    Size                    aOutputSize; // Pixel
    Point                   aDDLastEntryPos;
    Point                   aDDLastRectPos;
    Point                   aDDPaintOffs;
    Point                   aDDStartPos;
    SvtIconChoiceCtrl*      pView;
    IcnCursor_Impl*         pImpCursor;
    IcnGridMap_Impl*        pGridMap;
    long                    nMaxVirtWidth;  // max. width aVirtOutputSize for ALIGN_TOP
    long                    nMaxVirtHeight; // max. height aVirtOutputSize for ALIGN_LEFT
    SvxIconChoiceCtrlEntryList_impl*    pZOrderList;
    SvxIconChoiceCtrlColumnInfoMap* pColumns;
    IcnViewEdit_Impl*       pEdit;
    WinBits                 nWinBits;
    long                    nMaxBoundHeight;            // height of highest BoundRects
    sal_uInt16              nFlags;
    sal_uInt16              nCurTextDrawFlags;
    ImplSVEvent *           nUserEventAdjustScrBars;
    ImplSVEvent *           nUserEventShowCursor;
    SvxIconChoiceCtrlEntry* pCurHighlightFrame;
    bool                    bHighlightFramePressed;
    SvxIconChoiceCtrlEntry* pHead;                      // top left entry
    SvxIconChoiceCtrlEntry* pCursor;
    SvxIconChoiceCtrlEntry* pPrevDropTarget;
    SvxIconChoiceCtrlEntry* pHdlEntry;
    SvxIconChoiceCtrlEntry* pDDRefEntry;
    VirtualDevice*          pDDDev;
    VirtualDevice*          pDDBufDev;
    VirtualDevice*          pDDTempDev;
    VirtualDevice*          pEntryPaintDev;
    SvxIconChoiceCtrlEntry* pAnchor;                    // for selection
    LocalFocus              aFocus;                             // Data for focusrect
    ::svt::AccessibleFactoryAccess aAccFactory;

    SvxIconChoiceCtrlEntry* pCurEditedEntry;
    SvxIconChoiceCtrlTextMode eTextMode;
    SelectionMode           eSelectionMode;
    sal_uLong               nSelectionCount;
    SvxIconChoiceCtrlPositionMode ePositionMode;
    bool                    bBoundRectsDirty;
    bool                    bUpdateMode;
    bool                    bEntryEditingEnabled;

    void                ShowCursor( bool bShow );

    void                ImpArrange( bool bKeepPredecessors = false );
    void                AdjustVirtSize( const Rectangle& );
    void                ResetVirtSize();
    void                CheckScrollBars();

                        DECL_LINK( ScrollUpDownHdl, ScrollBar * );
                        DECL_LINK( ScrollLeftRightHdl, ScrollBar * );
                        DECL_LINK(EditTimeoutHdl, void *);
                        DECL_LINK( UserEventHdl, void* );
                        DECL_LINK( EndScrollHdl, void* );
                        DECL_LINK( AutoArrangeHdl, void* );
                        DECL_LINK( DocRectChangedHdl, void* );
                        DECL_LINK( VisRectChangedHdl, void* );
                        DECL_LINK( CallSelectHdlHdl, void* );

    void                AdjustScrollBars( bool bVirtSizeGrowedOnly = false);
    void                PositionScrollBars( long nRealWidth, long nRealHeight );
    long                GetScrollBarPageSize( long nVisibleRange ) const
                        {
                            return ((nVisibleRange*75)/100);
                        }
    long                GetScrollBarLineSize() const
                        {
                            return nMaxBoundHeight / 2;
                        }
    bool                HandleScrollCommand( const CommandEvent& rCmd );
    void                ToDocPos( Point& rPosPixel )
                        {
                            rPosPixel -= pView->GetMapMode().GetOrigin();
                        }
    void                InitScrollBarBox();
    void                ToggleSelection( SvxIconChoiceCtrlEntry* );
    void                DeselectAllBut( SvxIconChoiceCtrlEntry*, bool bPaintSync = false );
    void                Center( SvxIconChoiceCtrlEntry* pEntry ) const;
    void                StopEditTimer() { aEditTimer.Stop(); }
    void                StartEditTimer() { aEditTimer.Start(); }
    void                ImpHideDDIcon();
    void                CallSelectHandler( SvxIconChoiceCtrlEntry* );
    void                SelectRect(
                            SvxIconChoiceCtrlEntry* pEntry1,
                            SvxIconChoiceCtrlEntry* pEntry2,
                            bool bAdd = true,
                            std::vector<Rectangle*>* pOtherRects = 0
                        );

    void                SelectRange(
                            SvxIconChoiceCtrlEntry* pStart,
                            SvxIconChoiceCtrlEntry* pEnd,
                            bool bAdd = true
                        );

    void                AddSelectedRect( const Rectangle& );
    void                AddSelectedRect(
                            SvxIconChoiceCtrlEntry* pEntry1,
                            SvxIconChoiceCtrlEntry* pEntry2
                        );

    void                ClearSelectedRectList();
    void                ClearColumnList();
    Rectangle           CalcMaxTextRect( const SvxIconChoiceCtrlEntry* pEntry ) const;

    void                ClipAtVirtOutRect( Rectangle& rRect ) const;
    void                AdjustAtGrid( const SvxIconChoiceCtrlEntryPtrVec& rRow, SvxIconChoiceCtrlEntry* pStart=0 );
    Point               AdjustAtGrid(
                            const Rectangle& rCenterRect, // balance point of object (typically Bmp-Rect)
                            const Rectangle& rBoundRect
                        ) const;
    sal_uLong           GetPredecessorGrid( const Point& rDocPos) const;

    void                InitPredecessors();
    void                ClearPredecessors();

    bool                CheckVerScrollBar();
    bool                CheckHorScrollBar();
    void                CancelUserEvents();
    void                EntrySelected(
                            SvxIconChoiceCtrlEntry* pEntry,
                            bool bSelect,
                            bool bSyncPaint
                        );
    void                RepaintEntries( sal_uInt16 nEntryFlagsMask );
    void                SetListPositions();
    void                SetDefaultTextSize();
    bool                IsAutoArrange() const
                        {
                            return (ePositionMode == IcnViewPositionModeAutoArrange);
                        }
    bool                IsAutoAdjust() const
                        {
                            return (ePositionMode == IcnViewPositionModeAutoAdjust);
                        }
    void                DocRectChanged() { aDocRectChangedTimer.Start(); }
    void                VisRectChanged() { aVisRectChangedTimer.Start(); }
    void                SetOrigin( const Point&, bool bDoNotUpdateWallpaper = false );

                        DECL_LINK(TextEditEndedHdl, void *);

    void                ShowFocus ( Rectangle& rRect );
    void                DrawFocusRect ( OutputDevice* pOut );

    bool                IsMnemonicChar( sal_Unicode cChar, sal_uLong& rPos ) const;

public:

    long                nGridDX;
    long                nGridDY;
    long                nHorSBarHeight;
    long                nVerSBarWidth;

                        SvxIconChoiceCtrl_Impl( SvtIconChoiceCtrl* pView, WinBits nWinStyle );
                        ~SvxIconChoiceCtrl_Impl();

    bool                SetChoiceWithCursor ( bool bDo = true ) { bool bOld = bChooseWithCursor; bChooseWithCursor = bDo; return bOld; }
    void                Clear( bool bInCtor = false );
    void                SetStyle( WinBits nWinStyle );
    WinBits             GetStyle() const { return nWinBits; }
    void                InsertEntry( SvxIconChoiceCtrlEntry*, size_t nPos, const Point* pPos=0 );
    void                CreateAutoMnemonics( MnemonicGenerator* _pGenerator = NULL );
    void                FontModified();
    void                SelectAll( bool bSelect = true, bool bPaint = true );
    void                SelectEntry(
                            SvxIconChoiceCtrlEntry*,
                            bool bSelect,
                            bool bCallHdl = true,
                            bool bAddToSelection = false,
                            bool bSyncPaint = false
                        );
    void                Paint( const Rectangle& rRect );
    bool                MouseButtonDown( const MouseEvent& );
    bool                MouseButtonUp( const MouseEvent& );
    bool                MouseMove( const MouseEvent&);
    bool                RequestHelp( const HelpEvent& rHEvt );
    void                SetCursor_Impl(
                            SvxIconChoiceCtrlEntry* pOldCursor,
                            SvxIconChoiceCtrlEntry* pNewCursor,
                            bool bMod1,
                            bool bShift,
                            bool bPaintSync = false
                        );
    bool                KeyInput( const KeyEvent& );
    void                Resize();
    void                GetFocus();
    void                LoseFocus();
    void                SetUpdateMode( bool bUpdate );
    bool                GetUpdateMode() const { return bUpdateMode; }
    void                PaintEntry(
                            SvxIconChoiceCtrlEntry* pEntry,
                            bool bIsBackgroundPainted=false
                        );
    void                PaintEntry(
                            SvxIconChoiceCtrlEntry*,
                            const Point&,
                            OutputDevice* pOut = 0,
                            bool bIsBackgroundPainted = false
                        );
    void                PaintEntryVirtOutDev( SvxIconChoiceCtrlEntry* );

    void                SetEntryPos(
                            SvxIconChoiceCtrlEntry* pEntry,
                            const Point& rPos,
                            bool bAdjustRow = false,
                            bool bCheckScrollBars = false,
                            bool bKeepGridMap = false
                        );

    void                InvalidateEntry( SvxIconChoiceCtrlEntry* );

    void                SetNoSelection();

    SvxIconChoiceCtrlEntry* GetCurEntry() const { return pCursor; }
    void                SetCursor(
                            SvxIconChoiceCtrlEntry*,
                            // true == carry selection when single-selecting
                            bool bSyncSingleSelection = true,
                            bool bShowFocusAsync = false
                        );

    SvxIconChoiceCtrlEntry* GetEntry( const Point& rDocPos, bool bHit = false );

    Point               GetEntryPos( SvxIconChoiceCtrlEntry* );
    void                MakeEntryVisible( SvxIconChoiceCtrlEntry* pEntry, bool bBound = true );

    void                Arrange(
                            bool bKeepPredecessors = false,
                            long nSetMaxVirtWidth =0,
                            long nSetMaxVirtHeight =0
                        );

    Rectangle           CalcFocusRect( SvxIconChoiceCtrlEntry* );
    Rectangle           CalcBmpRect( SvxIconChoiceCtrlEntry*, const Point* pPos = 0 );
    Rectangle           CalcTextRect(
                            SvxIconChoiceCtrlEntry*,
                            const Point* pPos = 0,
                            bool bForInplaceEdit = false,
                            const OUString* pStr = 0
                        );

    long                CalcBoundingWidth( SvxIconChoiceCtrlEntry* ) const;
    long                CalcBoundingHeight( SvxIconChoiceCtrlEntry* ) const;
    Size                CalcBoundingSize( SvxIconChoiceCtrlEntry* ) const;
    void                FindBoundingRect( SvxIconChoiceCtrlEntry* pEntry );
    void                SetBoundingRect_Impl(
                            SvxIconChoiceCtrlEntry* pEntry,
                            const Point& rPos,
                            const Size& rBoundingSize
                        );
    // recalculates all invalid BoundRects
    void                RecalcAllBoundingRectsSmart();
    const Rectangle&    GetEntryBoundRect( SvxIconChoiceCtrlEntry* );
    void                InvalidateBoundingRect( Rectangle& rRect )
                        {
                            rRect.Right() = LONG_MAX;
                            bBoundRectsDirty = true;
                        }
    bool            IsBoundingRectValid( const Rectangle& rRect ) const { return ( rRect.Right() != LONG_MAX ); }

    void                PaintEmphasis(
                            const Rectangle& rRect1,
                            const Rectangle& rRect2,
                            bool bSelected,
                            bool bDropTarget,
                            bool bCursored,
                            OutputDevice* pOut,
                            bool bIsBackgroundPainted = false
                        );

    void                PaintItem(
                            const Rectangle& rRect,
                            IcnViewFieldType eItem,
                            SvxIconChoiceCtrlEntry* pEntry,
                            sal_uInt16 nPaintFlags,
                            OutputDevice* pOut,
                            const OUString* pStr = 0,
                            ::vcl::ControlLayoutData* _pLayoutData = NULL
                        );

    // recalculates all BoundingRects if bMustRecalcBoundingRects == true
    void                CheckBoundingRects() { if (bBoundRectsDirty) RecalcAllBoundingRectsSmart(); }
    // recalculates all invalidated BoundingRects
    void                ShowTargetEmphasis( SvxIconChoiceCtrlEntry* pEntry, bool bShow );
    void                Command( const CommandEvent& rCEvt );
    void                ToTop( SvxIconChoiceCtrlEntry* );

    sal_uLong           GetSelectionCount() const;
    void                SetGrid( const Size& );
    Size                GetMinGrid() const;
    void                Scroll( long nDeltaX, long nDeltaY, bool bScrollBar = false );
    const Size&         GetItemSize( SvxIconChoiceCtrlEntry*, IcnViewFieldType ) const;

    void                HideDDIcon();

    bool                IsOver(
                            std::vector<Rectangle*>* pSelectedRectList,
                            const Rectangle& rEntryBoundRect
                        ) const;

    void                SelectRect(
                            const Rectangle&,
                            bool bAdd = true,
                            std::vector<Rectangle*>* pOtherRects = 0
                        );

    bool            IsTextHit( SvxIconChoiceCtrlEntry* pEntry, const Point& rDocPos );
    void                MakeVisible(
                            const Rectangle& rDocPos,
                            bool bInScrollBarEvent=false,
                            bool bCallRectChangedHdl = true
                        );

    void                AdjustEntryAtGrid( SvxIconChoiceCtrlEntry* pStart = 0 );
#ifdef DBG_UTIL
    void                SetEntryTextMode(
                            SvxIconChoiceCtrlTextMode,
                            SvxIconChoiceCtrlEntry* pEntry = 0
                        );
#endif
    void                EnableEntryEditing( bool bEnable ) { bEntryEditingEnabled = bEnable; }
    bool                IsEntryEditingEnabled() const { return bEntryEditingEnabled; }
    bool                IsEntryEditing() const { return (pCurEditedEntry!=0); }
    void                EditEntry( SvxIconChoiceCtrlEntry* pEntry );
    void                StopEntryEditing( bool bCancel );
    size_t              GetEntryCount() const { return aEntries.size(); }
    SvxIconChoiceCtrlEntry* GetEntry( size_t nPos )
                            {
                                return aEntries[ nPos ];
                            }
    SvxIconChoiceCtrlEntry* GetEntry( size_t nPos ) const
                            {
                                return aEntries[ nPos ];
                            }
    SvxIconChoiceCtrlEntry* GetFirstSelectedEntry( sal_uLong& ) const;
    SvxIconChoiceCtrlEntry* GetHdlEntry() const { return pHdlEntry; }
    void                SetHdlEntry( SvxIconChoiceCtrlEntry* pEntry ) { pHdlEntry = pEntry; }

    SvxIconChoiceCtrlTextMode GetEntryTextModeSmart( const SvxIconChoiceCtrlEntry* pEntry ) const;
    void                SetSelectionMode( SelectionMode eMode ) { eSelectionMode=eMode; }
    SelectionMode       GetSelectionMode() const { return eSelectionMode; }
    bool                AreEntriesMoved() const { return ((nFlags & F_MOVED_ENTRIES) != 0); }
    void                SetEntriesMoved( bool bMoved )
                        {
                            if( bMoved )
                                nFlags |= F_MOVED_ENTRIES;
                            else
                                nFlags &= ~(F_MOVED_ENTRIES);
                        }
    sal_uLong           GetEntryListPos( SvxIconChoiceCtrlEntry* ) const;
    void                SetEntryImageSize( const Size& rSize ) { aImageSize = rSize; }
    void                InitSettings();
    Rectangle           GetOutputRect() const;

    bool                ArePredecessorsSet() const { return (pHead != 0); }
    SvxIconChoiceCtrlEntry* GetPredecessorHead() const { return pHead; }
    void                SetEntryPredecessor(SvxIconChoiceCtrlEntry* pEntry,SvxIconChoiceCtrlEntry* pPredecessor);
    // only delivers valid results when in AutoArrange mode!
    SvxIconChoiceCtrlEntry* FindEntryPredecessor( SvxIconChoiceCtrlEntry* pEntry, const Point& );

    void                SetPositionMode( SvxIconChoiceCtrlPositionMode );
    SvxIconChoiceCtrlPositionMode GetPositionMode() const { return ePositionMode;}

    void                SetColumn( sal_uInt16 nIndex, const SvxIconChoiceCtrlColumnInfo& );
    const SvxIconChoiceCtrlColumnInfo* GetColumn( sal_uInt16 nIndex ) const;

    Rectangle           GetDocumentRect() const { return Rectangle( Point(), aVirtOutputSize ); }
    Rectangle           GetVisibleRect() const { return GetOutputRect(); }

    void                SetEntryHighlightFrame(
                            SvxIconChoiceCtrlEntry* pEntry,
                            bool bKeepHighlightFlags = false
                        );
    void                HideEntryHighlightFrame();
    void                DrawHighlightFrame(
                            OutputDevice* pOut,
                            const Rectangle& rBmpRect,
                            bool bHide
                        );
    void                StopSelectTimer() { aCallSelectHdlTimer.Stop(); }

    void                CallEventListeners( sal_uLong nEvent, void* pData = NULL );

    inline ::svt::IAccessibleFactory& GetAccessibleFactory()
                        {
                            return aAccFactory.getFactory();
                        }
};



typedef std::map<sal_uInt16, SvxIconChoiceCtrlEntryPtrVec> IconChoiceMap;

class IcnCursor_Impl
{
    SvxIconChoiceCtrl_Impl* pView;
    boost::scoped_ptr<IconChoiceMap> pColumns;
    boost::scoped_ptr<IconChoiceMap> pRows;
    long                    nCols;
    long                    nRows;
    short                   nDeltaWidth;
    short                   nDeltaHeight;
    SvxIconChoiceCtrlEntry* pCurEntry;
    void                    SetDeltas();
    void                    ImplCreate();
    void                    Create() {  if( !pColumns ) ImplCreate(); }

    sal_uInt16              GetSortListPos(
                                SvxIconChoiceCtrlEntryPtrVec& rList,
                                long nValue,
                                int bVertical);
    SvxIconChoiceCtrlEntry* SearchCol(
                                sal_uInt16 nCol,
                                sal_uInt16 nTop,
                                sal_uInt16 nBottom,
                                sal_uInt16 nPref,
                                bool bDown,
                                bool bSimple
                            );

    SvxIconChoiceCtrlEntry* SearchRow(
                                sal_uInt16 nRow,
                                sal_uInt16 nRight,
                                sal_uInt16 nLeft,
                                sal_uInt16 nPref,
                                bool bRight,
                                bool bSimple
                            );

public:
                            IcnCursor_Impl( SvxIconChoiceCtrl_Impl* pOwner );
                            ~IcnCursor_Impl();
    void                    Clear();

    // for Cursortravelling etc.
    SvxIconChoiceCtrlEntry* GoLeftRight( SvxIconChoiceCtrlEntry*, bool bRight );
    SvxIconChoiceCtrlEntry* GoUpDown( SvxIconChoiceCtrlEntry*, bool bDown );
    SvxIconChoiceCtrlEntry* GoPageUpDown( SvxIconChoiceCtrlEntry*, bool bDown );

    // Creates a list of entries for every row (height = nGridDY) sorted by
    // BoundRect.Left(). A list may be empty. The lists become the property of
    // the caller and have to be deleted with DestroyGridAdjustData.
    void                    CreateGridAjustData( IconChoiceMap& pLists, SvxIconChoiceCtrlEntry* pRow=0);
    static void             DestroyGridAdjustData( IconChoiceMap& rLists );
};



typedef sal_uLong GridId;

#define GRID_NOT_FOUND  ((GridId)ULONG_MAX)

class IcnGridMap_Impl
{
    Rectangle               _aLastOccupiedGrid;
    SvxIconChoiceCtrl_Impl* _pView;
    bool    *               _pGridMap;
    sal_uInt16              _nGridCols, _nGridRows;

    void                Expand();
    void                Create_Impl();
    void                Create() { if(!_pGridMap) Create_Impl(); }

    void                GetMinMapSize( sal_uInt16& rDX, sal_uInt16& rDY ) const;

public:
                        IcnGridMap_Impl(SvxIconChoiceCtrl_Impl* pView);
                        ~IcnGridMap_Impl();

    void                Clear();

    GridId              GetGrid( const Point& rDocPos, bool* pbClipped = 0 );
    GridId              GetGrid( sal_uInt16 nGridX, sal_uInt16 nGridY );
    GridId              GetUnoccupiedGrid( bool bOccupyFound=true );

    void                OccupyGrids( const SvxIconChoiceCtrlEntry*, bool bOccupy = true );
    void                OccupyGrid( GridId nId, bool bOccupy = true )
                        {
                            DBG_ASSERT(!_pGridMap || nId<(sal_uLong)(_nGridCols*_nGridRows),"OccupyGrid: Bad GridId");
                            if(_pGridMap && nId < (sal_uLong)(_nGridCols *_nGridRows) )
                                _pGridMap[ nId ] = bOccupy;
                        }

    Rectangle           GetGridRect( GridId );
    void                GetGridCoord( GridId, sal_uInt16& rGridX, sal_uInt16& rGridY );
    static sal_uLong    GetGridCount(
                            const Size& rSizePixel,
                            sal_uInt16 nGridWidth,
                            sal_uInt16 nGridHeight
                        );

    void                OutputSizeChanged();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
