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

#ifndef INCLUDED_FRAMEWORK_INC_UIELEMENT_EDITTOOLBARCONTROLLER_HXX
#define INCLUDED_FRAMEWORK_INC_UIELEMENT_EDITTOOLBARCONTROLLER_HXX

#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>

#include <uielement/complextoolbarcontroller.hxx>
#include <vcl/edit.hxx>

class ToolBox;

namespace framework
{

class EditControl;

class IEditListener
{
    public:
        virtual void Modify() = 0;
        virtual void KeyInput( const KeyEvent& rKEvt ) = 0;
        virtual void GetFocus() = 0;
        virtual void LoseFocus() = 0;
        virtual bool PreNotify( NotifyEvent& rNEvt ) = 0;

    protected:
        ~IEditListener() {}
};

class EditToolbarController : public IEditListener,
                              public ComplexToolbarController

{
    public:
        EditToolbarController( const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rxContext,
                               const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                               ToolBox* pToolBar,
                               sal_uInt16 nID,
                               sal_Int32 nWidth,
                               const OUString& aCommand );
        virtual ~EditToolbarController();

        // XComponent
        virtual void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        // IEditListener
        virtual void Modify() SAL_OVERRIDE;
        virtual void KeyInput( const KeyEvent& rKEvt ) SAL_OVERRIDE;
        virtual void GetFocus() SAL_OVERRIDE;
        virtual void LoseFocus() SAL_OVERRIDE;
        virtual bool PreNotify( NotifyEvent& rNEvt ) SAL_OVERRIDE;

    protected:
        virtual void executeControlCommand( const ::com::sun::star::frame::ControlCommand& rControlCommand ) SAL_OVERRIDE;
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue> getExecuteArgs(sal_Int16 KeyModifier) const SAL_OVERRIDE;

    private:
        EditControl*    m_pEditControl;
};

}

#endif // INCLUDED_FRAMEWORK_INC_UIELEMENT_EDITTOOLBARCONTROLLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
