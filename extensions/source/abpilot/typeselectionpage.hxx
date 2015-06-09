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

#ifndef INCLUDED_EXTENSIONS_SOURCE_ABPILOT_TYPESELECTIONPAGE_HXX
#define INCLUDED_EXTENSIONS_SOURCE_ABPILOT_TYPESELECTIONPAGE_HXX

#include "abspage.hxx"
#include "addresssettings.hxx"
#include <vcl/edit.hxx>


namespace abp
{



    //= TypeSelectionPage

    class TypeSelectionPage : public AddressBookSourcePage
    {
    protected:
        RadioButton* m_pEvolution;
        RadioButton* m_pEvolutionGroupwise;
        RadioButton* m_pEvolutionLdap;
        RadioButton* m_pMORK;
        RadioButton* m_pThunderbird;
        RadioButton* m_pKab;
        RadioButton* m_pMacab;
        RadioButton* m_pLDAP;
        RadioButton* m_pOutlook;
        RadioButton* m_pOE;
        RadioButton* m_pOther;

        struct ButtonItem {
            RadioButton *m_pItem;
            AddressSourceType m_eType;
            bool         m_bVisible;

            ButtonItem( RadioButton *pItem,
                        AddressSourceType eType,
                        bool         bVisible ) :
                    m_pItem( pItem ),
                    m_eType( eType ),
                    m_bVisible( bVisible )
            {}
        };

        ::std::vector< ButtonItem > m_aAllTypes;

    public:
        TypeSelectionPage( OAddessBookSourcePilot* _pParent );
        virtual ~TypeSelectionPage();

        // retrieves the currently selected type
        AddressSourceType   getSelectedType() const;

    protected:
        // OWizardPage overridables
        virtual void        initializePage() SAL_OVERRIDE;
        virtual bool        commitPage( ::svt::WizardTypes::CommitPageReason _eReason ) SAL_OVERRIDE;

        // TabDialog overridables
        virtual void        ActivatePage() SAL_OVERRIDE;
        virtual void        DeactivatePage() SAL_OVERRIDE;

        // OImportPage overridables
        virtual bool        canAdvance() const SAL_OVERRIDE;

    private:
        DECL_LINK( OnTypeSelected, void* );

        void                selectType( AddressSourceType _eType );
    };


}   // namespace abp


#endif // INCLUDED_EXTENSIONS_SOURCE_ABPILOT_TYPESELECTIONPAGE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
