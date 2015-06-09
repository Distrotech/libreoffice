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

#ifndef INCLUDED_SD_SOURCE_UI_INC_OPTSITEM_HXX
#define INCLUDED_SD_SOURCE_UI_INC_OPTSITEM_HXX

#include <unotools/configitem.hxx>
#include <sfx2/module.hxx>
#include <sfx2/app.hxx>
#include <sfx2/sfxsids.hrc>
#include <svx/optgrid.hxx>
#include <svx/dlgutil.hxx>
#include "sddllapi.h"

// Option ranges

#define SD_OPTIONS_NONE     0x00000000
#define SD_OPTIONS_ALL      0xffffffff

#define SD_OPTIONS_LAYOUT   0x00000001
#define SD_OPTIONS_CONTENTS 0x00000002
#define SD_OPTIONS_MISC     0x00000004
#define SD_OPTIONS_SNAP     0x00000008
#define SD_OPTIONS_ZOOM     0x00000010
#define SD_OPTIONS_GRID     0x00000020
#define SD_OPTIONS_PRINT    0x00000040

class SdOptions;

namespace sd {
class FrameView;
}

class SdOptionsGeneric;

class SD_DLLPUBLIC SdOptionsItem : public ::utl::ConfigItem
{

private:

    const SdOptionsGeneric& mrParent;

public:

    SdOptionsItem( const SdOptionsGeneric& rParent, const OUString& rSubTree );
    virtual ~SdOptionsItem();

    virtual void            Commit() SAL_OVERRIDE;
    virtual void            Notify( const com::sun::star::uno::Sequence<OUString>& aPropertyNames) SAL_OVERRIDE;

    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > GetProperties(
                                const ::com::sun::star::uno::Sequence< OUString >& rNames );
    bool                    PutProperties( const com::sun::star::uno::Sequence< OUString >& rNames,
                                           const com::sun::star::uno::Sequence< com::sun::star::uno::Any>& rValues );
    void                    SetModified();
};

class SD_DLLPUBLIC SdOptionsGeneric
{
friend class SdOptionsItem;

private:

    OUString         maSubTree;
    SdOptionsItem*          mpCfgItem;
    sal_uInt16                  mnConfigId;
    bool                    mbInit          : 1;
    bool                    mbEnableModify  : 1;

    SAL_DLLPRIVATE void Commit( SdOptionsItem& rCfgItem ) const;
    SAL_DLLPRIVATE ::com::sun::star::uno::Sequence< OUString > GetPropertyNames() const;

protected:

    void                    Init() const;
    void                    OptionsChanged() { if( mpCfgItem && mbEnableModify ) mpCfgItem->SetModified(); }

protected:

    virtual void            GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const = 0;
    virtual bool            ReadData( const ::com::sun::star::uno::Any* pValues ) = 0;
    virtual bool            WriteData( ::com::sun::star::uno::Any* pValues ) const = 0;

public:

                            SdOptionsGeneric( sal_uInt16 nConfigId, const OUString& rSubTree );
                            virtual ~SdOptionsGeneric();

    const OUString&  GetSubTree() const { return maSubTree; }
    sal_uInt16                  GetConfigId() const { return mnConfigId; }

    void                    EnableModify( bool bModify ) { mbEnableModify = bModify; }

    void                    Store();

    static bool             isMetricSystem();
};

class SD_DLLPUBLIC SdOptionsLayout : public SdOptionsGeneric
{
private:

