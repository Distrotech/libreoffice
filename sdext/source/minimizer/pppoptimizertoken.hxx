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

#ifndef INCLUDED_SDEXT_SOURCE_MINIMIZER_PPPOPTIMIZERTOKEN_HXX
#define INCLUDED_SDEXT_SOURCE_MINIMIZER_PPPOPTIMIZERTOKEN_HXX

#include <rtl/ustring.hxx>

enum PPPOptimizerTokenEnum
{
    TK_rdmNavi,
    TK_lnNavSep1,
    TK_lnNavSep2,
    TK_btnNavBack,
    TK_btnNavNext,
    TK_btnNavFinish,
    TK_btnNavCancel,

    TK_FixedText0Pg0,
    TK_FixedText1Pg0,
    TK_Separator1Pg0,
    TK_FixedText2Pg0,
    TK_ListBox0Pg0,
    TK_Button0Pg0,
    TK_RadioButton0Pg1,
    TK_RadioButton1Pg1,
    TK_FixedText0Pg1,
    TK_CheckBox1Pg1,
    TK_FixedText1Pg1,
    TK_FixedText2Pg1,
    TK_FormattedField0Pg1,
    TK_ComboBox0Pg1,
    TK_CheckBox2Pg1,
    TK_FixedText0Pg2,
    TK_FixedText1Pg2,
    TK_CheckBox0Pg2,
    TK_RadioButton0Pg2,
    TK_RadioButton1Pg2,
    TK_FixedText0Pg3,
    TK_CheckBox0Pg3,
    TK_FixedText1Pg3,
    TK_CheckBox1Pg3,
    TK_FixedText2Pg3,
    TK_CheckBox2Pg3,
    TK_CheckBox3Pg3,
    TK_ListBox0Pg3,
    TK_Separator0Pg4,
    TK_RadioButton0Pg4,
    TK_RadioButton1Pg4,
    TK_Separator1Pg4,
    TK_FixedText0Pg4,
    TK_FixedText1Pg4,
    TK_FixedText2Pg4,
    TK_FixedText3Pg4,
    TK_FixedText4Pg4,
    TK_FixedText5Pg4,
    TK_FixedText6Pg4,
    TK_FixedText7Pg4,
    TK_FixedText8Pg4,
    TK_CheckBox1Pg4,
    TK_ComboBox0Pg4,

    TK__blank,
    TK__self,
    TK_Activated,
    TK_Align,
    TK_Alpha,
    TK_Animated,
    TK_Background,
    TK_BitmapPath,
    TK_Border,
    TK_Closeable,
    TK_ColorMode,
    TK_Complete,
    TK_Compression,
    TK_CurrentItemID,
    TK_DefaultButton,
    TK_DocumentService,
    TK_Dropdown,
    TK_EffectiveValue,
    TK_EffectiveMin,
    TK_EffectiveMax,
    TK_Enabled,
    TK_Extensions,
    TK_FileSizeDestination,
    TK_FileSizeSource,
    TK_FillBitmap,
    TK_FillBitmapLogicalSize,
    TK_FillBitmapMode,
    TK_FillBitmapSizeX,
    TK_FillBitmapSizeY,
    TK_FillBitmapURL,
    TK_FillStyle,
    TK_FilterData,
    TK_FilterName,
    TK_Flags,
    TK_FontDescriptor,
    TK_Graphic,
    TK_GraphicCrop,
    TK_GraphicCropLogic,
    TK_GraphicURL,
    TK_GraphicStreamURL,
    TK_Height,
    TK_Hidden,
    TK_ID,
    TK_ImageURL,
    TK_InformationDialog,
    TK_InputStream,
    TK_Interlaced,
    TK_IsInternal,
    TK_Label,
    TK_LineCount,
    TK_LogicalHeight,
    TK_LogicalWidth,
    TK_LogicalSize,
    TK_MimeType,
    TK_Moveable,
    TK_MultiLine,
    TK_MultiSelection,
    TK_Name,
    TK_Orientation,
    TK_OutputStream,
    TK_PixelHeight,
    TK_PixelWidth,
    TK_PositionX,
    TK_PositionY,
    TK_Progress,
    TK_ProgressValue,
    TK_ProgressValueMax,
    TK_ProgressValueMin,
    TK_PushButtonType,
    TK_Quality,
    TK_ReadOnly,
    TK_Repeat,
    TK_ScaleImage,
    TK_SelectedItems,
    TK_Settings,
    TK_Size100thMM,
    TK_SizePixel,
    TK_Spin,
    TK_Step,
    TK_State,
    TK_StatusDispatcher,
    TK_StringItemList,
    TK_Strings,
    TK_TabIndex,
    TK_Template,
    TK_Text,
    TK_Title,
    TK_Transparent,
    TK_Type,
    TK_UIName,
    TK_Value,
    TK_Width,
    TK_ZOrder,

