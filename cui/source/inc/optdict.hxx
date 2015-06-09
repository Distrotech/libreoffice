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
#ifndef INCLUDED_CUI_SOURCE_INC_OPTDICT_HXX
#define INCLUDED_CUI_SOURCE_INC_OPTDICT_HXX

#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>
#include <vcl/group.hxx>
#include <vcl/combobox.hxx>
#include <vcl/timer.hxx>
#include <vcl/edit.hxx>
#include <vcl/decoview.hxx>
#include <com/sun/star/util/Language.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>


#include <svtools/simptabl.hxx>
#include <svx/langbox.hxx>

namespace com{namespace sun{namespace star{
namespace linguistic2{
    class XDictionary;
    class XSpellChecker1;
}}}}

// forward ---------------------------------------------------------------


// class SvxNewDictionaryDialog ------------------------------------------

class SvxNewDictionaryDialog : public ModalDialog
{
private:
    Edit*                pNameEdit;
    SvxLanguageBox*      pLanguageLB;
    CheckBox*            pExceptBtn;
    OKButton*            pOKBtn;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellChecker1 >     xSpell;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XDictionary >    xNewDic;

    DECL_LINK(OKHdl_Impl, void *);
    DECL_LINK(ModifyHdl_Impl, void *);

public:
    SvxNewDictionaryDialog( vcl::Window* pParent,
            ::com::sun::star::uno::Reference<
                ::com::sun::star::linguistic2::XSpellChecker1 >  &xSpl );

    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XDictionary >
                GetNewDictionary() { return xNewDic; }
};

// class SvxDictEdit ----------------------------------------------------

class SvxDictEdit : public Edit
{
    Link    aActionLink;
    bool    bSpaces;

    public:
                    SvxDictEdit(vcl::Window* pParent, const ResId& rResId) :
                        Edit(pParent, rResId), bSpaces(false){}
                    SvxDictEdit(vcl::Window* pParent, WinBits aWB) :
                        Edit(pParent, aWB), bSpaces(false){}

    void            SetActionHdl( const Link& rLink )
                                { aActionLink = rLink;}

    void            SetSpaces(bool bSet)
                                {bSpaces = bSet;}

    virtual void    KeyInput( const KeyEvent& rKEvent ) SAL_OVERRIDE;
};

// class SvxEditDictionaryDialog -----------------------------------------

class SvxEditDictionaryDialog : public ModalDialog
{
private:

    ListBox*                 pAllDictsLB;
    FixedText*               pLangFT;
    SvxLanguageBox*          pLangLB;

    SvxDictEdit*             pWordED;
    FixedText*               pReplaceFT;
    SvxDictEdit*             pReplaceED;
    SvTabListBox*            pWordsLB;
    PushButton*              pNewReplacePB;
    PushButton*              pDeletePB;

    OUString                sModify;
    OUString                sNew;
    DecorationView          aDecoView;

    ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XDictionary >  > aDics;  //! snapshot copy to work on
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellChecker1 >     xSpell;

    short               nOld;
    long                nWidth;
    bool            bFirstSelect;
    bool            bDoNothing;
    bool                bDicIsReadonly;

    DECL_LINK(SelectBookHdl_Impl, void *);
    DECL_LINK(SelectLangHdl_Impl, void *);
    DECL_LINK(SelectHdl, SvTabListBox*);
    DECL_LINK(NewDelHdl, PushButton*);
    DECL_LINK(ModifyHdl, Edit*);


    void            ShowWords_Impl( sal_uInt16 nId );
    void            SetLanguage_Impl( ::com::sun::star::util::Language nLanguage );
    bool            IsDicReadonly_Impl() const { return bDicIsReadonly; }
    void            SetDicReadonly_Impl( ::com::sun::star::uno::Reference<
                            ::com::sun::star::linguistic2::XDictionary >  &xDic );

    void            RemoveDictEntry(SvTreeListEntry* pEntry);
    sal_uLong       GetLBInsertPos(const OUString &rDicWord);

protected:

//    virtual void    Paint( const Rectangle& rRect );

public:
    SvxEditDictionaryDialog( vcl::Window* pParent,
            const OUString& rName,
            ::com::sun::star::uno::Reference<
                ::com::sun::star::linguistic2::XSpellChecker1> &xSpl );
    virtual ~SvxEditDictionaryDialog();

    sal_uInt16 GetSelectedDict() {return pAllDictsLB->GetSelectEntryPos();}
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