    bool    bRuler; // Layout/Display/Ruler
    bool    bMoveOutline;   // Layout/Display/Contur
    bool    bDragStripes;   // Layout/Display/Guide
    bool    bHandlesBezier; // Layout/Display/Bezier
    bool    bHelplines; // Layout/Display/Helpline
    sal_uInt16  nMetric;                    // Layout/Other/MeasureUnit
    sal_uInt16  nDefTab;                    // Layout/Other/TabStop

protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:
            SdOptionsLayout( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsLayout() {}

    bool    operator==( const SdOptionsLayout& rOpt ) const;

    bool    IsRulerVisible() const { Init(); return bRuler; }
    bool    IsMoveOutline() const { Init(); return bMoveOutline; }
    bool    IsDragStripes() const { Init(); return bDragStripes; }
    bool    IsHandlesBezier() const { Init(); return bHandlesBezier; }
    bool    IsHelplines() const { Init(); return bHelplines; }
    sal_uInt16  GetMetric() const { Init(); return( ( 0xffff == nMetric ) ? (sal_uInt16)SfxModule::GetCurrentFieldUnit() : nMetric ); }
    sal_uInt16  GetDefTab() const { Init(); return nDefTab; }

    void    SetRulerVisible( bool bOn = true ) { if( bRuler != bOn ) { OptionsChanged(); bRuler = bOn; } }
    void    SetMoveOutline( bool bOn = true ) { if( bMoveOutline != bOn ) { OptionsChanged(); bMoveOutline = bOn; } }
    void    SetDragStripes( bool bOn = true ) { if( bDragStripes != bOn ) { OptionsChanged(); bDragStripes = bOn; } }
    void    SetHandlesBezier( bool bOn = true ) { if( bHandlesBezier != bOn ) { OptionsChanged(); bHandlesBezier = bOn; } }
    void    SetHelplines( bool bOn = true ) { if( bHelplines != bOn ) { OptionsChanged(); bHelplines = bOn; } }
    void    SetMetric( sal_uInt16 nInMetric ) { if( nMetric != nInMetric ) { OptionsChanged(); nMetric = nInMetric; } }
    void    SetDefTab( sal_uInt16 nTab ) { if( nDefTab != nTab ) { OptionsChanged(); nDefTab = nTab; } }
};

class SD_DLLPUBLIC SdOptionsLayoutItem : public SfxPoolItem
{
public:

                            SdOptionsLayoutItem( sal_uInt16 nWhich);
                            SdOptionsLayoutItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;

    void                    SetOptions( SdOptions* pOpts ) const;

    SdOptionsLayout&        GetOptionsLayout() { return maOptionsLayout; }
private:
    SdOptionsLayout maOptionsLayout;
};

class SD_DLLPUBLIC SdOptionsContents : public SdOptionsGeneric
{
private:
protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsContents( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsContents() {}

    bool    operator==( const SdOptionsContents& rOpt ) const;
};

class SD_DLLPUBLIC SdOptionsContentsItem : public SfxPoolItem
{
public:

                            SdOptionsContentsItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;

    void                    SetOptions( SdOptions* pOpts ) const;

    SdOptionsContents&      GetOptionsContents() { return maOptionsContents; }
private:
    SdOptionsContents       maOptionsContents;
};

class SD_DLLPUBLIC SdOptionsMisc : public SdOptionsGeneric
{
private:

    sal_uLong   nDefaultObjectSizeWidth;
    sal_uLong   nDefaultObjectSizeHeight;

    bool    bStartWithTemplate      : 1;    // Misc/NewDoc/AutoPilot
    bool    bMarkedHitMovesAlways   : 1;    // Misc/ObjectMoveable
    bool    bMoveOnlyDragging       : 1;    // Currently, not in use !!!
    bool    bCrookNoContortion      : 1;    // Misc/NoDistort
    bool    bQuickEdit              : 1;    // Misc/TextObject/QuickEditing
    bool    bMasterPageCache        : 1;    // Misc/BackgroundCache
    bool    bDragWithCopy           : 1;    // Misc/CopyWhileMoving
    bool    bPickThrough            : 1;    // Misc/TextObject/Selectable
    bool    bDoubleClickTextEdit    : 1;    // Misc/DclickTextedit
    bool    bClickChangeRotation    : 1;    // Misc/RotateClick
    bool    bEnableSdremote         : 1;    // Misc/Start/EnableSdremote
    bool    bEnablePresenterScreen : 1;    // Misc/Start/EnablePresenterDisplay
    bool    bSolidDragging          : 1;    // Misc/ModifyWithAttributes
    bool    bSummationOfParagraphs  : 1;    // misc/SummationOfParagraphs
    bool    bShowUndoDeleteWarning  : 1;    // Misc/ShowUndoDeleteWarning
    // #i75315#
    bool    bSlideshowRespectZOrder : 1;    // Misc/SlideshowRespectZOrder
    bool    bShowComments           : 1;    // Misc/ShowComments

