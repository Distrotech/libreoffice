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

#ifndef INCLUDED_FRAMEWORK_INC_XML_STATUSBARDOCUMENTHANDLER_HXX
#define INCLUDED_FRAMEWORK_INC_XML_STATUSBARDOCUMENTHANDLER_HXX

#include <framework/statusbarconfiguration.hxx>

#include <com/sun/star/xml/sax/XDocumentHandler.hpp>

#include <rtl/ustring.hxx>
#include <cppuhelper/implbase1.hxx>

#include <boost/unordered_map.hpp>
#include <stdtypes.h>
#include <framework/fwedllapi.h>

namespace framework{

// Hash code function for using in all hash maps of follow implementation.

class FWE_DLLPUBLIC OReadStatusBarDocumentHandler :
                                        public ::cppu::WeakImplHelper1< ::com::sun::star::xml::sax::XDocumentHandler >
{
    public:
        enum StatusBar_XML_Entry
        {
            SB_ELEMENT_STATUSBAR,
            SB_ELEMENT_STATUSBARITEM,
            SB_ATTRIBUTE_URL,
            SB_ATTRIBUTE_ALIGN,
            SB_ATTRIBUTE_STYLE,
            SB_ATTRIBUTE_AUTOSIZE,
            SB_ATTRIBUTE_OWNERDRAW,
            SB_ATTRIBUTE_WIDTH,
            SB_ATTRIBUTE_OFFSET,
            SB_ATTRIBUTE_HELPURL,
            SB_XML_ENTRY_COUNT
        };

        enum StatusBar_XML_Namespace
        {
            SB_NS_STATUSBAR,
            SB_NS_XLINK,
            SB_XML_NAMESPACES_COUNT
        };

        OReadStatusBarDocumentHandler( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& aStatusBarItems );
        virtual ~OReadStatusBarDocumentHandler();

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
        throw ( ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL endDocument(void)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL startElement(
            const OUString& aName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL endElement(const OUString& aName)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL characters(const OUString& aChars)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL ignorableWhitespace(const OUString& aWhitespaces)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL processingInstruction(const OUString& aTarget,
                                                    const OUString& aData)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void SAL_CALL setDocumentLocator(
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > &xLocator)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    private:
        OUString getErrorLineString();

        class StatusBarHashMap : public ::boost::unordered_map< OUString             ,
                                                         StatusBar_XML_Entry            ,
                                                         OUStringHash,
                                                         ::std::equal_to< OUString > >
        {
            public:
                inline void free()
                {
                    StatusBarHashMap().swap( *this );// get rid of reserved capacity
                }
        };

        bool                                                                            m_bStatusBarStartFound;
        bool                                                                            m_bStatusBarEndFound;
        bool                                                                            m_bStatusBarItemStartFound;
        StatusBarHashMap                                                                    m_aStatusBarMap;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >    m_aStatusBarItems;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator >            m_xLocator;
};

class FWE_DLLPUBLIC OWriteStatusBarDocumentHandler
{
    public:
        OWriteStatusBarDocumentHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rStatusBarItems,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >& rWriteDocHandler );
        virtual ~OWriteStatusBarDocumentHandler();

        void WriteStatusBarDocument() throw
            ( ::com::sun::star::xml::sax::SAXException,
              ::com::sun::star::uno::RuntimeException );

    protected:
        virtual void WriteStatusBarItem(
            const OUString& rCommandURL,
            const OUString& rHelpURL,
            sal_Int16            nOffset,
            sal_Int16            nStyle,
            sal_Int16            nWidth ) throw
            ( ::com::sun::star::xml::sax::SAXException,
              ::com::sun::star::uno::RuntimeException );

        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >       m_aStatusBarItems;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >    m_xWriteDocumentHandler;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >      m_xEmptyList;
        OUString                                                                     m_aXMLStatusBarNS;
        OUString                                                                     m_aXMLXlinkNS;
        OUString                                                                     m_aAttributeType;
        OUString                                                                     m_aAttributeURL;
};

} // namespace framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
