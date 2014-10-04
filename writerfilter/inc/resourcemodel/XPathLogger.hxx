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
#ifndef INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_XPATHLOGGER_HXX
#define INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_XPATHLOGGER_HXX

#ifdef DEBUG_CONTEXT_HANDLER

#include <stack>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace writerfilter
{

class XPathLogger
{
    typedef boost::unordered_map<std::string, unsigned int> TokenMap_t;
    typedef boost::shared_ptr<TokenMap_t> TokenMapPointer_t;

    TokenMapPointer_t mp_tokenMap;
    std::stack<TokenMapPointer_t> m_tokenMapStack;
    std::vector<std::string> m_path;
    std::string m_currentPath;

    void updateCurrentPath();

public:
    explicit XPathLogger();
    virtual ~XPathLogger();

    std::string getXPath() const;
    void startElement(std::string _token);
    void endElement();
};
}

#endif
#endif // INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_XPATHLOGGER_HXX
