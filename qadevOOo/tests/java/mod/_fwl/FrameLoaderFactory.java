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

package mod._fwl;

import com.sun.star.beans.NamedValue;
import java.io.PrintWriter;

import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.utils;

import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.frame.FrameLoaderFactory</code>. <p>
 *
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::container::XNameAccess</code></li>
 *  <li> <code>com::sun::star::container::XElementAccess</code></li>
 *  <li> <code>com::sun::star::lang::XMultiServiceFactory</code></li>
 * </ul> <p>
 *
 * @see com.sun.star.container.XNameAccess
 * @see com.sun.star.container.XElementAccess
 * @see com.sun.star.lang.XMultiServiceFactory
 * @see ifc.container._XNameAccess
 * @see ifc.container._XElementAccess
 * @see ifc.lang._XMultiServiceFactory
 */
public class FrameLoaderFactory extends TestCase {

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.frame.FrameLoaderFactory</code>. <p>
    */
    protected TestEnvironment createTestEnvironment
            (TestParameters Param, PrintWriter log) {
        XInterface oObj = null;
        Object oInterface = null ;

        try {
            oInterface = ((XMultiServiceFactory)Param.getMSF()).createInstance
                ("com.sun.star.frame.FrameLoaderFactory") ;
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't get service");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get FrameLoaderFactory", e );
        }

        if (oInterface == null) {
            log.println("Service wasn't created") ;
            throw new StatusException(Status.failed("Service wasn't created")) ;
        }

        oObj = (XInterface) oInterface ;
        log.println("ImplName: "+utils.getImplName(oObj));

        log.println( "creating a new environment for object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        XNameAccess xNA = UnoRuntime.queryInterface
            (XNameAccess.class, oObj);
        tEnv.addObjRelation("XMSF.serviceNames", xNA.getElementNames());

        // com.sun.star.container.XContainerQuery
        NamedValue[] querySequenze = new NamedValue[1];
        NamedValue query = new NamedValue();
        query.Name = "Name";
        query.Value = "com.sun.star.frame.Bibliography";
        querySequenze[0] = query;

        tEnv.addObjRelation("XContainerQuery.createSubSetEnumerationByProperties",
            querySequenze);

        return tEnv;
    } // finish method getTestEnvironment

}