    bool    bPreviewNewEffects;
    bool    bPreviewChangedEffects;
    bool    bPreviewTransitions;

    sal_Int32   mnDisplay;

    sal_Int32 mnPenColor;
    double mnPenWidth;

    /** This value controls the device to use for formatting documents.
        The currently supported values are 0 for the current printer or 1
        for the printer independent virtual device the can be retrieved from
        the modules.
    */
    sal_uInt16  mnPrinterIndependentLayout;     // Misc/Compatibility/PrinterIndependentLayout
// Misc

protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsMisc( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsMisc() {}

    bool    operator==( const SdOptionsMisc& rOpt ) const;

    bool    IsStartWithTemplate() const { Init(); return bStartWithTemplate; }
    bool    IsMarkedHitMovesAlways() const { Init(); return bMarkedHitMovesAlways; }
    bool    IsMoveOnlyDragging() const { Init(); return bMoveOnlyDragging; }
    bool    IsCrookNoContortion() const { Init(); return bCrookNoContortion; }
    bool    IsQuickEdit() const { Init(); return bQuickEdit; }
    bool    IsMasterPagePaintCaching() const { Init(); return bMasterPageCache; }
    bool    IsDragWithCopy() const { Init(); return bDragWithCopy; }
    bool    IsPickThrough() const { Init(); return bPickThrough; }
    bool    IsDoubleClickTextEdit() const { Init(); return bDoubleClickTextEdit; }
    bool    IsClickChangeRotation() const { Init(); return bClickChangeRotation; }
    bool    IsEnableSdremote() const { Init(); return bEnableSdremote; }
    bool    IsEnablePresenterScreen() const { Init(); return bEnablePresenterScreen; }
    bool    IsSolidDragging() const { Init(); return bSolidDragging; }
    bool    IsSummationOfParagraphs() const { Init(); return bSummationOfParagraphs; };

    /** Return the currently selected printer independent layout mode.
        @return
            Returns 1 for printer independent layout enabled and 0 when it
            is disabled.  Other values are reserved for future use.
    */
    sal_uInt16  GetPrinterIndependentLayout() const { Init(); return mnPrinterIndependentLayout; };
    bool    IsShowUndoDeleteWarning() const { Init(); return bShowUndoDeleteWarning; }
    bool    IsSlideshowRespectZOrder() const { Init(); return bSlideshowRespectZOrder; }
    sal_uLong   GetDefaultObjectSizeWidth() const { Init(); return nDefaultObjectSizeWidth; }
    sal_uLong   GetDefaultObjectSizeHeight() const { Init(); return nDefaultObjectSizeHeight; }

    bool    IsPreviewNewEffects() const { Init(); return bPreviewNewEffects; }
    bool    IsPreviewChangedEffects() const { Init(); return bPreviewChangedEffects; }
    bool    IsPreviewTransitions() const { Init(); return bPreviewTransitions; }

    sal_Int32   GetDisplay() const;
    void        SetDisplay( sal_Int32 nDisplay = 0 );

    sal_Int32 GetPresentationPenColor() const { Init(); return mnPenColor; }
    void      SetPresentationPenColor( sal_Int32 nPenColor ) { if( mnPenColor != nPenColor ) { OptionsChanged(); mnPenColor = nPenColor; } }

    double    GetPresentationPenWidth() const { Init(); return mnPenWidth; }
    void      SetPresentationPenWidth( double nPenWidth ) { if( mnPenWidth != nPenWidth ) { OptionsChanged(); mnPenWidth = nPenWidth; } }

    void    SetStartWithTemplate( bool bOn = true ) { if( bStartWithTemplate != bOn ) { OptionsChanged(); bStartWithTemplate = bOn; } }
    void    SetMarkedHitMovesAlways( bool bOn = true ) { if( bMarkedHitMovesAlways != bOn ) { OptionsChanged(); bMarkedHitMovesAlways = bOn; } }
    void    SetMoveOnlyDragging( bool bOn = true ) { if( bMoveOnlyDragging != bOn ) { OptionsChanged(); bMoveOnlyDragging = bOn; } }
    void    SetCrookNoContortion( bool bOn = true ) { if( bCrookNoContortion != bOn ) { OptionsChanged(); bCrookNoContortion = bOn; } }
    void    SetQuickEdit( bool bOn = true ) { if( bQuickEdit != bOn ) { OptionsChanged(); bQuickEdit = bOn; } }
    void    SetMasterPagePaintCaching( bool bOn = true ) { if( bMasterPageCache != bOn ) { OptionsChanged(); bMasterPageCache = bOn; } }
    void    SetDragWithCopy( bool bOn = true ) { if( bDragWithCopy != bOn ) { OptionsChanged(); bDragWithCopy = bOn; } }
    void    SetPickThrough( bool bOn = true ) { if( bPickThrough != bOn ) { OptionsChanged(); bPickThrough = bOn; } }
    void    SetDoubleClickTextEdit( bool bOn = true ) { if( bDoubleClickTextEdit != bOn ) { OptionsChanged(); bDoubleClickTextEdit = bOn; } }
    void    SetClickChangeRotation( bool bOn = true ) { if( bClickChangeRotation != bOn ) { OptionsChanged(); bClickChangeRotation = bOn; } }
    void    SetEnableSdremote( bool bOn = true ) { if( bEnableSdremote != bOn ) { OptionsChanged(); bEnableSdremote = bOn; } }
    void    SetEnablePresenterScreen( bool bOn = true ) { if( bEnablePresenterScreen != bOn ) { OptionsChanged(); bEnablePresenterScreen = bOn; } }
    void    SetSummationOfParagraphs( bool bOn = true ){ if ( bOn != bSummationOfParagraphs ) { OptionsChanged(); bSummationOfParagraphs = bOn; } }
    /** Set the printer independent layout mode.
        @param nOn
            The default value is to switch printer independent layout on,
            hence the parameters name.  Use 0 for turning it off.  Other
            values are reserved for future use.
    */
    void    SetPrinterIndependentLayout (sal_uInt16 nOn = 1 ){ if ( nOn != mnPrinterIndependentLayout ) { OptionsChanged(); mnPrinterIndependentLayout = nOn; } }
    void    SetSolidDragging( bool bOn = true ) { if( bSolidDragging != bOn ) { OptionsChanged(); bSolidDragging = bOn; } }
    void    SetShowUndoDeleteWarning( bool bOn = true ) { if( bShowUndoDeleteWarning != bOn ) { OptionsChanged(); bShowUndoDeleteWarning = bOn; } }
    void    SetSlideshowRespectZOrder( bool bOn = true ) { if( bSlideshowRespectZOrder != bOn ) { OptionsChanged(); bSlideshowRespectZOrder = bOn; } }
    void    SetDefaultObjectSizeWidth( sal_uLong nWidth ) { if( nDefaultObjectSizeWidth != nWidth ) { OptionsChanged(); nDefaultObjectSizeWidth = nWidth; } }
    void    SetDefaultObjectSizeHeight( sal_uLong nHeight ) { if( nDefaultObjectSizeHeight != nHeight ) { OptionsChanged(); nDefaultObjectSizeHeight = nHeight; } }

    void    SetPreviewNewEffects( bool bOn )  { if( bPreviewNewEffects != bOn ) { OptionsChanged(); bPreviewNewEffects = bOn; } }
    void    SetPreviewChangedEffects( bool bOn )  { if( bPreviewChangedEffects != bOn ) { OptionsChanged(); bPreviewChangedEffects = bOn; } }
    void    SetPreviewTransitions( bool bOn )  { if( bPreviewTransitions != bOn ) { OptionsChanged(); bPreviewTransitions = bOn; } }

    bool    IsShowComments() const { Init(); return bShowComments; }
    void    SetShowComments( bool bShow )  { if( bShowComments != bShow ) { OptionsChanged(); bShowComments = bShow; } }
};

class SD_DLLPUBLIC SdOptionsMiscItem : public SfxPoolItem
{
public:

