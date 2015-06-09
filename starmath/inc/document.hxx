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

#ifndef INCLUDED_STARMATH_INC_DOCUMENT_HXX
#define INCLUDED_STARMATH_INC_DOCUMENT_HXX

#define SMDLL   1

#include <rtl/ustring.hxx>
#include <rtl/strbuf.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/objsh.hxx>
#include <sot/storage.hxx>
#include <svl/lstner.hxx>
#include <vcl/jobset.hxx>
#include <vcl/virdev.hxx>
#include <sax/fshelper.hxx>
#include <oox/core/filterbase.hxx>
#include <oox/mathml/import.hxx>

#include <set>

#include "format.hxx"
#include "parse.hxx"
#include "smmod.hxx"

class SmNode;
class SfxMenuBarManager;
class SfxPrinter;
class Printer;
class SmCursor;

#define STAROFFICE_XML  "StarOffice XML (Math)"
#define MATHML_XML      "MathML XML (Math)"

/* Access to printer should happen through this class only
 * ==========================================================================
 *
 * The printer can belong to the document or the OLE-Container. If the document
 * is an OLE-Document the printer generally belongs to the container too.
 * But the container maybe works with a different MapUnit than the server.
 * Referring to the MapMode the printer will be accordingly adjusted in the
 * constructor and restored in the destructor. This brings that this class
 * is always allowed to exists only a short time (e.g. while painting).
 * The control whether the printer is self-generated, gotten from the server
 * or is NULL then, is taken by the DocShell in the method GetPrt(), for
 * which the access is friend of the DocShell too.
 */

class SmDocShell;
class EditEngine;



class SmPrinterAccess
{
    Printer* pPrinter;
    OutputDevice* pRefDev;
public:
    SmPrinterAccess( SmDocShell &rDocShell );
    ~SmPrinterAccess();
    Printer* GetPrinter()  { return pPrinter; }
    OutputDevice* GetRefDev()  { return pRefDev; }
};




void SetEditEngineDefaultFonts(SfxItemPool &rEditEngineItemPool);



class SmDocShell : public SfxObjectShell, public SfxListener
{
    friend class SmPrinterAccess;
    friend class SmModel;
    friend class SmCursor;

    OUString            aText;
    SmFormat            aFormat;
    SmParser            aInterpreter;
    OUString            aAccText;
    SmNode             *pTree;
    SfxItemPool        *pEditEngineItemPool;
    EditEngine         *pEditEngine;
    SfxPrinter         *pPrinter;       //q.v. comment to SmPrinter Access!
    Printer            *pTmpPrinter;    //ditto
    sal_uInt16          nModifyCount;
    bool                bIsFormulaArranged;
    SmCursor           *pCursor;
    std::set< OUString >    aUsedSymbols;   // to export used symbols only when saving



    virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType,
                        const SfxHint& rHint, const TypeId& rHintType) SAL_OVERRIDE;

    bool        WriteAsMathType3( SfxMedium& );

    virtual void        Draw(OutputDevice *pDevice,
                             const JobSetup & rSetup,
                             sal_uInt16 nAspect = ASPECT_CONTENT) SAL_OVERRIDE;

    virtual void        FillClass(SvGlobalName* pClassName,
                                  sal_uInt32*  pFormat,
                                  OUString* pAppName,
                                  OUString* pFullTypeName,
                                  OUString* pShortTypeName,
                                  sal_Int32 nFileFormat,
                                  bool bTemplate = false ) const SAL_OVERRIDE;

    virtual bool        SetData( const OUString& rData );
    virtual sal_uLong       GetMiscStatus() const SAL_OVERRIDE;
    virtual void        OnDocumentPrinterChanged( Printer * ) SAL_OVERRIDE;
    virtual bool        InitNew( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage ) SAL_OVERRIDE;
    virtual bool        Load( SfxMedium& rMedium ) SAL_OVERRIDE;
            void        ImplSave(  SvStorageStreamRef xStrm  );
    virtual bool        Save() SAL_OVERRIDE;
    virtual bool        SaveAs( SfxMedium& rMedium ) SAL_OVERRIDE;
    virtual bool        ConvertTo( SfxMedium &rMedium ) SAL_OVERRIDE;
    virtual bool        SaveCompleted( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage ) SAL_OVERRIDE;

    Printer             *GetPrt();
    OutputDevice*       GetRefDev();

    bool                IsFormulaArranged() const { return bIsFormulaArranged; }
    void                SetFormulaArranged(bool bVal) { bIsFormulaArranged = bVal; }

    virtual bool        ConvertFrom(SfxMedium &rMedium) SAL_OVERRIDE;

    /** Called whenever the formula is changed
     * Deletes the current cursor
     */
    void                InvalidateCursor();

    bool writeFormulaOoxml( ::sax_fastparser::FSHelperPtr pSerializer, oox::core::OoxmlVersion version );
    void writeFormulaRtf(OStringBuffer& rBuffer, rtl_TextEncoding nEncoding);
    void readFormulaOoxml( oox::formulaimport::XmlStream& stream );

public:
    TYPEINFO_OVERRIDE();
    SFX_DECL_INTERFACE(SFX_INTERFACE_SMA_START+1)

    using SotObject::GetInterface;
    SFX_DECL_OBJECTFACTORY();

private:
    /// SfxInterface initializer.
    static void InitInterface_Impl();

public:
                SmDocShell( const sal_uInt64 i_nSfxCreationFlags );
    virtual     ~SmDocShell();

    void        LoadSymbols();
    void        SaveSymbols();

    void        ArrangeFormula();

    //Access for the View. This access is not for the OLE-case!
    //and for the communication with the SFX!
    //All internal printer uses should work with the SmPrinterAccess only
    bool        HasPrinter()    { return 0 != pPrinter; }
    SfxPrinter *GetPrinter()    { GetPrt(); return pPrinter; }
    void        SetPrinter( SfxPrinter * );

    const OUString GetComment() const;

    // to replace chars that can not be saved with the document...
    bool    ReplaceBadChars();

    void        UpdateText();
    void        SetText(const OUString& rBuffer);
    OUString    GetText() { return (aText); }
    void        SetFormat(SmFormat& rFormat);
    const SmFormat&  GetFormat() { return (aFormat); }

    void            Parse();
    SmParser &      GetParser() { return aInterpreter; }
    const SmNode *  GetFormulaTree() const  { return pTree; }
    void            SetFormulaTree(SmNode *&rTree) { pTree = rTree; }

    const std::set< OUString > &    GetUsedSymbols() const  { return aUsedSymbols; }

    OUString        GetAccessibleText();

    EditEngine &    GetEditEngine();
    SfxItemPool &   GetEditEngineItemPool();

    void        DrawFormula(OutputDevice &rDev, Point &rPosition, bool bDrawSelection = false);
    Size        GetSize();

    void        Repaint();

    virtual     ::svl::IUndoManager *GetUndoManager () SAL_OVERRIDE;

    virtual     SfxItemPool& GetPool() const;

    void        Execute( SfxRequest& rReq );
    void        GetState(SfxItemSet &);

    virtual void SetVisArea (const Rectangle & rVisArea) SAL_OVERRIDE;
    virtual void SetModified(bool bModified) SAL_OVERRIDE;

    /** Get a cursor for modifying this document
     * @remarks Don't store this reference, a new cursor may be made...
     */
    SmCursor&   GetCursor();
    /** True, if cursor have previously been requested and thus
     * has some sort of position.
     */
    bool        HasCursor() { return pCursor != NULL; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
