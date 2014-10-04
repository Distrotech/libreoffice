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

#include "sal/config.h"

#include <cassert>
#include <set>

#include "com/sun/star/container/NoSuchElementException.hpp"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "sal/types.h"
#include "xmlreader/span.hxx"
#include "xmlreader/xmlreader.hxx"

#include "parsemanager.hxx"
#include "parser.hxx"

namespace configmgr {

ParseManager::ParseManager(
    OUString const & url, rtl::Reference< Parser > const & parser)
    SAL_THROW((
        css::container::NoSuchElementException, css::uno::RuntimeException)):
    reader_(url), parser_(parser)
{
    assert(parser.is());
    int id;
    id = reader_.registerNamespaceIri(
        xmlreader::Span(
            RTL_CONSTASCII_STRINGPARAM("http://openoffice.org/2001/registry")));
    assert(id == NAMESPACE_OOR);
    id = reader_.registerNamespaceIri(
        xmlreader::Span(
            RTL_CONSTASCII_STRINGPARAM("http://www.w3.org/2001/XMLSchema")));
    assert(id == NAMESPACE_XS);
    id = reader_.registerNamespaceIri(
        xmlreader::Span(
            RTL_CONSTASCII_STRINGPARAM(
                "http://www.w3.org/2001/XMLSchema-instance")));
    assert(id == NAMESPACE_XSI);
    (void)id;
}

bool ParseManager::parse(std::set< OUString > const * existingDependencies) {
    for (;;) {
        switch (itemData_.is()
                ? xmlreader::XmlReader::RESULT_BEGIN
                : reader_.nextItem(
                    parser_->getTextMode(), &itemData_, &itemNamespaceId_))
        {
        case xmlreader::XmlReader::RESULT_BEGIN:
            if (!parser_->startElement(
                    reader_, itemNamespaceId_, itemData_, existingDependencies))
            {
                return false;
            }
            break;
        case xmlreader::XmlReader::RESULT_END:
            parser_->endElement(reader_);
            break;
        case xmlreader::XmlReader::RESULT_TEXT:
            parser_->characters(itemData_);
            break;
        case xmlreader::XmlReader::RESULT_DONE:
            return true;
        }
        itemData_.clear();
    }
}

ParseManager::~ParseManager() {}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