                            SdOptionsMiscItem( sal_uInt16 nWhich);
                            SdOptionsMiscItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;

    void                    SetOptions( SdOptions* pOpts ) const;

    SdOptionsMisc&          GetOptionsMisc() { return maOptionsMisc; }
    const SdOptionsMisc&    GetOptionsMisc() const { return maOptionsMisc; }
private:
    SdOptionsMisc           maOptionsMisc;
};

class SD_DLLPUBLIC SdOptionsSnap : public SdOptionsGeneric
{
private:

    bool    bSnapHelplines  : 1;    // Snap/Object/SnapLine
    bool    bSnapBorder     : 1;    // Snap/Object/PageMargin
    bool    bSnapFrame      : 1;    // Snap/Object/ObjectFrame
    bool    bSnapPoints     : 1;    // Snap/Object/ObjectPoint
    bool    bOrtho          : 1;    // Snap/Position/CreatingMoving
    bool    bBigOrtho       : 1;    // Snap/Position/ExtendEdges
    bool    bRotate         : 1;    // Snap/Position/Rotating
    sal_Int16   nSnapArea;              // Snap/Object/Range
    sal_Int16   nAngle;                 // Snap/Position/RotatingValue
    sal_Int16   nBezAngle;              // Snap/Position/PointReduction

protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsSnap( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsSnap() {}

    bool    operator==( const SdOptionsSnap& rOpt ) const;

    bool    IsSnapHelplines() const { Init(); return bSnapHelplines; }
    bool    IsSnapBorder() const { Init(); return bSnapBorder; }
    bool    IsSnapFrame() const { Init(); return bSnapFrame; }
    bool    IsSnapPoints() const { Init(); return bSnapPoints; }
    bool    IsOrtho() const { Init(); return bOrtho; }
    bool    IsBigOrtho() const { Init(); return bBigOrtho; }
    bool    IsRotate() const { Init(); return bRotate; }
    sal_Int16   GetSnapArea() const { Init(); return nSnapArea; }
    sal_Int16   GetAngle() const { Init(); return nAngle; }
    sal_Int16   GetEliminatePolyPointLimitAngle() const { Init(); return nBezAngle; }

    void    SetSnapHelplines( bool bOn = true ) { if( bSnapHelplines != bOn ) { OptionsChanged(); bSnapHelplines = bOn; } }
    void    SetSnapBorder( bool bOn = true ) { if( bSnapBorder != bOn ) { OptionsChanged(); bSnapBorder = bOn; } }
    void    SetSnapFrame( bool bOn = true ) { if( bSnapFrame != bOn ) { OptionsChanged(); bSnapFrame = bOn; } }
    void    SetSnapPoints( bool bOn = true ) { if( bSnapPoints != bOn ) { OptionsChanged(); bSnapPoints = bOn; } }
    void    SetOrtho( bool bOn = true ) { if( bOrtho != bOn ) { OptionsChanged(); bOrtho = bOn; } }
    void    SetBigOrtho( bool bOn = true ) { if( bBigOrtho != bOn ) { OptionsChanged(); bBigOrtho = bOn; } }
    void    SetRotate( bool bOn = true ) { if( bRotate != bOn ) { OptionsChanged(); bRotate = bOn; } }
    void    SetSnapArea( sal_Int16 nIn ) { if( nSnapArea != nIn ) { OptionsChanged(); nSnapArea = nIn; } }
    void    SetAngle( sal_Int16 nIn ) { if( nAngle != nIn ) { OptionsChanged(); nAngle = nIn; } }
    void    SetEliminatePolyPointLimitAngle( sal_Int16 nIn ) { if( nBezAngle != nIn ) { OptionsChanged(); nBezAngle = nIn; } }
};

class SD_DLLPUBLIC SdOptionsSnapItem : public SfxPoolItem
{
public:

