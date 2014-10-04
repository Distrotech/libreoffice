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
#ifndef _SFX_FRMDESCRHXX
#define _SFX_FRMDESCRHXX

#include <rtl/ustring.hxx>
#include <sal/config.h>
#include <sfx2/dllapi.h>
#include <sfx2/sfxsids.hrc>
#include <svl/poolitem.hxx>
#include <tools/gen.hxx>
#include <tools/urlobj.hxx>

class SvStream;
struct SfxFrameDescriptor_Impl;
class SfxFrameDescriptor;
class Wallpaper;


// The SfxFrame descriptors build a recursive structure, that covers all the
// required data in-order to display the frame document.
// Through a SfxFrameSetDescriptor access is given to the whole underlying
// structure. Due to the nature of the SfxFrames document, the
// SfxFramesSetDescriptor is not only the content of this document, but also
// describes the view on it. The FrameSet is made up of lines, which in turn,
// contains the actual window . A line can be horizontally or vertically
// aligned, from which also the alignment of the FrameSet is given.


enum ScrollingMode
{
    ScrollingYes,
    ScrollingNo,
    ScrollingAuto
};

enum SizeSelector
{
    SIZE_ABS,
    SIZE_PERCENT,
    SIZE_REL
};

#define BORDER_SET          2
#define BORDER_YES          1
#define BORDER_NO           0
#define SPACING_NOT_SET     -1L
#define SIZE_NOT_SET        -1L

class SfxItemSet;
struct SfxFrameProperties;

class SFX2_DLLPUBLIC SfxFrameDescriptor
{
    INetURLObject           aURL;
    INetURLObject           aActualURL;
    OUString                aName;
    Size                    aMargin;
    long                    nWidth;
    ScrollingMode           eScroll;
    SizeSelector            eSizeSelector;
    sal_uInt16              nHasBorder;
    sal_uInt16              nItemId;
    bool                    bResizeHorizontal;
    bool                    bResizeVertical;
    bool                    bHasUI;
    bool                    bReadOnly;
    SfxFrameDescriptor_Impl* pImp;

public:
                            SfxFrameDescriptor();
                            ~SfxFrameDescriptor();

                            // FileName/URL
    SfxItemSet*             GetArgs();
    const INetURLObject&    GetURL() const
                            { return aURL; }
    void                    SetURL( const OUString& rURL );
    const INetURLObject&    GetActualURL() const
                            { return aActualURL; }
    void                    SetActualURL( const INetURLObject& rURL );
    void                    SetActualURL( const OUString& rURL );
    void                    SetReadOnly( bool bSet ) { bReadOnly = bSet;}
    bool                    IsReadOnly(  ) const { return bReadOnly;}
    void                    SetEditable( bool bSet );
    bool                    IsEditable() const;

                            // Size
    void                    SetWidth( long n )
                            { nWidth = n; }
    void                    SetWidthPercent( long n )
                            { nWidth = n; eSizeSelector = SIZE_PERCENT; }
    void                    SetWidthRel( long n )
                            { nWidth = n; eSizeSelector = SIZE_REL; }
    void                    SetWidthAbs( long n )
                            { nWidth = n; eSizeSelector = SIZE_ABS; }
    long                    GetWidth() const
                            { return nWidth; }
    SizeSelector            GetSizeSelector() const
                            { return eSizeSelector; }
    bool                    IsResizable() const
                            { return bResizeHorizontal && bResizeVertical; }
    void                    SetResizable( bool bRes )
                            { bResizeHorizontal = bResizeVertical = bRes; }

                            // FrameName
    const OUString&         GetName() const
                            { return aName; }
    void                    SetName( const OUString& rName )
                            { aName = rName; }

                            // Margin, Scrolling
    const Size&             GetMargin() const
                            { return aMargin; }
    void                    SetMargin( const Size& rMargin )
                            { aMargin = rMargin; }
    ScrollingMode           GetScrollingMode() const
                            { return eScroll; }
    void                    SetScrollingMode( ScrollingMode eMode )
                            { eScroll = eMode; }

                            // FrameBorder
    void                    SetWallpaper( const Wallpaper& rWallpaper );
    bool                    HasFrameBorder() const;

