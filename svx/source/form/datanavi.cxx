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


#include <sal/macros.h>
#include "datanavi.hxx"
#include "fmservs.hxx"

#include "datanavi.hrc"
#include "fmhelp.hrc"
#include <svx/svxids.hrc>
#include <tools/rcid.h>
#include <tools/diagnose_ex.h>
#include <svx/xmlexchg.hxx>
#include <svx/fmshell.hxx>
#include <svtools/miscopt.hxx>
#include <unotools/pathoptions.hxx>
#include <unotools/viewoptions.hxx>
#include <svtools/svtools.hrc>
#include "svtools/treelistentry.hxx"
#include <sfx2/app.hxx>
#include <sfx2/filedlghelper.hxx>
#include <sfx2/objitem.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include "com/sun/star/ui/dialogs/TemplateDescription.hpp"
#include <com/sun/star/xforms/XFormsSupplier.hpp>
#include <com/sun/star/xml/dom/XDocument.hpp>
#include <com/sun/star/xml/dom/DOMException.hpp>
#include <com/sun/star/form/binding/XValueBinding.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/string.hxx>

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::datatransfer;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::dom::events;
using namespace ::svx;

#define CFGNAME_DATANAVIGATOR       "DataNavigator"
#define CFGNAME_SHOWDETAILS         "ShowDetails"
#define MSG_VARIABLE                "%1"
#define MODELNAME                   "$MODELNAME"
#define INSTANCENAME                "$INSTANCENAME"
#define ELEMENTNAME                 "$ELEMENTNAME"
#define ATTRIBUTENAME               "$ATTRIBUTENAME"
#define SUBMISSIONNAME              "$SUBMISSIONNAME"
#define BINDINGNAME                 "$BINDINGNAME"


namespace svxform
{


    // properties of instance
    #define PN_INSTANCE_MODEL       "Instance"
    #define PN_INSTANCE_ID          "ID"
    #define PN_INSTANCE_URL         "URL"

    // properties of binding
    #define PN_BINDING_ID           "BindingID"
    #define PN_BINDING_EXPR         "BindingExpression"
    #define PN_BINDING_MODEL        "Model"
    #define PN_BINDING_NAMESPACES   "ModelNamespaces"
    #define PN_READONLY_EXPR        "ReadonlyExpression"
    #define PN_RELEVANT_EXPR        "RelevantExpression"
    #define PN_REQUIRED_EXPR        "RequiredExpression"
    #define PN_CONSTRAINT_EXPR      "ConstraintExpression"
    #define PN_CALCULATE_EXPR       "CalculateExpression"
    #define PN_BINDING_TYPE         "Type"

    // properties of submission
    #define PN_SUBMISSION_ID        "ID"
    #define PN_SUBMISSION_BIND      "Bind"
    #define PN_SUBMISSION_REF       "Ref"
    #define PN_SUBMISSION_ACTION    "Action"
    #define PN_SUBMISSION_METHOD    "Method"
    #define PN_SUBMISSION_REPLACE   "Replace"

    // other const strings
    #define TRUE_VALUE              "true()"
    #define NEW_ELEMENT             "newElement"
    #define NEW_ATTRIBUTE           "newAttribute"
    #define EVENTTYPE_CHARDATA      "DOMCharacterDataModified"
    #define EVENTTYPE_ATTR          "DOMAttrModified"

    #define MIN_PAGE_COUNT          3 // at least one instance, one submission and one binding page

    struct ItemNode
    {
        Reference< css::xml::dom::XNode >   m_xNode;
        Reference< XPropertySet >           m_xPropSet;

        ItemNode( const Reference< css::xml::dom::XNode >& _rxNode ) :
            m_xNode( _rxNode ) {}
        ItemNode( const Reference< XPropertySet >& _rxSet ) :
            m_xPropSet( _rxSet ) {}
    };


    // class DataTreeListBox

    DataTreeListBox::DataTreeListBox( XFormsPage* pPage, DataGroupType _eGroup, const ResId& rResId ) :

        SvTreeListBox( pPage, rResId ),

        m_pXFormsPage   ( pPage ),
        m_eGroup        ( _eGroup )

    {
        EnableContextMenuHandling();

        if ( DGTInstance == m_eGroup )
            SetDragDropMode( SV_DRAGDROP_CTRL_MOVE |SV_DRAGDROP_CTRL_COPY | SV_DRAGDROP_APP_COPY );
    }

    DataTreeListBox::~DataTreeListBox()
    {
        DeleteAndClear();
    }

    sal_Int8 DataTreeListBox::AcceptDrop( const AcceptDropEvent& /*rEvt*/ )
    {
        return DND_ACTION_NONE;
    }
    sal_Int8 DataTreeListBox::ExecuteDrop( const ExecuteDropEvent& /*rEvt*/ )
    {
        return DND_ACTION_NONE;
    }
    void DataTreeListBox::StartDrag( sal_Int8 /*_nAction*/, const Point& /*_rPosPixel*/ )
    {
        SvTreeListEntry* pSelected = FirstSelected();
        if ( !pSelected )
            // no drag without an entry
            return;

        if ( m_eGroup == DGTBinding )
            // for the moment, bindings cannot be dragged.
            // #i59395# / 2005-12-15 / frank.schoenheit@sun.com
            return;

        // GetServiceNameForNode() requires a datatype repository which
        // will be automatically build if requested???
        Reference< css::xforms::XModel > xModel( m_pXFormsPage->GetXFormsHelper(), UNO_QUERY );
        Reference< css::xforms::XDataTypeRepository > xDataTypes =
            xModel->getDataTypeRepository();
        if(!xDataTypes.is())
            return;

        using namespace ::com::sun::star::uno;

        ItemNode *pItemNode = static_cast<ItemNode*>(pSelected->GetUserData());

        if ( !pItemNode )
        {
            // the only known (and allowed?) case where this happens are sub-entries of a submission
            // entry
            DBG_ASSERT( DGTSubmission == m_eGroup, "DataTreeListBox::StartDrag: how this?" );
            pSelected = GetParent( pSelected );
            DBG_ASSERT( pSelected && !GetParent( pSelected ), "DataTreeListBox::StartDrag: what kind of entry *is* this?" );
                // on the submission page, we have only top-level entries (the submission themself)
                // plus direct children of those (facets of a submission)
            pItemNode = pSelected ? static_cast< ItemNode* >( pSelected->GetUserData() ) : NULL;
            if ( !pItemNode )
                return;
        }

        OXFormsDescriptor desc;
        desc.szName = GetEntryText(pSelected);
        if(pItemNode->m_xNode.is()) {
            // a valid node interface tells us that we need to create a control from a binding
            desc.szServiceName = m_pXFormsPage->GetServiceNameForNode(pItemNode->m_xNode);
            desc.xPropSet = m_pXFormsPage->GetBindingForNode(pItemNode->m_xNode);
            DBG_ASSERT( desc.xPropSet.is(), "DataTreeListBox::StartDrag(): invalid node binding" );
        }
        else {
            desc.szServiceName = FM_COMPONENT_COMMANDBUTTON;
            desc.xPropSet = pItemNode->m_xPropSet;
        }
        OXFormsTransferable *pTransferable = new OXFormsTransferable(desc);
        Reference< XTransferable > xEnsureDelete = pTransferable;
        if(pTransferable) {
            EndSelection();
            pTransferable->StartDrag( this, DND_ACTION_COPY );
        }
    }