                            SdOptionsSnapItem( sal_uInt16 nWhich);
                            SdOptionsSnapItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;

    void                    SetOptions( SdOptions* pOpts ) const;

    SdOptionsSnap&          GetOptionsSnap() { return maOptionsSnap; }
private:
    SdOptionsSnap           maOptionsSnap;
};

class SdOptionsZoom : public SdOptionsGeneric
{
private:

    sal_Int32   nX; // Zoom/ScaleX
    sal_Int32   nY; // Zoom/ScaleY

protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsZoom( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsZoom() {}

    bool    operator==( const SdOptionsZoom& rOpt ) const;

    void    GetScale( sal_Int32& rX, sal_Int32& rY ) const { Init(); rX = nX; rY = nY; }
    void    SetScale( sal_Int32 nInX, sal_Int32 nInY ) { if( nX != nInX || nY != nInY ) { OptionsChanged(); nX = nInX; nY = nInY; } }
};

class SdOptionsGrid : public SdOptionsGeneric, public SvxOptionsGrid
{
protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsGrid( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsGrid();

    void    SetDefaults();
    bool    operator==( const SdOptionsGrid& rOpt ) const;

    sal_uInt32  GetFldDrawX() const { Init(); return SvxOptionsGrid::GetFldDrawX(); }
    sal_uInt32  GetFldDivisionX() const { Init(); return SvxOptionsGrid::GetFldDivisionX(); }
    sal_uInt32  GetFldDrawY() const { Init(); return SvxOptionsGrid::GetFldDrawY(); }
    sal_uInt32  GetFldDivisionY() const { Init(); return SvxOptionsGrid::GetFldDivisionY(); }
    sal_uInt32  GetFldSnapX() const { Init(); return SvxOptionsGrid::GetFldSnapX(); }
    sal_uInt32  GetFldSnapY() const { Init(); return SvxOptionsGrid::GetFldSnapY(); }
    bool    IsUseGridSnap() const { Init(); return SvxOptionsGrid::GetUseGridSnap(); }
    bool    IsSynchronize() const { Init(); return SvxOptionsGrid::GetSynchronize(); }
    bool    IsGridVisible() const { Init(); return SvxOptionsGrid::GetGridVisible(); }
    bool    IsEqualGrid() const { Init(); return SvxOptionsGrid::GetEqualGrid(); }

    void    SetFldDrawX( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldDrawX() ) { OptionsChanged(); SvxOptionsGrid::SetFldDrawX( nSet ); } }
    void    SetFldDivisionX( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldDivisionX() ) { OptionsChanged(); SvxOptionsGrid::SetFldDivisionX( nSet ); } }
    void    SetFldDrawY( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldDrawY() ) { OptionsChanged(); SvxOptionsGrid::SetFldDrawY( nSet ); } }
    void    SetFldDivisionY( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldDivisionY() ) { OptionsChanged(); SvxOptionsGrid::SetFldDivisionY( nSet ); } }
    void    SetFldSnapX( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldSnapX() ) { OptionsChanged(); SvxOptionsGrid::SetFldSnapX( nSet ); } }
    void    SetFldSnapY( sal_uInt32 nSet ) { if( nSet != SvxOptionsGrid::GetFldSnapY() ) { OptionsChanged(); SvxOptionsGrid::SetFldSnapY( nSet ); } }
    void    SetUseGridSnap( bool bSet ) { if( bSet != SvxOptionsGrid::GetUseGridSnap() ) { OptionsChanged(); SvxOptionsGrid::SetUseGridSnap( bSet ); } }
    void    SetSynchronize( bool bSet ) { if( bSet != SvxOptionsGrid::GetSynchronize() ) { OptionsChanged(); SvxOptionsGrid::SetSynchronize( bSet ); } }
    void    SetGridVisible( bool bSet ) { if( bSet != SvxOptionsGrid::GetGridVisible() ) { OptionsChanged(); SvxOptionsGrid::SetGridVisible( bSet ); } }
    void    SetEqualGrid( bool bSet ) { if( bSet != SvxOptionsGrid::GetEqualGrid() ) { OptionsChanged(); SvxOptionsGrid::SetEqualGrid( bSet ); } }
};

class SdOptionsGridItem : public SvxGridItem
{

public:
                            SdOptionsGridItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    void                    SetOptions( SdOptions* pOpts ) const;
};

class SD_DLLPUBLIC SdOptionsPrint : public SdOptionsGeneric
{
private:

