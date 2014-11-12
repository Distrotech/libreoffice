/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 This file has been autogenerated by update_pch.sh . It is possible to edit it
 manually (such as when an include file has been moved/renamed/removed. All such
 manual changes will be rewritten by the next run of update_pch.sh (which presumably
 also fixes all possible problems, so it's usually better to use it).
*/

#include "boost/scoped_ptr.hpp"
#include "com/sun/star/document/XStorageBasedDocument.hpp"
#include "cppuhelper/factory.hxx"
#include "editeng/AccessibleStringWrap.hxx"
#include "editeng/borderline.hxx"
#include "editeng/boxitem.hxx"
#include "editeng/editobj.hxx"
#include "editeng/editstat.hxx"
#include "editeng/editview.hxx"
#include "editeng/fhgtitem.hxx"
#include "editeng/flditem.hxx"
#include "editeng/fontitem.hxx"
#include "editeng/forbiddencharacterstable.hxx"
#include "editeng/frmdiritem.hxx"
#include "editeng/lineitem.hxx"
#include "editeng/outlobj.hxx"
#include "editeng/shaditem.hxx"
#include "editeng/unolingu.hxx"
#include "editeng/writingmodeitem.hxx"
#include "editeng/xmlcnitm.hxx"
#include "officecfg/Office/Common.hxx"
#include "osl/diagnose.h"
#include "sal/config.h"
#include "sal/types.h"
#include "xmloff/DashStyle.hxx"
#include "xmloff/GradientStyle.hxx"
#include "xmloff/HatchStyle.hxx"
#include "xmloff/ImageStyle.hxx"
#include "xmloff/MarkerStyle.hxx"
#include "xmloff/attrlist.hxx"
#include "xmloff/nmspmap.hxx"
#include "xmloff/xmlmetae.hxx"
#include "xmloff/xmlnmspe.hxx"
#include "xmloff/xmltoken.hxx"
#include <algorithm>
#include <avmedia/mediaitem.hxx>
#include <avmedia/mediaplayer.hxx>
#include <avmedia/mediawindow.hxx>
#include <basegfx/color/bcolor.hxx>
#include <basegfx/color/bcolormodifier.hxx>
#include <basegfx/color/bcolortools.hxx>
#include <basegfx/curve/b2dcubicbezier.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <basegfx/matrix/b3dhommatrix.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/point/b3dpoint.hxx>
#include <basegfx/polygon/b2dlinegeometry.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolygonclipper.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <basegfx/polygon/b2dpolypolygoncutter.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/polygon/b3dpolygon.hxx>
#include <basegfx/polygon/b3dpolygontools.hxx>
#include <basegfx/polygon/b3dpolypolygontools.hxx>
#include <basegfx/range/b2drange.hxx>
#include <basegfx/range/b2drectangle.hxx>
#include <basegfx/range/b3drange.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/tools/unotools.hxx>
#include <basegfx/tuple/b2dtuple.hxx>
#include <basegfx/vector/b2dvector.hxx>
#include <basegfx/vector/b2enums.hxx>
#include <basegfx/vector/b3dvector.hxx>
#include <basic/sbxvar.hxx>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/unordered_map.hpp>
#include <cassert>
#include <cmath>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/awt/Gradient.hpp>
#include <com/sun/star/awt/InvalidateStyle.hpp>
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/awt/Size.hpp>
#include <com/sun/star/awt/TextAlign.hpp>
#include <com/sun/star/awt/XBitmap.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#include <com/sun/star/awt/XView.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/PropertyValues.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/chart/ChartAxisArrangeOrderType.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XIdentifierContainer.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/document/EventObject.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/document/XExporter.hpp>
#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/document/XGraphicObjectResolver.hpp>
#include <com/sun/star/document/XImporter.hpp>
#include <com/sun/star/document/XStorageBasedDocument.hpp>
#include <com/sun/star/drawing/BitmapMode.hpp>
#include <com/sun/star/drawing/CameraGeometry.hpp>
#include <com/sun/star/drawing/CircleKind.hpp>
#include <com/sun/star/drawing/ConnectorType.hpp>
#include <com/sun/star/drawing/DashStyle.hpp>
#include <com/sun/star/drawing/Direction3D.hpp>
#include <com/sun/star/drawing/DoubleSequence.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeAdjustmentValue.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeGluePointType.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeParameterPair.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeParameterType.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeSegment.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeSegmentCommand.hpp>
#include <com/sun/star/drawing/EnhancedCustomShapeTextFrame.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <com/sun/star/drawing/FlagSequence.hpp>
#include <com/sun/star/drawing/GluePoint2.hpp>
#include <com/sun/star/drawing/GraphicExportFilter.hpp>
#include <com/sun/star/drawing/GraphicFilterRequest.hpp>
#include <com/sun/star/drawing/Hatch.hpp>
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#include <com/sun/star/drawing/LineCap.hpp>
#include <com/sun/star/drawing/LineDash.hpp>
#include <com/sun/star/drawing/LineJoint.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/MeasureKind.hpp>
#include <com/sun/star/drawing/MeasureTextHorzPos.hpp>
#include <com/sun/star/drawing/MeasureTextVertPos.hpp>
#include <com/sun/star/drawing/NormalsKind.hpp>
#include <com/sun/star/drawing/PointSequence.hpp>
#include <com/sun/star/drawing/PointSequenceSequence.hpp>
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#include <com/sun/star/drawing/PolyPolygonShape3D.hpp>
#include <com/sun/star/drawing/Position3D.hpp>
#include <com/sun/star/drawing/ProjectionMode.hpp>
#include <com/sun/star/drawing/ShadeMode.hpp>
#include <com/sun/star/drawing/TextAnimationDirection.hpp>
#include <com/sun/star/drawing/TextAnimationKind.hpp>
#include <com/sun/star/drawing/TextFitToSizeType.hpp>
#include <com/sun/star/drawing/TextHorizontalAdjust.hpp>
#include <com/sun/star/drawing/TextVerticalAdjust.hpp>
#include <com/sun/star/drawing/TextureKind.hpp>
#include <com/sun/star/drawing/TextureKind2.hpp>
#include <com/sun/star/drawing/TextureMode.hpp>
#include <com/sun/star/drawing/TextureProjectionMode.hpp>
#include <com/sun/star/drawing/XCustomShapeEngine.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/drawing/XEnhancedCustomShapeDefaulter.hpp>
#include <com/sun/star/drawing/XGraphicExportFilter.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/embed/Aspects.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/EmbedMisc.hpp>
#include <com/sun/star/embed/EmbedStates.hpp>
#include <com/sun/star/embed/EntryInitModes.hpp>
#include <com/sun/star/embed/NoVisualAreaSizeException.hpp>
#include <com/sun/star/embed/XEmbedObjectCreator.hpp>
#include <com/sun/star/embed/XEmbedObjectFactory.hpp>
#include <com/sun/star/embed/XEmbedPersist.hpp>
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#include <com/sun/star/embed/XInplaceClient.hpp>
#include <com/sun/star/embed/XInplaceObject.hpp>
#include <com/sun/star/embed/XLinkageSupport.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/embed/XTransactedObject.hpp>
#include <com/sun/star/embed/XWindowSupplier.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/status/ClipboardFormats.hpp>
#include <com/sun/star/frame/status/ItemStatus.hpp>
#include <com/sun/star/gallery/GalleryItemType.hpp>
#include <com/sun/star/graphic/GraphicType.hpp>
#include <com/sun/star/graphic/PrimitiveFactory2D.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/graphic/XGraphicProvider.hpp>
#include <com/sun/star/graphic/XGraphicRenderer.hpp>
#include <com/sun/star/graphic/XPrimitiveFactory2D.hpp>
#include <com/sun/star/i18n/BreakIterator.hpp>
#include <com/sun/star/i18n/CharacterIteratorMode.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <com/sun/star/i18n/WordType.hpp>
#include <com/sun/star/io/XActiveDataControl.hpp>
#include <com/sun/star/io/XActiveDataSink.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XObjectInputStream.hpp>
#include <com/sun/star/io/XObjectOutputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XPersistObject.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/lang/NoSupportException.hpp>
#include <com/sun/star/lang/ServiceNotRegisteredException.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/linguistic2/LinguServiceManager.hpp>
#include <com/sun/star/linguistic2/XAvailableLocales.hpp>
#include <com/sun/star/media/ZoomLevel.hpp>
#include <com/sun/star/rendering/XSpriteCanvas.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/style/GraphicLocation.hpp>
#include <com/sun/star/style/ParagraphAdjust.hpp>
#include <com/sun/star/style/VerticalAlignment.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/system/SystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/table/BorderLine2.hpp>
#include <com/sun/star/table/TableBorder.hpp>
#include <com/sun/star/table/XMergeableCell.hpp>
#include <com/sun/star/table/XMergeableCellRange.hpp>
#include <com/sun/star/table/XTable.hpp>
#include <com/sun/star/task/XInteractionContinuation.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/text/GraphicCrop.hpp>
#include <com/sun/star/text/RelOrientation.hpp>
#include <com/sun/star/text/WritingMode.hpp>
#include <com/sun/star/text/textfield/Type.hpp>
#include <com/sun/star/ucb/NameClash.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>
#include <com/sun/star/ucb/TransferInfo.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/MeasureUnit.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/util/XCloneable.hpp>
#include <com/sun/star/util/XModeChangeBroadcaster.hpp>
#include <com/sun/star/util/XModeChangeListener.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XUpdatable.hpp>
#include <com/sun/star/xml/sax/InputSource.hpp>
#include <com/sun/star/xml/sax/Parser.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/xml/sax/Writer.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <comphelper/anytostring.hxx>
#include <comphelper/classids.hxx>
#include <comphelper/embeddedobjectcontainer.hxx>
#include <comphelper/extract.hxx>
#include <comphelper/interaction.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/propertysetinfo.hxx>
#include <comphelper/scopeguard.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/servicehelper.hxx>
#include <comphelper/serviceinfohelper.hxx>
#include <comphelper/storagehelper.hxx>
#include <comphelper/string.hxx>
#include <comphelper/types.hxx>
#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/exc_hlp.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <cppuhelper/supportsservice.hxx>
#include <cppuhelper/typeprovider.hxx>
#include <cppuhelper/weak.hxx>
#include <cstdio>
#include <cstdlib>
#include <drawinglayer/animation/animationtiming.hxx>
#include <drawinglayer/attribute/fillgradientattribute.hxx>
#include <drawinglayer/attribute/fillgraphicattribute.hxx>
#include <drawinglayer/attribute/fillhatchattribute.hxx>
#include <drawinglayer/attribute/lineattribute.hxx>
#include <drawinglayer/attribute/linestartendattribute.hxx>
#include <drawinglayer/attribute/materialattribute3d.hxx>
#include <drawinglayer/attribute/sdrallattribute3d.hxx>
#include <drawinglayer/attribute/sdrfillattribute.hxx>
#include <drawinglayer/attribute/sdrfillgraphicattribute.hxx>
#include <drawinglayer/attribute/sdrlightattribute3d.hxx>
#include <drawinglayer/attribute/sdrlightingattribute3d.hxx>
#include <drawinglayer/attribute/sdrlineattribute.hxx>
#include <drawinglayer/attribute/sdrlinestartendattribute.hxx>
#include <drawinglayer/attribute/sdrobjectattribute3d.hxx>
#include <drawinglayer/attribute/sdrsceneattribute3d.hxx>
#include <drawinglayer/attribute/sdrshadowattribute.hxx>
#include <drawinglayer/attribute/strokeattribute.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <drawinglayer/geometry/viewinformation3d.hxx>
#include <drawinglayer/primitive2d/animatedprimitive2d.hxx>
#include <drawinglayer/primitive2d/backgroundcolorprimitive2d.hxx>
#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <drawinglayer/primitive2d/borderlineprimitive2d.hxx>
#include <drawinglayer/primitive2d/clippedborderlineprimitive2d.hxx>
#include <drawinglayer/primitive2d/controlprimitive2d.hxx>
#include <drawinglayer/primitive2d/discreteshadowprimitive2d.hxx>
#include <drawinglayer/primitive2d/embedded3dprimitive2d.hxx>
#include <drawinglayer/primitive2d/fillgradientprimitive2d.hxx>
#include <drawinglayer/primitive2d/graphicprimitive2d.hxx>
#include <drawinglayer/primitive2d/gridprimitive2d.hxx>
#include <drawinglayer/primitive2d/groupprimitive2d.hxx>
#include <drawinglayer/primitive2d/helplineprimitive2d.hxx>
#include <drawinglayer/primitive2d/hiddengeometryprimitive2d.hxx>
#include <drawinglayer/primitive2d/invertprimitive2d.hxx>
#include <drawinglayer/primitive2d/markerarrayprimitive2d.hxx>
#include <drawinglayer/primitive2d/maskprimitive2d.hxx>
#include <drawinglayer/primitive2d/mediaprimitive2d.hxx>
#include <drawinglayer/primitive2d/metafileprimitive2d.hxx>
#include <drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx>
#include <drawinglayer/primitive2d/objectinfoprimitive2d.hxx>
#include <drawinglayer/primitive2d/pagepreviewprimitive2d.hxx>
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>
#include <drawinglayer/primitive2d/polypolygonprimitive2d.hxx>
#include <drawinglayer/primitive2d/sceneprimitive2d.hxx>
#include <drawinglayer/primitive2d/sdrdecompositiontools2d.hxx>
#include <drawinglayer/primitive2d/shadowprimitive2d.hxx>
#include <drawinglayer/primitive2d/textdecoratedprimitive2d.hxx>
#include <drawinglayer/primitive2d/texthierarchyprimitive2d.hxx>
#include <drawinglayer/primitive2d/textlayoutdevice.hxx>
#include <drawinglayer/primitive2d/textprimitive2d.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <drawinglayer/primitive2d/transparenceprimitive2d.hxx>
#include <drawinglayer/primitive2d/unifiedtransparenceprimitive2d.hxx>
#include <drawinglayer/primitive2d/wrongspellprimitive2d.hxx>
#include <drawinglayer/primitive3d/baseprimitive3d.hxx>
#include <drawinglayer/primitive3d/drawinglayer_primitivetypes3d.hxx>
#include <drawinglayer/primitive3d/modifiedcolorprimitive3d.hxx>
#include <drawinglayer/primitive3d/polygonprimitive3d.hxx>
#include <drawinglayer/primitive3d/polypolygonprimitive3d.hxx>
#include <drawinglayer/primitive3d/sdrcubeprimitive3d.hxx>
#include <drawinglayer/primitive3d/sdrextrudeprimitive3d.hxx>
#include <drawinglayer/primitive3d/sdrlatheprimitive3d.hxx>
#include <drawinglayer/primitive3d/sdrpolypolygonprimitive3d.hxx>
#include <drawinglayer/primitive3d/sdrsphereprimitive3d.hxx>
#include <drawinglayer/primitive3d/transformprimitive3d.hxx>
#include <drawinglayer/processor2d/baseprocessor2d.hxx>
#include <drawinglayer/processor2d/contourextractor2d.hxx>
#include <drawinglayer/processor2d/hittestprocessor2d.hxx>
#include <drawinglayer/processor2d/linegeometryextractor2d.hxx>
#include <drawinglayer/processor2d/objectinfoextractor2d.hxx>
#include <drawinglayer/processor2d/processor2dtools.hxx>
#include <drawinglayer/processor2d/textaspolygonextractor2d.hxx>
#include <drawinglayer/processor3d/baseprocessor3d.hxx>
#include <drawinglayer/processor3d/cutfindprocessor3d.hxx>
#include <editeng/adjustitem.hxx>
#include <editeng/autokernitem.hxx>
#include <editeng/borderline.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/brushitem.hxx>
#include <editeng/bulletitem.hxx>
#include <editeng/charreliefitem.hxx>
#include <editeng/charscaleitem.hxx>
#include <editeng/cmapitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/contouritem.hxx>
#include <editeng/crossedoutitem.hxx>
#include <editeng/editdata.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editobj.hxx>
#include <editeng/editstat.hxx>
#include <editeng/editview.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/eerdll.hxx>
#include <editeng/emphasismarkitem.hxx>
#include <editeng/escapementitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/flditem.hxx>
#include <editeng/flstitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/frmdiritem.hxx>
#include <editeng/itemtype.hxx>
#include <editeng/kernitem.hxx>
#include <editeng/lineitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/measfld.hxx>
#include <editeng/numitem.hxx>
#include <editeng/outliner.hxx>
#include <editeng/outlobj.hxx>
#include <editeng/postitem.hxx>
#include <editeng/scripttypeitem.hxx>
#include <editeng/shdditem.hxx>
#include <editeng/svxenum.hxx>
#include <editeng/svxfont.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/unoedhlp.hxx>
#include <editeng/unofield.hxx>
#include <editeng/unoforou.hxx>
#include <editeng/unolingu.hxx>
#include <editeng/unonrule.hxx>
#include <editeng/unoprnms.hxx>
#include <editeng/unotext.hxx>
#include <editeng/unoviwou.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/writingmodeitem.hxx>
#include <editeng/wrlmitem.hxx>
#include <float.h>
#include <framework/interaction.hxx>
#include <functional>
#include <i18nlangtag/lang.h>
#include <i18nlangtag/languagetag.hxx>
#include <i18nlangtag/mslangid.hxx>
#include <i18nutil/unicode.hxx>
#include <limits>
#include <linguistic/misc.hxx>
#include <map>
#include <math.h>
#include <memory>
#include <osl/diagnose.h>
#include <osl/endian.h>
#include <osl/file.hxx>
#include <osl/mutex.hxx>
#include <osl/process.h>
#include <osl/thread.h>
#include <osl/thread.hxx>
#include <osl/time.h>
#include <rtl/crc.h>
#include <rtl/instance.hxx>
#include <rtl/math.hxx>
#include <rtl/ref.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/ustrbuf.hxx>
#include <rtl/ustring.hxx>
#include <sal/config.h>
#include <sal/log.hxx>
#include <sal/macros.h>
#include <sax/tools/converter.hxx>
#include <set>
#include <sfx2/app.hxx>
#include <sfx2/basedlgs.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/childwin.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/fcontnr.hxx>
#include <sfx2/filedlghelper.hxx>
#include <sfx2/frmdescr.hxx>
#include <sfx2/htmlmode.hxx>
#include <sfx2/ipclient.hxx>
#include <sfx2/linkmgr.hxx>
#include <sfx2/lnkbase.hxx>
#include <sfx2/module.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/querystatus.hxx>
#include <sfx2/request.hxx>
#include <sfx2/sfxstatuslistener.hxx>
#include <sfx2/sidebar/Theme.hxx>
#include <sfx2/templdlg.hxx>
#include <sfx2/tplpitem.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/viewsh.hxx>
#include <sot/exchange.hxx>
#include <sot/factory.hxx>
#include <sot/filelist.hxx>
#include <sot/formats.hxx>
#include <sot/storage.hxx>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <svl/aeitem.hxx>
#include <svl/brdcst.hxx>
#include <svl/eitem.hxx>
#include <svl/flagitem.hxx>
#include <svl/grabbagitem.hxx>
#include <svl/hint.hxx>
#include <svl/intitem.hxx>
#include <svl/isethint.hxx>
#include <svl/itemiter.hxx>
#include <svl/itempool.hxx>
#include <svl/itemprop.hxx>
#include <svl/itemset.hxx>
#include <svl/lstner.hxx>
#include <svl/metitem.hxx>
#include <svl/outstrm.hxx>
#include <svl/poolitem.hxx>
#include <svl/ptitem.hxx>
#include <svl/rectitem.hxx>
#include <svl/rngitem.hxx>
#include <svl/smplhint.hxx>
#include <svl/stritem.hxx>
#include <svl/style.hxx>
#include <svl/urihelper.hxx>
#include <svl/urlbmk.hxx>
#include <svl/whiter.hxx>
#include <svl/zforlist.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <svtools/borderhelper.hxx>
#include <svtools/colorcfg.hxx>
#include <svtools/ctrlbox.hxx>
#include <svtools/ctrltool.hxx>
#include <svtools/embedhlp.hxx>
#include <svtools/filechangedchecker.hxx>
#include <svtools/grfmgr.hxx>
#include <svtools/langtab.hxx>
#include <svtools/optionsdrawinglayer.hxx>
#include <svtools/popupmenucontrollerbase.hxx>
#include <svtools/popupwindowcontroller.hxx>
#include <svtools/toolbarmenu.hxx>
#include <svtools/transfer.hxx>
#include <svtools/treelistentry.hxx>
#include <svtools/unoevent.hxx>
#include <svtools/unoimap.hxx>
#include <svtools/valueset.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/helper/convert.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <tools/b3dtrans.hxx>
#include <tools/bigint.hxx>
#include <tools/color.hxx>
#include <tools/datetime.hxx>
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>
#include <tools/fldunit.hxx>
#include <tools/fract.hxx>
#include <tools/gen.hxx>
#include <tools/globname.hxx>
#include <tools/helpers.hxx>
#include <tools/line.hxx>
#include <tools/mapunit.hxx>
#include <tools/poly.hxx>
#include <tools/rcid.h>
#include <tools/resmgr.hxx>
#include <tools/shl.hxx>
#include <tools/stream.hxx>
#include <tools/tenccvt.hxx>
#include <tools/urlobj.hxx>
#include <tools/vcompat.hxx>
#include <tools/zcodec.hxx>
#include <ucbhelper/content.hxx>
#include <unotools/datetime.hxx>
#include <unotools/fontoptions.hxx>
#include <unotools/intlwrapper.hxx>
#include <unotools/localedatawrapper.hxx>
#include <unotools/localfilehelper.hxx>
#include <unotools/pathoptions.hxx>
#include <unotools/saveopt.hxx>
#include <unotools/streamwrap.hxx>
#include <unotools/syslocale.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <vcl/FilterConfigItem.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/bmpacc.hxx>
#include <vcl/builder.hxx>
#include <vcl/canvastools.hxx>
#include <vcl/ctrl.hxx>
#include <vcl/cursor.hxx>
#include <vcl/cvtgrf.hxx>
#include <vcl/dibtools.hxx>
#include <vcl/fltcall.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/gfxlink.hxx>
#include <vcl/gradient.hxx>
#include <vcl/graph.hxx>
#include <vcl/graphicfilter.hxx>
#include <vcl/graphictools.hxx>
#include <vcl/hatch.hxx>
#include <vcl/jobset.hxx>
#include <vcl/lazydelete.hxx>
#include <vcl/lineinfo.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/metaact.hxx>
#include <vcl/metric.hxx>
#include <vcl/mnemonic.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/outdev.hxx>
#include <vcl/pdfextoutdevdata.hxx>
#include <vcl/region.hxx>
#include <vcl/salbtype.hxx>
#include <vcl/settings.hxx>
#include <vcl/split.hxx>
#include <vcl/svapp.hxx>
#include <vcl/svgdata.hxx>
#include <vcl/timer.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/virdev.hxx>
#include <vcl/window.hxx>
#include <vcl/wmf.hxx>
#include <vcl/wrkwin.hxx>
#include <vector>
#include <xmloff/xmlictxt.hxx>

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */