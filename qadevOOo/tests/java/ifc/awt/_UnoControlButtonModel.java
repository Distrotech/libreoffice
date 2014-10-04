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

package ifc.awt;

import lib.MultiPropertyTest;

/*
* Testing <code>com.sun.star.awt.UnoControlButtonModel</code>
* service properties :
* <ul>
*  <li><code> BackgroundColor</code></li>
*  <li><code> DefaultButton</code></li>
*  <li><code> DefaultControl</code></li>
*  <li><code> Enabled</code></li>
*  <li><code> FontDescriptor</code></li>
*  <li><code> Label</code></li>
*  <li><code> Printable</code></li>
*  <li><code> Tabstop</code></li>
*  <li><code> TextColor</code></li>
* </ul>
* @see com.sun.star.awt.UnoControlButtonModel
*/
public class _UnoControlButtonModel extends MultiPropertyTest {

    /**
    * Redefined method returns value, that differs from property value.
    */
    protected PropertyTester ColorTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue) {
            if (util.ValueComparer.equalValue(oldValue, new Integer(17)))
                return new Integer(25);
            else
                return new Integer(17);
        }
    };

    /**
    * Redefined method returns value, that differs from property value.
    */
    protected PropertyTester BoolTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue) {
            if (util.ValueComparer.equalValue(oldValue, new Boolean(true)))
                return new Boolean(false);
            else
                return new Boolean(true);
        }
    };

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _BackgroundColor() {
        log.println("Testing with custom Property tester") ;
        testProperty("BackgroundColor", ColorTester) ;
    }

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _TextLineColor() {
        log.println("Testing with custom Property tester") ;
        testProperty("TextLineColor", ColorTester) ;
    }

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _TextColor() {
        log.println("Testing with custom Property tester") ;
        testProperty("TextColor", ColorTester) ;
    }

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _Tabstop() {
        log.println("Testing with custom Property tester") ;
        testProperty("Tabstop", BoolTester) ;
    }

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _PushButtonType() {
        log.println("Testing with custom Property tester") ;
        testProperty("PushButtonType",
            new Short((short)0), new Short((short)1));
    }


}

