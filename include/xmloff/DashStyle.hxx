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

#ifndef INCLUDED_XMLOFF_DASHSTYLE_HXX
#define INCLUDED_XMLOFF_DASHSTYLE_HXX

#include <sal/config.h>
#include <xmloff/dllapi.h>
#include <sal/types.h>

class SvXMLImport;
class SvXMLExport;
namespace com { namespace sun { namespace star {
    namespace uno { template<class A> class Reference; }
    namespace xml { namespace sax { class XAttributeList; } }
    namespace uno { class Any; }
} } }


class XMLOFF_DLLPUBLIC XMLDashStyleImport
{
    SvXMLImport& rImport;

public:
    XMLDashStyleImport( SvXMLImport& rImport );
    ~XMLDashStyleImport();

    void importXML(
        const ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
        ::com::sun::star::uno::Any& rValue,
        OUString& rStrName );
};


class XMLOFF_DLLPUBLIC XMLDashStyleExport
{
    SvXMLExport& rExport;

public:
    XMLDashStyleExport( SvXMLExport& rExport );
    ~XMLDashStyleExport();

    bool exportXML( const OUString& rStrName,
                        const ::com::sun::star::uno::Any& rValue );
};

#endif // INCLUDED_XMLOFF_DASHSTYLE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
