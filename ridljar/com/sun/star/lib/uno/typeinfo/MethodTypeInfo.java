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
package com.sun.star.lib.uno.typeinfo;

import com.sun.star.uno.Type;

public class MethodTypeInfo extends TypeInfo
{
    protected int m_index;
    private final Type m_unoType; // @since UDK 3.2

    /**
       Create a method type info with a UNO return type that cannot
       unambiguously be represented as a Java&nbsp;1.2 type.

       @param name the name of this method; must not be <code>null</code>

       @param index the index among the direct members

       @param flags any flags (<code>ONEWAY</code>, <code>UNSIGNED</code>,
       <code>ANY</code>, <code>INTERFACE</code>)

       @param unoType the exact UNO return type; or <code>null</code> if the UNO
       type is already unambiguously represented by the Java&nbsp;1.2 type

       @since UDK 3.2
     */
    public MethodTypeInfo(String name, int index, int flags, Type unoType) {
        super(name, flags);
        m_index = index;
        m_unoType = unoType;
    }

    public MethodTypeInfo(String name, int index, int flags)
    {
        this(name, index, flags, null);
    }

    public int getIndex()
    {
        return m_index;
    }

    public boolean isReturnUnsigned()
    {
        return isUnsigned();
    }

    public boolean isOneway()
    {
        return (m_flags & TypeInfo.ONEWAY) != 0;
    }

    public boolean isConst()
    {
        return (m_flags & TypeInfo.CONST) != 0;
    }

    /**
       Get the exact UNO return type of this method type info, in case it cannot
       unambiguously be represented as a Java&nbsp;1.2 type.

       @return the exact UNO return type of this method type info, or
       <code>null</code> if the UNO type is already unambiguously represented by
       the Java&nbsp;1.2 type

       @since UDK 3.2
     */
    public final Type getUnoType() {
        return m_unoType;
    }
}


