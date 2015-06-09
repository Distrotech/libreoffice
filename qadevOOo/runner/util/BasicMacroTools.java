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
package util;

import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.frame.XFrame;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.script.XLibraryContainer;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.*;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;



public class BasicMacroTools {
    private final XDispatchProvider mDispProv;
    private final XMultiServiceFactory mMSF;
    private final XURLTransformer mParser;
    private final XNameAccess mLCxNA; //LibraryContainer::XNameAccess
    private final XLibraryContainer mLCxLC; //LibraryContainer::XLibraryContainer

    /*
     *While initializing the Basic Libraries will be appendend to the Document
     */
    public BasicMacroTools(XMultiServiceFactory msf, XModel xModel,
                           XComponent xDoc) throws java.lang.Exception {
        mMSF = msf;
        mDispProv = makeDispatchProvider(xModel);
        mParser = makeParser(mMSF);

        Object DocLibCont = null;

        try {
            XPropertySet xDocProps = UnoRuntime.queryInterface(
                                             XPropertySet.class, xDoc);
            DocLibCont = xDocProps.getPropertyValue("BasicLibraries");
        } catch (com.sun.star.uno.Exception e) {
            throw new Exception(
                    "Couldn't get BasicLibraries-Container from document", e);
        }

        mLCxNA = UnoRuntime.queryInterface(XNameAccess.class,
                                                         DocLibCont);

        mLCxLC = UnoRuntime.queryInterface(
                         XLibraryContainer.class, DocLibCont);
    }

    /*
     * While initializing the Basic Libraries will be appendend to the Office
    */
    public BasicMacroTools(XMultiServiceFactory msf, XModel xModel)
                    throws java.lang.Exception {
        mMSF = msf;
        mDispProv = makeDispatchProvider(xModel);
        mParser = makeParser(mMSF);

        Object ASLC = null;

        try {
            ASLC = mMSF.createInstance(
                           "com.sun.star.script.ApplicationScriptLibraryContainer");
        } catch (com.sun.star.uno.Exception e) {
            throw new Exception("Couldn't create ApplicationScriptLibraryContainer", e);
        }

        mLCxNA = UnoRuntime.queryInterface(XNameAccess.class,
                                                         ASLC);

        mLCxLC = UnoRuntime.queryInterface(
                         XLibraryContainer.class, ASLC);
    }

    private static XDispatchProvider makeDispatchProvider(XModel aModel)
                                                   throws java.lang.Exception {
        XController xController = aModel.getCurrentController();
        XFrame xFrame = xController.getFrame();

        if (xFrame == null) {
            throw new Exception("Could not create DispatchProvider");
        }

        return UnoRuntime.queryInterface(
                       XDispatchProvider.class, xFrame);
    }

    private static XURLTransformer makeParser(XMultiServiceFactory mMSF)
                                       throws java.lang.Exception {
        return UnoRuntime.queryInterface(
                       XURLTransformer.class, mMSF.createInstance(
                               "com.sun.star.util.URLTransformer"));
    }

    public void loadLibrary(String LibraryName, String LibraryURL)
                     throws java.lang.Exception {
        try {
            appendLibrary(LibraryName, LibraryURL);
        } catch (java.lang.Exception e) {
            throw new Exception("Could not append Library " + LibraryName, e);
        }

        try {
            mLCxLC.loadLibrary(LibraryName);
        } catch (com.sun.star.container.NoSuchElementException e) {
            throw new Exception("Could not load Library " + LibraryName, e);
        } catch (com.sun.star.lang.WrappedTargetException e) {
            throw new Exception("Could not load Library " + LibraryName, e);
        }
    }

    private void appendLibrary(String LibraryName, String LibraryURL)
                        throws java.lang.Exception {
        try {
            removeLibrary(LibraryName);
        } catch (java.lang.Exception e) {
        }

        try {
            mLCxLC.createLibraryLink(LibraryName, LibraryURL, false);
        } catch (com.sun.star.container.ElementExistException e) {
            throw new Exception("Library " + LibraryName + "already exists.", e);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            throw new Exception("Could not link Basic library " + LibraryName, e);
        }
    }

    public void removeLibrary(String LibraryName) throws java.lang.Exception {
        if (mLCxNA.hasByName(LibraryName)) {
            try {
                mLCxLC.removeLibrary(LibraryName);
            } catch (com.sun.star.container.NoSuchElementException e) {
                throw new Exception("Could not remove Basic library " + LibraryName + ", Library does not exist", e);
            } catch (com.sun.star.lang.WrappedTargetException e) {
                throw new Exception("Could not remove Basic library " + LibraryName, e);
            }
        }
    }

    public void runMarco(String MacroName) throws java.lang.Exception {
        URL[] aParseURL = new URL[1];
        aParseURL[0] = new URL();
        aParseURL[0].Complete = "macro://./" + MacroName; //Standard.Stock.GetSymbol('micro','')";
        mParser.parseStrict(aParseURL);

        URL aURL = aParseURL[0];
        XDispatch xDispatcher = mDispProv.queryDispatch(aURL, "", 0);

        if (xDispatcher != null) {
            xDispatcher.dispatch(aURL, null);
        } else {
            throw new Exception("Could not run Macro " + MacroName);
        }
    }

    /**
     * Set the given <CODE>secureURL</CODE> as secure URL for marco execution.
     * The macros of documents located in <CODE>secureURL</CODE> will be executed
     * automatically.
     * @param xMSF the XMultiServiceFactory
     * @param secureURL the URL the documet is located
     * @throws java.lang.Exception throws this exception on any error
     */
    public static void addSecureBasicMarcosURL(XMultiServiceFactory xMSF, String secureURL)
        throws Exception {

        secureURL = utils.getFullURL(secureURL);

        // configure Office to allow to execute macos
        PropertyValue[] ProvArgs = new PropertyValue [1];
        PropertyValue Arg = new PropertyValue();
        Arg.Name = "nodepath";
        Arg.Value = "/org.openoffice.Office.Common/Security";
        ProvArgs[0] = Arg;

        Object oProvider = xMSF.createInstance("com.sun.star.configuration.ConfigurationProvider");


        XMultiServiceFactory oProviderMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, oProvider);

        Object oSecure = oProviderMSF.createInstanceWithArguments(
            "com.sun.star.configuration.ConfigurationUpdateAccess",
            ProvArgs);

        XPropertySet oSecureProps = UnoRuntime.queryInterface(XPropertySet.class, oSecure);

        Object oScripting = oSecureProps.getPropertyValue("Scripting");
        XPropertySet oScriptingSettings = UnoRuntime.queryInterface(XPropertySet.class, oScripting);

        oScriptingSettings.setPropertyValue("SecureURL", new String[]{secureURL});
        oScriptingSettings.setPropertyValue("OfficeBasic", Integer.valueOf(2));

        XChangesBatch oSecureChange = UnoRuntime.queryInterface(XChangesBatch.class, oSecure);
        oSecureChange.commitChanges();
    }
}