    PopupMenu* DataTreeListBox::CreateContextMenu()
    {
        PopupMenu* pMenu = new PopupMenu( SVX_RES( RID_MENU_DATANAVIGATOR ) );
        if ( DGTInstance == m_eGroup )
            pMenu->RemoveItem( pMenu->GetItemPos( TBI_ITEM_ADD ) );
        else
        {
            pMenu->RemoveItem( pMenu->GetItemPos( TBI_ITEM_ADD_ELEMENT ) );
            pMenu->RemoveItem( pMenu->GetItemPos( TBI_ITEM_ADD_ATTRIBUTE ) );

            if ( DGTSubmission == m_eGroup )
            {
                pMenu->SetItemText( TBI_ITEM_ADD, SVX_RESSTR( RID_STR_DATANAV_ADD_SUBMISSION ) );
                pMenu->SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( RID_STR_DATANAV_EDIT_SUBMISSION ) );
                pMenu->SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( RID_STR_DATANAV_REMOVE_SUBMISSION ) );
            }
            else
            {
                pMenu->SetItemText( TBI_ITEM_ADD, SVX_RESSTR( RID_STR_DATANAV_ADD_BINDING ) );
                pMenu->SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( RID_STR_DATANAV_EDIT_BINDING ) );
                pMenu->SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( RID_STR_DATANAV_REMOVE_BINDING ) );
            }
        }
        m_pXFormsPage->EnableMenuItems( pMenu );
        return pMenu;
    }

    void DataTreeListBox::ExcecuteContextMenuAction( sal_uInt16 _nSelectedPopupEntry )
    {
        m_pXFormsPage->DoMenuAction( _nSelectedPopupEntry );
    }

    void DataTreeListBox::RemoveEntry( SvTreeListEntry* _pEntry )
    {
        if ( _pEntry )
        {
            delete static_cast< ItemNode* >( _pEntry->GetUserData() );
            SvTreeListBox::GetModel()->Remove( _pEntry );
        }
    }

    void DataTreeListBox::DeleteAndClear()
    {
        sal_uIntPtr i, nCount = GetEntryCount();
        for ( i = 0; i < nCount; ++i )
        {
            SvTreeListEntry* pEntry = GetEntry(i);
            if ( pEntry )
                delete static_cast< ItemNode* >( pEntry->GetUserData() );
        }

        Clear();
    }


    // class XFormsPage

    XFormsPage::XFormsPage( Window* pParent, DataNavigatorWindow* _pNaviWin, DataGroupType _eGroup ) :

        TabPage( pParent, SVX_RES( RID_SVX_XFORMS_TABPAGES ) ),

        m_aToolBox      ( this, SVX_RES( TB_ITEMS ) ),
        m_aItemList     ( this, _eGroup, SVX_RES( LB_ITEMS ) ),
        m_pNaviWin      ( _pNaviWin ),
        m_bHasModel     ( false ),
        m_eGroup        ( _eGroup ),
        m_TbxImageList  ( SVX_RES( IL_TBX_BMPS ) ),
        m_bLinkOnce     ( false )

    {
        FreeResource();

        const ImageList& rImageList = m_TbxImageList;
        m_aToolBox.SetItemImage( TBI_ITEM_ADD, rImageList.GetImage( IID_ITEM_ADD ) );
        m_aToolBox.SetItemImage( TBI_ITEM_ADD_ELEMENT, rImageList.GetImage( IID_ITEM_ADD_ELEMENT ) );
        m_aToolBox.SetItemImage( TBI_ITEM_ADD_ATTRIBUTE, rImageList.GetImage( IID_ITEM_ADD_ATTRIBUTE ) );
        m_aToolBox.SetItemImage( TBI_ITEM_EDIT, rImageList.GetImage( IID_ITEM_EDIT ) );
        m_aToolBox.SetItemImage( TBI_ITEM_REMOVE, rImageList.GetImage( IID_ITEM_REMOVE ) );

        if ( DGTInstance == m_eGroup )
            m_aToolBox.RemoveItem( m_aToolBox.GetItemPos( TBI_ITEM_ADD ) );
        else
        {
            m_aToolBox.RemoveItem( m_aToolBox.GetItemPos( TBI_ITEM_ADD_ELEMENT ) );
            m_aToolBox.RemoveItem( m_aToolBox.GetItemPos( TBI_ITEM_ADD_ATTRIBUTE ) );

            if ( DGTSubmission == m_eGroup )
            {
                m_aToolBox.SetItemText( TBI_ITEM_ADD, SVX_RESSTR( RID_STR_DATANAV_ADD_SUBMISSION ) );
                m_aToolBox.SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( RID_STR_DATANAV_EDIT_SUBMISSION ) );
                m_aToolBox.SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( RID_STR_DATANAV_REMOVE_SUBMISSION ) );
            }
            else
            {
                m_aToolBox.SetItemText( TBI_ITEM_ADD, SVX_RESSTR( RID_STR_DATANAV_ADD_BINDING ) );
                m_aToolBox.SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( RID_STR_DATANAV_EDIT_BINDING ) );
                m_aToolBox.SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( RID_STR_DATANAV_REMOVE_BINDING ) );
            }
        }

        const Size aTbxSz( m_aToolBox.CalcWindowSizePixel() );
        m_aToolBox.SetSizePixel( aTbxSz );
        m_aToolBox.SetOutStyle( SvtMiscOptions().GetToolboxStyle() );
        m_aToolBox.SetSelectHdl( LINK( this, XFormsPage, TbxSelectHdl ) );
        Point aPos = m_aItemList.GetPosPixel();
        aPos.Y() = aTbxSz.Height();
        m_aItemList.SetPosPixel( aPos );

        m_aItemList.SetSelectHdl( LINK( this, XFormsPage, ItemSelectHdl ) );
        m_aItemList.SetNodeDefaultImages();
        WinBits nBits = WB_BORDER | WB_TABSTOP | WB_HIDESELECTION | WB_NOINITIALSELECTION;
        if ( DGTInstance == m_eGroup || DGTSubmission == m_eGroup )
            nBits |= WB_HASBUTTONS | WB_HASLINES | WB_HASLINESATROOT | WB_HASBUTTONSATROOT;
        m_aItemList.SetStyle( m_aItemList.GetStyle() | nBits  );
        m_aItemList.Show();
        ItemSelectHdl( &m_aItemList );
    }

    XFormsPage::~XFormsPage()
    {
    }

    IMPL_LINK_NOARG(XFormsPage, TbxSelectHdl)
    {
        DoToolBoxAction( m_aToolBox.GetCurItemId() );
        return 0;
    }

    IMPL_LINK_NOARG(XFormsPage, ItemSelectHdl)
    {
        EnableMenuItems( NULL );
        return 0;
    }

    void XFormsPage::AddChildren(
        SvTreeListEntry* _pParent, const ImageList& _rImgLst,
        const Reference< css::xml::dom::XNode >& _xNode )
    {
        DBG_ASSERT( m_xUIHelper.is(), "XFormsPage::AddChildren(): invalid UIHelper" );

        try
        {
            Reference< css::xml::dom::XNodeList > xNodeList = _xNode->getChildNodes();
            if ( xNodeList.is() )
            {
                bool bShowDetails = m_pNaviWin->IsShowDetails();
                sal_Int32 i, nNodeCount = xNodeList->getLength();
                for ( i = 0; i < nNodeCount; ++i )
                {
                    Reference< css::xml::dom::XNode > xChild = xNodeList->item(i);
                    css::xml::dom::NodeType eChildType = xChild->getNodeType();
                    Image aExpImg, aCollImg;
                    switch ( eChildType )
                    {
                        case css::xml::dom::NodeType_ATTRIBUTE_NODE:
                            aExpImg = aCollImg = _rImgLst.GetImage( IID_ATTRIBUTE );
                            break;
                        case css::xml::dom::NodeType_ELEMENT_NODE:
                            aExpImg = aCollImg = _rImgLst.GetImage( IID_ELEMENT );
                            break;
                        case css::xml::dom::NodeType_TEXT_NODE:
                            aExpImg = aCollImg = _rImgLst.GetImage( IID_TEXT );
                            break;
                        default:
                            aExpImg = aCollImg = _rImgLst.GetImage( IID_OTHER );
                    }

                    OUString sName = m_xUIHelper->getNodeDisplayName( xChild, bShowDetails );
                    if ( !sName.isEmpty() )
                    {
                        ItemNode* pNode = new ItemNode( xChild );
                        SvTreeListEntry* pEntry = m_aItemList.InsertEntry(
                            sName, aExpImg, aCollImg, _pParent, false, TREELIST_APPEND, pNode );
                        if ( xChild->hasAttributes() )
                        {
                            Reference< css::xml::dom::XNamedNodeMap > xMap = xChild->getAttributes();
                            if ( xMap.is() )
                            {
                                aExpImg = aCollImg = _rImgLst.GetImage( IID_ATTRIBUTE );
                                sal_Int32 j, nMapLen = xMap->getLength();
                                for ( j = 0; j < nMapLen; ++j )
                                {
                                    Reference< css::xml::dom::XNode > xAttr = xMap->item(j);
                                    pNode = new ItemNode( xAttr );
                                    OUString sAttrName =
                                        m_xUIHelper->getNodeDisplayName( xAttr, bShowDetails );
                                    m_aItemList.InsertEntry(
                                        sAttrName, aExpImg, aCollImg,
                                        pEntry, false, TREELIST_APPEND, pNode );
                                }
                            }
                        }
                        if ( xChild->hasChildNodes() )
                            AddChildren( pEntry, _rImgLst, xChild );
                    }
                }
            }
        }
        catch( Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    bool XFormsPage::DoToolBoxAction( sal_uInt16 _nToolBoxID ) {

        bool bHandled = false;
        bool bIsDocModified = false;
        m_pNaviWin->DisableNotify( true );

        switch ( _nToolBoxID )
        {
        case TBI_ITEM_ADD:
        case TBI_ITEM_ADD_ELEMENT:
        case TBI_ITEM_ADD_ATTRIBUTE:
        {
            bHandled = true;
            Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
            DBG_ASSERT( xModel.is(), "XFormsPage::DoToolBoxAction(): Action without model" );
            if ( DGTSubmission == m_eGroup )
            {
                AddSubmissionDialog aDlg( this, NULL, m_xUIHelper );
                if ( aDlg.Execute() == RET_OK && aDlg.GetNewSubmission().is() )
                {
                    try
                    {
                        Reference< css::xforms::XSubmission > xNewSubmission = aDlg.GetNewSubmission();
                        Reference< XSet > xSubmissions( xModel->getSubmissions(), UNO_QUERY );
                        xSubmissions->insert( makeAny( xNewSubmission ) );
                        Reference< XPropertySet > xNewPropSet( xNewSubmission, UNO_QUERY );
                        SvTreeListEntry* pEntry = AddEntry( xNewPropSet );
                        m_aItemList.Select( pEntry, true );
                        bIsDocModified = true;
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while adding submission" );
                    }
                }
            }
            else
            {
                DataItemType eType = DITElement;
                SvTreeListEntry* pEntry = m_aItemList.FirstSelected();
                ItemNode* pNode = NULL;
                Reference< css::xml::dom::XNode > xParentNode;
                Reference< XPropertySet > xNewBinding;
                sal_uInt16 nResId = 0;
                bool bIsElement = true;
                if ( DGTInstance == m_eGroup )
                {
                    if ( !m_sInstanceURL.isEmpty() )
                    {
                        LinkedInstanceWarningBox aMsgBox( this );
                        if ( aMsgBox.Execute() != RET_OK )
                            return bHandled;
                    }

                    DBG_ASSERT( pEntry, "XFormsPage::DoToolBoxAction(): no entry" );
                    ItemNode* pParentNode = static_cast< ItemNode* >( pEntry->GetUserData() );
                    DBG_ASSERT( pParentNode, "XFormsPage::DoToolBoxAction(): no parent node" );
                    xParentNode = pParentNode->m_xNode;
                    Reference< css::xml::dom::XNode > xNewNode;
                    if ( TBI_ITEM_ADD_ELEMENT == _nToolBoxID )
                    {
                        try
                        {
                            nResId = RID_STR_DATANAV_ADD_ELEMENT;
                            xNewNode = m_xUIHelper->createElement( xParentNode, NEW_ELEMENT );
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while create element" );
                        }
                    }
                    else
                    {
                        nResId = RID_STR_DATANAV_ADD_ATTRIBUTE;
                        bIsElement = false;
                        eType = DITAttribute;
                        try
                        {
                            xNewNode = m_xUIHelper->createAttribute( xParentNode, NEW_ATTRIBUTE );
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while create attribute" );
                        }
                    }

                    try
                    {
                        xNewNode = xParentNode->appendChild( xNewNode );
                    }
                    catch ( css::xml::dom::DOMException& e )
                    {
                        if ( e.Code == css::xml::dom::DOMExceptionType_DOMSTRING_SIZE_ERR )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): domexception: size error" );
                        }
                        SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): domexception while append child" );
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while append child" );
                    }

                    try
                    {
                        Reference< css::xml::dom::XNode > xPNode;
                        if ( xNewNode.is() )
                             xPNode = xNewNode->getParentNode();
                        // attributes don't have parents in the DOM model
                        DBG_ASSERT( TBI_ITEM_ADD_ATTRIBUTE == _nToolBoxID
                                    || xPNode.is(), "XFormsPage::DoToolboxAction(): node not added" );
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                    }

                    try
                    {
                        m_xUIHelper->getBindingForNode( xNewNode, sal_True );
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while get binding for node" );
                    }
                    pNode = new ItemNode( xNewNode );
                }
                else
                {
                    try
                    {
                        nResId = RID_STR_DATANAV_ADD_BINDING;
                        xNewBinding = xModel->createBinding();
                        Reference< XSet > xBindings( xModel->getBindings(), UNO_QUERY );
                        xBindings->insert( makeAny( xNewBinding ) );
                        pNode = new ItemNode( xNewBinding );
                        eType = DITBinding;
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::DoToolBoxAction(): exception while adding binding" );
                    }
                }

                AddDataItemDialog aDlg( this, pNode, m_xUIHelper );
                aDlg.SetText( SVX_RESSTR( nResId ) );
                aDlg.InitText( eType );
                short nReturn = aDlg.Execute();
                if (  DGTInstance == m_eGroup )
                {
                    if ( RET_OK == nReturn )
                    {
                        SvTreeListEntry* pNewEntry = AddEntry( pNode, bIsElement );
                        m_aItemList.MakeVisible( pNewEntry );
                        m_aItemList.Select( pNewEntry, true );
                        bIsDocModified = true;
                    }
                    else
                    {
                        try
                        {
                            Reference< css::xml::dom::XNode > xPNode;
                            Reference< css::xml::dom::XNode > xNode =
                                xParentNode->removeChild( pNode->m_xNode );
                            if ( xNode.is() )
                                xPNode = xNode->getParentNode();
                            DBG_ASSERT( !xPNode.is(), "XFormsPage::RemoveEntry(): node not removed" );
                            delete pNode;
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                        }
                    }
                }
                else
                {
                    if ( RET_OK == nReturn )
                    {
                        SvTreeListEntry* pNewEntry = AddEntry( xNewBinding );
                        m_aItemList.Select( pNewEntry, true );
                        bIsDocModified = true;
                    }
                    else
                    {
                        try
                        {
                            Reference< XSet > xBindings( xModel->getBindings(), UNO_QUERY );
                            xBindings->remove( makeAny( xNewBinding ) );
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                        }
                    }
                    delete pNode;
                }
            }
        }
        break;

        case TBI_ITEM_EDIT:
        {
            bHandled = true;
            SvTreeListEntry* pEntry = m_aItemList.FirstSelected();
            if ( pEntry )
            {
                if ( DGTSubmission == m_eGroup && m_aItemList.GetParent( pEntry ) )
                    pEntry = m_aItemList.GetParent( pEntry );
                ItemNode* pNode = static_cast< ItemNode* >( pEntry->GetUserData() );
                if ( DGTInstance == m_eGroup || DGTBinding == m_eGroup )
                {
                    if ( DGTInstance == m_eGroup && !m_sInstanceURL.isEmpty() )
                    {
                        LinkedInstanceWarningBox aMsgBox( this );
                        if ( aMsgBox.Execute() != RET_OK )
                            return bHandled;
                    }

                    AddDataItemDialog aDlg( this, pNode, m_xUIHelper );
                    DataItemType eType = DITElement;
                    sal_uInt16 nResId = RID_STR_DATANAV_EDIT_ELEMENT;
                    if ( pNode && pNode->m_xNode.is() )
                    {
                        try
                        {
                            css::xml::dom::NodeType eChildType = pNode->m_xNode->getNodeType();
                            if ( eChildType == css::xml::dom::NodeType_ATTRIBUTE_NODE )
                            {
                                nResId = RID_STR_DATANAV_EDIT_ATTRIBUTE;
                                eType = DITAttribute;
                            }
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                        }
                    }
                    else if ( DGTBinding == m_eGroup )
                    {
                        nResId = RID_STR_DATANAV_EDIT_BINDING;
                        eType = DITBinding;
                    }
                    aDlg.SetText( SVX_RESSTR( nResId ) );
                    aDlg.InitText( eType );
                    if ( aDlg.Execute() == RET_OK )
                    {
                        // Set the new name
                        OUString sNewName;
                        if ( DGTInstance == m_eGroup )
                        {
                            try
                            {
                                sNewName = m_xUIHelper->getNodeDisplayName(
                                    pNode->m_xNode, m_pNaviWin->IsShowDetails() );
                            }
                            catch ( Exception& )
                            {
                                SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                            }
                        }
                        else
                        {
                            try
                            {
                                OUString sDelim( ": " );
                                OUString sTemp;
                                pNode->m_xPropSet->getPropertyValue( PN_BINDING_ID ) >>= sTemp;
                                sNewName += sTemp;
                                sNewName += sDelim;
                                pNode->m_xPropSet->getPropertyValue( PN_BINDING_EXPR ) >>= sTemp;
                                sNewName += sTemp;
                            }
                            catch ( Exception& )
                            {
                                SAL_WARN( "svx.form", "XFormsPage::DoToolboxAction(): exception caught" );
                            }
                        }

                        m_aItemList.SetEntryText( pEntry, sNewName );
                        bIsDocModified = true;
                    }
                }
                else
                {
                    AddSubmissionDialog aDlg( this, pNode, m_xUIHelper );
                    aDlg.SetText( SVX_RESSTR( RID_STR_DATANAV_EDIT_SUBMISSION ) );
                    if ( aDlg.Execute() == RET_OK )
                    {
                        EditEntry( pNode->m_xPropSet );
                        bIsDocModified = true;
                    }
                }
            }
        }
        break;

        case TBI_ITEM_REMOVE:
        {
            bHandled = true;
            if ( DGTInstance == m_eGroup && !m_sInstanceURL.isEmpty() )
            {
                LinkedInstanceWarningBox aMsgBox( this );
                if ( aMsgBox.Execute() != RET_OK )
                    return bHandled;
            }
            bIsDocModified = RemoveEntry();
        }
        break;

        case MID_INSERT_CONTROL:
        {
            OSL_FAIL( "XFormsPage::DoToolboxAction: MID_INSERT_CONTROL not implemented, yet!" );
        }
        break;

        default:
            OSL_FAIL( "XFormsPage::DoToolboxAction: unknown ID!" );
            break;
        }

        m_pNaviWin->DisableNotify( false );
        EnableMenuItems( NULL );
        if ( bIsDocModified )
            m_pNaviWin->SetDocModified();
        return bHandled;
    }


    SvTreeListEntry* XFormsPage::AddEntry( ItemNode* _pNewNode, bool _bIsElement )
    {
        SvTreeListEntry* pParent = m_aItemList.FirstSelected();
        const ImageList& rImageList = m_pNaviWin->GetItemImageList();
        sal_uInt16 nImageID = ( _bIsElement ) ? IID_ELEMENT : IID_ATTRIBUTE;
        Image aImage = rImageList.GetImage( nImageID );
        OUString sName;
        try
        {
            sName = m_xUIHelper->getNodeDisplayName(
                _pNewNode->m_xNode, m_pNaviWin->IsShowDetails() );
        }
        catch ( Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return m_aItemList.InsertEntry(
            sName, aImage, aImage, pParent, false, TREELIST_APPEND, _pNewNode );
    }


    SvTreeListEntry* XFormsPage::AddEntry( const Reference< XPropertySet >& _rEntry )
    {
        SvTreeListEntry* pEntry = NULL;
        const ImageList& rImageList = m_pNaviWin->GetItemImageList();
        Image aImage = rImageList.GetImage( IID_ELEMENT );

        ItemNode* pNode = new ItemNode( _rEntry );
        OUString sTemp;

        if ( DGTSubmission == m_eGroup )
        {
            try
            {
                // ID
                _rEntry->getPropertyValue( PN_SUBMISSION_ID ) >>= sTemp;
                pEntry = m_aItemList.InsertEntry( sTemp, aImage, aImage, NULL, false, TREELIST_APPEND, pNode );
                // Action
                _rEntry->getPropertyValue( PN_SUBMISSION_ACTION ) >>= sTemp;
                OUString sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_ACTION );
                sEntry += sTemp;
                m_aItemList.InsertEntry( sEntry, aImage, aImage, pEntry );
                // Method
                _rEntry->getPropertyValue( PN_SUBMISSION_METHOD ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_METHOD );
                sEntry +=  m_aMethodString.toUI( sTemp );
                m_aItemList.InsertEntry( sEntry, aImage, aImage, pEntry );
                // Ref
                _rEntry->getPropertyValue( PN_SUBMISSION_REF ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_REF );
                sEntry += sTemp;
                m_aItemList.InsertEntry( sEntry, aImage, aImage, pEntry );
                // Bind
                _rEntry->getPropertyValue( PN_SUBMISSION_BIND ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_BIND );
                sEntry += sTemp;
                m_aItemList.InsertEntry( sEntry, aImage, aImage, pEntry );
                // Replace
                _rEntry->getPropertyValue( PN_SUBMISSION_REPLACE ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_REPLACE );
                sEntry += m_aReplaceString.toUI( sTemp );
                m_aItemList.InsertEntry( sEntry, aImage, aImage, pEntry );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "XFormsPage::AddEntry(Ref): exception caught" );
            }
        }
        else // then Binding Page
        {
            try
            {
                OUString sDelim( ": " );
                OUString sName;
                _rEntry->getPropertyValue( PN_BINDING_ID ) >>= sTemp;
                sName += sTemp;
                sName += sDelim;
                _rEntry->getPropertyValue( PN_BINDING_EXPR ) >>= sTemp;
                sName += sTemp;
                pEntry = m_aItemList.InsertEntry(
                    sName, aImage, aImage, NULL, false, TREELIST_APPEND, pNode );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "XFormsPage::AddEntry(Ref): exception caught" );
            }
        }

        return pEntry;
    }


    void XFormsPage::EditEntry( const Reference< XPropertySet >& _rEntry )
    {
        SvTreeListEntry* pEntry = NULL;
        OUString sTemp;

        if ( DGTSubmission == m_eGroup )
        {
            try
            {
                pEntry = m_aItemList.FirstSelected();

                // #i36262# may be called for submission entry *or* for
                // submission children. If we don't have any children, we
                // assume the latter case and use the parent
                if( m_aItemList.GetEntry( pEntry, 0 ) == NULL )
                {
                    pEntry = m_aItemList.GetModel()->GetParent( pEntry );
                }

                _rEntry->getPropertyValue( PN_SUBMISSION_ID ) >>= sTemp;
                m_aItemList.SetEntryText( pEntry, sTemp );

                _rEntry->getPropertyValue( PN_SUBMISSION_BIND ) >>= sTemp;
                OUString sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_BIND );
                sEntry += sTemp;
                sal_uIntPtr nPos = 0;
                SvTreeListEntry* pChild = m_aItemList.GetEntry( pEntry, nPos++ );
                m_aItemList.SetEntryText( pChild, sEntry );
                _rEntry->getPropertyValue( PN_SUBMISSION_REF ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_REF );
                sEntry += sTemp;
                pChild = m_aItemList.GetEntry( pEntry, nPos++ );
                m_aItemList.SetEntryText( pChild, sEntry );
                _rEntry->getPropertyValue( PN_SUBMISSION_ACTION ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_ACTION );
                sEntry += sTemp;
                pChild = m_aItemList.GetEntry( pEntry, nPos++ );
                m_aItemList.SetEntryText( pChild, sEntry );
                _rEntry->getPropertyValue( PN_SUBMISSION_METHOD ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_METHOD );
                sEntry += m_aMethodString.toUI( sTemp );
                pChild = m_aItemList.GetEntry( pEntry, nPos++ );
                m_aItemList.SetEntryText( pChild, sEntry );
                _rEntry->getPropertyValue( PN_SUBMISSION_REPLACE ) >>= sTemp;
                sEntry = SVX_RESSTR( RID_STR_DATANAV_SUBM_REPLACE );
                sEntry += m_aReplaceString.toUI( sTemp );
                pChild = m_aItemList.GetEntry( pEntry, nPos++ );
                m_aItemList.SetEntryText( pChild, sEntry );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "XFormsPage::EditEntry(): exception caught" );
            }
        }
    }


    bool XFormsPage::RemoveEntry()
    {
        bool bRet = false;
        SvTreeListEntry* pEntry = m_aItemList.FirstSelected();
        if ( pEntry &&
             ( DGTInstance != m_eGroup || m_aItemList.GetParent( pEntry ) ) )
        {
            Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
            DBG_ASSERT( xModel.is(), "XFormsPage::RemoveEntry(): no model" );
            ItemNode* pNode = static_cast< ItemNode* >( pEntry->GetUserData() );
            DBG_ASSERT( pNode, "XFormsPage::RemoveEntry(): no node" );

            if ( DGTInstance == m_eGroup )
            {
                try
                {
                    DBG_ASSERT( pNode->m_xNode.is(), "XFormsPage::RemoveEntry(): no XNode" );
                    css::xml::dom::NodeType eChildType = pNode->m_xNode->getNodeType();
                    bool bIsElement = ( eChildType == css::xml::dom::NodeType_ELEMENT_NODE );
                    sal_uInt16 nResId = bIsElement ? RID_QRY_REMOVE_ELEMENT : RID_QRY_REMOVE_ATTRIBUTE;
                    OUString sVar = bIsElement ? OUString(ELEMENTNAME) : OUString(ATTRIBUTENAME);
                    QueryBox aQBox( this, SVX_RES( nResId ) );
                    OUString sMessText = aQBox.GetMessText();
                    sMessText = sMessText.replaceFirst(
                        sVar, m_xUIHelper->getNodeDisplayName( pNode->m_xNode, sal_False ) );
                    aQBox.SetMessText( sMessText );
                    if ( aQBox.Execute() == RET_YES )
                    {
                        SvTreeListEntry* pParent = m_aItemList.GetParent( pEntry );
                        DBG_ASSERT( pParent, "XFormsPage::RemoveEntry(): no parent entry" );
                        ItemNode* pParentNode = static_cast< ItemNode* >( pParent->GetUserData() );
                        DBG_ASSERT( pParentNode && pParentNode->m_xNode.is(), "XFormsPage::RemoveEntry(): no parent XNode" );

                        Reference< css::xml::dom::XNode > xPNode;
                        Reference< css::xml::dom::XNode > xNode =
                            pParentNode->m_xNode->removeChild( pNode->m_xNode );
                        if ( xNode.is() )
                            xPNode = xNode->getParentNode();
                        DBG_ASSERT( !xPNode.is(), "XFormsPage::RemoveEntry(): node not removed" );
                        bRet = true;
                    }
                }
                catch ( Exception& )
                {
                    SAL_WARN( "svx.form", "XFormsPage::RemoveEntry(): exception caught" );
                }
            }
            else
            {
                DBG_ASSERT( pNode->m_xPropSet.is(), "XFormsPage::RemoveEntry(): no propset" );
                bool bSubmission = ( DGTSubmission == m_eGroup );
                sal_uInt16 nResId = bSubmission ? RID_QRY_REMOVE_SUBMISSION : RID_QRY_REMOVE_BINDING;
                OUString sProperty = bSubmission ? OUString(PN_SUBMISSION_ID) : OUString(PN_BINDING_ID);
                OUString sSearch = bSubmission ? OUString(SUBMISSIONNAME) : OUString(BINDINGNAME);
                OUString sName;
                try
                {
                    pNode->m_xPropSet->getPropertyValue( sProperty ) >>= sName;
                }
                catch ( Exception& )
                {
                    SAL_WARN( "svx.form", "XFormsPage::RemoveEntry(): exception caught" );
                }
                QueryBox aQBox( this, SVX_RES( nResId ) );
                OUString sMessText = aQBox.GetMessText();
                sMessText = sMessText.replaceFirst( sSearch, sName);
                aQBox.SetMessText( sMessText );
                if ( aQBox.Execute() == RET_YES )
                {
                    try
                    {
                        if ( bSubmission )
                            xModel->getSubmissions()->remove( makeAny( pNode->m_xPropSet ) );
                        else // then Binding Page
                            xModel->getBindings()->remove( makeAny( pNode->m_xPropSet ) );
                        bRet = true;
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::RemoveEntry(): exception caught" );
                    }
                }
            }

            if ( bRet )
                m_aItemList.RemoveEntry( pEntry );
        }

        return bRet;
    }


    bool XFormsPage::Notify( NotifyEvent& rNEvt )
    {
        bool nHandled = false;

        if ( rNEvt.GetType() == EVENT_KEYINPUT )
        {
            sal_uInt16 nCode = rNEvt.GetKeyEvent()->GetKeyCode().GetCode();

            switch ( nCode )
            {
                case KEY_DELETE:
                    nHandled = DoMenuAction( TBI_ITEM_REMOVE );
                    break;
            }
        }

        return nHandled || Window::Notify( rNEvt );
    }

    void XFormsPage::Resize()
    {
        Size aSize = GetOutputSizePixel();
        Size aTbxSize = m_aToolBox.GetSizePixel();
        aTbxSize.Width() = aSize.Width();
        m_aToolBox.SetSizePixel( aTbxSize );
        aSize.Width() -= 4;
        aSize.Height() -= ( 4 + aTbxSize.Height() );
        m_aItemList.SetPosSizePixel( Point( 2, 2 + aTbxSize.Height() ), aSize );
    }

    OUString XFormsPage::SetModel( const Reference< css::xforms::XModel >& _xModel, sal_uInt16 _nPagePos )
    {
        DBG_ASSERT( _xModel.is(), "XFormsPage::SetModel(): invalid model" );

        m_xUIHelper = Reference< css::xforms::XFormsUIHelper1 >( _xModel, UNO_QUERY );
        OUString sRet;
        m_bHasModel = true;
        const ImageList& rImageList = m_pNaviWin->GetItemImageList();

        switch ( m_eGroup )
        {
            case DGTInstance :
            {
                DBG_ASSERT( _nPagePos != TAB_PAGE_NOTFOUND, "XFormsPage::SetModel(): invalid page position" );
                try
                {
                    Reference< XContainer > xContainer( _xModel->getInstances(), UNO_QUERY );
                    if ( xContainer.is() )
                        m_pNaviWin->AddContainerBroadcaster( xContainer );

                    Reference< XEnumerationAccess > xNumAccess( _xModel->getInstances(), UNO_QUERY );
                    if ( xNumAccess.is() )
                    {
                        Reference < XEnumeration > xNum = xNumAccess->createEnumeration();
                        if ( xNum.is() && xNum->hasMoreElements() )
                        {
                            sal_uInt16 nIter = 0;
                            while ( xNum->hasMoreElements() )
                            {
                                if ( nIter == _nPagePos )
                                {
                                    Sequence< PropertyValue > xPropSeq;
                                    Any aAny = xNum->nextElement();
                                    if ( aAny >>= xPropSeq )
                                        sRet = LoadInstance( xPropSeq, rImageList );
                                    else
                                    {
                                        SAL_WARN( "svx.form", "XFormsPage::SetModel(): invalid instance" );
                                    }
                                    break;
                                }
                                else
                                {
                                    xNum->nextElement();
                                    nIter++;
                                }
                            }
                        }
                    }
                }
                catch( Exception& )
                {
                    SAL_WARN( "svx.form", "XFormsPage::SetModel(): exception caught" );
                }
                break;
            }

            case DGTSubmission :
            {
                DBG_ASSERT( TAB_PAGE_NOTFOUND == _nPagePos, "XFormsPage::SetModel(): invalid page position" );
                try
                {
                    Reference< XContainer > xContainer( _xModel->getSubmissions(), UNO_QUERY );
                    if ( xContainer.is() )
                        m_pNaviWin->AddContainerBroadcaster( xContainer );

                    Reference< XEnumerationAccess > xNumAccess( _xModel->getSubmissions(), UNO_QUERY );
                    if ( xNumAccess.is() )
                    {
                        Reference < XEnumeration > xNum = xNumAccess->createEnumeration();
                        if ( xNum.is() && xNum->hasMoreElements() )
                        {
                            while ( xNum->hasMoreElements() )
                            {
                                Reference< XPropertySet > xPropSet;
                                Any aAny = xNum->nextElement();
                                if ( aAny >>= xPropSet )
                                    AddEntry( xPropSet );
                            }
                        }
                    }
                }
                catch( Exception& )
                {
                    SAL_WARN( "svx.form", "XFormsPage::SetModel(): exception caught" );
                }
                break;
            }

            case DGTBinding :
            {
                DBG_ASSERT( TAB_PAGE_NOTFOUND == _nPagePos, "XFormsPage::SetModel(): invalid page position" );
                try
                {
                    Reference< XContainer > xContainer( _xModel->getBindings(), UNO_QUERY );
                    if ( xContainer.is() )
                        m_pNaviWin->AddContainerBroadcaster( xContainer );

                    Reference< XEnumerationAccess > xNumAccess( _xModel->getBindings(), UNO_QUERY );
                    if ( xNumAccess.is() )
                    {
                        Reference < XEnumeration > xNum = xNumAccess->createEnumeration();
                        if ( xNum.is() && xNum->hasMoreElements() )
                        {
                            Image aImage1 = rImageList.GetImage( IID_ELEMENT );
                            Image aImage2 = rImageList.GetImage( IID_ELEMENT );
                            OUString sDelim( ": " );
                            while ( xNum->hasMoreElements() )
                            {
                                Reference< XPropertySet > xPropSet;
                                Any aAny = xNum->nextElement();
                                if ( aAny >>= xPropSet )
                                {
                                    OUString sEntry;
                                    OUString sTemp;
                                    xPropSet->getPropertyValue( PN_BINDING_ID ) >>= sTemp;
                                    sEntry += sTemp;
                                    sEntry += sDelim;
                                    xPropSet->getPropertyValue( PN_BINDING_EXPR ) >>= sTemp;
                                    sEntry += sTemp;

                                    ItemNode* pNode = new ItemNode( xPropSet );
                                    m_aItemList.InsertEntry(
                                        sEntry, aImage1, aImage2, NULL, false, TREELIST_APPEND, pNode );
                                }
                            }
                        }
                    }
                }
                catch( Exception& )
                {
                    SAL_WARN( "svx.form", "XFormsPage::SetModel(): exception caught" );
                }
                break;
            }
            default:
                OSL_FAIL( "XFormsPage::SetModel: unknown group!" );
                break;
        }

        EnableMenuItems( NULL );

        return sRet;
    }

    void XFormsPage::ClearModel()
    {
        m_bHasModel = false;
        m_aItemList.DeleteAndClear();
    }

    OUString XFormsPage::LoadInstance(
        const Sequence< PropertyValue >& _xPropSeq, const ImageList& _rImgLst )
    {
        OUString sRet;
        OUString sTemp;
        OUString sInstModel = PN_INSTANCE_MODEL;
        OUString sInstName = PN_INSTANCE_ID;
        OUString sInstURL = PN_INSTANCE_URL;
        const PropertyValue* pProps = _xPropSeq.getConstArray();
        const PropertyValue* pPropsEnd = pProps + _xPropSeq.getLength();
        for ( ; pProps != pPropsEnd; ++pProps )
        {
            if ( sInstModel == pProps->Name )
            {
                Reference< css::xml::dom::XNode > xRoot;
                if ( pProps->Value >>= xRoot )
                {
                    try
                    {
                        Reference< XEventTarget > xTarget( xRoot, UNO_QUERY );
                        if ( xTarget.is() )
                            m_pNaviWin->AddEventBroadcaster( xTarget );

                    #if OSL_DEBUG_LEVEL > 0
                        css::xml::dom::NodeType eNodeType = xRoot->getNodeType(); (void)eNodeType;
                    #endif
                        OUString sNodeName =
                            m_xUIHelper->getNodeDisplayName( xRoot, m_pNaviWin->IsShowDetails() );
                        if ( sNodeName.isEmpty() )
                            sNodeName = xRoot->getNodeName();
                        if ( xRoot->hasChildNodes() )
                            AddChildren( NULL, _rImgLst, xRoot );
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "XFormsPage::LoadInstance(): exception caught" );
                    }
                }
            }
            else if ( sInstName == pProps->Name && ( pProps->Value >>= sTemp ) )
                m_sInstanceName = sRet = sTemp;
            else if ( sInstURL == pProps->Name && ( pProps->Value >>= sTemp ) )
                m_sInstanceURL = sTemp;
        }

        return sRet;
    }


    bool XFormsPage::DoMenuAction( sal_uInt16 _nMenuID )
    {
        return DoToolBoxAction( _nMenuID );
    }


    void XFormsPage::EnableMenuItems( Menu* _pMenu )
    {
        bool bEnableAdd = false;
        bool bEnableEdit = false;
        bool bEnableRemove = false;

        SvTreeListEntry* pEntry = m_aItemList.FirstSelected();
        if ( pEntry )
        {
            bEnableAdd = true;
            bool bSubmitChild = false;
            if ( DGTSubmission == m_eGroup && m_aItemList.GetParent( pEntry ) )
            {
                pEntry = m_aItemList.GetParent( pEntry );
                bSubmitChild = true;
            }
            ItemNode* pNode = static_cast< ItemNode* >( pEntry->GetUserData() );
            if ( pNode && ( pNode->m_xNode.is() || pNode->m_xPropSet.is() ) )
            {
                bEnableEdit = true;
                bEnableRemove = ( bSubmitChild != true );
                if ( DGTInstance == m_eGroup && !m_aItemList.GetParent( pEntry ) )
                    bEnableRemove = false;
                if ( pNode->m_xNode.is() )
                {
                    try
                    {
                        css::xml::dom::NodeType eChildType = pNode->m_xNode->getNodeType();
                        if ( eChildType != css::xml::dom::NodeType_ELEMENT_NODE
                            && eChildType != css::xml::dom::NodeType_DOCUMENT_NODE )
                        {
                            bEnableAdd = false;
                        }
                    }
                    catch ( Exception& )
                    {
                       SAL_WARN( "svx.form", "XFormsPage::EnableMenuItems(): exception caught" );
                    }
                }
            }
        }
        else if ( m_eGroup != DGTInstance )
            bEnableAdd = true;

        m_aToolBox.EnableItem( TBI_ITEM_ADD, bEnableAdd );
        m_aToolBox.EnableItem( TBI_ITEM_ADD_ELEMENT, bEnableAdd );
        m_aToolBox.EnableItem( TBI_ITEM_ADD_ATTRIBUTE, bEnableAdd );
        m_aToolBox.EnableItem( TBI_ITEM_EDIT, bEnableEdit );
        m_aToolBox.EnableItem( TBI_ITEM_REMOVE, bEnableRemove );

        if ( _pMenu )
        {
            _pMenu->EnableItem( TBI_ITEM_ADD, bEnableAdd );
            _pMenu->EnableItem( TBI_ITEM_ADD_ELEMENT, bEnableAdd );
            _pMenu->EnableItem( TBI_ITEM_ADD_ATTRIBUTE, bEnableAdd );
            _pMenu->EnableItem( TBI_ITEM_EDIT, bEnableEdit );
            _pMenu->EnableItem( TBI_ITEM_REMOVE, bEnableRemove );
        }
        if ( DGTInstance == m_eGroup )
        {
            sal_uInt16 nResId1 = RID_STR_DATANAV_EDIT_ELEMENT;
            sal_uInt16 nResId2 = RID_STR_DATANAV_REMOVE_ELEMENT;
            if ( pEntry )
            {
                ItemNode* pNode = static_cast< ItemNode* >( pEntry->GetUserData() );
                if ( pNode && pNode->m_xNode.is() )
                {
                    try
                    {
                        css::xml::dom::NodeType eChildType = pNode->m_xNode->getNodeType();
                        if ( eChildType == css::xml::dom::NodeType_ATTRIBUTE_NODE )
                        {
                            nResId1 = RID_STR_DATANAV_EDIT_ATTRIBUTE;
                            nResId2 = RID_STR_DATANAV_REMOVE_ATTRIBUTE;
                        }
                    }
                    catch ( Exception& )
                    {
                       SAL_WARN( "svx.form", "XFormsPage::EnableMenuItems(): exception caught" );
                    }
                }
            }
            m_aToolBox.SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( nResId1 ) );
            m_aToolBox.SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( nResId2 ) );
            if ( _pMenu )
            {
                _pMenu->SetItemText( TBI_ITEM_EDIT, SVX_RESSTR( nResId1 ) );
                _pMenu->SetItemText( TBI_ITEM_REMOVE, SVX_RESSTR( nResId2 ) );
            }
        }
    }



    // class DataNavigatorWindow

    DataNavigatorWindow::DataNavigatorWindow( Window* pParent, SfxBindings* pBindings ) :

        Window( pParent, SVX_RES( RID_SVXWIN_DATANAVIGATOR ) ),

        m_aModelsBox        ( this, SVX_RES( LB_MODELS ) ),
        m_aModelBtn         ( this, SVX_RES( MB_MODELS ) ),
        m_aTabCtrl          ( this, SVX_RES( TC_ITEMS ) ),
        m_aInstanceBtn      ( this, SVX_RES( MB_INSTANCES ) ),

        m_pInstPage         ( NULL ),
        m_pSubmissionPage   ( NULL ),
        m_pBindingPage      ( NULL ),

        m_nMinWidth         ( 0 ),
        m_nMinHeight        ( 0 ),
        m_nBorderHeight     ( 0 ),
        m_nLastSelectedPos  ( LISTBOX_ENTRY_NOTFOUND ),
        m_bShowDetails      ( false ),
        m_bIsNotifyDisabled ( false ),

        m_aItemImageList    (       SVX_RES( IL_ITEM_BMPS ) ),
        m_xDataListener     ( new DataListener( this ) )

    {
        FreeResource();

        // init minimal metric
        m_a2Size = LogicToPixel( Size( 2, 2 ), MAP_APPFONT );
        m_a3Size = LogicToPixel( Size( 3, 3 ), MAP_APPFONT );
        Size aOutSz = GetOutputSizePixel();
        Size aLogSize = PixelToLogic( aOutSz, MAP_APPFONT );
        m_nMinWidth = aLogSize.Width();
        m_nMinHeight = aLogSize.Height();
        m_nBorderHeight = 4*m_a3Size.Height() +
            m_aModelBtn.GetSizePixel().Height() + m_aInstanceBtn.GetSizePixel().Height();

        // handler
        m_aModelsBox.SetSelectHdl( LINK( this, DataNavigatorWindow, ModelSelectHdl ) );
        Link aLink = LINK( this, DataNavigatorWindow, MenuSelectHdl );
        m_aModelBtn.SetSelectHdl( aLink );
        m_aInstanceBtn.SetSelectHdl( aLink );
        aLink = LINK( this, DataNavigatorWindow, MenuActivateHdl );
        m_aModelBtn.SetActivateHdl( aLink );
        m_aInstanceBtn.SetActivateHdl( aLink );
        m_aTabCtrl.SetActivatePageHdl( LINK( this, DataNavigatorWindow, ActivatePageHdl ) );
        m_aUpdateTimer.SetTimeout( 2000 );
        m_aUpdateTimer.SetTimeoutHdl( LINK( this, DataNavigatorWindow, UpdateHdl ) );

        // init tabcontrol
        m_aTabCtrl.Show();
        sal_Int32 nPageId = TID_INSTANCE;
        SvtViewOptions aViewOpt( E_TABDIALOG, CFGNAME_DATANAVIGATOR );
        if ( aViewOpt.Exists() )
        {
            nPageId = aViewOpt.GetPageID();
            aViewOpt.GetUserItem(CFGNAME_SHOWDETAILS) >>= m_bShowDetails;
        }

        Menu* pMenu = m_aInstanceBtn.GetPopupMenu();
        pMenu->SetItemBits( MID_SHOW_DETAILS, MIB_CHECKABLE );
        pMenu->CheckItem( MID_SHOW_DETAILS, m_bShowDetails );

        m_aTabCtrl.SetCurPageId( static_cast< sal_uInt16 >( nPageId ) );
        ActivatePageHdl( &m_aTabCtrl );

        // get our frame
        DBG_ASSERT( pBindings != NULL,
                    "DataNavigatorWindow::LoadModels(): no SfxBindings; can't get frame" );
        m_xFrame = Reference<XFrame>(
            pBindings->GetDispatcher()->GetFrame()->GetFrame().GetFrameInterface(),
            UNO_QUERY );
        DBG_ASSERT( m_xFrame.is(), "DataNavigatorWindow::LoadModels(): no frame" );
        // add frameaction listener
        Reference< XFrameActionListener > xListener(
            static_cast< XFrameActionListener* >( m_xDataListener.get() ), UNO_QUERY );
        m_xFrame->addFrameActionListener( xListener );

        // load xforms models of the current document
        LoadModels();
    }

    DataNavigatorWindow::~DataNavigatorWindow()
    {
        SvtViewOptions aViewOpt( E_TABDIALOG, CFGNAME_DATANAVIGATOR );
        aViewOpt.SetPageID( static_cast< sal_Int32 >( m_aTabCtrl.GetCurPageId() ) );
        Any aAny;
        aAny <<= m_bShowDetails;
        aViewOpt.SetUserItem(CFGNAME_SHOWDETAILS,aAny);

        delete m_pInstPage;
        delete m_pSubmissionPage;
        delete m_pBindingPage;

        sal_Int32 i, nCount = m_aPageList.size();
        for ( i = 0; i < nCount; ++i )
            delete m_aPageList[i];
        Reference< XFrameActionListener > xListener(
            static_cast< XFrameActionListener* >( m_xDataListener.get() ), UNO_QUERY );
        m_xFrame->removeFrameActionListener( xListener );
        RemoveBroadcaster();
        m_xDataListener.clear();
    }


    IMPL_LINK( DataNavigatorWindow, ModelSelectHdl, ListBox *, pBox )
    {
        sal_Int32 nPos = m_aModelsBox.GetSelectEntryPos();
        // pBox == NULL, if you want to force a new fill.
        if ( nPos != m_nLastSelectedPos || !pBox )
        {
            m_nLastSelectedPos = nPos;
            ClearAllPageModels( pBox != NULL );
            InitPages();
            SetPageModel();
        }

        return 0;
    }

    IMPL_LINK( DataNavigatorWindow, MenuSelectHdl, MenuButton *, pBtn )
    {
        bool bIsDocModified = false;
        Reference< css::xforms::XFormsUIHelper1 > xUIHelper;
        sal_Int32 nSelectedPos = m_aModelsBox.GetSelectEntryPos();
        OUString sSelectedModel( m_aModelsBox.GetEntry( nSelectedPos ) );
        Reference< css::xforms::XModel > xModel;
        try
        {
            Any aAny = m_xDataContainer->getByName( sSelectedModel );
            if ( aAny >>= xModel )
                xUIHelper = Reference< css::xforms::XFormsUIHelper1 >( xModel, UNO_QUERY );
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
        }
        DBG_ASSERT( xUIHelper.is(), "DataNavigatorWindow::MenuSelectHdl(): no UIHelper" );

        m_bIsNotifyDisabled = true;

        if ( &m_aModelBtn == pBtn )
        {
            switch ( pBtn->GetCurItemId() )
            {
                case MID_MODELS_ADD :
                {
                    AddModelDialog aDlg( this, false );
                    bool bShowDialog = true;
                    while ( bShowDialog )
                    {
                        bShowDialog = false;
                        if ( aDlg.Execute() == RET_OK )
                        {
                            OUString sNewName = aDlg.GetName();
                            bool bDocumentData = aDlg.GetModifyDoc();

                            if ( m_aModelsBox.GetEntryPos( sNewName ) != LISTBOX_ENTRY_NOTFOUND )
                            {
                                // error: model name already exists
                                ErrorBox aErrBox( this, SVX_RES( RID_ERR_DOUBLE_MODELNAME ) );
                                OUString sMessText = aErrBox.GetMessText();
                                sMessText = sMessText.replaceFirst( MSG_VARIABLE, sNewName );
                                aErrBox.SetMessText( sMessText );
                                aErrBox.Execute();
                                bShowDialog = true;
                            }
                            else
                            {
                                try
                                {
                                    // add new model to frame model
                                    Reference< css::xforms::XModel > xNewModel(
                                        xUIHelper->newModel( m_xFrameModel, sNewName ), UNO_SET_THROW );

                                    Reference< XPropertySet > xModelProps( xNewModel, UNO_QUERY_THROW );
                                    xModelProps->setPropertyValue(
                                        OUString( "ExternalData" ),
                                        makeAny( !bDocumentData ) );

                                    sal_Int32 nNewPos = m_aModelsBox.InsertEntry( sNewName );
                                    m_aModelsBox.SelectEntryPos( nNewPos );
                                    ModelSelectHdl( &m_aModelsBox );
                                    bIsDocModified = true;
                                }
                                catch ( Exception& )
                                {
                                    SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                                }
                            }
                        }
                    }
                    break;
                }
                case MID_MODELS_EDIT :
                {
                    AddModelDialog aDlg( this, true );
                    aDlg.SetName( sSelectedModel );

                    bool bDocumentData( false );
                    try
                    {
                        Reference< css::xforms::XFormsSupplier > xFormsSupp( m_xFrameModel, UNO_QUERY_THROW );
                        Reference< XNameContainer > xXForms( xFormsSupp->getXForms(), UNO_SET_THROW );
                        Reference< XPropertySet > xModelProps( xXForms->getByName( sSelectedModel ), UNO_QUERY_THROW );
                        bool bExternalData = false;
                        OSL_VERIFY( xModelProps->getPropertyValue(
                            OUString( "ExternalData" ) ) >>= bExternalData );
                        bDocumentData = !bExternalData;
                    }
                    catch( const Exception& )
                    {
                        DBG_UNHANDLED_EXCEPTION();
                    }
                    aDlg.SetModifyDoc( bDocumentData );

                    if ( aDlg.Execute() == RET_OK )
                    {
                        if ( aDlg.GetModifyDoc() != bool( bDocumentData ) )
                        {
                            bDocumentData = aDlg.GetModifyDoc();
                            try
                            {
                                Reference< css::xforms::XFormsSupplier > xFormsSupp( m_xFrameModel, UNO_QUERY_THROW );
                                Reference< XNameContainer > xXForms( xFormsSupp->getXForms(), UNO_SET_THROW );
                                Reference< XPropertySet > xModelProps( xXForms->getByName( sSelectedModel ), UNO_QUERY_THROW );
                                xModelProps->setPropertyValue(
                                    OUString( "ExternalData" ),
                                    makeAny( !bDocumentData ) );
                                bIsDocModified = true;
                            }
                            catch( const Exception& )
                            {
                                DBG_UNHANDLED_EXCEPTION();
                            }
                        }

                        OUString sNewName = aDlg.GetName();
                        if ( !sNewName.isEmpty() && ( sNewName != sSelectedModel ) )
                        {
                            try
                            {
                                xUIHelper->renameModel( m_xFrameModel, sSelectedModel, sNewName );

                                m_aModelsBox.RemoveEntry( nSelectedPos );
                                nSelectedPos = m_aModelsBox.InsertEntry( sNewName );
                                m_aModelsBox.SelectEntryPos( nSelectedPos );
                                bIsDocModified = true;
                            }
                            catch ( Exception& )
                            {
                                SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                            }
                        }
                    }
                    break;
                }
                case MID_MODELS_REMOVE :
                {
                    QueryBox aQBox( this, SVX_RES( RID_QRY_REMOVE_MODEL ) );
                    OUString sText = aQBox.GetMessText();
                    sText = sText.replaceFirst( MODELNAME, sSelectedModel );
                    aQBox.SetMessText( sText );
                    if ( aQBox.Execute() == RET_YES )
                    {
                        try
                        {
                            xUIHelper->removeModel( m_xFrameModel, sSelectedModel );
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                        }
                        m_aModelsBox.RemoveEntry( nSelectedPos );
                        if ( m_aModelsBox.GetEntryCount() <= nSelectedPos )
                            nSelectedPos = m_aModelsBox.GetEntryCount() - 1;
                        m_aModelsBox.SelectEntryPos( nSelectedPos );
                        ModelSelectHdl( &m_aModelsBox );
                        bIsDocModified = true;
                    }
                    break;
                }
                default:
                {
                    SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): wrong menu item" );
                }
            }
        }
        else if ( &m_aInstanceBtn == pBtn )
        {
            switch ( pBtn->GetCurItemId() )
            {
                case MID_INSTANCES_ADD :
                {
                    AddInstanceDialog aDlg( this, false );
                    if ( aDlg.Execute() == RET_OK )
                    {
                        sal_uInt16 nInst = GetNewPageId();
                        OUString sName = aDlg.GetName();
                        OUString sURL = aDlg.GetURL();
                        bool bLinkOnce = aDlg.IsLinkInstance();
                        try
                        {
                            Reference< css::xml::dom::XDocument > xNewInst =
                                xUIHelper->newInstance( sName, sURL, !bLinkOnce );
                        }
                        catch ( Exception& )
                        {
                            SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                        }
                        ModelSelectHdl( NULL );
                        m_aTabCtrl.SetCurPageId( nInst );
                        XFormsPage* pPage = GetCurrentPage( nInst );
                        pPage->SetInstanceName(sName);
                        pPage->SetInstanceURL(sURL);
                        pPage->SetLinkOnce(bLinkOnce);
                        ActivatePageHdl( &m_aTabCtrl );
                        bIsDocModified = true;
                    }
                    break;
                }
                case MID_INSTANCES_EDIT :
                {
                    sal_uInt16 nId = 0;
                    XFormsPage* pPage = GetCurrentPage( nId );
                    if ( pPage )
                    {
                        AddInstanceDialog aDlg( this, true );
                        aDlg.SetName( pPage->GetInstanceName() );
                        aDlg.SetURL( pPage->GetInstanceURL() );
                        aDlg.SetLinkInstance( pPage->GetLinkOnce() );
                        OUString sOldName = aDlg.GetName();
                        if ( aDlg.Execute() == RET_OK )
                        {
                            OUString sNewName = aDlg.GetName();
                            OUString sURL = aDlg.GetURL();
                            bool bLinkOnce = aDlg.IsLinkInstance();
                            try
                            {
                                xUIHelper->renameInstance( sOldName,
                                                           sNewName,
                                                           sURL,
                                                           !bLinkOnce );
                            }
                            catch ( Exception& )
                            {
                                SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                            }
                            pPage->SetInstanceName(sNewName);
                            pPage->SetInstanceURL(sURL);
                            pPage->SetLinkOnce(bLinkOnce);
                            m_aTabCtrl.SetPageText( nId, sNewName );
                            bIsDocModified = true;
                        }
                    }
                    break;
                }
                case MID_INSTANCES_REMOVE :
                {
                    sal_uInt16 nId = 0;
                    XFormsPage* pPage = GetCurrentPage( nId );
                    if ( pPage )
                    {
                        OUString sInstName = pPage->GetInstanceName();
                        QueryBox aQBox( this, SVX_RES( RID_QRY_REMOVE_INSTANCE ) );
                        OUString sMessText = aQBox.GetMessText();
                        sMessText = sMessText.replaceFirst( INSTANCENAME, sInstName );
                        aQBox.SetMessText( sMessText );
                        if ( aQBox.Execute() == RET_YES )
                        {
                            bool bDoRemove = false;
                            if ( nId > TID_INSTANCE )
                            {
                                PageList::iterator aPageListEnd = m_aPageList.end();
                                PageList::iterator aFoundPage =
                                    std::find( m_aPageList.begin(), aPageListEnd, pPage );
                                if ( aFoundPage != aPageListEnd )
                                {
                                    m_aPageList.erase( aFoundPage );
                                    delete pPage;
                                    bDoRemove = true;
                                }
                            }
                            else
                            {
                                DELETEZ( m_pInstPage );
                                bDoRemove = true;
                            }

                            if ( bDoRemove )
                            {
                                try
                                {
                                    xUIHelper->removeInstance( sInstName );
                                }
                                catch ( Exception& )
                                {
                                    SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): exception caught" );
                                }
                                m_aTabCtrl.RemovePage( nId );
                                m_aTabCtrl.SetCurPageId( TID_INSTANCE );
                                ModelSelectHdl( NULL );
                                bIsDocModified = true;
                            }
                        }
                    }
                    break;
                }
                case MID_SHOW_DETAILS :
                {
                    m_bShowDetails = !m_bShowDetails;
                    m_aInstanceBtn.GetPopupMenu()->CheckItem( MID_SHOW_DETAILS, m_bShowDetails );
                    ModelSelectHdl( &m_aModelsBox );
                    break;
                }
                default:
                {
                    SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): wrong menu item" );
                }
            }
        }
        else
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::MenuSelectHdl(): wrong button" );
        }

        m_bIsNotifyDisabled = false;

        if ( bIsDocModified )
            SetDocModified();
        return 0;
    }

    IMPL_LINK( DataNavigatorWindow, MenuActivateHdl, MenuButton *, pBtn )
    {
        Menu* pMenu = pBtn->GetPopupMenu();

        if ( &m_aInstanceBtn == pBtn )
        {
            bool bIsInstPage = ( m_aTabCtrl.GetCurPageId() >= TID_INSTANCE );
            pMenu->EnableItem( MID_INSTANCES_EDIT, bIsInstPage );
            pMenu->EnableItem( MID_INSTANCES_REMOVE,
                bIsInstPage && m_aTabCtrl.GetPageCount() > MIN_PAGE_COUNT );
            pMenu->EnableItem( MID_SHOW_DETAILS, bIsInstPage );
        }
        else if ( &m_aModelBtn == pBtn )
        {
            // we need at least one model!
            pMenu->EnableItem( MID_MODELS_REMOVE, m_aModelsBox.GetEntryCount() > 1 );
        }
        else
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::MenuActivateHdl(): wrong button" );
        }
        return 0;
    }

    IMPL_LINK_NOARG(DataNavigatorWindow, ActivatePageHdl)
    {
        sal_uInt16 nId = 0;
        XFormsPage* pPage = GetCurrentPage( nId );
        if ( pPage )
        {
            m_aTabCtrl.SetTabPage( nId, pPage );
            if ( m_xDataContainer.is() && !pPage->HasModel() )
                SetPageModel();
        }

        return 0;
    }

    IMPL_LINK_NOARG(DataNavigatorWindow, UpdateHdl)
    {
        ModelSelectHdl( NULL );
        return 0;
    }

    XFormsPage* DataNavigatorWindow::GetCurrentPage( sal_uInt16& rCurId )
    {
        rCurId = m_aTabCtrl.GetCurPageId();
        XFormsPage* pPage = NULL;
        switch ( rCurId )
        {
            case TID_SUBMISSION:
            {
                if ( !m_pSubmissionPage )
                    m_pSubmissionPage = new XFormsPage( &m_aTabCtrl, this, DGTSubmission );
                pPage = m_pSubmissionPage;
                break;
            }

            case TID_BINDINGS:
            {
                if ( !m_pBindingPage )
                    m_pBindingPage = new XFormsPage( &m_aTabCtrl, this, DGTBinding );
                pPage = m_pBindingPage;
                break;
            }

            case TID_INSTANCE:
            {
                if ( !m_pInstPage )
                    m_pInstPage = new XFormsPage( &m_aTabCtrl, this, DGTInstance );
                pPage = m_pInstPage;
                break;
            }
        }

        if ( rCurId > TID_INSTANCE )
        {
            sal_uInt16 nPos = m_aTabCtrl.GetPagePos( rCurId );
            if ( HasFirstInstancePage() && nPos > 0 )
                nPos--;
            if ( m_aPageList.size() > nPos )
                pPage = m_aPageList[nPos];
            else
            {
                pPage = new XFormsPage( &m_aTabCtrl, this, DGTInstance );
                m_aPageList.push_back( pPage );
            }
        }

        return pPage;
    }

    void DataNavigatorWindow::LoadModels()
    {
        if ( !m_xFrameModel.is() )
        {
            // get model of active frame
            Reference< XController > xCtrl = m_xFrame->getController();
            if ( xCtrl.is() )
            {
                try
                {
                    m_xFrameModel = xCtrl->getModel();
                }
                catch ( Exception& )
                {
                    SAL_WARN( "svx.form", "DataNavigatorWindow::LoadModels(): exception caught" );
                }
            }
        }

        if ( m_xFrameModel.is() )
        {
            try
            {
                Reference< css::xforms::XFormsSupplier > xFormsSupp( m_xFrameModel, UNO_QUERY );
                if ( xFormsSupp.is() )
                {
                    Reference< XNameContainer > xContainer = xFormsSupp->getXForms();
                    if ( xContainer.is() )
                    {
                        m_xDataContainer = xContainer;
                        Sequence< OUString > aNameList = m_xDataContainer->getElementNames();
                        sal_Int32 i, nCount = aNameList.getLength();
                        OUString* pNames = aNameList.getArray();
                        for ( i = 0; i < nCount; ++i )
                        {
                            Any aAny = m_xDataContainer->getByName( pNames[i] );
                            Reference< css::xforms::XModel > xFormsModel;
                            if ( aAny >>= xFormsModel )
                                m_aModelsBox.InsertEntry( xFormsModel->getID() );
                        }
                    }
                }
            }
            catch( Exception& )
            {
                SAL_WARN( "svx.form", "DataNavigatorWindow::LoadModels(): exception caught" );
            }
        }

        if ( m_aModelsBox.GetEntryCount() > 0 )
        {
            m_aModelsBox.SelectEntryPos(0);
            ModelSelectHdl( &m_aModelsBox );
        }
    }

    void DataNavigatorWindow::SetPageModel()
    {
        OUString sModel( m_aModelsBox.GetSelectEntry() );
        try
        {
            Any aAny = m_xDataContainer->getByName( sModel );
            Reference< css::xforms::XModel > xFormsModel;
            if ( aAny >>= xFormsModel )
            {
                sal_uInt16 nPagePos = TAB_PAGE_NOTFOUND;
                sal_uInt16 nId = 0;
                XFormsPage* pPage = GetCurrentPage( nId );
                DBG_ASSERT( pPage, "DataNavigatorWindow::SetPageModel(): no page" );
                if ( nId >= TID_INSTANCE )
                    // instance page
                    nPagePos = m_aTabCtrl.GetPagePos( nId );
                m_bIsNotifyDisabled = true;
                OUString sText = pPage->SetModel( xFormsModel, nPagePos );
                m_bIsNotifyDisabled = false;
                if ( !sText.isEmpty() )
                    m_aTabCtrl.SetPageText( nId, sText );
            }
        }
        catch ( NoSuchElementException& )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::SetPageModel(): no such element" );
        }
        catch( Exception& )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::SetPageModel(): unexpected exception" );
        }
    }

    void DataNavigatorWindow::InitPages()
    {
        OUString sModel( m_aModelsBox.GetSelectEntry() );
        try
        {
            Any aAny = m_xDataContainer->getByName( sModel );
            Reference< css::xforms::XModel > xModel;
            if ( aAny >>= xModel )
            {
                Reference< XEnumerationAccess > xNumAccess( xModel->getInstances(), UNO_QUERY );
                if ( xNumAccess.is() )
                {
                    Reference < XEnumeration > xNum = xNumAccess->createEnumeration();
                    if ( xNum.is() && xNum->hasMoreElements() )
                    {
                        sal_Int32 nAlreadyLoadedCount = m_aPageList.size();
                        if ( !HasFirstInstancePage() && nAlreadyLoadedCount > 0 )
                            nAlreadyLoadedCount--;
                        sal_Int32 nIdx = 0;
                        while ( xNum->hasMoreElements() )
                        {
                            if ( nIdx > nAlreadyLoadedCount )
                            {
                                Sequence< PropertyValue > xPropSeq;
                                if ( xNum->nextElement() >>= xPropSeq )
                                    CreateInstancePage( xPropSeq );
                                else
                                {
                                    SAL_WARN( "svx.form", "DataNavigator::InitPages(): invalid instance" );
                                }
                            }
                            else
                                xNum->nextElement();
                            nIdx++;
                        }
                    }
                }
            }
        }
        catch ( NoSuchElementException& )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::SetPageModel(): no such element" );
        }
        catch( Exception& )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::SetPageModel(): unexpected exception" );
        }
    }

    void DataNavigatorWindow::ClearAllPageModels( bool bClearPages )
    {
        if ( m_pInstPage )
            m_pInstPage->ClearModel();
        if ( m_pSubmissionPage )
            m_pSubmissionPage->ClearModel();
        if ( m_pBindingPage )
            m_pBindingPage->ClearModel();

        sal_Int32 i, nCount = m_aPageList.size();
        for ( i = 0; i < nCount; ++i )
        {
            XFormsPage* pPage = m_aPageList[i];
            pPage->ClearModel();
            if ( bClearPages )
                delete pPage;
        }

        if ( bClearPages )
        {
            m_aPageList.clear();
            while ( m_aTabCtrl.GetPageCount() > MIN_PAGE_COUNT )
                m_aTabCtrl.RemovePage( m_aTabCtrl.GetPageId( 1 ) );
        }
    }

    void DataNavigatorWindow::CreateInstancePage( const Sequence< PropertyValue >& _xPropSeq )
    {
        OUString sInstName;
        OUString sID( PN_INSTANCE_ID );
        const PropertyValue* pProps = _xPropSeq.getConstArray();
        const PropertyValue* pPropsEnd = pProps + _xPropSeq.getLength();
        for ( ; pProps != pPropsEnd; ++pProps )
        {
            if ( sID == pProps->Name )
            {
                pProps->Value >>= sInstName;
                break;
            }
        }

        sal_uInt16 nPageId = GetNewPageId();
        if ( sInstName.isEmpty() )
        {
            SAL_WARN( "svx.form", "DataNavigatorWindow::CreateInstancePage(): instance without name" );
            OUString sTemp("untitled");
            sTemp += OUString::number( nPageId );
            sInstName = sTemp;
        }
        m_aTabCtrl.InsertPage( nPageId, sInstName, m_aTabCtrl.GetPageCount() - 2 );
    }


    bool DataNavigatorWindow::HasFirstInstancePage() const
    {
        return ( m_aTabCtrl.GetPageId( 0 ) == TID_INSTANCE );
    }


    sal_uInt16 DataNavigatorWindow::GetNewPageId() const
    {
        sal_uInt16 i, nMax = 0, nCount = m_aTabCtrl.GetPageCount();
        for ( i = 0; i < nCount; ++i )
        {
            if ( nMax < m_aTabCtrl.GetPageId(i) )
                nMax = m_aTabCtrl.GetPageId(i);
        }
        return ( nMax + 1 );
    }


    void DataNavigatorWindow::Resize()
    {
        Window::Resize();

        Size aOutSz = GetOutputSizePixel();
        long nWidth = std::max( aOutSz.Width(), m_nMinWidth );
        long nHeight = std::max( aOutSz.Height(), m_nMinHeight );

        Size aSz = m_aModelsBox.GetSizePixel();
        aSz.Width() = nWidth - 3*m_a3Size.Width() - m_aModelBtn.GetSizePixel().Width();
        m_aModelsBox.SetSizePixel( aSz );
        Point aPos = m_aModelBtn.GetPosPixel();
        aPos.X() = m_aModelsBox.GetPosPixel().X() + aSz.Width() + m_a3Size.Width();
        m_aModelBtn.SetPosPixel( aPos );

        aSz = m_aTabCtrl.GetSizePixel();
        aSz.Width() = nWidth - 2*m_a3Size.Width();
        aSz.Height() = nHeight - m_nBorderHeight;
        m_aTabCtrl.SetSizePixel( aSz );
        // Instance button positioning
        aPos = m_aInstanceBtn.GetPosPixel();
        // right aligned
        aPos.X() = nWidth - m_aInstanceBtn.GetSizePixel().Width() - m_a3Size.Width();
        // under the tabcontrol
        aPos.Y() = m_aTabCtrl.GetPosPixel().Y() + aSz.Height() + m_a3Size.Height();
        m_aInstanceBtn.SetPosPixel( aPos );
    }


    void DataNavigatorWindow::SetDocModified()
    {
        SfxObjectShell* pCurrentDoc = SfxObjectShell::Current();
        DBG_ASSERT( pCurrentDoc, "DataNavigatorWindow::SetDocModified(): no objectshell" );
        if ( !pCurrentDoc->IsModified() && pCurrentDoc->IsEnableSetModified() )
            pCurrentDoc->SetModified();
    }


    void DataNavigatorWindow::NotifyChanges( bool _bLoadAll )
    {
        if ( !m_bIsNotifyDisabled )
        {
            if ( _bLoadAll )
            {
                // reset all members
                RemoveBroadcaster();
                m_xDataContainer.clear();
                m_xFrameModel.clear();
                m_aModelsBox.Clear();
                m_nLastSelectedPos = LISTBOX_ENTRY_NOTFOUND;
                // for a reload
                LoadModels();
            }
            else
                m_aUpdateTimer.Start();
        }
    }


    void DataNavigatorWindow::AddContainerBroadcaster( const XContainer_ref& xContainer )
    {
        Reference< XContainerListener > xListener(
            static_cast< XContainerListener* >( m_xDataListener.get() ), UNO_QUERY );
        xContainer->addContainerListener( xListener );
        m_aContainerList.push_back( xContainer );
    }


    void DataNavigatorWindow::AddEventBroadcaster( const XEventTarget_ref& xTarget )
    {
        Reference< XEventListener > xListener(
            static_cast< XEventListener* >( m_xDataListener.get() ), UNO_QUERY );
        xTarget->addEventListener( EVENTTYPE_CHARDATA, xListener, true );
        xTarget->addEventListener( EVENTTYPE_CHARDATA, xListener, false );
        xTarget->addEventListener( EVENTTYPE_ATTR, xListener, true );
        xTarget->addEventListener( EVENTTYPE_ATTR, xListener, false );
        m_aEventTargetList.push_back( xTarget );
    }


    void DataNavigatorWindow::RemoveBroadcaster()
    {
        Reference< XContainerListener > xContainerListener(
            static_cast< XContainerListener* >( m_xDataListener.get() ), UNO_QUERY );
        sal_Int32 i, nCount = m_aContainerList.size();
        for ( i = 0; i < nCount; ++i )
            m_aContainerList[i]->removeContainerListener( xContainerListener );
        Reference< XEventListener > xEventListener(
            static_cast< XEventListener* >( m_xDataListener.get() ), UNO_QUERY );
        nCount = m_aEventTargetList.size();
        for ( i = 0; i < nCount; ++i )
        {
            m_aEventTargetList[i]->removeEventListener( EVENTTYPE_CHARDATA, xEventListener, true );
            m_aEventTargetList[i]->removeEventListener( EVENTTYPE_CHARDATA, xEventListener, false );
            m_aEventTargetList[i]->removeEventListener( EVENTTYPE_ATTR, xEventListener, true );
            m_aEventTargetList[i]->removeEventListener( EVENTTYPE_ATTR, xEventListener, false );
        }
    }


    // class DataNavigator


    DataNavigator::DataNavigator( SfxBindings* _pBindings, SfxChildWindow* _pMgr, Window* _pParent ) :

        SfxDockingWindow( _pBindings, _pMgr, _pParent,
                          WinBits(WB_STDMODELESS|WB_SIZEABLE|WB_ROLLABLE|WB_3DLOOK|WB_DOCKABLE) ),
        SfxControllerItem( SID_FM_DATANAVIGATOR_CONTROL, *_pBindings ),

        m_aDataWin( this, _pBindings )

    {

        SetHelpId( HID_DATA_NAVIGATOR_WIN );
        SetText( SVX_RES( RID_STR_DATANAVIGATOR ) );

        Size aSize = m_aDataWin.GetOutputSizePixel();
        Size aLogSize = PixelToLogic( aSize, MAP_APPFONT );
        SfxDockingWindow::SetFloatingSize( aLogSize );

        m_aDataWin.Show();
    }


    DataNavigator::~DataNavigator()
    {
    }


    void DataNavigator::Update( FmFormShell* /*pFormShell*/ )
    {
    }

    void DataNavigator::StateChanged( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState )
    {
        if ( !pState  || SID_FM_DATANAVIGATOR_CONTROL != nSID )
            return;

        if ( eState >= SFX_ITEM_AVAILABLE )
        {
            FmFormShell* pShell = PTR_CAST( FmFormShell,((SfxObjectItem*)pState)->GetShell() );
            Update( pShell );
        }
        else
            Update( NULL );
    }


    void DataNavigator::GetFocus()
    {
        SfxDockingWindow::GetFocus();
    }


    bool DataNavigator::Close()
    {
        Update( NULL );
        return SfxDockingWindow::Close();
    }


    Size DataNavigator::CalcDockingSize( SfxChildAlignment eAlign )
    {
        if ( ( eAlign == SFX_ALIGN_TOP ) || ( eAlign == SFX_ALIGN_BOTTOM ) )
            return Size();

        return SfxDockingWindow::CalcDockingSize( eAlign );
    }


    SfxChildAlignment DataNavigator::CheckAlignment( SfxChildAlignment eActAlign, SfxChildAlignment eAlign )
    {
        switch ( eAlign )
        {
            case SFX_ALIGN_LEFT:
            case SFX_ALIGN_RIGHT:
            case SFX_ALIGN_NOALIGNMENT:
                return eAlign;
            default:
                break;
        }
        return eActAlign;
    }


    void DataNavigator::Resize()
    {
        SfxDockingWindow::Resize();

        Size aLogOutputSize = PixelToLogic( GetOutputSizePixel(), MAP_APPFONT );
        Size aLogExplSize = aLogOutputSize;
        aLogExplSize.Width() -= 2;
        aLogExplSize.Height() -= 2;

        Point aExplPos = LogicToPixel( Point(1,1), MAP_APPFONT );
        Size aExplSize = LogicToPixel( aLogExplSize, MAP_APPFONT );

        m_aDataWin.SetPosSizePixel( aExplPos, aExplSize );
    }



    // class NavigatorFrameManager



    SFX_IMPL_DOCKINGWINDOW( DataNavigatorManager, SID_FM_SHOW_DATANAVIGATOR )


    DataNavigatorManager::DataNavigatorManager(
        Window* _pParent, sal_uInt16 _nId, SfxBindings* _pBindings, SfxChildWinInfo* _pInfo ) :

        SfxChildWindow( _pParent, _nId )

    {
        pWindow = new DataNavigator( _pBindings, this, _pParent );
        eChildAlignment = SFX_ALIGN_RIGHT;
        pWindow->SetSizePixel( Size( 250, 400 ) );
        ( (SfxDockingWindow*)pWindow )->Initialize( _pInfo );
    }


    // class AddDataItemDialog


    AddDataItemDialog::AddDataItemDialog(Window* pParent, ItemNode* _pNode,
        const Reference< css::xforms::XFormsUIHelper1 >& _rUIHelper)
        : ModalDialog(pParent, "AddDataItemDialog" , "svx/ui/adddataitemdialog.ui")
        , m_xUIHelper(_rUIHelper)
        , m_pItemNode(_pNode)
        , m_eItemType(DITNone)
        , m_sFL_Element(SVX_RESSTR(RID_STR_ELEMENT))
        , m_sFL_Attribute(SVX_RESSTR(RID_STR_ATTRIBUTE))
        , m_sFL_Binding(SVX_RESSTR(RID_STR_BINDING))
        , m_sFT_BindingExp(SVX_RESSTR(RID_STR_BINDING_EXPR))
    {
        get(m_pItemFrame, "itemframe");
        get(m_pNameFT, "nameft");
        get(m_pNameED, "name");
        get(m_pDefaultFT, "valueft");
        get(m_pDefaultED, "value");
        get(m_pDefaultBtn, "browse");
        get(m_pSettingsFrame, "settingsframe");
        get(m_pDataTypeFT, "datatypeft");
        get(m_pDataTypeLB, "datatype");
        get(m_pRequiredCB, "required");
        get(m_pRequiredBtn, "requiredcond");
        get(m_pRelevantCB, "relevant");
        get(m_pRelevantBtn, "relevantcond");
        get(m_pConstraintCB, "constraint");
        get(m_pConstraintBtn, "constraintcond");
        get(m_pReadonlyCB, "readonly");
        get(m_pReadonlyBtn, "readonlycond");
        get(m_pCalculateCB, "calculate");
        get(m_pCalculateBtn, "calculatecond");
        get(m_pOKBtn, "ok");
        m_pDataTypeLB->SetDropDownLineCount( 10 );

        InitDialog();
        InitFromNode();
        InitDataTypeBox();
        CheckHdl( NULL );
    }


    AddDataItemDialog::~AddDataItemDialog()
    {
        if ( m_xTempBinding.is() )
        {
            Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
            if ( xModel.is() )
            {
                try
                {
                    Reference < XSet > xBindings = xModel->getBindings();
                    if ( xBindings.is() )
                        xBindings->remove( makeAny( m_xTempBinding ) );
                }
                catch (const Exception&)
                {
                    SAL_WARN( "svx.form", "AddDataItemDialog::Dtor(): exception caught" );
                }
            }
        }
        if( m_xUIHelper.is()  &&  m_xBinding.is() )
        {
            // remove binding, if it does not convey 'useful' information
            m_xUIHelper->removeBindingIfUseless( m_xBinding );
        }
    }


    IMPL_LINK( AddDataItemDialog, CheckHdl, CheckBox *, pBox )
    {
        // Condition buttons are only enable if their check box is checked
        m_pReadonlyBtn->Enable( m_pReadonlyCB->IsChecked() );
        m_pRequiredBtn->Enable( m_pRequiredCB->IsChecked() );
        m_pRelevantBtn->Enable( m_pRelevantCB->IsChecked() );
        m_pConstraintBtn->Enable( m_pConstraintCB->IsChecked() );
        m_pCalculateBtn->Enable( m_pCalculateCB->IsChecked() );

        if ( pBox && m_xTempBinding.is() )
        {
            OUString sTemp, sPropName;
            if ( m_pRequiredCB == pBox )
                sPropName = PN_REQUIRED_EXPR;
            else if ( m_pRelevantCB == pBox )
                sPropName = PN_RELEVANT_EXPR;
            else if ( m_pConstraintCB == pBox )
                sPropName = PN_CONSTRAINT_EXPR;
            else if ( m_pReadonlyCB == pBox )
                sPropName = PN_READONLY_EXPR;
            else if ( m_pCalculateCB == pBox )
                sPropName = PN_CALCULATE_EXPR;
            bool bIsChecked = pBox->IsChecked();
            m_xTempBinding->getPropertyValue( sPropName ) >>= sTemp;
            if ( bIsChecked && sTemp.isEmpty() )
                sTemp = TRUE_VALUE;
            else if ( !bIsChecked && !sTemp.isEmpty() )
                sTemp = OUString();
            m_xTempBinding->setPropertyValue( sPropName, makeAny( sTemp ) );
        }

        return 0;
    }


    IMPL_LINK( AddDataItemDialog, ConditionHdl, PushButton *, pBtn )
    {
        OUString sTemp, sPropName;
        if ( m_pDefaultBtn == pBtn )
            sPropName = PN_BINDING_EXPR;
        else if ( m_pRequiredBtn == pBtn )
            sPropName = PN_REQUIRED_EXPR;
        else if ( m_pRelevantBtn == pBtn )
            sPropName = PN_RELEVANT_EXPR;
        else if ( m_pConstraintBtn == pBtn )
            sPropName = PN_CONSTRAINT_EXPR;
        else if (m_pReadonlyBtn == pBtn)
            sPropName = PN_READONLY_EXPR;
        else if (m_pCalculateBtn == pBtn)
            sPropName = PN_CALCULATE_EXPR;
        AddConditionDialog aDlg( this, sPropName, m_xTempBinding );
        bool bIsDefBtn = ( m_pDefaultBtn == pBtn );
        OUString sCondition;
        if ( bIsDefBtn )
            sCondition = m_pDefaultED->GetText();
        else
        {
            m_xTempBinding->getPropertyValue( sPropName ) >>= sTemp;
            if ( sTemp.isEmpty() )
                sTemp = TRUE_VALUE;
            sCondition = sTemp;
        }
        aDlg.SetCondition( sCondition );

        if ( aDlg.Execute() == RET_OK )
        {
            OUString sNewCondition = aDlg.GetCondition();
            if ( bIsDefBtn )
                m_pDefaultED->SetText( sNewCondition );
            else
            {

                m_xTempBinding->setPropertyValue(
                    sPropName, makeAny( OUString( sNewCondition ) ) );
            }
        }
        return 0;
    }

    void copyPropSet( const Reference< XPropertySet >& xFrom, Reference< XPropertySet >& xTo )
    {
        DBG_ASSERT( xFrom.is(), "copyPropSet(): no source" );
        DBG_ASSERT( xTo.is(), "copyPropSet(): no target" );

        try
        {
            // get property names & infos, and iterate over target properties
            Sequence< Property > aProperties = xTo->getPropertySetInfo()->getProperties();
            sal_Int32 nProperties = aProperties.getLength();
            const Property* pProperties = aProperties.getConstArray();
            Reference< XPropertySetInfo > xFromInfo = xFrom->getPropertySetInfo();
            for ( sal_Int32 i = 0; i < nProperties; ++i )
            {
                const OUString& rName = pProperties[i].Name;

                // if both set have the property, copy the value
                // (catch and ignore exceptions, if any)
                if ( xFromInfo->hasPropertyByName( rName ) )
                {
                    // don't set readonly properties
                    Property aProperty = xFromInfo->getPropertyByName( rName );
                    if ( ( aProperty.Attributes & PropertyAttribute::READONLY ) == 0 )
                        xTo->setPropertyValue(rName, xFrom->getPropertyValue( rName ));
                }
                // else: no property? then ignore.
            }
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "copyPropSet(): exception caught" );
        }
    }


    IMPL_LINK_NOARG(AddDataItemDialog, OKHdl)
    {
        bool bIsHandleBinding = ( DITBinding == m_eItemType );
        bool bIsHandleText = ( DITText == m_eItemType );
        OUString sNewName( m_pNameED->GetText() );

        if ( ( !bIsHandleBinding && !bIsHandleText && !m_xUIHelper->isValidXMLName( sNewName ) ) ||
             ( bIsHandleBinding && sNewName.isEmpty() ) )
        {
            // Error and don't close the dialog
            ErrorBox aErrBox( this, SVX_RES( RID_ERR_INVALID_XMLNAME ) );
            OUString sMessText = aErrBox.GetMessText();
            sMessText = sMessText.replaceFirst( MSG_VARIABLE, sNewName );
            aErrBox.SetMessText( sMessText );
            aErrBox.Execute();
            return 0;
        }

        OUString sDataType( m_pDataTypeLB->GetSelectEntry() );
        m_xTempBinding->setPropertyValue( PN_BINDING_TYPE, makeAny( sDataType ) );

        if ( bIsHandleBinding )
        {
            // copy properties from temp binding to original binding
            copyPropSet( m_xTempBinding, m_pItemNode->m_xPropSet );
            try
            {
                OUString sValue = m_pNameED->GetText();
                m_pItemNode->m_xPropSet->setPropertyValue( PN_BINDING_ID, makeAny( sValue ) );
                sValue = m_pDefaultED->GetText();
                m_pItemNode->m_xPropSet->setPropertyValue( PN_BINDING_EXPR, makeAny( sValue ) );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddDataDialog::OKHdl(): exception caught" );
            }
        }
        else
        {
            // copy properties from temp binding to original binding
            copyPropSet( m_xTempBinding, m_xBinding );
            try
            {
                if ( bIsHandleText )
                    m_xUIHelper->setNodeValue( m_pItemNode->m_xNode, m_pDefaultED->GetText() );
                else
                {
                    Reference< css::xml::dom::XNode > xNewNode =
                        m_xUIHelper->renameNode( m_pItemNode->m_xNode, m_pNameED->GetText() );
                    m_xUIHelper->setNodeValue( xNewNode, m_pDefaultED->GetText() );
                    m_pItemNode->m_xNode = xNewNode;
                }
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddDataDialog::OKHdl(): exception caught" );
            }
        }
        // then close the dialog
        EndDialog( RET_OK );
        return 0;
    }


    void AddDataItemDialog::InitDialog()
    {
        // set handler
        Link aLink = LINK( this, AddDataItemDialog, CheckHdl );
        m_pRequiredCB->SetClickHdl( aLink );
        m_pRelevantCB->SetClickHdl( aLink );
        m_pConstraintCB->SetClickHdl( aLink );
        m_pReadonlyCB->SetClickHdl( aLink );
        m_pCalculateCB->SetClickHdl( aLink );

        aLink = LINK( this, AddDataItemDialog, ConditionHdl );
        m_pDefaultBtn->SetClickHdl( aLink );
        m_pRequiredBtn->SetClickHdl( aLink );
        m_pRelevantBtn->SetClickHdl( aLink );
        m_pConstraintBtn->SetClickHdl( aLink );
        m_pReadonlyBtn->SetClickHdl( aLink );
        m_pCalculateBtn->SetClickHdl( aLink );

        m_pOKBtn->SetClickHdl( LINK( this, AddDataItemDialog, OKHdl ) );
    }


    void AddDataItemDialog::InitFromNode()
    {
        if ( m_pItemNode )
        {
            if ( m_pItemNode->m_xNode.is() )
            {
                try
                {
                    // detect type of the node
                    css::xml::dom::NodeType eChildType = m_pItemNode->m_xNode->getNodeType();
                    switch ( eChildType )
                    {
                        case css::xml::dom::NodeType_ATTRIBUTE_NODE:
                            m_eItemType = DITAttribute;
                            break;
                        case css::xml::dom::NodeType_ELEMENT_NODE:
                            m_eItemType = DITElement;
                            break;
                        case css::xml::dom::NodeType_TEXT_NODE:
                            m_eItemType = DITText;
                            break;
                        default:
                            OSL_FAIL( "AddDataItemDialog::InitFronNode: cannot handle this node type!" );
                            break;
                    }

                    /** Get binding of the node and clone it
                        Then use this temporary binding in the dialog.
                        When the user click OK the temporary binding will be copied
                        into the original binding.
                     */

                    Reference< css::xml::dom::XNode > xNode = m_pItemNode->m_xNode;
                    m_xBinding = m_xUIHelper->getBindingForNode( xNode, sal_True );
                    if ( m_xBinding.is() )
                    {
                        Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
                        if ( xModel.is() )
                        {
                            m_xTempBinding = m_xUIHelper->cloneBindingAsGhost( m_xBinding );
                            Reference < XSet > xBindings = xModel->getBindings();
                            if ( xBindings.is() )
                                xBindings->insert( makeAny( m_xTempBinding ) );
                        }
                    }

                    if ( m_eItemType != DITText )
                    {
                        OUString sName( m_xUIHelper->getNodeName( m_pItemNode->m_xNode ) );
                        m_pNameED->SetText( sName );
                    }
                    m_pDefaultED->SetText( m_pItemNode->m_xNode->getNodeValue() );
                }
                catch( Exception& )
                {
                    SAL_WARN( "svx.form", "AddDataItemDialog::InitFromNode(): exception caught" );
                }
            }
            else if ( m_pItemNode->m_xPropSet.is() )
            {
                m_eItemType = DITBinding;
                Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
                if ( xModel.is() )
                {
                    try
                    {
                        m_xTempBinding = m_xUIHelper->cloneBindingAsGhost( m_pItemNode->m_xPropSet );
                        Reference < XSet > xBindings = xModel->getBindings();
                        if ( xBindings.is() )
                            xBindings->insert( makeAny( m_xTempBinding ) );
                    }
                    catch ( Exception& )
                    {
                        SAL_WARN( "svx.form", "AddDataItemDialog::InitFromNode(): exception caught" );
                    }
                }
                OUString sTemp;
                try
                {
                    Reference< XPropertySetInfo > xInfo = m_pItemNode->m_xPropSet->getPropertySetInfo();
                    if ( xInfo->hasPropertyByName( PN_BINDING_ID ) )
                    {
                        m_pItemNode->m_xPropSet->getPropertyValue( PN_BINDING_ID ) >>= sTemp;
                        m_pNameED->SetText( sTemp );
                        m_pItemNode->m_xPropSet->getPropertyValue( PN_BINDING_EXPR ) >>= sTemp;
                        m_pDefaultED->SetText( sTemp );
                    }
                    else if ( xInfo->hasPropertyByName( PN_SUBMISSION_BIND ) )
                    {
                        m_pItemNode->m_xPropSet->getPropertyValue( PN_SUBMISSION_ID ) >>= sTemp;
                        m_pNameED->SetText( sTemp );
                    }
                }
                catch( Exception& )
                {
                    SAL_WARN( "svx.form", "AddDataItemDialog::InitFromNode(): exception caught" );
                }

                Size a3and1Sz = LogicToPixel( Size( 3, 1 ), MAP_APPFONT );
                Size aNewSz = m_pDefaultED->GetSizePixel();
                Point aNewPnt = m_pDefaultED->GetPosPixel();
                aNewPnt.Y() += a3and1Sz.Height();
                aNewSz.Width() -= ( m_pDefaultBtn->GetSizePixel().Width() + a3and1Sz.Width() );
                m_pDefaultED->SetPosSizePixel( aNewPnt, aNewSz );
                m_pDefaultBtn->Show();
            }

            if ( m_xTempBinding.is() )
            {
                OUString sTemp;
                try
                {
                    if ( ( m_xTempBinding->getPropertyValue( PN_REQUIRED_EXPR ) >>= sTemp )
                        && !sTemp.isEmpty() )
                        m_pRequiredCB->Check( true );
                    if ( ( m_xTempBinding->getPropertyValue( PN_RELEVANT_EXPR ) >>= sTemp )
                        && !sTemp.isEmpty() )
                        m_pRelevantCB->Check( true );
                    if ( ( m_xTempBinding->getPropertyValue( PN_CONSTRAINT_EXPR ) >>= sTemp )
                        && !sTemp.isEmpty() )
                        m_pConstraintCB->Check( true );
                    if ( ( m_xTempBinding->getPropertyValue( PN_READONLY_EXPR ) >>= sTemp )
                        && !sTemp.isEmpty() )
                        m_pReadonlyCB->Check( true );
                    if ( ( m_xTempBinding->getPropertyValue( PN_CALCULATE_EXPR ) >>= sTemp )
                        && !sTemp.isEmpty() )
                        m_pCalculateCB->Check( true );
                }
                catch (const Exception&)
                {
                    SAL_WARN( "svx.form", "AddDataItemDialog::InitFromNode(): exception caught" );
                }
            }
        }

        if ( DITText == m_eItemType )
        {
            m_pSettingsFrame->Hide();
            m_pNameFT->Disable();
            m_pNameED->Disable();
        }
    }


    void AddDataItemDialog::InitDataTypeBox()
    {
        if ( m_eItemType != DITText )
        {
            Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
            if ( xModel.is() )
            {
                try
                {
                    Reference< css::xforms::XDataTypeRepository > xDataTypes =
                        xModel->getDataTypeRepository();
                    if ( xDataTypes.is() )
                    {
                        Sequence< OUString > aNameList = xDataTypes->getElementNames();
                        sal_Int32 i, nCount = aNameList.getLength();
                        OUString* pNames = aNameList.getArray();
                        for ( i = 0; i < nCount; ++i )
                            m_pDataTypeLB->InsertEntry( pNames[i] );
                    }

                    if ( m_xTempBinding.is() )
                    {
                        OUString sTemp;
                        if ( m_xTempBinding->getPropertyValue( PN_BINDING_TYPE ) >>= sTemp )
                        {
                            sal_Int32 nPos = m_pDataTypeLB->GetEntryPos( sTemp );
                            if ( LISTBOX_ENTRY_NOTFOUND == nPos )
                                nPos = m_pDataTypeLB->InsertEntry( sTemp );
                            m_pDataTypeLB->SelectEntryPos( nPos );
                        }
                    }
                }
                catch ( Exception& )
                {
                    SAL_WARN( "svx.form", "AddDataItemDialog::InitDataTypeBox(): exception caught" );
                }
            }
        }
    }

    void AddDataItemDialog::InitText( DataItemType _eType )
    {
        OUString sText;

        switch ( _eType )
        {
            case DITAttribute :
            {
                sText = m_sFL_Attribute;
                break;
            }

            case DITBinding :
            {
                sText = m_sFL_Binding;
                m_pDefaultFT->SetText( m_sFT_BindingExp );
                break;
            }

            default:
            {
                sText = m_sFL_Element;
            }
        }

        m_pItemFrame->set_label(sText);
    }


    // class AddConditionDialog


    AddConditionDialog::AddConditionDialog(Window* pParent,
        const OUString& _rPropertyName,
        const Reference< XPropertySet >& _rPropSet)
        : ModalDialog(pParent, "AddConditionDialog", "svx/ui/addconditiondialog.ui")
        , m_sPropertyName(_rPropertyName)
        , m_xBinding(_rPropSet)

    {
        get(m_pConditionED, "condition");
        get(m_pResultWin, "result");
        get(m_pEditNamespacesBtn, "edit");
        get(m_pOKBtn, "ok");
        DBG_ASSERT( m_xBinding.is(), "AddConditionDialog::Ctor(): no Binding" );

        m_pConditionED->set_height_request(m_pConditionED->GetTextHeight() * 4);
        m_pConditionED->set_width_request(m_pConditionED->approximate_char_width() * 62);
        m_pResultWin->set_height_request(m_pResultWin->GetTextHeight() * 4);
        m_pResultWin->set_width_request(m_pResultWin->approximate_char_width() * 62);

        m_pConditionED->SetModifyHdl( LINK( this, AddConditionDialog, ModifyHdl ) );
        m_pEditNamespacesBtn->SetClickHdl( LINK( this, AddConditionDialog, EditHdl ) );
        m_pOKBtn->SetClickHdl( LINK( this, AddConditionDialog, OKHdl ) );
        m_aResultTimer.SetTimeout( 500 );
        m_aResultTimer.SetTimeoutHdl( LINK( this, AddConditionDialog, ResultHdl ) );

        if ( !m_sPropertyName.isEmpty() )
        {
            try
            {
                OUString sTemp;
                if ( ( m_xBinding->getPropertyValue( m_sPropertyName ) >>= sTemp )
                    && !sTemp.isEmpty() )
                {
                    m_pConditionED->SetText( sTemp );
                }
                else
                {
//!                 m_xBinding->setPropertyValue( m_sPropertyName, makeAny( TRUE_VALUE ) );
                    m_pConditionED->SetText( TRUE_VALUE );
                }

                Reference< css::xforms::XModel > xModel;
                if ( ( m_xBinding->getPropertyValue( PN_BINDING_MODEL ) >>= xModel ) && xModel.is() )
                    m_xUIHelper = Reference< css::xforms::XFormsUIHelper1 >( xModel, UNO_QUERY );
            }
            catch (const Exception&)
            {
                SAL_WARN( "svx.form", "AddConditionDialog::Ctor(): exception caught" );
            }
        }

        DBG_ASSERT( m_xUIHelper.is(), "AddConditionDialog::Ctor(): no UIHelper" );
        ResultHdl( &m_aResultTimer );
    }


    IMPL_LINK_NOARG(AddConditionDialog, EditHdl)
    {
        Reference< XNameContainer > xNameContnr;
        try
        {
            m_xBinding->getPropertyValue( PN_BINDING_NAMESPACES ) >>= xNameContnr;
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "AddDataItemDialog::EditHdl(): exception caught" );
        }
        NamespaceItemDialog aDlg( this, xNameContnr );
        aDlg.Execute();
        try
        {
            m_xBinding->setPropertyValue( PN_BINDING_NAMESPACES, makeAny( xNameContnr ) );
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "AddDataItemDialog::EditHdl(): exception caught" );
        }
        return 0;
    }


    IMPL_LINK_NOARG(AddConditionDialog, OKHdl)
    {
/*!!!
        try
        {
            if ( m_xBinding.is() )
                m_xBinding->setPropertyValue( m_sPropertyName, makeAny( OUString( m_pConditionED->GetText() ) ) );
        }
        catch( const Exception& )
        {
            SAL_WARN( "svx.form", "AddConditionDialog, OKHdl: caught an exception!" );
        }
*/
        EndDialog( RET_OK );
        return 0;
    }


    IMPL_LINK_NOARG(AddConditionDialog, ModifyHdl)
    {
        m_aResultTimer.Start();
        return 0;
    }


    IMPL_LINK_NOARG(AddConditionDialog, ResultHdl)
    {
        OUString sCondition = comphelper::string::strip(m_pConditionED->GetText(), ' ');
        OUString sResult;
        if ( !sCondition.isEmpty() )
        {
            try
            {
                sResult = m_xUIHelper->getResultForExpression( m_xBinding, ( m_sPropertyName == PN_BINDING_EXPR ), sCondition );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddConditionDialog::ResultHdl(): exception caught" );
            }
        }
        m_pResultWin->SetText( sResult );
        return 0;
    }


    // class NamespaceItemDialog


    NamespaceItemDialog::NamespaceItemDialog(
        AddConditionDialog* _pCondDlg,
            Reference< XNameContainer >& _rContainer )
        : ModalDialog( _pCondDlg, "NamespaceDialog",
            "svx/ui/namespacedialog.ui" )
        , m_pConditionDlg(_pCondDlg)
        , m_rNamespaces(_rContainer)
    {
        get(m_pAddNamespaceBtn, "add");
        get(m_pEditNamespaceBtn, "edit");
        get(m_pDeleteNamespaceBtn, "delete");
        get(m_pOKBtn, "ok");

        SvSimpleTableContainer* pNamespacesListContainer =
            get<SvSimpleTableContainer>("namespaces");
        Size aControlSize(175, 72);
        aControlSize = LogicToPixel(aControlSize, MAP_APPFONT);
        pNamespacesListContainer->set_width_request(aControlSize.Width());
        pNamespacesListContainer->set_height_request(aControlSize.Height());
        m_pNamespacesList = new SvSimpleTable(*pNamespacesListContainer, 0);

        static long aStaticTabs[]= { 3, 0, 35, 200 };
        m_pNamespacesList->SvSimpleTable::SetTabs( aStaticTabs );
        OUString sHeader = get<FixedText>("prefix")->GetText();
        sHeader += "\t";
        sHeader += get<FixedText>("url")->GetText();
        m_pNamespacesList->InsertHeaderEntry(
            sHeader, HEADERBAR_APPEND, HIB_LEFT /*| HIB_FIXEDPOS | HIB_FIXED*/ );

        m_pNamespacesList->SetSelectHdl( LINK( this, NamespaceItemDialog, SelectHdl ) );
        Link aLink = LINK( this, NamespaceItemDialog, ClickHdl );
        m_pAddNamespaceBtn->SetClickHdl( aLink );
        m_pEditNamespaceBtn->SetClickHdl( aLink );
        m_pDeleteNamespaceBtn->SetClickHdl( aLink );
        m_pOKBtn->SetClickHdl( LINK( this, NamespaceItemDialog, OKHdl ) );

        LoadNamespaces();
        SelectHdl( m_pNamespacesList );
    }


    NamespaceItemDialog::~NamespaceItemDialog()
    {
        delete m_pNamespacesList;
    }


    IMPL_LINK( NamespaceItemDialog, SelectHdl, SvSimpleTable *,  EMPTYARG )
    {
        bool bEnable = ( m_pNamespacesList->FirstSelected() != NULL );
        m_pEditNamespaceBtn->Enable( bEnable );
        m_pDeleteNamespaceBtn->Enable( bEnable );

        return 0;
    }


    IMPL_LINK( NamespaceItemDialog, ClickHdl, PushButton *, pBtn )
    {
        if ( m_pAddNamespaceBtn == pBtn )
        {
            ManageNamespaceDialog aDlg( this, m_pConditionDlg, false );
            if ( aDlg.Execute() == RET_OK )
            {
                OUString sEntry = aDlg.GetPrefix();
                sEntry += "\t";
                sEntry += aDlg.GetURL();
                m_pNamespacesList->InsertEntry( sEntry );
            }
        }
        else if ( m_pEditNamespaceBtn == pBtn )
        {
            ManageNamespaceDialog aDlg( this, m_pConditionDlg, true );
            SvTreeListEntry* pEntry = m_pNamespacesList->FirstSelected();
            DBG_ASSERT( pEntry, "NamespaceItemDialog::ClickHdl(): no entry" );
            OUString sPrefix( m_pNamespacesList->GetEntryText( pEntry, 0 ) );
            aDlg.SetNamespace(
                sPrefix,
                m_pNamespacesList->GetEntryText( pEntry, 1 ) );
            if ( aDlg.Execute() == RET_OK )
            {
                // if a prefix was changed, mark the old prefix as 'removed'
                if( sPrefix != aDlg.GetPrefix() )
                    m_aRemovedList.push_back( sPrefix );

                m_pNamespacesList->SetEntryText( aDlg.GetPrefix(), pEntry, 0 );
                m_pNamespacesList->SetEntryText( aDlg.GetURL(), pEntry, 1 );
            }
        }
        else if ( m_pDeleteNamespaceBtn == pBtn )
        {
            SvTreeListEntry* pEntry = m_pNamespacesList->FirstSelected();
            DBG_ASSERT( pEntry, "NamespaceItemDialog::ClickHdl(): no entry" );
            OUString sPrefix( m_pNamespacesList->GetEntryText( pEntry, 0 ) );
            m_aRemovedList.push_back( sPrefix );
            m_pNamespacesList->GetModel()->Remove( pEntry );
        }
        else
        {
            SAL_WARN( "svx.form", "NamespaceItemDialog::ClickHdl(): invalid button" );
        }

        SelectHdl( m_pNamespacesList );
        return 0;
    }


    IMPL_LINK_NOARG(NamespaceItemDialog, OKHdl)
    {
        try
        {
            // update namespace container
            sal_Int32 i, nRemovedCount = m_aRemovedList.size();
            for( i = 0; i < nRemovedCount; ++i )
                m_rNamespaces->removeByName( m_aRemovedList[i] );

            sal_Int32 nEntryCount = m_pNamespacesList->GetEntryCount();
            for( i = 0; i < nEntryCount; ++i )
            {
                SvTreeListEntry* pEntry = m_pNamespacesList->GetEntry(i);
                OUString sPrefix( m_pNamespacesList->GetEntryText( pEntry, 0 ) );
                OUString sURL( m_pNamespacesList->GetEntryText( pEntry, 1 ) );

                if ( m_rNamespaces->hasByName( sPrefix ) )
                    m_rNamespaces->replaceByName( sPrefix, makeAny( sURL ) );
                else
                    m_rNamespaces->insertByName( sPrefix, makeAny( sURL ) );
            }
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "NamespaceItemDialog::OKHdl(): exception caught" );
        }
        // and close the dialog
        EndDialog( RET_OK );
        return 0;
    }


    void NamespaceItemDialog::LoadNamespaces()
    {
        try
        {
            Sequence< OUString > aAllNames = m_rNamespaces->getElementNames();
            const OUString* pAllNames = aAllNames.getConstArray();
            const OUString* pAllNamesEnd = pAllNames + aAllNames.getLength();
            for ( ; pAllNames != pAllNamesEnd; ++pAllNames )
            {
                OUString sURL;
                OUString sPrefix = *pAllNames;
                if ( m_rNamespaces->hasByName( sPrefix ) )
                {
                    Any aAny = m_rNamespaces->getByName( sPrefix );
                    if ( aAny >>= sURL )
                    {
                        OUString sEntry( sPrefix );
                        sEntry += "\t";
                        sEntry += sURL;

                        m_pNamespacesList->InsertEntry( sEntry );
                    }
                }
            }
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "NamespaceItemDialog::LoadNamespaces(): exception caught" );
        }
    }


    // class ManageNamespaceDialog


    ManageNamespaceDialog::ManageNamespaceDialog(Window* pParent, AddConditionDialog* _pCondDlg, bool bIsEdit)
        : ModalDialog(pParent, "AddNamespaceDialog", "svx/ui/addnamespacedialog.ui")
        , m_pConditionDlg ( _pCondDlg )
    {
        get(m_pOKBtn, "ok");
        get(m_pPrefixED, "prefix");
        get(m_pUrlED, "url");

        if (bIsEdit)
            SetText(get<FixedText>("alttitle")->GetText());

        m_pOKBtn->SetClickHdl( LINK( this, ManageNamespaceDialog, OKHdl ) );
    }


    IMPL_LINK_NOARG(ManageNamespaceDialog, OKHdl)
    {
        OUString sPrefix = m_pPrefixED->GetText();

        try
        {
            if ( !m_pConditionDlg->GetUIHelper()->isValidPrefixName( sPrefix ) )
            {
                ErrorBox aErrBox( this, SVX_RES( RID_ERR_INVALID_XMLPREFIX ) );
                OUString sMessText = aErrBox.GetMessText();
                sMessText = sMessText.replaceFirst( MSG_VARIABLE, sPrefix );
                aErrBox.SetMessText( sMessText );
                aErrBox.Execute();
                return 0;
            }
        }
        catch ( Exception& )
        {
            SAL_WARN( "svx.form", "ManageNamespacesDialog::OKHdl(): exception caught" );
        }

        // no error so close the dialog
        EndDialog( RET_OK );
        return 0;
    }


    // class AddSubmissionDialog


    AddSubmissionDialog::AddSubmissionDialog(
        Window* pParent, ItemNode* _pNode,
        const Reference< css::xforms::XFormsUIHelper1 >& _rUIHelper)
        : ModalDialog(pParent, "AddSubmissionDialog",
            "svx/ui/addsubmissiondialog.ui")
        , m_pItemNode(_pNode)
        , m_xUIHelper(_rUIHelper)
    {
        get(m_pNameED, "name");
        get(m_pActionED, "action");
        get(m_pMethodLB, "method");
        get(m_pRefED, "expression");
        get(m_pRefBtn, "browse");
        get(m_pBindLB, "binding");
        get(m_pReplaceLB, "replace");
        get(m_pOKBtn, "ok");
        FillAllBoxes();

        m_pRefBtn->SetClickHdl( LINK( this, AddSubmissionDialog, RefHdl ) );
        m_pOKBtn->SetClickHdl( LINK( this, AddSubmissionDialog, OKHdl ) );
    }


    AddSubmissionDialog::~AddSubmissionDialog()
    {
        // #i38991# if we have added a binding, we need to remove it as well.
        if( m_xCreatedBinding.is() && m_xUIHelper.is() )
            m_xUIHelper->removeBindingIfUseless( m_xCreatedBinding );
    }


    IMPL_LINK_NOARG(AddSubmissionDialog, RefHdl)
    {
        AddConditionDialog aDlg( this, PN_BINDING_EXPR, m_xTempBinding );
        aDlg.SetCondition( m_pRefED->GetText() );
        if ( aDlg.Execute() == RET_OK )
            m_pRefED->SetText( aDlg.GetCondition() );

        return 0;
    }


    IMPL_LINK_NOARG(AddSubmissionDialog, OKHdl)
    {
        OUString sName(m_pNameED->GetText());
        if(sName.isEmpty()) {

            ErrorBox aErrorBox(this,SVX_RES(RID_ERR_EMPTY_SUBMISSIONNAME));
            aErrorBox.SetText( Application::GetDisplayName() );
            aErrorBox.Execute();
            return 0;
        }

        if ( !m_xSubmission.is() )
        {
            DBG_ASSERT( !m_xNewSubmission.is(),
                "AddSubmissionDialog::OKHdl(): new submission already exists" );

            // add a new submission
            Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
            if ( xModel.is() )
            {
                try
                {
                    m_xNewSubmission = xModel->createSubmission();
                    m_xSubmission = Reference< XPropertySet >( m_xNewSubmission, UNO_QUERY );
                }
                catch ( Exception& )
                {
                    SAL_WARN( "svx.form", "AddSubmissionDialog::OKHdl(): exception caught" );
                }
            }
        }

        if ( m_xSubmission.is() )
        {
            OUString sTemp = m_pNameED->GetText();
            try
            {
                m_xSubmission->setPropertyValue( PN_SUBMISSION_ID, makeAny( sTemp ) );
                sTemp = m_pActionED->GetText();
                m_xSubmission->setPropertyValue( PN_SUBMISSION_ACTION, makeAny( sTemp ) );
                sTemp = m_aMethodString.toAPI( m_pMethodLB->GetSelectEntry() );
                m_xSubmission->setPropertyValue( PN_SUBMISSION_METHOD, makeAny( sTemp ) );
                sTemp = m_pRefED->GetText();
                m_xSubmission->setPropertyValue( PN_SUBMISSION_REF, makeAny( sTemp ) );
                OUString sEntry = m_pBindLB->GetSelectEntry();
                sal_Int32 nColonIdx = sEntry.indexOf(':');
                if (nColonIdx != -1)
                    sEntry = sEntry.copy(0, nColonIdx);
                sTemp = sEntry;
                m_xSubmission->setPropertyValue( PN_SUBMISSION_BIND, makeAny( sTemp ) );
                sTemp = m_aReplaceString.toAPI( m_pReplaceLB->GetSelectEntry() );
                m_xSubmission->setPropertyValue( PN_SUBMISSION_REPLACE, makeAny( sTemp ) );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddSubmissionDialog::OKHdl(): exception caught" );
            }
        }

        EndDialog( RET_OK );
        return 0;
    }


    void AddSubmissionDialog::FillAllBoxes()
    {
        // method box
        m_pMethodLB->InsertEntry( SVX_RESSTR( RID_STR_METHOD_POST   ) );
        m_pMethodLB->InsertEntry( SVX_RESSTR( RID_STR_METHOD_PUT ) );
        m_pMethodLB->InsertEntry( SVX_RESSTR( RID_STR_METHOD_GET ) );
        m_pMethodLB->SelectEntryPos(0);

        // binding box
        Reference< css::xforms::XModel > xModel( m_xUIHelper, UNO_QUERY );
        if ( xModel.is() )
        {
            try
            {
                Reference< XEnumerationAccess > xNumAccess( xModel->getBindings(), UNO_QUERY );
                if ( xNumAccess.is() )
                {
                    Reference < XEnumeration > xNum = xNumAccess->createEnumeration();
                    if ( xNum.is() && xNum->hasMoreElements() )
                    {
                        OUString sDelim( ": " );
                        while ( xNum->hasMoreElements() )
                        {
                            Reference< XPropertySet > xPropSet;
                            Any aAny = xNum->nextElement();
                            if ( aAny >>= xPropSet )
                            {
                                OUString sEntry;
                                OUString sTemp;
                                xPropSet->getPropertyValue( PN_BINDING_ID ) >>= sTemp;
                                sEntry += sTemp;
                                sEntry += sDelim;
                                xPropSet->getPropertyValue( PN_BINDING_EXPR ) >>= sTemp;
                                sEntry += sTemp;
                                m_pBindLB->InsertEntry( sEntry );

                                if ( !m_xTempBinding.is() )
                                    m_xTempBinding = xPropSet;
                            }
                        }
                    }
                }
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddSubmissionDialog::FillAllBoxes(): exception caught" );
            }
        }

        // #i36342# we need a temporary binding; create one if no existing binding
        // is found
        if( !m_xTempBinding.is() )
        {
            m_xCreatedBinding = m_xUIHelper->getBindingForNode(
                Reference<css::xml::dom::XNode>(
                    xModel->getDefaultInstance()->getDocumentElement(),
                    UNO_QUERY_THROW ),
                sal_True );
            m_xTempBinding = m_xCreatedBinding;
        }

        // replace box
        m_pReplaceLB->InsertEntry( SVX_RESSTR( RID_STR_REPLACE_NONE ) );
        m_pReplaceLB->InsertEntry( SVX_RESSTR( RID_STR_REPLACE_INST ) );
        m_pReplaceLB->InsertEntry( SVX_RESSTR( RID_STR_REPLACE_DOC ) );


        // init the controls with the values of the submission
        if ( m_pItemNode && m_pItemNode->m_xPropSet.is() )
        {
            m_xSubmission = m_pItemNode->m_xPropSet;
            OUString sTemp;
            try
            {
                m_xSubmission->getPropertyValue( PN_SUBMISSION_ID ) >>= sTemp;
                m_pNameED->SetText( sTemp );
                m_xSubmission->getPropertyValue( PN_SUBMISSION_ACTION ) >>= sTemp;
                m_pActionED->SetText( sTemp );
                m_xSubmission->getPropertyValue( PN_SUBMISSION_REF ) >>= sTemp;
                m_pRefED->SetText( sTemp );

                m_xSubmission->getPropertyValue( PN_SUBMISSION_METHOD ) >>= sTemp;
                sTemp = m_aMethodString.toUI( sTemp );
                sal_Int32 nPos = m_pMethodLB->GetEntryPos( sTemp );
                if ( LISTBOX_ENTRY_NOTFOUND == nPos )
                    nPos = m_pMethodLB->InsertEntry( sTemp );
                m_pMethodLB->SelectEntryPos( nPos );

                m_xSubmission->getPropertyValue( PN_SUBMISSION_BIND ) >>= sTemp;
                nPos = m_pBindLB->GetEntryPos( sTemp );
                if ( LISTBOX_ENTRY_NOTFOUND == nPos )
                    nPos = m_pBindLB->InsertEntry( sTemp );
                m_pBindLB->SelectEntryPos( nPos );

                m_xSubmission->getPropertyValue( PN_SUBMISSION_REPLACE ) >>= sTemp;
                sTemp = m_aReplaceString.toUI( sTemp );
                if ( sTemp.isEmpty() )
                    sTemp = m_pReplaceLB->GetEntry(0); // first entry == "none"
                nPos = m_pReplaceLB->GetEntryPos( sTemp );
                if ( LISTBOX_ENTRY_NOTFOUND == nPos )
                    nPos = m_pReplaceLB->InsertEntry( sTemp );
                m_pReplaceLB->SelectEntryPos( nPos );
            }
            catch ( Exception& )
            {
                SAL_WARN( "svx.form", "AddSubmissionDialog::FillAllBoxes(): exception caught" );
            }
        }

        m_pRefBtn->Enable( m_xTempBinding.is() );
    }


    // class AddModelDialog


    AddModelDialog::AddModelDialog(Window* pParent, bool bIsEdit)
        : ModalDialog(pParent, "AddModelDialog", "svx/ui/addmodeldialog.ui")
    {
        get(m_pNameED, "name");
        get(m_pModifyCB, "modify");

        if (bIsEdit)
            SetText(get<FixedText>("alttitle")->GetText());
    }


    // class AddInstanceDialog


    AddInstanceDialog::AddInstanceDialog(Window* pParent, bool _bEdit)
        : ModalDialog(pParent, "AddInstanceDialog" , "svx/ui/addinstancedialog.ui")
    {
        get(m_pNameED, "name");
        get(m_pURLFT, "urlft");
        get(m_pURLED, "url");
        get(m_pFilePickerBtn, "browse");
        get(m_pLinkInstanceCB, "link");

        if ( _bEdit )
            SetText(get<FixedText>("alttitle")->GetText());

        m_pURLED->DisableHistory();
        m_pFilePickerBtn->SetClickHdl( LINK( this, AddInstanceDialog, FilePickerHdl ) );

        // load the filter name from fps_office resource
        m_sAllFilterName = ResId(STR_FILTERNAME_ALL, *ResMgr::CreateResMgr("fps_office")).toString();
    }


    IMPL_LINK_NOARG(AddInstanceDialog, FilePickerHdl)
    {
        ::sfx2::FileDialogHelper aDlg(
            css::ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE, 0 );
        INetURLObject aFile( SvtPathOptions().GetWorkPath() );

        aDlg.AddFilter( m_sAllFilterName, OUString(FILEDIALOG_FILTER_ALL) );
        OUString sFilterName( "XML" );
        aDlg.AddFilter( sFilterName, "*.xml" );
        aDlg.SetCurrentFilter( sFilterName );
        aDlg.SetDisplayDirectory( aFile.GetMainURL( INetURLObject::NO_DECODE ) );

        if( aDlg.Execute() == ERRCODE_NONE )
            m_pURLED->SetText( aDlg.GetPath() );

        return 0;
    }


    // class LinkedInstanceWarningBox


    LinkedInstanceWarningBox::LinkedInstanceWarningBox( Window* pParent ) :

        MessBox( pParent, SVX_RES( RID_QRY_LINK_WARNING ) )

    {
        SetText( Application::GetDisplayName() );
        SetImage( QueryBox::GetStandardImage() );
        AddButton( SVX_RESSTR( RID_STR_DATANAV_LINKWARN_BUTTON ), RET_OK, BUTTONDIALOG_DEFBUTTON );
        AddButton( BUTTON_CANCEL, RET_CANCEL, BUTTONDIALOG_CANCELBUTTON );
    }


}   // namespace svxform



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */