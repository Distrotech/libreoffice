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
#ifndef INCLUDED_CHART2_SOURCE_INC_MEDIADESCRIPTORHELPER_HXX
#define INCLUDED_CHART2_SOURCE_INC_MEDIADESCRIPTORHELPER_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include "charttoolsdllapi.hxx"

/*
* This class helps to read and write the properties mentioned in the service description
* com.sun.star.document.MediaDescriptor from and to a sequence of PropertyValues.
* Properties that are not mentioned in the service description
* are stored in the member AdditionalProperties.
*
* As an additional feature this helper class can generate a reduced sequence of PropertyValues
* that does not contain properties which are known to be only view relevant. This
* reduced sequence than might be attached to a model directly.
*/

namespace apphelper
{

class OOO_DLLPUBLIC_CHARTTOOLS MediaDescriptorHelper
{
private:
    //MediaDescriptorHelper(){};
public:
    MediaDescriptorHelper( const ::com::sun::star::uno::Sequence<
                            ::com::sun::star::beans::PropertyValue > & rMediaDescriptor );

    ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue > getReducedForModel();

public:
    //all properties given in the constructor are stored in the following three sequences

    ::com::sun::star::uno::Sequence<
        ::com::sun::star::beans::PropertyValue >
                        m_aRegularProperties; //these are the properties which are described in service com.sun.star.document.MediaDescriptor and not marked as deprecated

    ::com::sun::star::uno::Sequence<
        ::com::sun::star::beans::PropertyValue >
                        m_aDeprecatedProperties; //these are properties which are described in service com.sun.star.document.MediaDescriptor but are marked as deprecated

    ::com::sun::star::uno::Sequence<
        ::com::sun::star::beans::PropertyValue >
                        m_aAdditionalProperties; //these are properties which are not described in service com.sun.star.document.MediaDescriptor

    //properties which should be given to a model are additionally stored in this sequence (not documented properties and deprecated properties are not included!)
    ::com::sun::star::uno::Sequence<
        ::com::sun::star::beans::PropertyValue >
                        m_aModelProperties; //these are properties which are not described in service com.sun.star.document.MediaDescriptor

    //@todo define this for debug only, except URL
    bool            AsTemplate; //document is a template.
    bool            ISSET_AsTemplate;
    OUString     Author;
    bool            ISSET_Author;
    OUString     CharacterSet; //identifier of used character set.
    bool            ISSET_CharacterSet;
    OUString     Comment;
    bool            ISSET_Comment;

    ::com::sun::star::uno::Any
                        ComponentData;
    bool            ISSET_ComponentData;
    OUString     FileName; //deprecated, same as url
    bool            ISSET_FileName;
    ::com::sun::star::uno::Any
                        FilterData;
    bool            ISSET_FilterData;
    OUString     FilterName; //internal filter name.
    bool            ISSET_FilterName;
    OUString     FilterFlags;//deprecated,
    bool            ISSET_FilterFlags;
    OUString     FilterOptions;
    bool            ISSET_FilterOptions;
            //not documented ... @todo remove?
    OUString     FrameName; //name of target frame.
    bool            ISSET_FrameName;
    bool            Hidden; //load document, invisible.
    bool            ISSET_Hidden;

    OUString     HierarchicalDocumentName;
    bool            ISSET_HierarchicalDocumentName;

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >
                        OutputStream; //a stream to receive the document data for saving
    bool            ISSET_OutputStream;
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >
                        InputStream; //content of document.
    bool            ISSET_InputStream;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                        InteractionHandler; //::com::sun::star::task::XInteractionHandler
    bool            ISSET_InteractionHandler;

    OUString     JumpMark;   //specifies the name of a mark within the document where the first view is to position itself.
    bool            ISSET_JumpMark;
    OUString     MediaType; //mime type.
    bool            ISSET_MediaType;
    OUString     OpenFlags; //deprecated
    bool            ISSET_OpenFlags;
    bool            OpenNewView; //opens a new view for an already loaded document.
    bool            ISSET_OpenNewView;
    bool            Overwrite; //opens a new view for an already loaded document.
    bool            ISSET_Overwrite;
    OUString     Password;
    bool            ISSET_Password;

        //not documented ... @todo remove?
    ::com::sun::star::awt::Rectangle
                        PosSize; //position and size of document window.
    bool            ISSET_PosSize;

    ::com::sun::star::uno::Sequence< sal_Int8 >
                        PostData; //contains the data for HTTP post method as a sequence of bytes.
    bool            ISSET_PostData;
    OUString     PostString; //deprecated, contains the data for HTTP post method as a sequence of bytes.
    bool            ISSET_PostString;
    bool            Preview; //show preview.
    bool            ISSET_Preview;
    bool            ReadOnly; //open document readonly.
    bool            ISSET_ReadOnly;
    OUString     Referer; //name of document referrer.
    bool            ISSET_Referer;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                        StatusIndicator; //::com::sun::star::task::XStatusIndicator
    bool            ISSET_StatusIndicator;
        //not documented ... @todo remove?
    bool            Silent; //prevents dialogs to query for more information.
    bool            ISSET_Silent;
    OUString     TemplateName; //deprecated, name of the template instead of the URL.
    bool            ISSET_TemplateName;
    OUString     TemplateRegionName; //deprecated, name of the region of the template.
    bool            ISSET_TemplateRegionName;
    bool            Unpacked;
    bool            ISSET_Unpacked;
    OUString     URL;// FileName, URL of the document.
    bool            ISSET_URL;
    sal_Int16           Version; //storage version.
    bool            ISSET_Version;

    ::com::sun::star::uno::Any
                        ViewData;
    bool            ISSET_ViewData;
    sal_Int16           ViewId; //id of the initial view.
    bool            ISSET_ViewId;

    // new framework objects
    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >
                        Storage;
    bool            ISSET_Storage;
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >
                        Stream;
    bool            ISSET_Stream;

    // undocumented SFX Properties
    ::com::sun::star::uno::Sequence< sal_Int32 >
                        WinExtent;
    bool            ISSET_WinExtent;
    bool            SetEmbedded;
    bool            ISSET_SetEmbedded;

protected:
SAL_DLLPRIVATE void impl_init();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
