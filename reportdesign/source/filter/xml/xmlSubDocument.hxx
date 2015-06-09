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
#ifndef INCLUDED_REPORTDESIGN_SOURCE_FILTER_XML_XMLSUBDOCUMENT_HXX
#define INCLUDED_REPORTDESIGN_SOURCE_FILTER_XML_XMLSUBDOCUMENT_HXX

#include "xmlReportElementBase.hxx"
#include <com/sun/star/report/XReportComponent.hpp>
#include <vector>

namespace rptxml
{
    class ORptFilter;
    class OXMLCell;
    class OXMLSubDocument : public OXMLReportElementBase, public IMasterDetailFieds
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>   m_xComponent;
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>   m_xFake;
        ::std::vector< OUString> m_aMasterFields;
        ::std::vector< OUString> m_aDetailFields;
        OXMLCell*       m_pCellParent;
        sal_Int32       m_nCurrentCount;
        bool            m_bContainsShape;

        OXMLSubDocument(const OXMLSubDocument&);
        void operator =(const OXMLSubDocument&);

        virtual SvXMLImportContext* _CreateChildContext( sal_uInt16 nPrefix,
                    const OUString& rLocalName,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList ) SAL_OVERRIDE;
    public:

        OXMLSubDocument( ORptFilter& rImport
                    ,sal_uInt16 nPrfx
                    ,const OUString& rLName
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent >& _xComponent
                    ,OXMLTable* _pContainer
                    ,OXMLCell* _pCellParent);
        virtual ~OXMLSubDocument();

        virtual void EndElement() SAL_OVERRIDE;
        virtual void addMasterDetailPair(const ::std::pair< OUString,OUString >& _aPair) SAL_OVERRIDE;
    };

} // namespace rptxml


#endif // RPT_XMLSubDocument_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