    bool    bDraw               : 1;    // Print/Content/Drawing
    bool    bNotes              : 1;    // Print/Content/Note
    bool    bHandout            : 1;    // Print/Content/Handout
    bool    bOutline            : 1;    // Print/Content/Outline
    bool    bDate               : 1;    // Print/Other/Date
    bool    bTime               : 1;    // Print/Other/Time
    bool    bPagename           : 1;    // Print/Other/PageName
    bool    bHiddenPages        : 1;    // Print/Other/HiddenPage
    bool    bPagesize           : 1;    // Print/Page/PageSize
    bool    bPagetile           : 1;    // Print/Page/PageTile
    bool    bWarningPrinter     : 1;    //  These flags you get
    bool    bWarningSize        : 1;    //  from the common options,
    bool    bWarningOrientation : 1;    //  currently org.openoffice.Office.Common.xml (class OfaMiscCfg ; sfx2/misccfg.hxx )
    bool    bBooklet            : 1;    // Print/Page/Booklet
    bool    bFront              : 1;    // Print/Page/BookletFront
    bool    bBack               : 1;    // Print/Page/BookletFront
    bool    bCutPage            : 1;    // NOT persistent !!!
    bool    bPaperbin           : 1;    // Print/Other/FromPrinterSetup
    bool    mbHandoutHorizontal : 1;    // Order Page previews on Handout Pages horizontal
    sal_uInt16  mnHandoutPages;             // Number of page previews on handout page (only 1/2/4/6/9 are supported)
    sal_uInt16  nQuality;                   // Print/Other/Quality

protected:

