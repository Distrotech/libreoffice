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
#ifndef INCLUDED_VCL_SOURCE_EDIT_TEXTUND2_HXX
#define INCLUDED_VCL_SOURCE_EDIT_TEXTUND2_HXX

#include "textundo.hxx"
#include <vcl/textdata.hxx>

class TextUndoDelPara : public TextUndo
{
private:
    bool            mbDelObject;
    sal_uLong       mnPara;
    TextNode*       mpNode; // points at the valid not-destroyed object

public:
                    TYPEINFO_OVERRIDE();
                    TextUndoDelPara( TextEngine* pTextEngine, TextNode* pNode, sal_uLong nPara );
                    virtual ~TextUndoDelPara();

    virtual void    Undo() SAL_OVERRIDE;
    virtual void    Redo() SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

class TextUndoConnectParas : public TextUndo
{
private:
    sal_uLong           mnPara;
    sal_uInt16          mnSepPos;

public:
                    TYPEINFO_OVERRIDE();
                    TextUndoConnectParas( TextEngine* pTextEngine, sal_uLong nPara, sal_uInt16 nSepPos );
                    virtual ~TextUndoConnectParas();

    virtual void    Undo() SAL_OVERRIDE;
    virtual void    Redo() SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

class TextUndoSplitPara : public TextUndo
{
private:
    sal_uLong           mnPara;
    sal_uInt16          mnSepPos;

public:
                    TYPEINFO_OVERRIDE();
                    TextUndoSplitPara( TextEngine* pTextEngine, sal_uLong nPara, sal_uInt16 nSepPos );
                    virtual ~TextUndoSplitPara();

    virtual void    Undo() SAL_OVERRIDE;
    virtual void    Redo() SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

class TextUndoInsertChars : public TextUndo
{
private:
    TextPaM         maTextPaM;
    OUString        maText;

public:
                    TYPEINFO_OVERRIDE();
                    TextUndoInsertChars( TextEngine* pTextEngine, const TextPaM& rTextPaM, const OUString& rStr );

    virtual void    Undo() SAL_OVERRIDE;
    virtual void    Redo() SAL_OVERRIDE;

    virtual bool    Merge( SfxUndoAction *pNextAction ) SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

class TextUndoRemoveChars : public TextUndo
{
private:
    TextPaM         maTextPaM;
    OUString        maText;

public:
                    TYPEINFO_OVERRIDE();
                    TextUndoRemoveChars( TextEngine* pTextEngine, const TextPaM& rTextPaM, const OUString& rStr );

    virtual void    Undo() SAL_OVERRIDE;
    virtual void    Redo() SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

class TextUndoSetAttribs: public TextUndo
{
private:
    TextSelection       maSelection;

public:
                        TYPEINFO_OVERRIDE();
                        TextUndoSetAttribs( TextEngine* pTextEngine, const TextSelection& rESel );
                        virtual ~TextUndoSetAttribs();

    virtual void        Undo() SAL_OVERRIDE;
    virtual void        Redo() SAL_OVERRIDE;

    virtual OUString GetComment () const SAL_OVERRIDE;
};

#endif // INCLUDED_VCL_SOURCE_EDIT_TEXTUND2_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
