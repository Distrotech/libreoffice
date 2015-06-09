/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/config.h>
#include <test/bootstrapfixture.hxx>
#include <test/unoapi_test.hxx>
#include <rtl/strbuf.hxx>
#include <osl/file.hxx>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/document/XEmbeddedScripts.hpp>
#include <com/sun/star/script/XStorageBasedLibraryContainer.hpp>
#include <com/sun/star/script/XLibraryContainer.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>

#include <sfx2/app.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/sfxmodelfactory.hxx>
#include <svl/intitem.hxx>
#include <comphelper/processfactory.hxx>

#include <basic/sbxdef.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


class DialogSaveTest : public UnoApiTest
{
public:
    DialogSaveTest();

    void test();

    CPPUNIT_TEST_SUITE(DialogSaveTest);
// Should we disable this test on MOX and WNT?
// #if !defined(MACOSX) && !defined(WNT)
    CPPUNIT_TEST(test);
// #endif
    CPPUNIT_TEST_SUITE_END();

};


DialogSaveTest::DialogSaveTest()
      : UnoApiTest("/dbaccess/qa/extras/testdocuments")
{
}

void DialogSaveTest::test()
{
    // UnoApiTest::setUp (via InitVCL) puts each test under a locked SolarMutex,
    // but at least the below xDocCloseable->close call could lead to a deadlock
    // then, and it looks like none of the code here requires the SolarMutex to
    // be locked anyway:
    SolarMutexReleaser rel;

    const OUString sStandard("Standard");
    const OUString aFileName(getURLFromWorkdir("CppunitTest/testDialogSave.odb"));
    {
        uno::Reference< lang::XComponent > xComponent = loadFromDesktop(aFileName);
        CPPUNIT_ASSERT(xComponent.is());
        uno::Reference< frame::XStorable > xDocStorable(xComponent, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xDocStorable.is());
        uno::Reference< document::XEmbeddedScripts > xDocScr(xComponent, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xDocScr.is());
        uno::Reference< script::XStorageBasedLibraryContainer > xStorBasLib(xDocScr->getBasicLibraries());
        CPPUNIT_ASSERT(xStorBasLib.is());
        uno::Reference< script::XLibraryContainer > xBasLib(xStorBasLib, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xBasLib.is());
        uno::Reference< script::XStorageBasedLibraryContainer > xStorDlgLib(xDocScr->getDialogLibraries());
        CPPUNIT_ASSERT(xStorDlgLib.is());
        uno::Reference< script::XLibraryContainer > xDlgLib(xStorDlgLib, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xDlgLib.is());
        xBasLib->loadLibrary(sStandard);
        CPPUNIT_ASSERT(xBasLib->isLibraryLoaded(sStandard));
        // the whole point of this test is to test the "save" operation
        // when the Basic library is loaded, but not the Dialog library
        CPPUNIT_ASSERT(!xDlgLib->isLibraryLoaded(sStandard));

        // make some change to enable a save
        // uno::Reference< document::XDocumentPropertiesSupplier > xDocPropSuppl(xComponent, UNO_QUERY_THROW);
        // CPPUNIT_ASSERT(xDocPropSuppl.is());
        // uno::Reference< document::XDocumentPropertiesSupplier > xDocProps(xDocPropSuppl->getDocumentProperties());
        // CPPUNIT_ASSERT(xDocProps.is());
        // xDocProps.setTitle(xDocProps.getTitle() + " suffix");
        uno::Reference< util::XModifiable > xDocMod(xComponent, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xDocMod.is());
        xDocMod->setModified(sal_True);

        // now save; the code path to exercise in this test is the "store to same location"
        //           do *not* change to store(As|To|URL)!
        xDocStorable->store();

        // close
        uno::Reference< util::XCloseable > xDocCloseable(xComponent, UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xDocCloseable.is());
        xDocCloseable->close(false);

        // All our uno::References are (should?) be invalid now -> let them go out of scope
    }
    {
        uno::Sequence<uno::Any> args(1);
        args[0] <<= aFileName;
        Reference<container::XHierarchicalNameAccess> xHNA(getMultiServiceFactory()->createInstanceWithArguments("com.sun.star.packages.Package", args), UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xHNA.is());
        Reference< beans::XPropertySet > xPS(xHNA->getByHierarchicalName("Dialogs/Standard/Dialog1.xml"), UNO_QUERY_THROW);
        CPPUNIT_ASSERT(xPS.is());
        sal_Int64 nSize = 0;
        CPPUNIT_ASSERT(xPS->getPropertyValue("Size") >>= nSize);
        CPPUNIT_ASSERT(nSize != 0);
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(DialogSaveTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