    virtual void GetPropNameArray( const char**& ppNames, sal_uLong& rCount ) const SAL_OVERRIDE;
    virtual bool ReadData( const ::com::sun::star::uno::Any* pValues ) SAL_OVERRIDE;
    virtual bool WriteData( ::com::sun::star::uno::Any* pValues ) const SAL_OVERRIDE;

public:

            SdOptionsPrint( sal_uInt16 nConfigId, bool bUseConfig );
            virtual ~SdOptionsPrint() {}

    bool    operator==( const SdOptionsPrint& rOpt ) const;

    bool    IsDraw() const { Init(); return bDraw; }
    bool    IsNotes() const { Init(); return bNotes; }
    bool    IsHandout() const { Init(); return bHandout; }
    bool    IsOutline() const { Init(); return bOutline; }
    bool    IsDate() const { Init(); return bDate; }
    bool    IsTime() const { Init(); return bTime; }
    bool    IsPagename() const { Init(); return bPagename; }
    bool    IsHiddenPages() const { Init(); return bHiddenPages; }
    bool    IsPagesize() const { Init(); return bPagesize; }
    bool    IsPagetile() const { Init(); return bPagetile; }
    bool    IsWarningPrinter() const { Init(); return bWarningPrinter; }
    bool    IsWarningSize() const { Init(); return bWarningSize; }
    bool    IsWarningOrientation() const { Init(); return bWarningOrientation; }
    bool    IsBooklet() const { Init(); return bBooklet; }
    bool    IsFrontPage() const { Init(); return bFront; }
    bool    IsBackPage() const { Init(); return bBack; }
    bool    IsCutPage() const { Init(); return bCutPage; }
    bool    IsPaperbin() const { Init(); return bPaperbin; }
    sal_uInt16  GetOutputQuality() const { Init(); return nQuality; }
    bool    IsHandoutHorizontal() const { Init(); return mbHandoutHorizontal; }
    sal_uInt16  GetHandoutPages() const { Init(); return mnHandoutPages; }

