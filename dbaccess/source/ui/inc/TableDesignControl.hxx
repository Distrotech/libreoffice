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
#ifndef INCLUDED_DBACCESS_SOURCE_UI_INC_TABLEDESIGNCONTROL_HXX
#define INCLUDED_DBACCESS_SOURCE_UI_INC_TABLEDESIGNCONTROL_HXX

#include <svtools/tabbar.hxx>
#include <svtools/editbrowsebox.hxx>

#include "IClipBoardTest.hxx"
#include "TypeInfo.hxx"

namespace dbaui
{
    class OTableDesignView;

    class OTableRowView : public ::svt::EditBrowseBox, public IClipboardTest
    {
        friend class OTableDesignUndoAct;

    protected:
        long    m_nDataPos;    ///< currently needed row
        long    m_nCurrentPos; ///< current position of selected column

    private:
        sal_uInt16  m_nCurUndoActId;

    protected:
        bool    m_bCurrentModified;
        bool    m_bUpdatable;
        bool    m_bClipboardFilled;

    public:
        OTableRowView(vcl::Window* pParent);
        virtual ~OTableRowView();

        virtual void SetCellData( long nRow, sal_uInt16 nColId, const TOTypeInfoSP& _pTypeInfo ) = 0;
        virtual void SetCellData( long nRow, sal_uInt16 nColId, const ::com::sun::star::uno::Any& _rNewData ) = 0;
        virtual ::com::sun::star::uno::Any          GetCellData( long nRow, sal_uInt16 nColId ) = 0;
        virtual void SetControlText( long nRow, sal_uInt16 nColId, const OUString& rText ) = 0;
        virtual OUString GetControlText( long nRow, sal_uInt16 nColId ) = 0;

        virtual OTableDesignView* GetView() const = 0;

        sal_uInt16 GetCurUndoActId(){ return m_nCurUndoActId; }

        // IClipboardTest
        virtual void cut() SAL_OVERRIDE;
        virtual void copy() SAL_OVERRIDE;
        virtual void paste() SAL_OVERRIDE;

    protected:
        void Paste( long nRow );

        virtual void CopyRows()                             = 0;
        virtual void DeleteRows()                           = 0;
        virtual void InsertRows( long nRow )                = 0;
        virtual void InsertNewRows( long nRow )             = 0;

        virtual bool IsPrimaryKeyAllowed( long nRow )   = 0;
        virtual bool IsInsertNewAllowed( long nRow )    = 0;
        virtual bool IsDeleteAllowed( long nRow )       = 0;

        virtual bool IsUpdatable() const {return m_bUpdatable;}
        virtual void SetUpdatable( bool bUpdate=true );

        virtual RowStatus GetRowStatus(long nRow) const SAL_OVERRIDE;
        virtual void KeyInput(const KeyEvent& rEvt) SAL_OVERRIDE;
        virtual void Command( const CommandEvent& rEvt ) SAL_OVERRIDE;

        virtual void Init() SAL_OVERRIDE;
    };
}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