    bool                    IsFrameBorderOn() const
                            { return ( nHasBorder & BORDER_YES ) != 0; }

    void                    SetFrameBorder( bool bBorder )
                            {
                                nHasBorder = bBorder ?
                                            BORDER_YES | BORDER_SET :
                                            BORDER_NO | BORDER_SET;
                            }
    bool                    IsFrameBorderSet() const
                            { return (nHasBorder & BORDER_SET) != 0; }
    void                    ResetBorder()
                            { nHasBorder = 0; }

    bool                    HasUI() const
                            { return bHasUI; }
    void                    SetHasUI( bool bOn )
                            { bHasUI = bOn; }

                            // Attribute for Splitwindow
    sal_uInt16                  GetItemId() const
                            { return nItemId; }
    void                    SetItemId( sal_uInt16 nId )
                            { nItemId = nId; }

                            // Copy for example for Views
    SfxFrameDescriptor*     Clone( bool bWithIds = true ) const;
};

// No block to implement a =operator
struct SfxFrameProperties
{
    OUString                            aURL;
    OUString                            aName;
    long                                lMarginWidth;
    long                                lMarginHeight;
    long                                lSize;
    long                                lSetSize;
    long                                lFrameSpacing;
    long                                lInheritedFrameSpacing;
    ScrollingMode                       eScroll;
    SizeSelector                        eSizeSelector;
    SizeSelector                        eSetSizeSelector;
    bool                                bHasBorder;
    bool                                bBorderSet;
    bool                                bResizable;
    bool                                bSetResizable;
    bool                                bIsRootSet;
    bool                                bIsInColSet;
    bool                                bHasBorderInherited;
    SfxFrameDescriptor*                 pFrame;

private:
    SfxFrameProperties( SfxFrameProperties& ) {}
public:
                                        SfxFrameProperties()
                                            : lMarginWidth( SIZE_NOT_SET ),
                                              lMarginHeight( SIZE_NOT_SET ),
                                              lSize( 1L ),
                                              lSetSize( 1L ),
                                              lFrameSpacing( SPACING_NOT_SET ),
                                              lInheritedFrameSpacing( SPACING_NOT_SET ),
                                              eScroll( ScrollingAuto ),
                                              eSizeSelector( SIZE_REL ),
                                              eSetSizeSelector( SIZE_REL ),
                                              bHasBorder( true ),
                                              bBorderSet( true ),
                                              bResizable( true ),
                                              bSetResizable( true ),
                                              bIsRootSet( false ),
                                              bIsInColSet( false ),
                                              bHasBorderInherited( true ),
                                              pFrame( 0 ) {}

                                        ~SfxFrameProperties() { delete pFrame; }

    bool                                operator ==( const SfxFrameProperties& ) const;
    SfxFrameProperties&                 operator =( const SfxFrameProperties &rProp );
};

class SfxFrameDescriptorItem : public SfxPoolItem
{
    SfxFrameProperties                  aProperties;
public:
                                        TYPEINFO_OVERRIDE();

                                        SfxFrameDescriptorItem ( const sal_uInt16 nId = SID_FRAMEDESCRIPTOR )
                                            : SfxPoolItem( nId )
                                        {}

                                        SfxFrameDescriptorItem( const SfxFrameDescriptorItem& rCpy )
                                            : SfxPoolItem( rCpy )
                                        {
                                            aProperties = rCpy.aProperties;
                                        }

    virtual                             ~SfxFrameDescriptorItem();

    virtual bool                        operator ==( const SfxPoolItem& ) const SAL_OVERRIDE;
    SfxFrameDescriptorItem&             operator =( const SfxFrameDescriptorItem & );

    virtual SfxItemPresentation         GetPresentation( SfxItemPresentation ePres,
                                            SfxMapUnit eCoreMetric,
                                            SfxMapUnit ePresMetric,
                                            OUString &rText, const IntlWrapper * = 0 ) const SAL_OVERRIDE;

    virtual SfxPoolItem*                Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;

    const SfxFrameProperties&           GetProperties() const
                                        { return aProperties; }
    void                                SetProperties( const SfxFrameProperties& rProp )
                                        { aProperties = rProp; }
};

#endif // #ifndef _SFX_FRMDESCRHXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