    void    SetDraw( bool bOn = true ) { if( bDraw != bOn ) { OptionsChanged(); bDraw = bOn; } }
    void    SetNotes( bool bOn = true ) { if( bNotes != bOn ) { OptionsChanged(); bNotes = bOn; } }
    void    SetHandout( bool bOn = true ) { if( bHandout != bOn ) { OptionsChanged(); bHandout = bOn; } }
    void    SetOutline( bool bOn = true ) { if( bOutline != bOn ) { OptionsChanged(); bOutline = bOn; } }
    void    SetDate( bool bOn = true ) { if( bDate != bOn ) { OptionsChanged(); bDate = bOn; } }
    void    SetTime( bool bOn = true ) { if( bTime != bOn ) { OptionsChanged(); bTime = bOn; } }
    void    SetPagename( bool bOn = true ) { if( bPagename != bOn ) { OptionsChanged(); bPagename = bOn; } }
    void    SetHiddenPages( bool bOn = true ) { if( bHiddenPages != bOn ) { OptionsChanged(); bHiddenPages = bOn; } }
    void    SetPagesize( bool bOn = true ) { if( bPagesize != bOn ) { OptionsChanged(); bPagesize = bOn; } }
    void    SetPagetile( bool bOn = true ) { if( bPagetile != bOn ) { OptionsChanged(); bPagetile = bOn; } }
    void    SetWarningPrinter( bool bOn = true ) { if( bWarningPrinter != bOn ) { OptionsChanged(); bWarningPrinter = bOn; } }
    void    SetWarningSize( bool bOn = true ) { if( bWarningSize != bOn ) { OptionsChanged(); bWarningSize = bOn; } }
    void    SetWarningOrientation( bool bOn = true ) { if( bWarningOrientation != bOn ) { OptionsChanged(); bWarningOrientation = bOn; } }
    void    SetBooklet( bool bOn = true ) { if( bBooklet != bOn ) { OptionsChanged(); bBooklet = bOn; } }
    void    SetFrontPage( bool bOn = true ) { if( bFront != bOn ) { OptionsChanged(); bFront = bOn; } }
    void    SetBackPage( bool bOn = true ) { if( bBack != bOn ) { OptionsChanged(); bBack = bOn; } }
    void    SetCutPage( bool bOn = true ) { if( bCutPage != bOn ) { OptionsChanged(); bCutPage = bOn; } }
    void    SetPaperbin( bool bOn = true ) { if( bPaperbin != bOn ) { OptionsChanged(); bPaperbin = bOn; } }
    void    SetOutputQuality( sal_uInt16 nInQuality ) { if( nQuality != nInQuality ) { OptionsChanged(); nQuality = nInQuality; } }
    void    SetHandoutHorizontal( bool bHandoutHorizontal ) { if( mbHandoutHorizontal != bHandoutHorizontal ) { OptionsChanged(); mbHandoutHorizontal = bHandoutHorizontal; } }
    void    SetHandoutPages( sal_uInt16 nHandoutPages ) { if( nHandoutPages != mnHandoutPages ) { OptionsChanged(); mnHandoutPages = nHandoutPages; } }
};

class SD_DLLPUBLIC SdOptionsPrintItem : public SfxPoolItem
{
public:

                            SdOptionsPrintItem( sal_uInt16 nWhich);
                            SdOptionsPrintItem( sal_uInt16 nWhich, SdOptions* pOpts, ::sd::FrameView* pView = NULL );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;

    void                    SetOptions( SdOptions* pOpts ) const;

    SdOptionsPrint&         GetOptionsPrint() { return maOptionsPrint; }
    const SdOptionsPrint&   GetOptionsPrint() const { return maOptionsPrint; }
private:
    SdOptionsPrint  maOptionsPrint;
};

class SdOptions : public SdOptionsLayout, public SdOptionsContents,
                  public SdOptionsMisc, public SdOptionsSnap,
                  public SdOptionsZoom, public SdOptionsGrid,
                  public SdOptionsPrint
{
public:

                        SdOptions( sal_uInt16 nConfigId );
                        virtual ~SdOptions();

    void                StoreConfig( sal_uLong nOptionRange = SD_OPTIONS_ALL );
};

#endif // INCLUDED_SD_SOURCE_UI_INC_OPTSITEM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
