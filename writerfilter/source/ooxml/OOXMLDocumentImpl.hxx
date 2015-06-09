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
#ifndef INCLUDED_WRITERFILTER_SOURCE_OOXML_OOXMLDOCUMENTIMPL_HXX
#define INCLUDED_WRITERFILTER_SOURCE_OOXML_OOXMLDOCUMENTIMPL_HXX

#include <ooxml/OOXMLDocument.hxx>

#include <com/sun/star/xml/sax/XFastTokenHandler.hpp>
#include <com/sun/star/xml/dom/XDocument.hpp>

#include "OOXMLPropertySet.hxx"

#include <vector>

namespace writerfilter {
namespace ooxml
{

class OOXMLDocumentImpl : public OOXMLDocument
{
    OOXMLStream::Pointer_t mpStream;
    css::uno::Reference<css::task::XStatusIndicator> mxStatusIndicator;
    sal_Int32 mnXNoteId;
    Id mXNoteType;

    css::uno::Reference<css::frame::XModel> mxModel;
    css::uno::Reference<css::drawing::XDrawPage> mxDrawPage;
    css::uno::Reference<css::xml::dom::XDocument> mxGlossaryDocDom;
    css::uno::Sequence < css::uno::Sequence< css::uno::Any > > mxGlossaryDomList;
    css::uno::Reference<css::xml::sax::XFastShapeContextHandler> mxShapeContext;
    css::uno::Reference<css::xml::dom::XDocument> mxThemeDom;
    css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > mxCustomXmlDomList;
    css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > mxCustomXmlDomPropsList;
    css::uno::Reference<css::xml::dom::XDocument> mxCustomXmlProsDom;
    css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > mxActiveXDomList;
    css::uno::Sequence<css::uno::Reference<css::io::XInputStream> > mxActiveXBinList;
    css::uno::Reference<css::io::XInputStream> mxActiveXBin;
    css::uno::Reference<css::io::XInputStream> mxEmbeddings;
    css::uno::Sequence < css::beans::PropertyValue > mxEmbeddingsList;
    std::vector<css::beans::PropertyValue> mxEmbeddingsListTemp;
    bool mbIsSubstream;
    /// How many paragraphs equal to 1 percent?
    sal_Int32 mnPercentSize;
    /// Position progress when it was last updated, possibly not after every paragraph in case of large documents.
    sal_Int32 mnProgressLastPos;
    /// Current position progress, updated after every paragraph.
    sal_Int32 mnProgressCurrentPos;
    /// End position, i.e. the estimated number of paragraphs.
    sal_Int32 mnProgressEndPos;

protected:
    virtual void resolveFastSubStream(Stream & rStream,
                                      OOXMLStream::StreamType_t nType);

    virtual void resolveFastSubStreamWithId(Stream & rStream,
                                      writerfilter::Reference<Stream>::Pointer_t pStream,
                      sal_uInt32 nId);

    css::uno::Reference<css::xml::dom::XDocument> importSubStream(OOXMLStream::StreamType_t nType);

    void importSubStreamRelations(OOXMLStream::Pointer_t pStream, OOXMLStream::StreamType_t nType);

    writerfilter::Reference<Stream>::Pointer_t
    getSubStream(const OUString & rId);

    writerfilter::Reference<Stream>::Pointer_t
    getXNoteStream(OOXMLStream::StreamType_t nType,
                   const Id & rType,
                   const sal_Int32 nNoteId);

    void setIsSubstream( bool bSubstream ) { mbIsSubstream = bSubstream; };
    void resolveCustomXmlStream(Stream & rStream);
    void resolveActiveXStream(Stream & rStream);
    void resolveGlossaryStream(Stream & rStream);
    void resolveEmbeddingsStream(OOXMLStream::Pointer_t pStream);
public:
    OOXMLDocumentImpl(OOXMLStream::Pointer_t pStream, const css::uno::Reference<css::task::XStatusIndicator>& xStatusIndicator);
    virtual ~OOXMLDocumentImpl();

    virtual void resolve(Stream & rStream) SAL_OVERRIDE;

    virtual void resolveFootnote(Stream & rStream,
                                 const Id & rType,
                                 const sal_Int32 nNoteId) SAL_OVERRIDE;
    virtual void resolveEndnote(Stream & rStream,
                                const Id & rType,
                                const sal_Int32 nNoteId) SAL_OVERRIDE;
    virtual void resolveHeader(Stream & rStream,
                               const sal_Int32 type,
                               const OUString & rId) SAL_OVERRIDE;
    virtual void resolveFooter(Stream & rStream,
                               const sal_Int32 type,
                               const OUString & rId) SAL_OVERRIDE;

    virtual void resolveComment(Stream & rStream, const sal_Int32 nId) SAL_OVERRIDE;

    virtual OOXMLPropertySet * getPicturePropSet
    (const OUString & rId);
    virtual void resolvePicture(Stream & rStream, const OUString & rId) SAL_OVERRIDE;

    virtual OUString getTargetForId(const OUString & rId) SAL_OVERRIDE;

    virtual void setModel(css::uno::Reference<css::frame::XModel> xModel) SAL_OVERRIDE;
    virtual css::uno::Reference<css::frame::XModel> getModel() SAL_OVERRIDE;
    virtual void setDrawPage(css::uno::Reference<css::drawing::XDrawPage> xDrawPage) SAL_OVERRIDE;
    virtual css::uno::Reference<css::drawing::XDrawPage> getDrawPage() SAL_OVERRIDE;
    virtual css::uno::Reference<css::io::XInputStream> getInputStream() SAL_OVERRIDE;
    virtual css::uno::Reference<css::io::XInputStream> getStorageStream() SAL_OVERRIDE;
    virtual css::uno::Reference<css::io::XInputStream> getInputStreamForId(const OUString & rId) SAL_OVERRIDE;
    virtual void setXNoteId(const sal_Int32 nId) SAL_OVERRIDE;
    virtual sal_Int32 getXNoteId() const SAL_OVERRIDE;
    virtual void setXNoteType(const Id & rId) SAL_OVERRIDE;
    virtual const Id & getXNoteType() const SAL_OVERRIDE;
    virtual const OUString & getTarget() const SAL_OVERRIDE;
    virtual css::uno::Reference<css::xml::sax::XFastShapeContextHandler> getShapeContext( ) SAL_OVERRIDE;
    virtual void setShapeContext( css::uno::Reference<css::xml::sax::XFastShapeContextHandler> xContext ) SAL_OVERRIDE;
    virtual void setThemeDom(css::uno::Reference<css::xml::dom::XDocument> xThemeDom) SAL_OVERRIDE;
    virtual css::uno::Reference<css::xml::dom::XDocument> getThemeDom() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > getCustomXmlDomList() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > getCustomXmlDomPropsList() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::uno::Reference<css::xml::dom::XDocument> > getActiveXDomList() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::uno::Reference<css::io::XInputStream> > getActiveXBinList() SAL_OVERRIDE;
    virtual css::uno::Reference<css::xml::dom::XDocument> getGlossaryDocDom() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::uno::Sequence< css::uno::Any> >  getGlossaryDomList() SAL_OVERRIDE;
    virtual css::uno::Sequence<css::beans::PropertyValue >  getEmbeddingsList() SAL_OVERRIDE;

    void incrementProgress();
};
}}
#endif // OOXML_DOCUMENT_IMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
