/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_WRITERPERFECT_QA_UNIT_WPFTIMPORTTESTBASE_HXX
#define INCLUDED_WRITERPERFECT_QA_UNIT_WPFTIMPORTTESTBASE_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <rtl/ustring.hxx>

#include <test/bootstrapfixture.hxx>

#include <unotest/filters-test.hxx>

namespace com { namespace sun { namespace star {
    namespace beans { struct PropertyValue; }
    namespace container { class XNameAccess; }
    namespace document { class XFilter; }
    namespace frame { class XDesktop2; }
    namespace ucb { class XSimpleFileAccess; }
} } }

namespace writerperfect
{
namespace test
{

class WpftImportTestBase
    : public ::test::FiltersTest
    , public ::test::BootstrapFixture
{
public:
    explicit WpftImportTestBase(const rtl::OUString &rFactoryURL);

    virtual void setUp() SAL_OVERRIDE;
    virtual void tearDown() SAL_OVERRIDE;

protected:
    void doTest(const rtl::OUString &rFilter, const rtl::OUString &rPath);

private:
    virtual bool load(const OUString &, const OUString &rURL, const OUString &,
        unsigned int, unsigned int, unsigned int) SAL_OVERRIDE;

    void impl_detectFilterName(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> &rDescriptor, const rtl::OUString &rTypeName);

private:
    const rtl::OUString m_aFactoryURL;
    com::sun::star::uno::Reference<com::sun::star::frame::XDesktop2> m_xDesktop;
    com::sun::star::uno::Reference<com::sun::star::ucb::XSimpleFileAccess> m_xFileAccess;
    com::sun::star::uno::Reference<com::sun::star::document::XFilter> m_xFilter;
    com::sun::star::uno::Reference<com::sun::star::container::XNameAccess> m_xTypeMap;
};

}
}

#endif // INCLUDED_WRITERPERFECT_QA_UNIT_WPFTIMPORTTESTBASE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
