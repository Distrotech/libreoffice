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
package complex.tdoc;

import com.sun.star.beans.XPropertySetInfoChangeListener;
import com.sun.star.beans.XPropertySetInfoChangeNotifier;
import share.LogWriter;

/**
 *
 */
public class _XPropertySetInfoChangeNotifier {
    public XPropertySetInfoChangeNotifier oObj = null;
    public LogWriter log = null;

    PropertySetInfoChangeListener listener = new PropertySetInfoChangeListener();
    String[] args = null;


    public boolean _addPropertiesChangeListener() {
        oObj.addPropertySetInfoChangeListener(listener);
        return true;
    }

    public boolean _removePropertiesChangeListener() {
        oObj.removePropertySetInfoChangeListener(listener);
        return true;
    }

    private class PropertySetInfoChangeListener implements XPropertySetInfoChangeListener {
        public boolean disposed = false;
        public boolean propChanged = false;

        public void disposing(com.sun.star.lang.EventObject eventObject) {
            disposed = true;
        }

        public void propertySetInfoChange(com.sun.star.beans.PropertySetInfoChangeEvent propertySetInfoChangeEvent) {
        }

    }

}
