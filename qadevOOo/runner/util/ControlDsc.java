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

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;

/**
 * Describes a Control to be inserted in a container
 */

public class ControlDsc extends InstDescr {

    private String name = null;
    final String ifcName = "com.sun.star.form.XFormComponent";
    String service = "com.sun.star.form.component.CommandButton";

    public ControlDsc( String kind ) {
        service="com.sun.star.form.component."+kind;
        initControl();
    }
    public String getName() {
        return name;
    }

    public String getIfcName() {
        return ifcName;
    }
    public String getService() {
        return service;
    }

    private void initControl() {
        try {
            ifcClass = Class.forName( ifcName );
        }
        catch( ClassNotFoundException cnfE ) {
        }
    }
    public XInterface createInstance( XMultiServiceFactory docMSF ) {

        Object SrvObj = null;
        try {
                SrvObj = docMSF.createInstance( service );
        }
        catch( com.sun.star.uno.Exception cssuE ){
        }

        XInterface Control = (XInterface)UnoRuntime.queryInterface(ifcClass, SrvObj );

        return Control;

    }
}