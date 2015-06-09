/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "undorangename.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "docfunc.hxx"
#include "sc.hrc"

#include <o3tl/ptr_container.hxx>
#include <sfx2/app.hxx>

#include <memory>
#include <utility>

using ::std::unique_ptr;

ScUndoAllRangeNames::ScUndoAllRangeNames(
    ScDocShell* pDocSh,
    const std::map<OUString, ScRangeName*>& rOldNames,
    const boost::ptr_map<OUString, ScRangeName>& rNewNames) :
    ScSimpleUndo(pDocSh)
{
    std::map<OUString, ScRangeName*>::const_iterator itr, itrEnd;
    for (itr = rOldNames.begin(), itrEnd = rOldNames.end(); itr != itrEnd; ++itr)
    {
        unique_ptr<ScRangeName> p(new ScRangeName(*itr->second));
        o3tl::ptr_container::insert(maOldNames, itr->first, std::move(p));
    }

    boost::ptr_map<OUString, ScRangeName>::const_iterator it, itEnd;
    for (it = rNewNames.begin(), itEnd = rNewNames.end(); it != itEnd; ++it)
    {
        unique_ptr<ScRangeName> p(new ScRangeName(*it->second));
        o3tl::ptr_container::insert(maNewNames, it->first, std::move(p));
    }
}

ScUndoAllRangeNames::~ScUndoAllRangeNames()
{
}

void ScUndoAllRangeNames::Undo()
{
    DoChange(maOldNames);
}

void ScUndoAllRangeNames::Redo()
{
    DoChange(maNewNames);
}

void ScUndoAllRangeNames::Repeat(SfxRepeatTarget& /*rTarget*/)
{
}

bool ScUndoAllRangeNames::CanRepeat(SfxRepeatTarget& /*rTarget*/) const
{
    return false;
}

OUString ScUndoAllRangeNames::GetComment() const
{
    return ScGlobal::GetRscString(STR_UNDO_RANGENAMES);
}

void ScUndoAllRangeNames::DoChange(const boost::ptr_map<OUString, ScRangeName>& rNames)
{
    ScDocument& rDoc = pDocShell->GetDocument();

    rDoc.PreprocessRangeNameUpdate();
    rDoc.SetAllRangeNames(rNames);
    rDoc.CompileHybridFormula();

    SfxGetpApp()->Broadcast(SfxSimpleHint(SC_HINT_AREAS_CHANGED));
}

ScUndoAddRangeData::ScUndoAddRangeData(ScDocShell* pDocSh, ScRangeData* pRangeData, SCTAB nTab) :
    ScSimpleUndo(pDocSh),
    mpRangeData(new ScRangeData(*pRangeData)),
    mnTab(nTab)
{

}

ScUndoAddRangeData::~ScUndoAddRangeData()
{
    delete mpRangeData;
}

void ScUndoAddRangeData::Undo()
{
    ScDocument& rDoc = pDocShell->GetDocument();
    ScRangeName* pRangeName = NULL;
    if (mnTab == -1)
    {
        pRangeName = rDoc.GetRangeName();
    }
    else
    {
        pRangeName = rDoc.GetRangeName( mnTab );
    }
    pRangeName->erase(*mpRangeData);
    SfxGetpApp()->Broadcast( SfxSimpleHint( SC_HINT_AREAS_CHANGED ) );

}

void ScUndoAddRangeData::Redo()
{
    ScDocument& rDoc = pDocShell->GetDocument();
    ScRangeName* pRangeName = NULL;
    if (mnTab == -1)
    {
        pRangeName = rDoc.GetRangeName();
    }
    else
    {
        pRangeName = rDoc.GetRangeName( mnTab );
    }
    pRangeName->insert(new ScRangeData(*mpRangeData));
    SfxGetpApp()->Broadcast( SfxSimpleHint( SC_HINT_AREAS_CHANGED ) );
}

void ScUndoAddRangeData::Repeat(SfxRepeatTarget& /*rTarget*/)
{
}

bool ScUndoAddRangeData::CanRepeat(SfxRepeatTarget& /*rTarget*/) const
{
    return false;
}

OUString ScUndoAddRangeData::GetComment() const
{
    return ScGlobal::GetRscString(STR_UNDO_RANGENAMES);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
