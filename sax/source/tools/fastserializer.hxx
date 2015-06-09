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

#ifndef INCLUDED_SAX_SOURCE_TOOLS_FASTSERIALIZER_HXX
#define INCLUDED_SAX_SOURCE_TOOLS_FASTSERIALIZER_HXX

#include <com/sun/star/xml/sax/XFastTokenHandler.hpp>
#include <com/sun/star/io/XOutputStream.hpp>

#include <sax/fastattribs.hxx>
#include <sax/fshelper.hxx>
#include <CachedOutputStream.hxx>

#include <stack>
#include <map>
#include <boost/shared_ptr.hpp>

namespace sax_fastparser {

struct TokenValue
{
    sal_Int32   nToken;
    const char *pValue;
    TokenValue(sal_Int32 _nToken, const char *_pValue) : nToken(_nToken), pValue(_pValue) {}
};
typedef std::vector<TokenValue> TokenValueList;

/// Receives notification of sax document events to write into an XOutputStream.
class FastSaxSerializer
{
    typedef ::com::sun::star::uno::Sequence< ::sal_Int8 > Int8Sequence;
    typedef ::com::sun::star::uno::Sequence< ::sal_Int32 > Int32Sequence;

public:
    FastSaxSerializer( const css::uno::Reference< css::io::XOutputStream >& xOutputStream );
    ~FastSaxSerializer();

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > getOutputStream();
    /// called by FSHelper to put data in for writeTokenValueList
    TokenValueList& getTokenValueList() { return maTokenValues; }

    /** called by the parser when parsing of an XML stream is started.
     */
    void startDocument();

    /** called by the parser after the last XML element of a stream is processed.
     */
    void endDocument();

    /** receives notification of the beginning of an element.

        @param Element
            contains the integer token from the <type>XFastTokenHandler</type>
            registered at the <type>XFastParser</type>.<br>

            If the element has a namespace that was registered with the
            <type>XFastParser</type>, <param>Element</param> contains the integer
            token of the elements local name from the <type>XFastTokenHandler</type>
            and the integer token of the namespace combined with an arithmetic
            <b>or</b> operation.

        @param pAttrList
            Contains a <type>FastAttributeList</type> to access the attributes
            from the element.

    */
    void startFastElement( ::sal_Int32 Element, FastAttributeList* pAttrList = NULL );

    /** receives notification of the end of an known element.
        @see startFastElement
     */
    void endFastElement( ::sal_Int32 Element );

    /** receives notification of the beginning of a single element.

        @param Element
            contains the integer token from the <type>XFastTokenHandler</type>
            registered at the <type>XFastParser</type>.<br>

            If the element has a namespace that was registered with the
            <type>XFastParser</type>, <param>Element</param> contains the integer
            token of the elements local name from the <type>XFastTokenHandler</type>
            and the integer token of the namespace combined with an arithmetic
            <b>or</b> operation.

        @param pAttrList
            Contains a <type>FastAttributeList</type> to access the attributes
            from the element.

    */
    void singleFastElement( ::sal_Int32 Element, FastAttributeList* pAttrList = NULL );

    // C++ helpers
    void writeId( ::sal_Int32 Element );
    OString getId( ::sal_Int32 Element );

    void write( double value );
    void write( const OUString& s, bool bEscape = false );
    void write( const OString& s, bool bEscape = false );
    void write( const char* pStr, sal_Int32 nLen, bool bEscape = false );

public:
    /** From now on, don't write directly to the stream, but to top of a stack.

        This is to be able to change the order of the data being written.
        If you need to write eg.
          p, r, rPr, [something], /rPr, t, [text], /t, /r, /p,
        but get it in order
          p, r, t, [text], /t, rPr, [something], /rPr, /r, /p,
        simply do
          p, r, mark(), t, [text], /t, mark(), rPr, [something], /rPr,
          mergeTopMarks( MERGE_MARKS_PREPEND ), mergeTopMarks( MERGE_MARKS_APPEND ), /r, /p
        and you are done.
     */
    void mark( const Int32Sequence& aOrder = Int32Sequence() );

    /** Merge 2 topmost marks.

        The possibilities: prepend the top before the second top-most
        mark, append it, append it later or ignore; prepending brings the possibility
        to switch parts of the output, appending later allows to write some
        output in advance.

        Writes the result to the output stream if the mark stack becomes empty
        by the operation.

        When the MERGE_MARKS_POSTPONE is specified, the merge happens just
        before the next merge.

        @see mark()
     */
    void mergeTopMarks( sax_fastparser::MergeMarksEnum eMergeType = sax_fastparser::MERGE_MARKS_APPEND );

private:
    /** Helper class to cache data and write in chunks to XOutputStream or ForMerge::append.
     *  Its flush method needs to be called before touching maMarkStack
     *  to ensure correct order of ForSort methods.
     */
    CachedOutputStream maCachedOutputStream;
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastTokenHandler > mxFastTokenHandler;

    class ForMerge : public ForMergeBase
    {
        Int8Sequence maData;
        Int8Sequence maPostponed;

    public:
        ForMerge() : maData(), maPostponed() {}
        virtual ~ForMerge() {}

        virtual void setCurrentElement( ::sal_Int32 /*nToken*/ ) {}
        virtual Int8Sequence& getData();
#if OSL_DEBUG_LEVEL > 0
        virtual void print();
#endif

        virtual void prepend( const Int8Sequence &rWhat );
        virtual void append( const Int8Sequence &rWhat ) SAL_OVERRIDE;
        void postpone( const Int8Sequence &rWhat );

    protected:
        void resetData( );
        static void merge( Int8Sequence &rTop, const Int8Sequence &rMerge, bool bAppend );
    };

    class ForSort : public ForMerge
    {
        std::map< ::sal_Int32, Int8Sequence > maData;
        sal_Int32 mnCurrentElement;

        Int32Sequence maOrder;

    public:
        ForSort( const Int32Sequence& aOrder ) :
            ForMerge(),
            maData(),
            mnCurrentElement( 0 ),
            maOrder( aOrder ) {}

        void setCurrentElement( ::sal_Int32 nToken ) SAL_OVERRIDE;

        virtual Int8Sequence& getData() SAL_OVERRIDE;

#if OSL_DEBUG_LEVEL > 0
        virtual void print() SAL_OVERRIDE;
#endif

        virtual void prepend( const Int8Sequence &rWhat ) SAL_OVERRIDE;
        virtual void append( const Int8Sequence &rWhat ) SAL_OVERRIDE;
    private:
        void sort();
    };

    ::std::stack< boost::shared_ptr< ForMerge > > maMarkStack;
    bool mbMarkStackEmpty;
    // Would be better to use OStringBuffer instead of these two
    // but then we couldn't get the rtl_String* member :-(
    rtl_String *mpDoubleStr;
    sal_Int32 mnDoubleStrCapacity;
    TokenValueList maTokenValues;

#ifdef DBG_UTIL
    ::std::stack<sal_Int32> m_DebugStartedElements;
#endif

    void writeTokenValueList();
    void writeFastAttributeList(FastAttributeList& rAttrList);

    /** Forward the call to the output stream, or write to the stack.

        The latter in the case that we are inside a mark().
     */
    void writeBytes( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData );
    void writeBytes( const char* pStr, size_t nLen );
};

} // namespace sax_fastparser

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
