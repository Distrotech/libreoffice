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
package com.sun.star.wizards.db;

import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.sdbc.DataType;
import com.sun.star.wizards.common.Properties;
import com.sun.star.wizards.common.PropertyNames;
// import com.sun.star.wizards.db.TypeInspector;

public class ColumnPropertySet
{

    TypeInspector oTypeInspector;
    public XPropertySet xPropertySet;
    private int nType;
    private String sTypeName = PropertyNames.EMPTY_STRING;

    public ColumnPropertySet(TypeInspector _oTypeInspector, XPropertySet _xPropertySet)
    {
        xPropertySet = _xPropertySet;
        oTypeInspector = _oTypeInspector;
    }

    private PropertyValue[] propertySet2PropertyValueArray(XPropertySet _xNewPropertySet) throws com.sun.star.uno.Exception
    {
        Property[] props = _xNewPropertySet.getPropertySetInfo().getProperties();
        PropertyValue[] ret = new PropertyValue[props.length];
        for (int i = 0; i < props.length; i++)
        {
            PropertyValue val = new PropertyValue();
            val.Name = props[i].Name;
            val.Value = _xNewPropertySet.getPropertyValue(val.Name);
            ret[i] = val;
        }
        return ret;
    }

    private void assignPropertyValues(String _sNewName, PropertyValue[] _aNewColPropertyValues, boolean _bsetDefaultProperties)
    {
        try
        {
            nType = ((Integer) Properties.getPropertyValue(_aNewColPropertyValues, "Type")).intValue();
            nType = oTypeInspector.convertDataType(nType);
            if (Properties.hasPropertyValue(_aNewColPropertyValues, "TypeName"))
            {
                sTypeName = (String) Properties.getPropertyValue(_aNewColPropertyValues, "TypeName");
            }
            Integer precision = null;
            if (Properties.hasPropertyValue(_aNewColPropertyValues, "Precision"))
            {
                precision = (Integer) Properties.getPropertyValue(_aNewColPropertyValues, "Precision");

            }
            if ((nType == DataType.VARCHAR) && (precision == null || precision.intValue() == 0))
            {
                precision = 50;
            }
            if (precision != null)
            {
                xPropertySet.setPropertyValue("Precision", precision);
            }
            setType(nType, sTypeName, precision);
            for (int i = 0; i < _aNewColPropertyValues.length; i++)
            {
                String sPropName = _aNewColPropertyValues[i].Name;
                if (_sNewName != null && sPropName.equals(PropertyNames.PROPERTY_NAME))
                {
                    xPropertySet.setPropertyValue(PropertyNames.PROPERTY_NAME, _sNewName);
                }
                else if (sPropName.equals("Precision"))
                {
                    // do nothing, see above
                }
                else if ((!sPropName.equals("Type")) && (!sPropName.equals("TypeName")))
                {
                    Object oColValue = _aNewColPropertyValues[i].Value;
                    assignPropertyValue(sPropName, oColValue);
                }
            }
            if (_bsetDefaultProperties)
            {
                assignPropertyValue("IsNullable", new Integer(oTypeInspector.isNullable(xPropertySet)));
            }
        }
        catch (Exception e)
        {
            e.printStackTrace(System.err);
        }

    }

    public void assignPropertyValues(PropertyValue[] _aNewColPropertyValues, boolean _bsetDefaultProperties)
    {
        assignPropertyValues(null /* dont change the name */, _aNewColPropertyValues, _bsetDefaultProperties);
    }

    public void assignNewPropertySet(String _sNewName, XPropertySet _xNewPropertySet)
    {
        try
        {
            assignPropertyValues(
                    _sNewName, propertySet2PropertyValueArray(_xNewPropertySet), false /*setDefaultProperties*/);
        }
        catch (Exception e)
        {
            e.printStackTrace(System.err);
        }
    }

    private int getPrecision()
    {
        try
        {
            return ((Integer) xPropertySet.getPropertyValue("Precision")).intValue();
        }
        catch (Exception e)
        {
            e.printStackTrace(System.err);
            return 0;
        }
    }

    private void setType(int _nType, String _sTypeName, Integer precision)
    {
        if (_sTypeName.equals(PropertyNames.EMPTY_STRING))
        {
            sTypeName = oTypeInspector.getDefaultTypeName(nType, precision);
        }
        else
        {
            sTypeName = _sTypeName;
        }
        nType = oTypeInspector.getDataType(sTypeName);
        assignPropertyValue("Type", new Integer(nType));
        assignPropertyValue("TypeName", sTypeName);
    }

    private void assignPropertyValue(String _spropname, Object _oValue)
    {
        try
        {
            if (_spropname.equals("Type"))
            {
                nType = ((Integer) _oValue).intValue();
                xPropertySet.setPropertyValue("Type", new Integer(nType));
            }
            else if (_spropname.equals(PropertyNames.PROPERTY_NAME))
            {
                String sName = (String) _oValue;
                if (!sName.equals(PropertyNames.EMPTY_STRING))
                {
                    xPropertySet.setPropertyValue(PropertyNames.PROPERTY_NAME, sName);
                }
            }
            else if (_spropname.equals("Scale"))
            {
                int nScale = ((Integer) _oValue).intValue();
                nScale = oTypeInspector.getScale(xPropertySet);
                xPropertySet.setPropertyValue("Scale", new Integer(nScale));
            }
            else if (_spropname.equals("IsNullable"))
            {
                int nNullability = ((Integer) _oValue).intValue();
                nNullability = oTypeInspector.getNullability(xPropertySet, nNullability);
                xPropertySet.setPropertyValue("IsNullable", new Integer(nNullability));
            }
            else if (_spropname.equals("TypeName"))
            {
                String sTypeName = (String) _oValue;
                xPropertySet.setPropertyValue("TypeName", sTypeName);
            }
            else
            {
                xPropertySet.setPropertyValue(_spropname, _oValue);
            }
        }
        catch (Exception e)
        {
            e.printStackTrace(System.err);
        }
    }

    private int getType()
    {
        return nType;
    }
}
