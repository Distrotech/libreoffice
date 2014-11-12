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
#ifndef INCLUDED_SFX2_SOURCE_DIALOG_FILEDLGIMPL_HXX
#define INCLUDED_SFX2_SOURCE_DIALOG_FILEDLGIMPL_HXX

#include <vcl/timer.hxx>
#include <vcl/graph.hxx>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/beans/StringPair.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerListener.hpp>
#include <com/sun/star/ui/dialogs/XDialogClosedListener.hpp>
#include <sfx2/fcontnr.hxx>
#include <sfx2/filedlghelper.hxx>
#include <comphelper/sequenceasvector.hxx>

class SfxFilterMatcher;
class GraphicFilter;
class FileDialogHelper;

namespace sfx2
{
    typedef ::com::sun::star::beans::StringPair FilterPair;

    class FileDialogHelper_Impl :
        public ::cppu::WeakImplHelper2<
            ::com::sun::star::ui::dialogs::XFilePickerListener,
            ::com::sun::star::ui::dialogs::XDialogClosedListener >
    {
        friend class FileDialogHelper;

        ::com::sun::star::uno::Reference < ::com::sun::star::ui::dialogs::XFilePicker > mxFileDlg;
        ::com::sun::star::uno::Reference < ::com::sun::star::container::XNameAccess >   mxFilterCFG;

        std::vector< FilterPair >   maFilters;

        SfxFilterMatcher*           mpMatcher;
        GraphicFilter*              mpGraphicFilter;
        FileDialogHelper*           mpAntiImpl;
        Window*                     mpPreferredParentWindow;

        ::comphelper::SequenceAsVector< OUString > mlLastURLs;

        OUString             maPath;
        OUString             maFileName;
        OUString             maCurFilter;
        OUString             maSelectFilter;
        OUString             maButtonLabel;

        Timer                       maPreviewTimer;
        Graphic                     maGraphic;

        const short                 m_nDialogType;

        SfxFilterFlags              m_nMustFlags;
        SfxFilterFlags              m_nDontFlags;

        ImplSVEvent *               mnPostUserEventId;

        ErrCode                     mnError;

        FileDialogHelper::Context   meContext;

        bool                    mbHasPassword           : 1;
        bool                    mbIsPwdEnabled          : 1;
        bool                    m_bHaveFilterOptions    : 1;
        bool                    mbHasVersions           : 1;
        bool                    mbHasAutoExt            : 1;
        bool                    mbAddGraphicFilter      : 1;
        bool                    mbHasPreview            : 1;
        bool                    mbShowPreview           : 1;
        bool                    mbIsSaveDlg             : 1;
        bool                    mbIsSaveACopyDlg        : 1;
        bool                    mbExport                : 1;

        bool                    mbDeleteMatcher         : 1;
        bool                    mbInsert                : 1;
        bool                    mbSystemPicker          : 1;
        bool                    mbPwdCheckBoxState      : 1;
        bool                    mbSelection             : 1;
        bool                    mbSelectionEnabled      : 1;
        bool                    mbHasSelectionBox       : 1;
        bool                    mbSelectionFltrEnabled  : 1;

    private:
        void                    addFilters( const OUString& rFactory,
                                            SfxFilterFlags nMust,
                                            SfxFilterFlags nDont );
        void                    addFilter( const OUString& rFilterName,
                                           const OUString& rExtension );
        void                    addGraphicFilter();
        void                    enablePasswordBox( bool bInit );
        void                    updateFilterOptionsBox();
        void                    updateExportButton();
        void                    updateSelectionBox();
        void                    updateVersions();
        void                    updatePreviewState( bool _bUpdatePreviewWindow = true );
        void                    dispose();

        void                    loadConfig();
        void                    saveConfig();

        const SfxFilter*        getCurentSfxFilter();
        bool                updateExtendedControl( sal_Int16 _nExtendedControlId, bool _bEnable );

        ErrCode                 getGraphic( const OUString& rURL, Graphic& rGraphic ) const;
        void                    setDefaultValues();

        void                    preExecute();
        void                    postExecute( sal_Int16 _nResult );
        sal_Int16               implDoExecute();
        void                    implStartExecute();

        void                    correctVirtualDialogType();

        void                    setControlHelpIds( const sal_Int16* _pControlId, const char** _pHelpId );

        bool                CheckFilterOptionsCapability( const SfxFilter* _pFilter );

        bool                isInOpenMode() const;
        OUString                getCurrentFilterUIName() const;

        void                    LoadLastUsedFilter( const OUString& _rContextIdentifier );
        void                    SaveLastUsedFilter( const OUString& _rContextIdentifier );
        void                    SaveLastUsedFilter( void );

        void                    implInitializeFileName( );

        void                    verifyPath( );

        void                    implGetAndCacheFiles( const ::com::sun::star::uno::Reference< XInterface >& xPicker  ,
                                                      std::vector<OUString>&               rpURLList,
                                                      const SfxFilter*                    pFilter  );

        DECL_LINK(TimeOutHdl_Impl, void *);
        DECL_LINK( HandleEvent, FileDialogHelper* );
        DECL_LINK( InitControls, void* );

    public:
        // XFilePickerListener methods
        virtual void SAL_CALL               fileSelectionChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent ) throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
        virtual void SAL_CALL               directoryChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent ) throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
        virtual OUString SAL_CALL    helpRequested( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent ) throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
        virtual void SAL_CALL               controlStateChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent ) throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
        virtual void SAL_CALL               dialogSizeChanged() throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        // XDialogClosedListener methods
        virtual void SAL_CALL               dialogClosed( const ::com::sun::star::ui::dialogs::DialogClosedEvent& _rEvent ) throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        // XEventListener methods
        virtual void SAL_CALL       disposing( const ::com::sun::star::lang::EventObject& Source ) throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        // handle XFilePickerListener events
        void                    handleFileSelectionChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent );
        void                    handleDirectoryChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent );
        OUString         handleHelpRequested( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent );
        void                    handleControlStateChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent );
        void                    handleDialogSizeChanged();

        // Own methods
                                FileDialogHelper_Impl(
                                    FileDialogHelper* _pAntiImpl,
                                    const short nDialogType,
                                    sal_Int64 nFlags,
                                    sal_Int16 nDialog = SFX2_IMPL_DIALOG_CONFIG,
                                    Window* _pPreferredParentWindow = NULL,
                                    const OUString& sStandardDir = OUString(),
                                    const ::com::sun::star::uno::Sequence< OUString >&   rBlackList = ::com::sun::star::uno::Sequence< OUString >()
                                );
        virtual                 ~FileDialogHelper_Impl();

        ErrCode                 execute( std::vector<OUString>& rpURLList,
                                         SfxItemSet *&   rpSet,
                                         OUString&       rFilter );
        ErrCode                 execute();

        void                    setFilter( const OUString& rFilter );

        /** sets the directory which should be browsed

            <p>If the given path does not point to a valid (existent and accessible) folder, the request
            is silently dropped</p>
        */
        void                    displayFolder( const OUString& rPath );
        void                    setFileName( const OUString& _rFile );

        OUString                getPath() const;
        OUString                getFilter() const;
        void                    getRealFilter( OUString& _rFilter ) const;

        ErrCode                 getGraphic( Graphic& rGraphic ) const;
        void                    createMatcher( const OUString& rFactory );

        bool                isShowFilterExtensionEnabled() const;
        void                    addFilterPair( const OUString& rFilter,
                                               const OUString& rFilterWithExtension );
        OUString         getFilterName( const OUString& rFilterWithExtension ) const;
        OUString         getFilterWithExtension( const OUString& rFilter ) const;

        void                    SetContext( FileDialogHelper::Context _eNewContext );

        inline bool         isSystemFilePicker() const { return mbSystemPicker; }
        inline bool         isPasswordEnabled() const { return mbIsPwdEnabled; }
    };

}   // end of namespace sfx2

#endif // INCLUDED_SFX2_SOURCE_DIALOG_FILEDLGIMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */