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

#ifndef INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_TAGLOGGER_HXX
#define INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_TAGLOGGER_HXX

#include <rtl/ustring.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <resourcemodel/WW8ResourceModel.hxx>
#include <string>
#include <boost/shared_ptr.hpp>
#include <libxml/xmlwriter.h>

namespace writerfilter
{
#ifdef DEBUG_WRITERFILTER
    class IdToString
    {
    public:
        typedef boost::shared_ptr<IdToString> Pointer_t;
        virtual std::string toString(const Id & id) const = 0;

    protected:
        ~IdToString() {}
    };
#endif

    class TagLogger
    {
    public:
        typedef boost::shared_ptr<TagLogger> Pointer_t;

    private:
        xmlTextWriterPtr pWriter;
        const char* pName;

        TagLogger(const char* name);

    public:
        ~TagLogger();

        static Pointer_t getInstance(const char * name);

#ifdef DEBUG_WRITERFILTER
        void setFileName(const std::string & filename);
        void startDocument();
        void endDocument();

        void element(const std::string & name);
        void unoPropertySet(css::uno::Reference<css::beans::XPropertySet> rPropSet);
        void startElement(const std::string & name);
#endif
        void attribute(const std::string & name, const std::string & value);
#ifdef DEBUG_WRITERFILTER
        void attribute(const std::string & name, const OUString & value);
        void attribute(const std::string & name, sal_uInt32 value);
        void attribute(const std::string & name, const css::uno::Any aAny);
        void chars(const std::string & chars);
        void chars(const OUString & chars);
        void endElement();

        void propertySet(writerfilter::Reference<Properties>::Pointer_t props,
                IdToString::Pointer_t pIdToString);
#endif
    };
}

#endif // INCLUDED_WRITERFILTER_INC_RESOURCEMODEL_TAGLOGGER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