    TK_LastUsedSettings,
    TK_Settings_Templates,
    TK_Settings_Templates_,
    TK_JPEGCompression,
    TK_JPEGQuality,
    TK_RemoveCropArea,
    TK_ImageResolution,
    TK_EmbedLinkedGraphics,
    TK_OLEOptimization,
    TK_OLEOptimizationType,
    TK_DeleteUnusedMasterPages,
    TK_DeleteHiddenSlides,
    TK_DeleteNotesPages,
    TK_CustomShowName,
    TK_SaveAs,
    TK_SaveAsURL,
    TK_OpenNewDocument,
    TK_EstimatedFileSize,

    TK_Status,
    TK_Pages,
    TK_CurrentPage,
    TK_GraphicObjects,
    TK_CurrentGraphicObject,
    TK_OLEObjects,
    TK_CurrentOLEObject,

    STR_SUN_OPTIMIZATION_WIZARD2,
    STR_STEPS,
    STR_BACK,
    STR_NEXT,
    STR_FINISH,
    STR_CANCEL,
    STR_INTRODUCTION,
    STR_INTRODUCTION_T,
    STR_CHOSE_SETTINGS,
    STR_REMOVE,
    STR_GRAPHIC_OPTIMIZATION,
    STR_IMAGE_OPTIMIZATION,
    STR_LOSSLESS_COMPRESSION,
    STR_JPEG_COMPRESSION,
    STR_QUALITY,
    STR_REMOVE_CROP_AREA,
    STR_IMAGE_RESOLUTION,
    STR_IMAGE_RESOLUTION_0,
    STR_IMAGE_RESOLUTION_1,
    STR_IMAGE_RESOLUTION_2,
    STR_IMAGE_RESOLUTION_3,
    STR_EMBED_LINKED_GRAPHICS,
    STR_OLE_OBJECTS,
    STR_OLE_OPTIMIZATION,
    STR_OLE_REPLACE,
    STR_ALL_OLE_OBJECTS,
    STR_ALIEN_OLE_OBJECTS_ONLY,
    STR_OLE_OBJECTS_DESC,
    STR_NO_OLE_OBJECTS_DESC,
    STR_SLIDES,
    STR_CHOOSE_SLIDES,
    STR_MASTER_PAGES,
    STR_DELETE_MASTER_PAGES,
    STR_DELETE_NOTES_PAGES,
    STR_DELETE_HIDDEN_SLIDES,
    STR_CUSTOM_SHOW,
    STR_SUMMARY,
    STR_SUMMARY_TITLE,
    STR_PROGRESS,
    STR_OBJECTS_OPTIMIZED,
    STR_APPLY_TO_CURRENT,
    STR_AUTOMATICALLY_OPEN,
    STR_SAVE_SETTINGS,
    STR_SAVE_AS,
    STR_DELETE_SLIDES,
    STR_OPTIMIZE_IMAGES,
    STR_CREATE_REPLACEMENT,
    STR_CURRENT_FILESIZE,
    STR_ESTIMATED_FILESIZE,
    STR_MB,
    STR_MY_SETTINGS,
    STR_DEFAULT_SESSION,
    STR_MODIFY_WARNING,
    STR_YES,
    STR_OK,
    STR_INFO_1,
    STR_INFO_2,
    STR_INFO_3,
    STR_INFO_4,
    STR_DUPLICATING_PRESENTATION,
    STR_DELETING_SLIDES,
    STR_OPTIMIZING_GRAPHICS,
    STR_CREATING_OLE_REPLACEMENTS,
    STR_FILESIZESEPARATOR,

    TK_NotFound
};

PPPOptimizerTokenEnum TKGet( const OUString& );
OUString TKGet( const PPPOptimizerTokenEnum );

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
