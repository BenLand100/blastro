#include "PCLStubs.h"
#include <QDebug>
#include <unordered_map>
#include <string>

namespace blastro {

static std::unordered_map<std::string, void*> s_stubMap;

static void* stub_Action_CreateAction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/CreateAction";
    }
    return nullptr;
}

static void* stub_Action_CreateActionSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/CreateActionSVG";
    }
    return nullptr;
}

static void* stub_Action_CreateActionSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/CreateActionSVGFile";
    }
    return nullptr;
}

static void* stub_Action_GetActionAccelerator(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionAccelerator";
    }
    return nullptr;
}

static void* stub_Action_GetActionIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionIcon";
    }
    return nullptr;
}

static void* stub_Action_GetActionMenuItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionMenuItem";
    }
    return nullptr;
}

static void* stub_Action_GetActionMenuText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionMenuText";
    }
    return nullptr;
}

static void* stub_Action_GetActionToolBar(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionToolBar";
    }
    return nullptr;
}

static void* stub_Action_GetActionToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/GetActionToolTip";
    }
    return nullptr;
}

static void* stub_Action_SetActionAccelerator(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionAccelerator";
    }
    return nullptr;
}

static void* stub_Action_SetActionExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionExecutionRoutine";
    }
    return nullptr;
}

static void* stub_Action_SetActionIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionIcon";
    }
    return nullptr;
}

static void* stub_Action_SetActionIconSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionIconSVG";
    }
    return nullptr;
}

static void* stub_Action_SetActionIconSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionIconSVGFile";
    }
    return nullptr;
}

static void* stub_Action_SetActionMenuText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionMenuText";
    }
    return nullptr;
}

static void* stub_Action_SetActionStateQueryRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionStateQueryRoutine";
    }
    return nullptr;
}

static void* stub_Action_SetActionToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Action/SetActionToolTip";
    }
    return nullptr;
}

static void* stub_Bitmap_AndBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/AndBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_AndBitmaps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/AndBitmaps";
    }
    return nullptr;
}

static void* stub_Bitmap_CloneBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CloneBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_CloneBitmapRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CloneBitmapRect";
    }
    return nullptr;
}

static void* stub_Bitmap_CopyBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CopyBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapFromData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapFromData";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapFromFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapFromFile";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapFromFile8(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapFromFile8";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapFromSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapFromSVG";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapFromSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapFromSVGFile";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateBitmapXPM(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateBitmapXPM";
    }
    return nullptr;
}

static void* stub_Bitmap_CreateEmptyBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/CreateEmptyBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_FillBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/FillBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_GetBitmapDevicePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/GetBitmapDevicePixelRatio";
    }
    return nullptr;
}

static void* stub_Bitmap_GetBitmapDimensions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/GetBitmapDimensions";
    }
    return nullptr;
}

static void* stub_Bitmap_GetBitmapFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/GetBitmapFormat";
    }
    return nullptr;
}

static void* stub_Bitmap_GetBitmapPixel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/GetBitmapPixel";
    }
    return nullptr;
}

static void* stub_Bitmap_GetBitmapScanLine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/GetBitmapScanLine";
    }
    return nullptr;
}

static void* stub_Bitmap_IsEmptyBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/IsEmptyBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_LoadBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/LoadBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_LoadBitmapData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/LoadBitmapData";
    }
    return nullptr;
}

static void* stub_Bitmap_MirroredBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/MirroredBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_OrBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/OrBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_OrBitmaps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/OrBitmaps";
    }
    return nullptr;
}

static void* stub_Bitmap_ReplaceBitmapColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/ReplaceBitmapColor";
    }
    return nullptr;
}

static void* stub_Bitmap_RotatedBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/RotatedBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_SaveBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/SaveBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_ScaledBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/ScaledBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_SetBitmapAlpha(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/SetBitmapAlpha";
    }
    return nullptr;
}

static void* stub_Bitmap_SetBitmapDevicePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/SetBitmapDevicePixelRatio";
    }
    return nullptr;
}

static void* stub_Bitmap_SetBitmapFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/SetBitmapFormat";
    }
    return nullptr;
}

static void* stub_Bitmap_SetBitmapPixel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/SetBitmapPixel";
    }
    return nullptr;
}

static void* stub_Bitmap_XorBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/XorBitmap";
    }
    return nullptr;
}

static void* stub_Bitmap_XorBitmapRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/XorBitmapRect";
    }
    return nullptr;
}

static void* stub_Bitmap_XorBitmaps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Bitmap/XorBitmaps";
    }
    return nullptr;
}

static void* stub_BitmapBox_CreateBitmapBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/CreateBitmapBox";
    }
    return nullptr;
}

static void* stub_BitmapBox_GetBitmapBoxAutoFitEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/GetBitmapBoxAutoFitEnabled";
    }
    return nullptr;
}

static void* stub_BitmapBox_GetBitmapBoxBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/GetBitmapBoxBitmap";
    }
    return nullptr;
}

static void* stub_BitmapBox_GetBitmapBoxMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/GetBitmapBoxMargin";
    }
    return nullptr;
}

static void* stub_BitmapBox_SetBitmapBoxAutoFitEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/SetBitmapBoxAutoFitEnabled";
    }
    return nullptr;
}

static void* stub_BitmapBox_SetBitmapBoxBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/SetBitmapBoxBitmap";
    }
    return nullptr;
}

static void* stub_BitmapBox_SetBitmapBoxMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "BitmapBox/SetBitmapBoxMargin";
    }
    return nullptr;
}

static void* stub_Brush_CloneBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CloneBrush";
    }
    return nullptr;
}

static void* stub_Brush_CreateBitmapBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CreateBitmapBrush";
    }
    return nullptr;
}

static void* stub_Brush_CreateBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CreateBrush";
    }
    return nullptr;
}

static void* stub_Brush_CreateConicalGradientBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CreateConicalGradientBrush";
    }
    return nullptr;
}

static void* stub_Brush_CreateLinearGradientBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CreateLinearGradientBrush";
    }
    return nullptr;
}

static void* stub_Brush_CreateRadialGradientBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/CreateRadialGradientBrush";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushBitmap";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushColor";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushConicalGradientParameters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushConicalGradientParameters";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushGradientSpread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushGradientSpread";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushGradientStops(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushGradientStops";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushGradientType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushGradientType";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushLinearGradientParameters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushLinearGradientParameters";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushRadialGradientParameters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushRadialGradientParameters";
    }
    return nullptr;
}

static void* stub_Brush_GetBrushStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/GetBrushStyle";
    }
    return nullptr;
}

static void* stub_Brush_SetBrushBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/SetBrushBitmap";
    }
    return nullptr;
}

static void* stub_Brush_SetBrushColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/SetBrushColor";
    }
    return nullptr;
}

static void* stub_Brush_SetBrushStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Brush/SetBrushStyle";
    }
    return nullptr;
}

static void* stub_Button_CreateCheckBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/CreateCheckBox";
    }
    return nullptr;
}

static void* stub_Button_CreatePushButton(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/CreatePushButton";
    }
    return nullptr;
}

static void* stub_Button_CreateRadioButton(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/CreateRadioButton";
    }
    return nullptr;
}

static void* stub_Button_CreateToolButton(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/CreateToolButton";
    }
    return nullptr;
}

static void* stub_Button_GetButtonChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonChecked";
    }
    return nullptr;
}

static void* stub_Button_GetButtonDefaultEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonDefaultEnabled";
    }
    return nullptr;
}

static void* stub_Button_GetButtonIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonIcon";
    }
    return nullptr;
}

static void* stub_Button_GetButtonIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonIconSize";
    }
    return nullptr;
}

static void* stub_Button_GetButtonPushed(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonPushed";
    }
    return nullptr;
}

static void* stub_Button_GetButtonText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonText";
    }
    return nullptr;
}

static void* stub_Button_GetButtonTristateEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetButtonTristateEnabled";
    }
    return nullptr;
}

static void* stub_Button_GetToolButtonCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/GetToolButtonCheckable";
    }
    return nullptr;
}

static void* stub_Button_SetButtonCheckEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonCheckEventRoutine";
    }
    return nullptr;
}

static void* stub_Button_SetButtonChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonChecked";
    }
    return nullptr;
}

static void* stub_Button_SetButtonClickEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonClickEventRoutine";
    }
    return nullptr;
}

static void* stub_Button_SetButtonDefaultEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonDefaultEnabled";
    }
    return nullptr;
}

static void* stub_Button_SetButtonIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonIcon";
    }
    return nullptr;
}

static void* stub_Button_SetButtonIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonIconSize";
    }
    return nullptr;
}

static void* stub_Button_SetButtonPressEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonPressEventRoutine";
    }
    return nullptr;
}

static void* stub_Button_SetButtonPushed(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonPushed";
    }
    return nullptr;
}

static void* stub_Button_SetButtonReleaseEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonReleaseEventRoutine";
    }
    return nullptr;
}

static void* stub_Button_SetButtonText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonText";
    }
    return nullptr;
}

static void* stub_Button_SetButtonTristateEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetButtonTristateEnabled";
    }
    return nullptr;
}

static void* stub_Button_SetToolButtonCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Button/SetToolButtonCheckable";
    }
    return nullptr;
}

static void* stub_CodeEditor_ClearEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/ClearEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_CreateCodeEditor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/CreateCodeEditor";
    }
    return nullptr;
}

static void* stub_CodeEditor_CreateEditorLineNumbersControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/CreateEditorLineNumbersControl";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorClearMatches(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorClearMatches";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorCopy(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorCopy";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorCut(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorCut";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorDelete(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorDelete";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorFind(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorFind";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorGotoMatchedParenthesis(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorGotoMatchedParenthesis";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorHighlightAllMatches(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorHighlightAllMatches";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorPaste(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorPaste";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorRedo(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorRedo";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorReplace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorReplace";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorReplaceAll(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorReplaceAll";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorSelectAll(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorSelectAll";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorUndo(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorUndo";
    }
    return nullptr;
}

static void* stub_CodeEditor_EditorUnselect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/EditorUnselect";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorBlockSelectionMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorBlockSelectionMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorCharacterCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorCharacterCount";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorCursorCoordinates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorCursorCoordinates";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorDynamicWordWrapMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorDynamicWordWrapMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorEncodedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorEncodedText";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorFilePath";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorHasSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorHasSelection";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorInsertMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorInsertMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorLineCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorLineCount";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorReadOnly";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorRedoSteps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorRedoSteps";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorSelectedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorSelectedText";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorSelectionCoordinates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorSelectionCoordinates";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_GetEditorUndoSteps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/GetEditorUndoSteps";
    }
    return nullptr;
}

static void* stub_CodeEditor_InsertEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/InsertEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_LoadEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/LoadEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_SaveEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SaveEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorBlockSelectionMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorBlockSelectionMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorCursorCoordinates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorCursorCoordinates";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorCursorPositionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorCursorPositionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorDynamicWordWrapMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorDynamicWordWrapMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorDynamicWordWrapModeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorDynamicWordWrapModeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorEncodedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorEncodedText";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorFilePath";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorInsertMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorInsertMode";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorOverwriteModeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorOverwriteModeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorReadOnly";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorSelectionCoordinates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorSelectionCoordinates";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorSelectionModeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorSelectionModeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorSelectionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorSelectionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorText";
    }
    return nullptr;
}

static void* stub_CodeEditor_SetEditorTextUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "CodeEditor/SetEditorTextUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ComboBox_ClearComboBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/ClearComboBox";
    }
    return nullptr;
}

static void* stub_ComboBox_CreateComboBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/CreateComboBox";
    }
    return nullptr;
}

static void* stub_ComboBox_FindComboBoxItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/FindComboBoxItem";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxAutoCompletionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxAutoCompletionEnabled";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxCurrentItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxCurrentItem";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxEditEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxEditEnabled";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxEditText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxEditText";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxIconSize";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxItemIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxItemIcon";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxItemText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxItemText";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxLength";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxMaxVisibleItemCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxMaxVisibleItemCount";
    }
    return nullptr;
}

static void* stub_ComboBox_GetComboBoxMinItemCharWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/GetComboBoxMinItemCharWidth";
    }
    return nullptr;
}

static void* stub_ComboBox_InsertComboBoxItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/InsertComboBoxItem";
    }
    return nullptr;
}

static void* stub_ComboBox_RemoveComboBoxItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/RemoveComboBoxItem";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxAutoCompletionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxAutoCompletionEnabled";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxCurrentItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxCurrentItem";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxEditEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxEditEnabled";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxEditText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxEditText";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxEditTextUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxEditTextUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxIconSize";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxItemHighlightedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxItemHighlightedEventRoutine";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxItemIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxItemIcon";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxItemSelectedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxItemSelectedEventRoutine";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxItemText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxItemText";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxListVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxListVisible";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxMaxVisibleItemCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxMaxVisibleItemCount";
    }
    return nullptr;
}

static void* stub_ComboBox_SetComboBoxMinItemCharWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ComboBox/SetComboBoxMinItemCharWidth";
    }
    return nullptr;
}

static void* stub_Control_ActivateWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/ActivateWindow";
    }
    return nullptr;
}

static void* stub_Control_AdjustControlToContents(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/AdjustControlToContents";
    }
    return nullptr;
}

static void* stub_Control_BringControlToFront(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/BringControlToFront";
    }
    return nullptr;
}

static void* stub_Control_ControlToLocal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/ControlToLocal";
    }
    return nullptr;
}

static void* stub_Control_CreateControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/CreateControl";
    }
    return nullptr;
}

static void* stub_Control_EnsureControlLayoutUpdated(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/EnsureControlLayoutUpdated";
    }
    return nullptr;
}

static void* stub_Control_GetChildByPos(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetChildByPos";
    }
    return nullptr;
}

static void* stub_Control_GetChildControlToFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetChildControlToFocus";
    }
    return nullptr;
}

static void* stub_Control_GetChildrenRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetChildrenRect";
    }
    return nullptr;
}

static void* stub_Control_GetClientRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetClientRect";
    }
    return nullptr;
}

static void* stub_Control_GetControlAlternateCanvasColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlAlternateCanvasColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlAncestry(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlAncestry";
    }
    return nullptr;
}

static void* stub_Control_GetControlBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlBackgroundColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlButtonColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlButtonColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlButtonTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlButtonTextColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlCanvasColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlCanvasColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlCursor";
    }
    return nullptr;
}

static void* stub_Control_GetControlDevicePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlDevicePixelRatio";
    }
    return nullptr;
}

static void* stub_Control_GetControlDisplayPixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlDisplayPixelRatio";
    }
    return nullptr;
}

static void* stub_Control_GetControlEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlEnabled";
    }
    return nullptr;
}

static void* stub_Control_GetControlExpansionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlExpansionEnabled";
    }
    return nullptr;
}

static void* stub_Control_GetControlFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlFocus";
    }
    return nullptr;
}

static void* stub_Control_GetControlFocusStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlFocusStyle";
    }
    return nullptr;
}

static void* stub_Control_GetControlFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlFont";
    }
    return nullptr;
}

static void* stub_Control_GetControlForegroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlForegroundColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlHighlightColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlHighlightColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlHighlightedTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlHighlightedTextColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlMaxSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlMaxSize";
    }
    return nullptr;
}

static void* stub_Control_GetControlMinSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlMinSize";
    }
    return nullptr;
}

static void* stub_Control_GetControlMouseTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlMouseTrackingEnabled";
    }
    return nullptr;
}

static void* stub_Control_GetControlParent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlParent";
    }
    return nullptr;
}

static void* stub_Control_GetControlPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlPosition";
    }
    return nullptr;
}

static void* stub_Control_GetControlResourcePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlResourcePixelRatio";
    }
    return nullptr;
}

static void* stub_Control_GetControlSizer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlSizer";
    }
    return nullptr;
}

static void* stub_Control_GetControlStyleSheet(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlStyleSheet";
    }
    return nullptr;
}

static void* stub_Control_GetControlTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlTextColor";
    }
    return nullptr;
}

static void* stub_Control_GetControlUnderMouseStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlUnderMouseStatus";
    }
    return nullptr;
}

static void* stub_Control_GetControlUpdatesEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlUpdatesEnabled";
    }
    return nullptr;
}

static void* stub_Control_GetControlVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlVisible";
    }
    return nullptr;
}

static void* stub_Control_GetControlVisibleRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlVisibleRect";
    }
    return nullptr;
}

static void* stub_Control_GetControlWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetControlWindow";
    }
    return nullptr;
}

static void* stub_Control_GetFocusChildControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetFocusChildControl";
    }
    return nullptr;
}

static void* stub_Control_GetFrameRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetFrameRect";
    }
    return nullptr;
}

static void* stub_Control_GetInfoText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetInfoText";
    }
    return nullptr;
}

static void* stub_Control_GetNextSiblingControlToFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetNextSiblingControlToFocus";
    }
    return nullptr;
}

static void* stub_Control_GetRealTimePreviewActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetRealTimePreviewActive";
    }
    return nullptr;
}

static void* stub_Control_GetTrackViewActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetTrackViewActive";
    }
    return nullptr;
}

static void* stub_Control_GetWindowOpacity(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetWindowOpacity";
    }
    return nullptr;
}

static void* stub_Control_GetWindowState(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetWindowState";
    }
    return nullptr;
}

static void* stub_Control_GetWindowTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetWindowTitle";
    }
    return nullptr;
}

static void* stub_Control_GetWindowToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GetWindowToolTip";
    }
    return nullptr;
}

static void* stub_Control_GlobalToLocal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/GlobalToLocal";
    }
    return nullptr;
}

static void* stub_Control_LocalToControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/LocalToControl";
    }
    return nullptr;
}

static void* stub_Control_LocalToGlobal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/LocalToGlobal";
    }
    return nullptr;
}

static void* stub_Control_LocalToParent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/LocalToParent";
    }
    return nullptr;
}

static void* stub_Control_ParentToLocal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/ParentToLocal";
    }
    return nullptr;
}

static void* stub_Control_RepaintControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/RepaintControl";
    }
    return nullptr;
}

static void* stub_Control_RepaintControlRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/RepaintControlRect";
    }
    return nullptr;
}

static void* stub_Control_RestyleControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/RestyleControl";
    }
    return nullptr;
}

static void* stub_Control_ScrollControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/ScrollControl";
    }
    return nullptr;
}

static void* stub_Control_ScrollControlRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/ScrollControlRect";
    }
    return nullptr;
}

static void* stub_Control_SendControlToBack(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SendControlToBack";
    }
    return nullptr;
}

static void* stub_Control_SetChildControlToFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetChildControlToFocus";
    }
    return nullptr;
}

static void* stub_Control_SetChildCreateEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetChildCreateEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetChildDestroyEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetChildDestroyEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetClientRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetClientRect";
    }
    return nullptr;
}

static void* stub_Control_SetCloseEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetCloseEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetControlAlternateCanvasColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlAlternateCanvasColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlBackgroundColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlButtonColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlButtonColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlButtonTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlButtonTextColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlCanvasColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlCanvasColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlCursor";
    }
    return nullptr;
}

static void* stub_Control_SetControlCursorToParent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlCursorToParent";
    }
    return nullptr;
}

static void* stub_Control_SetControlEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlEnabled";
    }
    return nullptr;
}

static void* stub_Control_SetControlExpansionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlExpansionEnabled";
    }
    return nullptr;
}

static void* stub_Control_SetControlFixedSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlFixedSize";
    }
    return nullptr;
}

static void* stub_Control_SetControlFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlFocus";
    }
    return nullptr;
}

static void* stub_Control_SetControlFocusStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlFocusStyle";
    }
    return nullptr;
}

static void* stub_Control_SetControlFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlFont";
    }
    return nullptr;
}

static void* stub_Control_SetControlForegroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlForegroundColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlHighlightColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlHighlightColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlHighlightedTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlHighlightedTextColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlMaxSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlMaxSize";
    }
    return nullptr;
}

static void* stub_Control_SetControlMinSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlMinSize";
    }
    return nullptr;
}

static void* stub_Control_SetControlMouseTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlMouseTrackingEnabled";
    }
    return nullptr;
}

static void* stub_Control_SetControlParent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlParent";
    }
    return nullptr;
}

static void* stub_Control_SetControlPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlPosition";
    }
    return nullptr;
}

static void* stub_Control_SetControlSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlSize";
    }
    return nullptr;
}

static void* stub_Control_SetControlSizer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlSizer";
    }
    return nullptr;
}

static void* stub_Control_SetControlStyleSheet(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlStyleSheet";
    }
    return nullptr;
}

static void* stub_Control_SetControlTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlTextColor";
    }
    return nullptr;
}

static void* stub_Control_SetControlUpdatesEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlUpdatesEnabled";
    }
    return nullptr;
}

static void* stub_Control_SetControlVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetControlVisible";
    }
    return nullptr;
}

static void* stub_Control_SetDestroyEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetDestroyEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetEnterEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetEnterEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetFileDragEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetFileDragEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetFileDropEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetFileDropEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetGetFocusEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetGetFocusEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetHideEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetHideEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetInfoText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetInfoText";
    }
    return nullptr;
}

static void* stub_Control_SetKeyPressEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetKeyPressEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetKeyReleaseEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetKeyReleaseEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetLeaveEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetLeaveEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetLoseFocusEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetLoseFocusEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetMouseDoubleClickEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetMouseDoubleClickEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetMouseMoveEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetMouseMoveEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetMousePressEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetMousePressEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetMouseReleaseEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetMouseReleaseEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetMoveEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetMoveEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetNextSiblingControlToFocus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetNextSiblingControlToFocus";
    }
    return nullptr;
}

static void* stub_Control_SetPaintEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetPaintEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetRealTimePreviewActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetRealTimePreviewActive";
    }
    return nullptr;
}

static void* stub_Control_SetResizeEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetResizeEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetShowEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetShowEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetTrackViewActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetTrackViewActive";
    }
    return nullptr;
}

static void* stub_Control_SetViewDragEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetViewDragEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetViewDropEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetViewDropEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetWheelEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetWheelEventRoutine";
    }
    return nullptr;
}

static void* stub_Control_SetWindowOpacity(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetWindowOpacity";
    }
    return nullptr;
}

static void* stub_Control_SetWindowTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetWindowTitle";
    }
    return nullptr;
}

static void* stub_Control_SetWindowToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/SetWindowToolTip";
    }
    return nullptr;
}

static void* stub_Control_StackControls(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/StackControls";
    }
    return nullptr;
}

static void* stub_Control_UpdateControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/UpdateControl";
    }
    return nullptr;
}

static void* stub_Control_UpdateControlRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Control/UpdateControlRect";
    }
    return nullptr;
}

static void* stub_Cursor_CloneCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Cursor/CloneCursor";
    }
    return nullptr;
}

static void* stub_Cursor_CreateBitmapCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Cursor/CreateBitmapCursor";
    }
    return nullptr;
}

static void* stub_Cursor_CreateCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Cursor/CreateCursor";
    }
    return nullptr;
}

static void* stub_Cursor_GetCursorHotSpot(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Cursor/GetCursorHotSpot";
    }
    return nullptr;
}

static void* stub_Dialog_CreateDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/CreateDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ExecuteDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ExecuteDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ExecuteGetDirectoryDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ExecuteGetDirectoryDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ExecuteOpenFileDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ExecuteOpenFileDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ExecuteOpenMultipleFilesDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ExecuteOpenMultipleFilesDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ExecuteSaveFileDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ExecuteSaveFileDialog";
    }
    return nullptr;
}

static void* stub_Dialog_GetDialogResizable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/GetDialogResizable";
    }
    return nullptr;
}

static void* stub_Dialog_OpenDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/OpenDialog";
    }
    return nullptr;
}

static void* stub_Dialog_ReturnDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/ReturnDialog";
    }
    return nullptr;
}

static void* stub_Dialog_SetDialogResizable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/SetDialogResizable";
    }
    return nullptr;
}

static void* stub_Dialog_SetExecuteDialogEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/SetExecuteDialogEventRoutine";
    }
    return nullptr;
}

static void* stub_Dialog_SetReturnDialogEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Dialog/SetReturnDialogEventRoutine";
    }
    return nullptr;
}

static void* stub_Edit_CreateEdit(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/CreateEdit";
    }
    return nullptr;
}

static void* stub_Edit_GetEditAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditAlignment";
    }
    return nullptr;
}

static void* stub_Edit_GetEditCaretPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditCaretPosition";
    }
    return nullptr;
}

static void* stub_Edit_GetEditMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditMask";
    }
    return nullptr;
}

static void* stub_Edit_GetEditMaxLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditMaxLength";
    }
    return nullptr;
}

static void* stub_Edit_GetEditModified(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditModified";
    }
    return nullptr;
}

static void* stub_Edit_GetEditPasswordEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditPasswordEnabled";
    }
    return nullptr;
}

static void* stub_Edit_GetEditReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditReadOnly";
    }
    return nullptr;
}

static void* stub_Edit_GetEditSelectedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditSelectedText";
    }
    return nullptr;
}

static void* stub_Edit_GetEditSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditSelection";
    }
    return nullptr;
}

static void* stub_Edit_GetEditText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditText";
    }
    return nullptr;
}

static void* stub_Edit_GetEditValid(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditValid";
    }
    return nullptr;
}

static void* stub_Edit_GetEditValidatingRegExp(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/GetEditValidatingRegExp";
    }
    return nullptr;
}

static void* stub_Edit_SetCaretPositionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetCaretPositionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_Edit_SetEditAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditAlignment";
    }
    return nullptr;
}

static void* stub_Edit_SetEditCaretPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditCaretPosition";
    }
    return nullptr;
}

static void* stub_Edit_SetEditCompletedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditCompletedEventRoutine";
    }
    return nullptr;
}

static void* stub_Edit_SetEditMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditMask";
    }
    return nullptr;
}

static void* stub_Edit_SetEditMaxLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditMaxLength";
    }
    return nullptr;
}

static void* stub_Edit_SetEditModified(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditModified";
    }
    return nullptr;
}

static void* stub_Edit_SetEditPasswordEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditPasswordEnabled";
    }
    return nullptr;
}

static void* stub_Edit_SetEditReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditReadOnly";
    }
    return nullptr;
}

static void* stub_Edit_SetEditSelected(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditSelected";
    }
    return nullptr;
}

static void* stub_Edit_SetEditSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditSelection";
    }
    return nullptr;
}

static void* stub_Edit_SetEditText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditText";
    }
    return nullptr;
}

static void* stub_Edit_SetEditValidatingRegExp(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetEditValidatingRegExp";
    }
    return nullptr;
}

static void* stub_Edit_SetReturnPressedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetReturnPressedEventRoutine";
    }
    return nullptr;
}

static void* stub_Edit_SetSelectionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetSelectionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_Edit_SetTextUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Edit/SetTextUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_CloseExternalProcessStream(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/CloseExternalProcessStream";
    }
    return nullptr;
}

static void* stub_ExternalProcess_CreateExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/CreateExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_EnumerateExternalProcessEnvironment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/EnumerateExternalProcessEnvironment";
    }
    return nullptr;
}

static void* stub_ExternalProcess_ExecuteProgram(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/ExecuteProgram";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessBytesAvailable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessBytesAvailable";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessBytesToWrite(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessBytesToWrite";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessErrorCode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessErrorCode";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessExitCode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessExitCode";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessExitStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessExitStatus";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessIsRunning(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessIsRunning";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessIsStarting(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessIsStarting";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessPID(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessPID";
    }
    return nullptr;
}

static void* stub_ExternalProcess_GetExternalProcessWorkingDirectory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/GetExternalProcessWorkingDirectory";
    }
    return nullptr;
}

static void* stub_ExternalProcess_KillExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/KillExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_PipeExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/PipeExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_ReadFromExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/ReadFromExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_RedirectExternalProcessToFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/RedirectExternalProcessToFile";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessEnvironment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessEnvironment";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessErrorEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessErrorEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessFinishedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessFinishedEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessStandardErrorDataAvailableEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessStandardErrorDataAvailableEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessStandardOutputDataAvailableEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessStandardOutputDataAvailableEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessStartedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessStartedEventRoutine";
    }
    return nullptr;
}

static void* stub_ExternalProcess_SetExternalProcessWorkingDirectory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/SetExternalProcessWorkingDirectory";
    }
    return nullptr;
}

static void* stub_ExternalProcess_StartExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/StartExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_StartProgram(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/StartProgram";
    }
    return nullptr;
}

static void* stub_ExternalProcess_TerminateExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/TerminateExternalProcess";
    }
    return nullptr;
}

static void* stub_ExternalProcess_WaitForExternalProcessDataAvailable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/WaitForExternalProcessDataAvailable";
    }
    return nullptr;
}

static void* stub_ExternalProcess_WaitForExternalProcessDataWritten(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/WaitForExternalProcessDataWritten";
    }
    return nullptr;
}

static void* stub_ExternalProcess_WaitForExternalProcessFinished(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/WaitForExternalProcessFinished";
    }
    return nullptr;
}

static void* stub_ExternalProcess_WaitForExternalProcessStarted(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/WaitForExternalProcessStarted";
    }
    return nullptr;
}

static void* stub_ExternalProcess_WriteToExternalProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ExternalProcess/WriteToExternalProcess";
    }
    return nullptr;
}

static void* stub_FileFormat_AddKeyword(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/AddKeyword";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginColorFilterArrayEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginColorFilterArrayEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginColorFilterArrayExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginColorFilterArrayExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginDisplayFunctionEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginDisplayFunctionEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginDisplayFunctionExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginDisplayFunctionExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginICCProfileEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginICCProfileEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginICCProfileExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginICCProfileExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginImagePropertyEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginImagePropertyEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginImagePropertyExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginImagePropertyExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginKeywordEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginKeywordEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginKeywordExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginKeywordExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginPropertyEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginPropertyEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginPropertyExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginPropertyExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginRGBWSEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginRGBWSEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginRGBWSExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginRGBWSExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginThumbnailEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginThumbnailEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_BeginThumbnailExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/BeginThumbnailExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_CanReadIncrementally(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CanReadIncrementally";
    }
    return nullptr;
}

static void* stub_FileFormat_CanWriteIncrementally(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CanWriteIncrementally";
    }
    return nullptr;
}

static void* stub_FileFormat_CloseImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CloseImage";
    }
    return nullptr;
}

static void* stub_FileFormat_CloseImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CloseImageFile";
    }
    return nullptr;
}

static void* stub_FileFormat_CreateFileFormatInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CreateFileFormatInstance";
    }
    return nullptr;
}

static void* stub_FileFormat_CreateImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CreateImage";
    }
    return nullptr;
}

static void* stub_FileFormat_CreateImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CreateImageFile";
    }
    return nullptr;
}

static void* stub_FileFormat_CreateImageFileEx(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/CreateImageFileEx";
    }
    return nullptr;
}

static void* stub_FileFormat_DisposeFormatSpecificData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/DisposeFormatSpecificData";
    }
    return nullptr;
}

static void* stub_FileFormat_EditFileFormatPreferences(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EditFileFormatPreferences";
    }
    return nullptr;
}

static void* stub_FileFormat_EndColorFilterArrayEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndColorFilterArrayEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndColorFilterArrayExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndColorFilterArrayExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndDisplayFunctionEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndDisplayFunctionEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndDisplayFunctionExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndDisplayFunctionExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndICCProfileEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndICCProfileEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndICCProfileExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndICCProfileExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndImagePropertyEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndImagePropertyEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndImagePropertyExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndImagePropertyExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndKeywordEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndKeywordEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndKeywordExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndKeywordExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndPropertyEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndPropertyEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndPropertyExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndPropertyExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndRGBWSEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndRGBWSEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndRGBWSExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndRGBWSExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EndThumbnailEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndThumbnailEmbedding";
    }
    return nullptr;
}

static void* stub_FileFormat_EndThumbnailExtraction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EndThumbnailExtraction";
    }
    return nullptr;
}

static void* stub_FileFormat_EnumerateFileFormats(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EnumerateFileFormats";
    }
    return nullptr;
}

static void* stub_FileFormat_EnumerateImageProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EnumerateImageProperties";
    }
    return nullptr;
}

static void* stub_FileFormat_EnumerateProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/EnumerateProperties";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatByFileExtension(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatByFileExtension";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatByMimeType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatByMimeType";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatByName(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatByName";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatCapabilities(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatCapabilities";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatDescription";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatFileExtensions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatFileExtensions";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatIcon";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatImplementation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatImplementation";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatInstanceFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatInstanceFormat";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatMimeTypes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatMimeTypes";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatName(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatName";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatSmallIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatSmallIcon";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatStatus";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFileFormatVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFileFormatVersion";
    }
    return nullptr;
}

static void* stub_FileFormat_GetFormatSpecificData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetFormatSpecificData";
    }
    return nullptr;
}

static void* stub_FileFormat_GetICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetICCProfile";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageColorFilterArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageColorFilterArray";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageCount";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageDescription";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageDisplayFunction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageDisplayFunction";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageFilePath";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageFormatInfo(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageFormatInfo";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageId";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageProperty";
    }
    return nullptr;
}

static void* stub_FileFormat_GetImageRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetImageRGBWS";
    }
    return nullptr;
}

static void* stub_FileFormat_GetKeywordCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetKeywordCount";
    }
    return nullptr;
}

static void* stub_FileFormat_GetNextKeyword(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetNextKeyword";
    }
    return nullptr;
}

static void* stub_FileFormat_GetProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetProperty";
    }
    return nullptr;
}

static void* stub_FileFormat_GetSelectedImageIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetSelectedImageIndex";
    }
    return nullptr;
}

static void* stub_FileFormat_GetThumbnail(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/GetThumbnail";
    }
    return nullptr;
}

static void* stub_FileFormat_IsImageFileOpen(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/IsImageFileOpen";
    }
    return nullptr;
}

static void* stub_FileFormat_OpenImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/OpenImageFile";
    }
    return nullptr;
}

static void* stub_FileFormat_OpenImageFileEx(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/OpenImageFileEx";
    }
    return nullptr;
}

static void* stub_FileFormat_QueryImageFileOptions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/QueryImageFileOptions";
    }
    return nullptr;
}

static void* stub_FileFormat_ReadImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/ReadImage";
    }
    return nullptr;
}

static void* stub_FileFormat_ReadSamples(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/ReadSamples";
    }
    return nullptr;
}

static void* stub_FileFormat_SelectImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SelectImage";
    }
    return nullptr;
}

static void* stub_FileFormat_SetFormatSpecificData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetFormatSpecificData";
    }
    return nullptr;
}

static void* stub_FileFormat_SetICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetICCProfile";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageColorFilterArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageColorFilterArray";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageDisplayFunction(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageDisplayFunction";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageId";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageOptions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageOptions";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageProperty";
    }
    return nullptr;
}

static void* stub_FileFormat_SetImageRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetImageRGBWS";
    }
    return nullptr;
}

static void* stub_FileFormat_SetProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetProperty";
    }
    return nullptr;
}

static void* stub_FileFormat_SetThumbnail(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/SetThumbnail";
    }
    return nullptr;
}

static void* stub_FileFormat_ValidateFormatSpecificData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/ValidateFormatSpecificData";
    }
    return nullptr;
}

static void* stub_FileFormat_WasInexactRead(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/WasInexactRead";
    }
    return nullptr;
}

static void* stub_FileFormat_WasLossyWrite(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/WasLossyWrite";
    }
    return nullptr;
}

static void* stub_FileFormat_WriteImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/WriteImage";
    }
    return nullptr;
}

static void* stub_FileFormat_WriteSamples(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormat/WriteSamples";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_BeginFileFormatDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/BeginFileFormatDefinition";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_EndFileFormatDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/EndFileFormatDefinition";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_EnterFileFormatDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/EnterFileFormatDefinitionContext";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_ExitFileFormatDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/ExitFileFormatDefinitionContext";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_GetFileFormatBeingDefined(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/GetFileFormatBeingDefined";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_IsFileFormatDefinitionContextActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/IsFileFormatDefinitionContextActive";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatAddKeywordRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatAddKeywordRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatAllowIncrementalReadRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatAllowIncrementalReadRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatAllowIncrementalWriteRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatAllowIncrementalWriteRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginColorFilterArrayEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginColorFilterArrayEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginColorFilterArrayExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginColorFilterArrayExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginDisplayFunctionEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginDisplayFunctionEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginDisplayFunctionExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginDisplayFunctionExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginICCProfileEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginICCProfileEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginICCProfileExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginICCProfileExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginImagePropertyEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginImagePropertyEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginImagePropertyExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginImagePropertyExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginKeywordEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginKeywordEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginKeywordExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginKeywordExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginPropertyEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginPropertyEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginPropertyExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginPropertyExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginRGBWSEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginRGBWSEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginRGBWSExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginRGBWSExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginThumbnailEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginThumbnailEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatBeginThumbnailExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatBeginThumbnailExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCaps(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCaps";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCloseImageRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCloseImageRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCloseRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCloseRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCreateImageRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCreateImageRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCreateRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCreateRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatCreationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatCreationRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatDescription";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatDestructionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatDestructionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatDisposeFormatSpecificDataRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatDisposeFormatSpecificDataRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEditPreferencesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEditPreferencesRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndColorFilterArrayEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndColorFilterArrayEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndColorFilterArrayExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndColorFilterArrayExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndDisplayFunctionEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndDisplayFunctionEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndDisplayFunctionExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndDisplayFunctionExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndICCProfileEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndICCProfileEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndICCProfileExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndICCProfileExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndImagePropertyEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndImagePropertyEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndImagePropertyExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndImagePropertyExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndKeywordEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndKeywordEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndKeywordExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndKeywordExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndPropertyEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndPropertyEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndPropertyExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndPropertyExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndRGBWSEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndRGBWSEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndRGBWSExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndRGBWSExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndThumbnailEmbeddingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndThumbnailEmbeddingRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEndThumbnailExtractionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEndThumbnailExtractionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEnumerateImagePropertiesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEnumerateImagePropertiesRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatEnumeratePropertiesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatEnumeratePropertiesRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetFilePathRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetFilePathRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetFormatSpecificDataRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetFormatSpecificDataRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetICCProfileRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetICCProfileRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageColorFilterArrayRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageColorFilterArrayRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageCountRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageCountRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageDescriptionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageDescriptionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageDisplayFunctionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageDisplayFunctionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageFormatInfoRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageFormatInfoRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageIdRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageIdRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImagePropertyRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImagePropertyRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetImageRGBWSRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetImageRGBWSRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetKeywordCountRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetKeywordCountRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetNextKeywordRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetNextKeywordRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetPropertyRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetPropertyRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetSelectedImageIndexRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetSelectedImageIndexRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatGetThumbnailRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatGetThumbnailRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconImage";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconImageFile";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconSVG";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconSVGFile";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconSmallImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconSmallImage";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIconSmallImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIconSmallImageFile";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatImplementation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatImplementation";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatIsOpenRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatIsOpenRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatOpenRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatOpenRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatQueryFormatStatusRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatQueryFormatStatusRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatQueryInexactReadRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatQueryInexactReadRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatQueryLossyWriteRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatQueryLossyWriteRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatQueryOptionsRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatQueryOptionsRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatReadImageRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatReadImageRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatReadSamplesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatReadSamplesRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetFormatSpecificDataRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetFormatSpecificDataRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetICCProfileRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetICCProfileRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImageColorFilterArrayRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImageColorFilterArrayRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImageDisplayFunctionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImageDisplayFunctionRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImageIdRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImageIdRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImageOptionsRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImageOptionsRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImagePropertyRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImagePropertyRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetImageRGBWSRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetImageRGBWSRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetPropertyRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetPropertyRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetSelectedImageIndexRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetSelectedImageIndexRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatSetThumbnailRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatSetThumbnailRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatValidateFormatSpecificDataRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatValidateFormatSpecificDataRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatVersion";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatWriteImageRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatWriteImageRoutine";
    }
    return nullptr;
}

static void* stub_FileFormatDefinition_SetFileFormatWriteSamplesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "FileFormatDefinition/SetFileFormatWriteSamplesRoutine";
    }
    return nullptr;
}

static void* stub_Font_CloneFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/CloneFont";
    }
    return nullptr;
}

static void* stub_Font_CreateFontByFace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/CreateFontByFace";
    }
    return nullptr;
}

static void* stub_Font_CreateFontByFamily(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/CreateFontByFamily";
    }
    return nullptr;
}

static void* stub_Font_EnumerateFontStyles(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/EnumerateFontStyles";
    }
    return nullptr;
}

static void* stub_Font_EnumerateFonts(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/EnumerateFonts";
    }
    return nullptr;
}

static void* stub_Font_EnumerateOptimalFontPointSizes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/EnumerateOptimalFontPointSizes";
    }
    return nullptr;
}

static void* stub_Font_EnumerateWritingSystems(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/EnumerateWritingSystems";
    }
    return nullptr;
}

static void* stub_Font_GetCharPixelWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetCharPixelWidth";
    }
    return nullptr;
}

static void* stub_Font_GetFontAscent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontAscent";
    }
    return nullptr;
}

static void* stub_Font_GetFontCharDefined(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontCharDefined";
    }
    return nullptr;
}

static void* stub_Font_GetFontDescent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontDescent";
    }
    return nullptr;
}

static void* stub_Font_GetFontExactMatch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontExactMatch";
    }
    return nullptr;
}

static void* stub_Font_GetFontFace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontFace";
    }
    return nullptr;
}

static void* stub_Font_GetFontFixedPitch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontFixedPitch";
    }
    return nullptr;
}

static void* stub_Font_GetFontHeight(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontHeight";
    }
    return nullptr;
}

static void* stub_Font_GetFontItalic(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontItalic";
    }
    return nullptr;
}

static void* stub_Font_GetFontKerning(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontKerning";
    }
    return nullptr;
}

static void* stub_Font_GetFontLineSpacing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontLineSpacing";
    }
    return nullptr;
}

static void* stub_Font_GetFontMaxWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontMaxWidth";
    }
    return nullptr;
}

static void* stub_Font_GetFontOverline(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontOverline";
    }
    return nullptr;
}

static void* stub_Font_GetFontPixelSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontPixelSize";
    }
    return nullptr;
}

static void* stub_Font_GetFontPointSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontPointSize";
    }
    return nullptr;
}

static void* stub_Font_GetFontScalable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontScalable";
    }
    return nullptr;
}

static void* stub_Font_GetFontStretchFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontStretchFactor";
    }
    return nullptr;
}

static void* stub_Font_GetFontStrikeOut(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontStrikeOut";
    }
    return nullptr;
}

static void* stub_Font_GetFontUnderline(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontUnderline";
    }
    return nullptr;
}

static void* stub_Font_GetFontWeight(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetFontWeight";
    }
    return nullptr;
}

static void* stub_Font_GetNominalFontFixedPitch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetNominalFontFixedPitch";
    }
    return nullptr;
}

static void* stub_Font_GetNominalFontItalic(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetNominalFontItalic";
    }
    return nullptr;
}

static void* stub_Font_GetNominalFontWeight(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetNominalFontWeight";
    }
    return nullptr;
}

static void* stub_Font_GetStringPixelRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetStringPixelRect";
    }
    return nullptr;
}

static void* stub_Font_GetStringPixelWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/GetStringPixelWidth";
    }
    return nullptr;
}

static void* stub_Font_SetFontFace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontFace";
    }
    return nullptr;
}

static void* stub_Font_SetFontFixedPitch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontFixedPitch";
    }
    return nullptr;
}

static void* stub_Font_SetFontItalic(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontItalic";
    }
    return nullptr;
}

static void* stub_Font_SetFontKerning(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontKerning";
    }
    return nullptr;
}

static void* stub_Font_SetFontOverline(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontOverline";
    }
    return nullptr;
}

static void* stub_Font_SetFontPixelSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontPixelSize";
    }
    return nullptr;
}

static void* stub_Font_SetFontPointSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontPointSize";
    }
    return nullptr;
}

static void* stub_Font_SetFontStretchFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontStretchFactor";
    }
    return nullptr;
}

static void* stub_Font_SetFontStrikeOut(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontStrikeOut";
    }
    return nullptr;
}

static void* stub_Font_SetFontUnderline(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontUnderline";
    }
    return nullptr;
}

static void* stub_Font_SetFontWeight(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Font/SetFontWeight";
    }
    return nullptr;
}

static void* stub_Frame_CreateFrame(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/CreateFrame";
    }
    return nullptr;
}

static void* stub_Frame_GetFrameBorderWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/GetFrameBorderWidth";
    }
    return nullptr;
}

static void* stub_Frame_GetFrameLineWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/GetFrameLineWidth";
    }
    return nullptr;
}

static void* stub_Frame_GetFrameStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/GetFrameStyle";
    }
    return nullptr;
}

static void* stub_Frame_SetFrameLineWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/SetFrameLineWidth";
    }
    return nullptr;
}

static void* stub_Frame_SetFrameStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Frame/SetFrameStyle";
    }
    return nullptr;
}

static void* stub_GPU_EnumerateCUDADevices(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/EnumerateCUDADevices";
    }
    return nullptr;
}

static void* stub_GPU_GetCUDADeviceMaxThreadsPerBlock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/GetCUDADeviceMaxThreadsPerBlock";
    }
    return nullptr;
}

static void* stub_GPU_GetCUDADeviceProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/GetCUDADeviceProperties";
    }
    return nullptr;
}

static void* stub_GPU_GetCUDADeviceSharedMemoryPerBlock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/GetCUDADeviceSharedMemoryPerBlock";
    }
    return nullptr;
}

static void* stub_GPU_GetCUDADeviceTotalGlobalMem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/GetCUDADeviceTotalGlobalMem";
    }
    return nullptr;
}

static void* stub_GPU_GetCUDASelectedDevice(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/GetCUDASelectedDevice";
    }
    return nullptr;
}

static void* stub_GPU_InitCUDARuntime(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/InitCUDARuntime";
    }
    return nullptr;
}

static void* stub_GPU_IsCUDADeviceAvailable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GPU/IsCUDADeviceAvailable";
    }
    return nullptr;
}

static void* stub_Global_Abort(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/Abort";
    }
    return nullptr;
}

static void* stub_Global_Allocate(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/Allocate";
    }
    return nullptr;
}

static void* stub_Global_ApplicationInstanceSlot(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ApplicationInstanceSlot";
    }
    return nullptr;
}

static void* stub_Global_BroadcastGlobalFiltersUpdated(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/BroadcastGlobalFiltersUpdated";
    }
    return nullptr;
}

static void* stub_Global_BroadcastImageUpdated(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/BroadcastImageUpdated";
    }
    return nullptr;
}

static void* stub_Global_BrowseProcessDocumentation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/BrowseProcessDocumentation";
    }
    return nullptr;
}

static void* stub_Global_CancelGlobalSettingsUpdate(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/CancelGlobalSettingsUpdate";
    }
    return nullptr;
}

static void* stub_Global_ClearError(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ClearError";
    }
    return nullptr;
}

static void* stub_Global_Deallocate(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/Deallocate";
    }
    return nullptr;
}

static void* stub_Global_DeleteSettingsItem(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/DeleteSettingsItem";
    }
    return nullptr;
}

static void* stub_Global_DisableAbort(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/DisableAbort";
    }
    return nullptr;
}

static void* stub_Global_EnableAbort(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/EnableAbort";
    }
    return nullptr;
}

static void* stub_Global_EnterGlobalSettingsUpdateContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/EnterGlobalSettingsUpdateContext";
    }
    return nullptr;
}

static void* stub_Global_ErrorMessage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ErrorMessage";
    }
    return nullptr;
}

static void* stub_Global_ExecuteCommand(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ExecuteCommand";
    }
    return nullptr;
}

static void* stub_Global_ExitGlobalSettingsUpdateContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ExitGlobalSettingsUpdateContext";
    }
    return nullptr;
}

static void* stub_Global_FlushConsole(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/FlushConsole";
    }
    return nullptr;
}

static void* stub_Global_GetApplicationConfigurationDirectory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetApplicationConfigurationDirectory";
    }
    return nullptr;
}

static void* stub_Global_GetConsole(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetConsole";
    }
    return nullptr;
}

static void* stub_Global_GetConsoleText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetConsoleText";
    }
    return nullptr;
}

static void* stub_Global_GetCursorPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetCursorPosition";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalColor";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalFlag";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalFont";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalInteger";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalReal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalReal";
    }
    return nullptr;
}

static void* stub_Global_GetGlobalString(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetGlobalString";
    }
    return nullptr;
}

static void* stub_Global_GetKeyboardModifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetKeyboardModifiers";
    }
    return nullptr;
}

static void* stub_Global_GetLastScriptResult(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetLastScriptResult";
    }
    return nullptr;
}

static void* stub_Global_GetPixInsightCodename(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetPixInsightCodename";
    }
    return nullptr;
}

static void* stub_Global_GetPixInsightVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetPixInsightVersion";
    }
    return nullptr;
}

static void* stub_Global_GetPixelTraitsLUT(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetPixelTraitsLUT";
    }
    return nullptr;
}

static void* stub_Global_GetProcessStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetProcessStatus";
    }
    return nullptr;
}

static void* stub_Global_GetProfilesDirectory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetProfilesDirectory";
    }
    return nullptr;
}

static void* stub_Global_GetReadoutOptions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetReadoutOptions";
    }
    return nullptr;
}

static void* stub_Global_GetSettingsItemGlobalAccess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetSettingsItemGlobalAccess";
    }
    return nullptr;
}

static void* stub_Global_GetThreadWindowId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetThreadWindowId";
    }
    return nullptr;
}

static void* stub_Global_GetToolTipWindowText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/GetToolTipWindowText";
    }
    return nullptr;
}

static void* stub_Global_HideToolTipWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/HideToolTipWindow";
    }
    return nullptr;
}

static void* stub_Global_IsGlobalSettingsUpdateContextActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/IsGlobalSettingsUpdateContextActive";
    }
    return nullptr;
}

static void* stub_Global_IsRealTimePreviewUpdating(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/IsRealTimePreviewUpdating";
    }
    return nullptr;
}

static void* stub_Global_LastError(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/LastError";
    }
    return nullptr;
}

static void* stub_Global_LaunchProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/LaunchProcessInstance";
    }
    return nullptr;
}

static void* stub_Global_LaunchProcessInstanceOnView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/LaunchProcessInstanceOnView";
    }
    return nullptr;
}

static void* stub_Global_LaunchProcessInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/LaunchProcessInterface";
    }
    return nullptr;
}

static void* stub_Global_MaxProcessorsAllowedForModule(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/MaxProcessorsAllowedForModule";
    }
    return nullptr;
}

static void* stub_Global_MessageBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/MessageBox";
    }
    return nullptr;
}

static void* stub_Global_ProcessEvents(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ProcessEvents";
    }
    return nullptr;
}

static void* stub_Global_ReadConsoleChar(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadConsoleChar";
    }
    return nullptr;
}

static void* stub_Global_ReadConsoleString(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadConsoleString";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsBlock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsBlock";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsFlag";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsInteger";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsReal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsReal";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsString(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsString";
    }
    return nullptr;
}

static void* stub_Global_ReadSettingsUnsignedInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ReadSettingsUnsignedInteger";
    }
    return nullptr;
}

static void* stub_Global_ResetProcessStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ResetProcessStatus";
    }
    return nullptr;
}

static void* stub_Global_SetCursorPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetCursorPosition";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalColor";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalFlag";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalFont";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalInteger";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalReal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalReal";
    }
    return nullptr;
}

static void* stub_Global_SetGlobalString(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetGlobalString";
    }
    return nullptr;
}

static void* stub_Global_SetReadoutOptions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetReadoutOptions";
    }
    return nullptr;
}

static void* stub_Global_SetRealTimePreviewOwner(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetRealTimePreviewOwner";
    }
    return nullptr;
}

static void* stub_Global_SetSettingsItemGlobalAccess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/SetSettingsItemGlobalAccess";
    }
    return nullptr;
}

static void* stub_Global_ShowConsole(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ShowConsole";
    }
    return nullptr;
}

static void* stub_Global_ShowToolTipWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ShowToolTipWindow";
    }
    return nullptr;
}

static void* stub_Global_UpdateRealTimePreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/UpdateRealTimePreview";
    }
    return nullptr;
}

static void* stub_Global_ValidateConsole(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/ValidateConsole";
    }
    return nullptr;
}

static void* stub_Global_WriteConsole(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteConsole";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsBlock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsBlock";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsFlag";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsInteger";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsReal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsReal";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsString(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsString";
    }
    return nullptr;
}

static void* stub_Global_WriteSettingsUnsignedInteger(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Global/WriteSettingsUnsignedInteger";
    }
    return nullptr;
}

static void* stub_Graphics_BeginBitmapPaint(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/BeginBitmapPaint";
    }
    return nullptr;
}

static void* stub_Graphics_BeginControlPaint(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/BeginControlPaint";
    }
    return nullptr;
}

static void* stub_Graphics_BeginSVGPaint(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/BeginSVGPaint";
    }
    return nullptr;
}

static void* stub_Graphics_CreateGraphics(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/CreateGraphics";
    }
    return nullptr;
}

static void* stub_Graphics_DrawArc(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawArc";
    }
    return nullptr;
}

static void* stub_Graphics_DrawArcD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawArcD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawBitmap";
    }
    return nullptr;
}

static void* stub_Graphics_DrawBitmapD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawBitmapD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawBitmapRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawBitmapRect";
    }
    return nullptr;
}

static void* stub_Graphics_DrawBitmapRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawBitmapRectD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawChord(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawChord";
    }
    return nullptr;
}

static void* stub_Graphics_DrawChordD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawChordD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawEllipse(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawEllipse";
    }
    return nullptr;
}

static void* stub_Graphics_DrawEllipseD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawEllipseD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawLine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawLine";
    }
    return nullptr;
}

static void* stub_Graphics_DrawLineD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawLineD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPie(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPie";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPieD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPieD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPoint(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPoint";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPointD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPointD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPolygon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPolygon";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPolygonD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPolygonD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPolyline(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPolyline";
    }
    return nullptr;
}

static void* stub_Graphics_DrawPolylineD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawPolylineD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawRect";
    }
    return nullptr;
}

static void* stub_Graphics_DrawRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawRectD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawRoundedRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawRoundedRect";
    }
    return nullptr;
}

static void* stub_Graphics_DrawRoundedRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawRoundedRectD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawScaledBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawScaledBitmap";
    }
    return nullptr;
}

static void* stub_Graphics_DrawScaledBitmapD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawScaledBitmapD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawScaledBitmapRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawScaledBitmapRect";
    }
    return nullptr;
}

static void* stub_Graphics_DrawScaledBitmapRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawScaledBitmapRectD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawText";
    }
    return nullptr;
}

static void* stub_Graphics_DrawTextD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawTextD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawTextRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawTextRect";
    }
    return nullptr;
}

static void* stub_Graphics_DrawTextRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawTextRectD";
    }
    return nullptr;
}

static void* stub_Graphics_DrawTiledBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawTiledBitmap";
    }
    return nullptr;
}

static void* stub_Graphics_DrawTiledBitmapD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/DrawTiledBitmapD";
    }
    return nullptr;
}

static void* stub_Graphics_EnableGraphicsAntialiasing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EnableGraphicsAntialiasing";
    }
    return nullptr;
}

static void* stub_Graphics_EnableGraphicsClipping(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EnableGraphicsClipping";
    }
    return nullptr;
}

static void* stub_Graphics_EnableGraphicsSmoothInterpolation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EnableGraphicsSmoothInterpolation";
    }
    return nullptr;
}

static void* stub_Graphics_EnableGraphicsTextAntialiasing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EnableGraphicsTextAntialiasing";
    }
    return nullptr;
}

static void* stub_Graphics_EnableGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EnableGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_Graphics_EndPaint(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/EndPaint";
    }
    return nullptr;
}

static void* stub_Graphics_FillChord(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillChord";
    }
    return nullptr;
}

static void* stub_Graphics_FillChordD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillChordD";
    }
    return nullptr;
}

static void* stub_Graphics_FillEllipse(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillEllipse";
    }
    return nullptr;
}

static void* stub_Graphics_FillEllipseD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillEllipseD";
    }
    return nullptr;
}

static void* stub_Graphics_FillPie(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillPie";
    }
    return nullptr;
}

static void* stub_Graphics_FillPieD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillPieD";
    }
    return nullptr;
}

static void* stub_Graphics_FillPolygon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillPolygon";
    }
    return nullptr;
}

static void* stub_Graphics_FillPolygonD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillPolygonD";
    }
    return nullptr;
}

static void* stub_Graphics_FillRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillRect";
    }
    return nullptr;
}

static void* stub_Graphics_FillRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillRectD";
    }
    return nullptr;
}

static void* stub_Graphics_FillRoundedRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillRoundedRect";
    }
    return nullptr;
}

static void* stub_Graphics_FillRoundedRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/FillRoundedRectD";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsAntialiasingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsAntialiasingEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsBackgroundBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsBackgroundBrush";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsBrush";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsBrushOrigin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsBrushOrigin";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsBrushOriginD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsBrushOriginD";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsClipRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsClipRect";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsClipRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsClipRectD";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsClippingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsClippingEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsCompositionOperator(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsCompositionOperator";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsFont";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsOpacity(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsOpacity";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsPen(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsPen";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsSmoothInterpolationEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsSmoothInterpolationEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsStatus";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsTextAntialiasingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsTextAntialiasingEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsTransformationEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsTransformationEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsTransformationMatrix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsTransformationMatrix";
    }
    return nullptr;
}

static void* stub_Graphics_GetGraphicsTransparentBackgroundEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetGraphicsTransparentBackgroundEnabled";
    }
    return nullptr;
}

static void* stub_Graphics_GetTextRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetTextRect";
    }
    return nullptr;
}

static void* stub_Graphics_GetTextRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/GetTextRectD";
    }
    return nullptr;
}

static void* stub_Graphics_MultiplyGraphicsTransformationMatrix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/MultiplyGraphicsTransformationMatrix";
    }
    return nullptr;
}

static void* stub_Graphics_PopGraphicsState(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/PopGraphicsState";
    }
    return nullptr;
}

static void* stub_Graphics_PushGraphicsState(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/PushGraphicsState";
    }
    return nullptr;
}

static void* stub_Graphics_ResetGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/ResetGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_Graphics_RotateGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/RotateGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_Graphics_ScaleGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/ScaleGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsBackgroundBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsBackgroundBrush";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsBrush";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsBrushOrigin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsBrushOrigin";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsBrushOriginD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsBrushOriginD";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsClipRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsClipRect";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsClipRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsClipRectD";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsCompositionOperator(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsCompositionOperator";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsFont";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsOpacity(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsOpacity";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsPen(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsPen";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsTransformationMatrix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsTransformationMatrix";
    }
    return nullptr;
}

static void* stub_Graphics_SetGraphicsTransparentBackground(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/SetGraphicsTransparentBackground";
    }
    return nullptr;
}

static void* stub_Graphics_ShearGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/ShearGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeChord(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeChord";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeChordD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeChordD";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeEllipse(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeEllipse";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeEllipseD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeEllipseD";
    }
    return nullptr;
}

static void* stub_Graphics_StrokePie(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokePie";
    }
    return nullptr;
}

static void* stub_Graphics_StrokePieD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokePieD";
    }
    return nullptr;
}

static void* stub_Graphics_StrokePolygon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokePolygon";
    }
    return nullptr;
}

static void* stub_Graphics_StrokePolygonD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokePolygonD";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeRect";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeRectD";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeRoundedRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeRoundedRect";
    }
    return nullptr;
}

static void* stub_Graphics_StrokeRoundedRectD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/StrokeRoundedRectD";
    }
    return nullptr;
}

static void* stub_Graphics_TransformPoints(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/TransformPoints";
    }
    return nullptr;
}

static void* stub_Graphics_TranslateGraphicsTransformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Graphics/TranslateGraphicsTransformation";
    }
    return nullptr;
}

static void* stub_GroupBox_CreateGroupBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/CreateGroupBox";
    }
    return nullptr;
}

static void* stub_GroupBox_GetGroupBoxCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/GetGroupBoxCheckable";
    }
    return nullptr;
}

static void* stub_GroupBox_GetGroupBoxChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/GetGroupBoxChecked";
    }
    return nullptr;
}

static void* stub_GroupBox_GetGroupBoxTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/GetGroupBoxTitle";
    }
    return nullptr;
}

static void* stub_GroupBox_SetGroupBoxCheckEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/SetGroupBoxCheckEventRoutine";
    }
    return nullptr;
}

static void* stub_GroupBox_SetGroupBoxCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/SetGroupBoxCheckable";
    }
    return nullptr;
}

static void* stub_GroupBox_SetGroupBoxChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/SetGroupBoxChecked";
    }
    return nullptr;
}

static void* stub_GroupBox_SetGroupBoxTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "GroupBox/SetGroupBoxTitle";
    }
    return nullptr;
}

static void* stub_ImageView_BeginViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/BeginViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_CancelViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/CancelViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_CommitViewportUpdates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/CommitViewportUpdates";
    }
    return nullptr;
}

static void* stub_ImageView_CreateImageView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/CreateImageView";
    }
    return nullptr;
}

static void* stub_ImageView_CreateImageViewViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/CreateImageViewViewport";
    }
    return nullptr;
}

static void* stub_ImageView_DeleteImageViewICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/DeleteImageViewICCProfile";
    }
    return nullptr;
}

static void* stub_ImageView_EndViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/EndViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewCMEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewCMEnabled";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewDisplayChannel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewDisplayChannel";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewICCProfile";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewICCProfileLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewICCProfileLength";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewImage";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewImageGeometry(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewImageGeometry";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewMode";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewRGBWS";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewSampleFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewSampleFormat";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewTransparencyMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewTransparencyMode";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewViewportOrigin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewViewportOrigin";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewViewportPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewViewportPosition";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewViewportSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewViewportSize";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewVisibleViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewVisibleViewportRect";
    }
    return nullptr;
}

static void* stub_ImageView_GetImageViewZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetImageViewZoomFactor";
    }
    return nullptr;
}

static void* stub_ImageView_GetViewportBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetViewportBitmap";
    }
    return nullptr;
}

static void* stub_ImageView_GetViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_GetViewportUpdateRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GetViewportUpdateRect";
    }
    return nullptr;
}

static void* stub_ImageView_GlobalToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/GlobalToViewport";
    }
    return nullptr;
}

static void* stub_ImageView_ImageScalarToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageScalarToViewport";
    }
    return nullptr;
}

static void* stub_ImageView_ImageScalarToViewportArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageScalarToViewportArray";
    }
    return nullptr;
}

static void* stub_ImageView_ImageScalarToViewportArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageScalarToViewportArrayD";
    }
    return nullptr;
}

static void* stub_ImageView_ImageScalarToViewportD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageScalarToViewportD";
    }
    return nullptr;
}

static void* stub_ImageView_ImageToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageToViewport";
    }
    return nullptr;
}

static void* stub_ImageView_ImageToViewportArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageToViewportArray";
    }
    return nullptr;
}

static void* stub_ImageView_ImageToViewportArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageToViewportArrayD";
    }
    return nullptr;
}

static void* stub_ImageView_ImageToViewportD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ImageToViewportD";
    }
    return nullptr;
}

static void* stub_ImageView_IsImageViewColorImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/IsImageViewColorImage";
    }
    return nullptr;
}

static void* stub_ImageView_LoadImageViewICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/LoadImageViewICCProfile";
    }
    return nullptr;
}

static void* stub_ImageView_ModifyViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ModifyViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_RegenerateImageRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/RegenerateImageRect";
    }
    return nullptr;
}

static void* stub_ImageView_RegenerateImageViewViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/RegenerateImageViewViewport";
    }
    return nullptr;
}

static void* stub_ImageView_RegenerateViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/RegenerateViewportRect";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewCMEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewCMEnabled";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewDisplayChannel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewDisplayChannel";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewICCProfile";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewMode";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewRGBWS";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewSampleFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewSampleFormat";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewScrollEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewScrollEventRoutine";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewTransparencyMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewTransparencyMode";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewViewport";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewViewportPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewViewportPosition";
    }
    return nullptr;
}

static void* stub_ImageView_SetImageViewZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/SetImageViewZoomFactor";
    }
    return nullptr;
}

static void* stub_ImageView_UpdateImageRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/UpdateImageRect";
    }
    return nullptr;
}

static void* stub_ImageView_UpdateImageViewViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/UpdateImageViewViewport";
    }
    return nullptr;
}

static void* stub_ImageView_UpdateViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/UpdateViewportRect";
    }
    return nullptr;
}

static void* stub_ImageView_UpdateViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/UpdateViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportScalarToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportScalarToImage";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportScalarToImageArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportScalarToImageArray";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportScalarToImageArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportScalarToImageArrayD";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportScalarToImageD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportScalarToImageD";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportToGlobal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportToGlobal";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportToImage";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportToImageArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportToImageArray";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportToImageArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportToImageArrayD";
    }
    return nullptr;
}

static void* stub_ImageView_ViewportToImageD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageView/ViewportToImageD";
    }
    return nullptr;
}

static void* stub_ImageWindow_AddImageWindowKeyword(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/AddImageWindowKeyword";
    }
    return nullptr;
}

static void* stub_ImageWindow_BeginViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/BeginViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_BringImageWindowToFront(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/BringImageWindowToFront";
    }
    return nullptr;
}

static void* stub_ImageWindow_CancelViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CancelViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_CelestialToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CelestialToImage";
    }
    return nullptr;
}

static void* stub_ImageWindow_ClearImageWindowAstrometricSolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ClearImageWindowAstrometricSolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_CloseImageWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CloseImageWindow";
    }
    return nullptr;
}

static void* stub_ImageWindow_CommitViewportUpdates(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CommitViewportUpdates";
    }
    return nullptr;
}

static void* stub_ImageWindow_CopyImageWindowAstrometricSolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CopyImageWindowAstrometricSolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_CreateImageWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CreateImageWindow";
    }
    return nullptr;
}

static void* stub_ImageWindow_CreatePreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/CreatePreview";
    }
    return nullptr;
}

static void* stub_ImageWindow_DeleteImageWindowICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/DeleteImageWindowICCProfile";
    }
    return nullptr;
}

static void* stub_ImageWindow_DeletePreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/DeletePreview";
    }
    return nullptr;
}

static void* stub_ImageWindow_DeletePreviews(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/DeletePreviews";
    }
    return nullptr;
}

static void* stub_ImageWindow_EndViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/EndViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_EnumerateImageWindows(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/EnumerateImageWindows";
    }
    return nullptr;
}

static void* stub_ImageWindow_EnumeratePreviews(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/EnumeratePreviews";
    }
    return nullptr;
}

static void* stub_ImageWindow_FitImageWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/FitImageWindow";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetActiveDynamicInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetActiveDynamicInterface";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetActiveImageWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetActiveImageWindow";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetCursorTolerance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetCursorTolerance";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDefaultICCProfileEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDefaultICCProfileEmbedding";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDefaultPropertiesEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDefaultPropertiesEmbedding";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDefaultResolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDefaultResolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDefaultThumbnailEmbedding(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDefaultThumbnailEmbedding";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDynamicCursorBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDynamicCursorBitmap";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetDynamicCursorHotSpot(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetDynamicCursorHotSpot";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetGlobalRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetGlobalRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageType";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowByFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowByFilePath";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowById(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowById";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowCMEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowCMEnabled";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowCopyFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowCopyFlag";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowCurrentView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowCurrentView";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowDevicePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowDevicePixelRatio";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowDisplayChannel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowDisplayChannel";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowDisplayPixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowDisplayPixelRatio";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowFileInfo(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowFileInfo";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowFilePath";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowFileURL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowFileURL";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowGlobalRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowGlobalRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowHasAstrometricSolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowHasAstrometricSolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowICCProfile";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowICCProfileLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowICCProfileLength";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowIconic(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowIconic";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowKeyword(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowKeyword";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowKeywordCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowKeywordCount";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMainView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMainView";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMask";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMaskEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMaskEnabled";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMaskMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMaskMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMaskVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMaskVisible";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowModifyCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowModifyCount";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowNewFlag(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowNewFlag";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowResolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowResolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowResourcePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowResourcePixelRatio";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowSampleFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowSampleFormat";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowTransparencyMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowTransparencyMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowViewportOrigin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowViewportOrigin";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowViewportPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowViewportPosition";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowViewportSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowViewportSize";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowVisible";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowVisibleViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowVisibleViewportRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetImageWindowZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetImageWindowZoomFactor";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetMaskReferenceCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetMaskReferenceCount";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetPreviewById(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetPreviewById";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetPreviewCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetPreviewCount";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetPreviewRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetPreviewRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetSelectedPreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetSelectedPreview";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetSwapDirectory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetSwapDirectory";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetTransparencyBackgroundBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetTransparencyBackgroundBrush";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetViewportBitmap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetViewportBitmap";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_GetViewportUpdateRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GetViewportUpdateRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_GlobalToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/GlobalToViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageScalarToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageScalarToViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageScalarToViewportArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageScalarToViewportArray";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageScalarToViewportArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageScalarToViewportArrayD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageScalarToViewportD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageScalarToViewportD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageToCelestial(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageToCelestial";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageToViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageToViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageToViewportArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageToViewportArray";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageToViewportArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageToViewportArrayD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ImageToViewportD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ImageToViewportD";
    }
    return nullptr;
}

static void* stub_ImageWindow_LoadImageWindowICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/LoadImageWindowICCProfile";
    }
    return nullptr;
}

static void* stub_ImageWindow_LoadImageWindows(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/LoadImageWindows";
    }
    return nullptr;
}

static void* stub_ImageWindow_ModifyPreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ModifyPreview";
    }
    return nullptr;
}

static void* stub_ImageWindow_ModifyViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ModifyViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_PurgeImageWindowProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/PurgeImageWindowProperties";
    }
    return nullptr;
}

static void* stub_ImageWindow_RegenerateImageRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/RegenerateImageRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_RegenerateImageWindowAstrometricSolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/RegenerateImageWindowAstrometricSolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_RegenerateImageWindowViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/RegenerateImageWindowViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_RegenerateViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/RegenerateViewportRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_RemoveImageWindowMaskReferences(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/RemoveImageWindowMaskReferences";
    }
    return nullptr;
}

static void* stub_ImageWindow_ResetImageWindowKeywords(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ResetImageWindowKeywords";
    }
    return nullptr;
}

static void* stub_ImageWindow_SelectPreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SelectPreview";
    }
    return nullptr;
}

static void* stub_ImageWindow_SendImageWindowToBack(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SendImageWindowToBack";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetDynamicCursor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetDynamicCursor";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetDynamicCursorXPM(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetDynamicCursorXPM";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetGlobalRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetGlobalRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageType";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowCMEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowCMEnabled";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowCurrentView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowCurrentView";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowDisplayChannel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowDisplayChannel";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowGlobalRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowGlobalRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowICCProfile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowICCProfile";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowIconic(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowIconic";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowMask";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowMaskEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowMaskEnabled";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowMaskMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowMaskMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowMaskVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowMaskVisible";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowRGBWS";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowResolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowResolution";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowSampleFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowSampleFormat";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowTransparencyMode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowTransparencyMode";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowViewportPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowViewportPosition";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowVisible";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetImageWindowZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetImageWindowZoomFactor";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetSwapDirectories(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetSwapDirectories";
    }
    return nullptr;
}

static void* stub_ImageWindow_SetTransparencyBackgroundBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/SetTransparencyBackgroundBrush";
    }
    return nullptr;
}

static void* stub_ImageWindow_TerminateDynamicSession(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/TerminateDynamicSession";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateImageRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateImageRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateImageWindowAstrometryMetadata(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateImageWindowAstrometryMetadata";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateImageWindowMaskReferences(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateImageWindowMaskReferences";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateImageWindowViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateImageWindowViewport";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateViewportRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateViewportRect";
    }
    return nullptr;
}

static void* stub_ImageWindow_UpdateViewportSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/UpdateViewportSelection";
    }
    return nullptr;
}

static void* stub_ImageWindow_ValidateImageWindowMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ValidateImageWindowMask";
    }
    return nullptr;
}

static void* stub_ImageWindow_ValidateImageWindowView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ValidateImageWindowView";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportScalarToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportScalarToImage";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportScalarToImageArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportScalarToImageArray";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportScalarToImageArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportScalarToImageArrayD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportScalarToImageD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportScalarToImageD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportToGlobal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportToGlobal";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportToImage";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportToImageArray(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportToImageArray";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportToImageArrayD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportToImageArrayD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ViewportToImageD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ViewportToImageD";
    }
    return nullptr;
}

static void* stub_ImageWindow_ZoomImageWindowToFit(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ImageWindow/ZoomImageWindowToFit";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_BeginInterfaceDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/BeginInterfaceDefinition";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_EndInterfaceDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/EndInterfaceDefinition";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_EnterInterfaceDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/EnterInterfaceDefinitionContext";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_ExitInterfaceDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/ExitInterfaceDefinitionContext";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_GetInterfaceBeingDefined(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/GetInterfaceBeingDefined";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_IsInterfaceDefinitionContextActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/IsInterfaceDefinitionContextActive";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetBeginReadoutNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetBeginReadoutNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetEndReadoutNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetEndReadoutNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalCMDisabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalCMDisabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalCMEnabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalCMEnabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalCMUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalCMUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalFiltersUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalFiltersUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalPreferencesUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalPreferencesUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetGlobalRGBWSUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetGlobalRGBWSUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageCMDisabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageCMDisabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageCMEnabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageCMEnabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageCMUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageCMUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageCreatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageCreatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageDeletedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageDeletedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageFocusedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageFocusedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageLockedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageLockedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageRGBWSUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageRGBWSUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageRenamedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageRenamedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageSTFDisabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageSTFDisabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageSTFEnabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageSTFEnabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageSTFUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageSTFUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageSavedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageSavedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageUnlockedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageUnlockedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetImageUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetImageUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceApplyGlobalRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceApplyGlobalRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceApplyRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceApplyRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceBrowseDocumentationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceBrowseDocumentationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceCancelRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceCancelRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDescription";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicKeyPressRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicKeyPressRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicKeyReleaseRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicKeyReleaseRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicModeEnterRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicModeEnterRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicModeExitRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicModeExitRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseDoubleClickRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseDoubleClickRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseEnterRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseEnterRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseLeaveRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseLeaveRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseMoveRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseMoveRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMousePressRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMousePressRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseReleaseRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseReleaseRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicMouseWheelRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicMouseWheelRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicPaintRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicPaintRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceDynamicUpdateQueryRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceDynamicUpdateQueryRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceEditPreferencesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceEditPreferencesRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceExecuteRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceExecuteRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceFeatures(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceFeatures";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconImage";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconImageFile";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconSVG";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconSVGFile";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconSmallImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconSmallImage";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceIconSmallImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceIconSmallImageFile";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceInitializationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceInitializationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceLaunchRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceLaunchRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceProcessImportRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceProcessImportRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceProcessInstantiationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceProcessInstantiationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceProcessTestInstantiationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceProcessTestInstantiationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceProcessValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceProcessValidationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceRealTimeCancelRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceRealTimeCancelRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceRealTimeGenerationFlagsRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceRealTimeGenerationFlagsRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceRealTimeGenerationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceRealTimeGenerationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceRealTimePreviewUpdatedRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceRealTimePreviewUpdatedRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceRealTimeUpdateQueryRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceRealTimeUpdateQueryRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceResetRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceResetRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceTrackViewUpdatedRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceTrackViewUpdatedRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetInterfaceVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetInterfaceVersion";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetMaskDisabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetMaskDisabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetMaskEnabledNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetMaskEnabledNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetMaskHiddenNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetMaskHiddenNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetMaskShownNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetMaskShownNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetMaskUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetMaskUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetProcessCreatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetProcessCreatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetProcessDeletedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetProcessDeletedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetProcessSavedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetProcessSavedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetProcessUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetProcessUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetReadoutOptionsUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetReadoutOptionsUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetRealTimePreviewGenerationFinishNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetRealTimePreviewGenerationFinishNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetRealTimePreviewGenerationStartNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetRealTimePreviewGenerationStartNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetRealTimePreviewLUTUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetRealTimePreviewLUTUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetRealTimePreviewOwnerChangeNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetRealTimePreviewOwnerChangeNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetTransparencyHiddenNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetTransparencyHiddenNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetTransparencyModeUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetTransparencyModeUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetUpdateReadoutNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetUpdateReadoutNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetViewPropertyDeletedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetViewPropertyDeletedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_InterfaceDefinition_SetViewPropertyUpdatedNotificationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "InterfaceDefinition/SetViewPropertyUpdatedNotificationRoutine";
    }
    return nullptr;
}

static void* stub_Label_CreateLabel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/CreateLabel";
    }
    return nullptr;
}

static void* stub_Label_GetLabelAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/GetLabelAlignment";
    }
    return nullptr;
}

static void* stub_Label_GetLabelMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/GetLabelMargin";
    }
    return nullptr;
}

static void* stub_Label_GetLabelRichTextEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/GetLabelRichTextEnabled";
    }
    return nullptr;
}

static void* stub_Label_GetLabelText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/GetLabelText";
    }
    return nullptr;
}

static void* stub_Label_GetLabelWordWrappingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/GetLabelWordWrappingEnabled";
    }
    return nullptr;
}

static void* stub_Label_SetLabelAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/SetLabelAlignment";
    }
    return nullptr;
}

static void* stub_Label_SetLabelMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/SetLabelMargin";
    }
    return nullptr;
}

static void* stub_Label_SetLabelRichTextEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/SetLabelRichTextEnabled";
    }
    return nullptr;
}

static void* stub_Label_SetLabelText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/SetLabelText";
    }
    return nullptr;
}

static void* stub_Label_SetLabelWordWrappingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Label/SetLabelWordWrappingEnabled";
    }
    return nullptr;
}

static void* stub_Module_EvaluateScript(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Module/EvaluateScript";
    }
    return nullptr;
}

static void* stub_Module_HasEntitlement(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Module/HasEntitlement";
    }
    return nullptr;
}

static void* stub_Module_LoadResource(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Module/LoadResource";
    }
    return nullptr;
}

static void* stub_Module_UnloadResource(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Module/UnloadResource";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_EnterModuleDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/EnterModuleDefinitionContext";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_ExitModuleDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/ExitModuleDefinitionContext";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_IsModuleDefinitionContextActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/IsModuleDefinitionContextActive";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_SetModuleAllocationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/SetModuleAllocationRoutine";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_SetModuleDeallocationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/SetModuleDeallocationRoutine";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_SetModuleOnLoadRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/SetModuleOnLoadRoutine";
    }
    return nullptr;
}

static void* stub_ModuleDefinition_SetModuleOnUnloadRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ModuleDefinition/SetModuleOnUnloadRoutine";
    }
    return nullptr;
}

static void* stub_Mutex_CreateMutex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/CreateMutex";
    }
    return nullptr;
}

static void* stub_Mutex_CreateReadWriteMutex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/CreateReadWriteMutex";
    }
    return nullptr;
}

static void* stub_Mutex_GetLockState(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/GetLockState";
    }
    return nullptr;
}

static void* stub_Mutex_Lock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/Lock";
    }
    return nullptr;
}

static void* stub_Mutex_LockForRead(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/LockForRead";
    }
    return nullptr;
}

static void* stub_Mutex_LockForWrite(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/LockForWrite";
    }
    return nullptr;
}

static void* stub_Mutex_Unlock(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Mutex/Unlock";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_CloseNetworkTransferConnection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/CloseNetworkTransferConnection";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_CreateNetworkTransfer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/CreateNetworkTransfer";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferBytesTransferred(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferBytesTransferred";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferContentType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferContentType";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferCustomHTTPHeaders(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferCustomHTTPHeaders";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferErrorInformation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferErrorInformation";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferIsAborted(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferIsAborted";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferProxyURL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferProxyURL";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferResponseCode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferResponseCode";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferStatus";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferTotalSpeed(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferTotalSpeed";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferTotalTime(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferTotalTime";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_GetNetworkTransferURL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/GetNetworkTransferURL";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_PerformNetworkTransferDownload(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/PerformNetworkTransferDownload";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_PerformNetworkTransferPOST(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/PerformNetworkTransferPOST";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_PerformNetworkTransferSMTP(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/PerformNetworkTransferSMTP";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_PerformNetworkTransferUpload(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/PerformNetworkTransferUpload";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferConnectionTimeout(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferConnectionTimeout";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferCustomHTTPHeaders(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferCustomHTTPHeaders";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferDownloadEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferDownloadEventRoutine";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferProgressEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferProgressEventRoutine";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferProxyURL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferProxyURL";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferSSL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferSSL";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferURL(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferURL";
    }
    return nullptr;
}

static void* stub_NetworkTransfer_SetNetworkTransferUploadEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "NetworkTransfer/SetNetworkTransferUploadEventRoutine";
    }
    return nullptr;
}

static void* stub_Numerical_CubicSplineGenerateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/CubicSplineGenerateD";
    }
    return nullptr;
}

static void* stub_Numerical_CubicSplineGenerateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/CubicSplineGenerateF";
    }
    return nullptr;
}

static void* stub_Numerical_CubicSplineInterpolateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/CubicSplineInterpolateD";
    }
    return nullptr;
}

static void* stub_Numerical_CubicSplineInterpolateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/CubicSplineInterpolateF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexInverseTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexInverseTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexInverseTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexInverseTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexOptimizedLengthD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexOptimizedLengthD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexOptimizedLengthF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexOptimizedLengthF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTComplexTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTComplexTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateComplexInverseTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateComplexInverseTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateComplexInverseTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateComplexInverseTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateComplexTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateComplexTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateComplexTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateComplexTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateRealInverseTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateRealInverseTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateRealInverseTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateRealInverseTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateRealTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateRealTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTCreateRealTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTCreateRealTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTDestroyTransform(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTDestroyTransform";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealInverseTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealInverseTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealInverseTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealInverseTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealOptimizedLengthD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealOptimizedLengthD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealOptimizedLengthF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealOptimizedLengthF";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealTransformD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealTransformD";
    }
    return nullptr;
}

static void* stub_Numerical_FFTRealTransformF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/FFTRealTransformF";
    }
    return nullptr;
}

static void* stub_Numerical_GaussJordanInPlaceD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/GaussJordanInPlaceD";
    }
    return nullptr;
}

static void* stub_Numerical_GaussJordanInPlaceF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/GaussJordanInPlaceF";
    }
    return nullptr;
}

static void* stub_Numerical_LinearFitD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/LinearFitD";
    }
    return nullptr;
}

static void* stub_Numerical_LinearFitF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/LinearFitF";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalCubicSplineGenerateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalCubicSplineGenerateD";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalCubicSplineGenerateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalCubicSplineGenerateF";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineGenerateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineGenerateD";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineGenerateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineGenerateF";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineGenerateUI16(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineGenerateUI16";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineGenerateUI32(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineGenerateUI32";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineGenerateUI8(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineGenerateUI8";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineInterpolateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineInterpolateD";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineInterpolateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineInterpolateF";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineInterpolateUI16(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineInterpolateUI16";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineInterpolateUI32(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineInterpolateUI32";
    }
    return nullptr;
}

static void* stub_Numerical_NaturalGridCubicSplineInterpolateUI8(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/NaturalGridCubicSplineInterpolateUI8";
    }
    return nullptr;
}

static void* stub_Numerical_SVDInPlaceD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SVDInPlaceD";
    }
    return nullptr;
}

static void* stub_Numerical_SVDInPlaceF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SVDInPlaceF";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineCreateD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineCreateD";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineCreateF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineCreateF";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineDeserialize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineDeserialize";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineDestroy(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineDestroy";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineDuplicate(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineDuplicate";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineEvaluate(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineEvaluate";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineEvaluateVectorD(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineEvaluateVectorD";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineEvaluateVectorF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineEvaluateVectorF";
    }
    return nullptr;
}

static void* stub_Numerical_SurfaceSplineSerialize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Numerical/SurfaceSplineSerialize";
    }
    return nullptr;
}

static void* stub_Pen_ClonePen(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/ClonePen";
    }
    return nullptr;
}

static void* stub_Pen_CreatePen(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/CreatePen";
    }
    return nullptr;
}

static void* stub_Pen_GetPenBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenBrush";
    }
    return nullptr;
}

static void* stub_Pen_GetPenCap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenCap";
    }
    return nullptr;
}

static void* stub_Pen_GetPenColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenColor";
    }
    return nullptr;
}

static void* stub_Pen_GetPenJoin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenJoin";
    }
    return nullptr;
}

static void* stub_Pen_GetPenStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenStyle";
    }
    return nullptr;
}

static void* stub_Pen_GetPenWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/GetPenWidth";
    }
    return nullptr;
}

static void* stub_Pen_SetPenBrush(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenBrush";
    }
    return nullptr;
}

static void* stub_Pen_SetPenCap(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenCap";
    }
    return nullptr;
}

static void* stub_Pen_SetPenColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenColor";
    }
    return nullptr;
}

static void* stub_Pen_SetPenJoin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenJoin";
    }
    return nullptr;
}

static void* stub_Pen_SetPenStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenStyle";
    }
    return nullptr;
}

static void* stub_Pen_SetPenWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Pen/SetPenWidth";
    }
    return nullptr;
}

static void* stub_Process_AllocateTableRows(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/AllocateTableRows";
    }
    return nullptr;
}

static void* stub_Process_AssignProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/AssignProcessInstance";
    }
    return nullptr;
}

static void* stub_Process_BrowseProcessDocumentation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/BrowseProcessDocumentation";
    }
    return nullptr;
}

static void* stub_Process_CloneProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/CloneProcessInstance";
    }
    return nullptr;
}

static void* stub_Process_CreateProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/CreateProcessInstance";
    }
    return nullptr;
}

static void* stub_Process_CreateProcessInstanceFromIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/CreateProcessInstanceFromIcon";
    }
    return nullptr;
}

static void* stub_Process_CreateProcessInstanceFromSourceCode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/CreateProcessInstanceFromSourceCode";
    }
    return nullptr;
}

static void* stub_Process_EditProcessPreferences(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EditProcessPreferences";
    }
    return nullptr;
}

static void* stub_Process_EnumerateProcessCategories(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EnumerateProcessCategories";
    }
    return nullptr;
}

static void* stub_Process_EnumerateProcessIcons(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EnumerateProcessIcons";
    }
    return nullptr;
}

static void* stub_Process_EnumerateProcessParameters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EnumerateProcessParameters";
    }
    return nullptr;
}

static void* stub_Process_EnumerateProcesses(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EnumerateProcesses";
    }
    return nullptr;
}

static void* stub_Process_EnumerateTableColumns(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/EnumerateTableColumns";
    }
    return nullptr;
}

static void* stub_Process_ExecuteGlobal(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ExecuteGlobal";
    }
    return nullptr;
}

static void* stub_Process_ExecuteOnImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ExecuteOnImage";
    }
    return nullptr;
}

static void* stub_Process_ExecuteOnView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ExecuteOnView";
    }
    return nullptr;
}

static void* stub_Process_GetExecutionTimes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetExecutionTimes";
    }
    return nullptr;
}

static void* stub_Process_GetInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetInterface";
    }
    return nullptr;
}

static void* stub_Process_GetParameterAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_Process_GetParameterAllowedCharacters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterAllowedCharacters";
    }
    return nullptr;
}

static void* stub_Process_GetParameterByName(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterByName";
    }
    return nullptr;
}

static void* stub_Process_GetParameterDefaultElementIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterDefaultElementIndex";
    }
    return nullptr;
}

static void* stub_Process_GetParameterDefaultValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterDefaultValue";
    }
    return nullptr;
}

static void* stub_Process_GetParameterDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterDescription";
    }
    return nullptr;
}

static void* stub_Process_GetParameterElementAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterElementAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_Process_GetParameterElementCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterElementCount";
    }
    return nullptr;
}

static void* stub_Process_GetParameterElementIdentifier(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterElementIdentifier";
    }
    return nullptr;
}

static void* stub_Process_GetParameterElementValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterElementValue";
    }
    return nullptr;
}

static void* stub_Process_GetParameterIdentifier(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterIdentifier";
    }
    return nullptr;
}

static void* stub_Process_GetParameterLengthLimits(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterLengthLimits";
    }
    return nullptr;
}

static void* stub_Process_GetParameterPrecision(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterPrecision";
    }
    return nullptr;
}

static void* stub_Process_GetParameterProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterProcess";
    }
    return nullptr;
}

static void* stub_Process_GetParameterRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterRange";
    }
    return nullptr;
}

static void* stub_Process_GetParameterReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterReadOnly";
    }
    return nullptr;
}

static void* stub_Process_GetParameterRequired(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterRequired";
    }
    return nullptr;
}

static void* stub_Process_GetParameterScientificNotation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterScientificNotation";
    }
    return nullptr;
}

static void* stub_Process_GetParameterScriptComment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterScriptComment";
    }
    return nullptr;
}

static void* stub_Process_GetParameterTable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterTable";
    }
    return nullptr;
}

static void* stub_Process_GetParameterType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterType";
    }
    return nullptr;
}

static void* stub_Process_GetParameterValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetParameterValue";
    }
    return nullptr;
}

static void* stub_Process_GetProcessAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_Process_GetProcessByName(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessByName";
    }
    return nullptr;
}

static void* stub_Process_GetProcessCategory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessCategory";
    }
    return nullptr;
}

static void* stub_Process_GetProcessDefaultInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessDefaultInterface";
    }
    return nullptr;
}

static void* stub_Process_GetProcessDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessDescription";
    }
    return nullptr;
}

static void* stub_Process_GetProcessIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessIcon";
    }
    return nullptr;
}

static void* stub_Process_GetProcessIdentifier(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessIdentifier";
    }
    return nullptr;
}

static void* stub_Process_GetProcessInstanceDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessInstanceDescription";
    }
    return nullptr;
}

static void* stub_Process_GetProcessInstanceProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessInstanceProcess";
    }
    return nullptr;
}

static void* stub_Process_GetProcessInstanceSourceCode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessInstanceSourceCode";
    }
    return nullptr;
}

static void* stub_Process_GetProcessInstanceVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessInstanceVersion";
    }
    return nullptr;
}

static void* stub_Process_GetProcessProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessProperties";
    }
    return nullptr;
}

static void* stub_Process_GetProcessScriptComment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessScriptComment";
    }
    return nullptr;
}

static void* stub_Process_GetProcessSmallIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessSmallIcon";
    }
    return nullptr;
}

static void* stub_Process_GetProcessVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetProcessVersion";
    }
    return nullptr;
}

static void* stub_Process_GetTableColumnByName(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetTableColumnByName";
    }
    return nullptr;
}

static void* stub_Process_GetTableRowCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetTableRowCount";
    }
    return nullptr;
}

static void* stub_Process_GetUpdatesViewHistory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/GetUpdatesViewHistory";
    }
    return nullptr;
}

static void* stub_Process_LaunchInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/LaunchInterface";
    }
    return nullptr;
}

static void* stub_Process_LaunchProcess(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/LaunchProcess";
    }
    return nullptr;
}

static void* stub_Process_LaunchProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/LaunchProcessInstance";
    }
    return nullptr;
}

static void* stub_Process_RunProcessCommandLine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/RunProcessCommandLine";
    }
    return nullptr;
}

static void* stub_Process_SetParameterValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/SetParameterValue";
    }
    return nullptr;
}

static void* stub_Process_SetProcessInstanceDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/SetProcessInstanceDescription";
    }
    return nullptr;
}

static void* stub_Process_ValidateGlobalExecution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateGlobalExecution";
    }
    return nullptr;
}

static void* stub_Process_ValidateImageExecution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateImageExecution";
    }
    return nullptr;
}

static void* stub_Process_ValidateInterface(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateInterface";
    }
    return nullptr;
}

static void* stub_Process_ValidateInterfaceLaunch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateInterfaceLaunch";
    }
    return nullptr;
}

static void* stub_Process_ValidateProcessInstance(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateProcessInstance";
    }
    return nullptr;
}

static void* stub_Process_ValidateViewExecution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateViewExecution";
    }
    return nullptr;
}

static void* stub_Process_ValidateViewExecutionMask(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Process/ValidateViewExecutionMask";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_BeginParameterDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/BeginParameterDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_BeginProcessDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/BeginProcessDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_BeginTableColumnDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/BeginTableColumnDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_DefineEnumerationAlias(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/DefineEnumerationAlias";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_DefineEnumerationElement(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/DefineEnumerationElement";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_EndParameterDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/EndParameterDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_EndProcessDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/EndProcessDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_EndTableColumnDefinition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/EndTableColumnDefinition";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_EnterProcessDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/EnterProcessDefinitionContext";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_ExitProcessDefinitionContext(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/ExitProcessDefinitionContext";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_GetParameterBeingDefined(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/GetParameterBeingDefined";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_GetProcessBeingDefined(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/GetProcessBeingDefined";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_IsProcessDefinitionContextActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/IsProcessDefinitionContextActive";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetBlockSizeLimits(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetBlockSizeLimits";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetDefaultBooleanValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetDefaultBooleanValue";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetDefaultEnumerationValueIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetDefaultEnumerationValueIndex";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetDefaultNumericValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetDefaultNumericValue";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetDefaultStringValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetDefaultStringValue";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterAllocationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterAllocationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterDescription";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterLengthQueryRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterLengthQueryRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterLockRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterLockRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterProcessVersionRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterProcessVersionRange";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterReadOnly";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterRequired(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterRequired";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterScriptComment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterScriptComment";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterUnlockRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterUnlockRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetParameterValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetParameterValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetPrecision(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetPrecision";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessAliasIdentifiers(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessAliasIdentifiers";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessAssignmentRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessAssignmentRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessBrowseDocumentationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessBrowseDocumentationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessCategory(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessCategory";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessClassInitializationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessClassInitializationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessClonationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessClonationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessCommandLineProcessingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessCommandLineProcessingRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessCreationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessCreationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessDefaultInterfaceSelectionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessDefaultInterfaceSelectionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessDescription";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessDestructionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessDestructionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessEditPreferencesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessEditPreferencesRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessExecutionPreferencesRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessExecutionPreferencesRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessExecutionValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessExecutionValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessGlobalExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessGlobalExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessGlobalExecutionValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessGlobalExecutionValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessHistoryUpdateValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessHistoryUpdateValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIPCGetStatusRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIPCGetStatusRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIPCSetParametersRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIPCSetParametersRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIPCStartRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIPCStartRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIPCStopRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIPCStopRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconImage";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconImageFile";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconSVG(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconSVG";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconSVGFile";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconSmallImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconSmallImage";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessIconSmallImageFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessIconSmallImageFile";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessImageExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessImageExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessImageExecutionValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessImageExecutionValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessInitializationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessInitializationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessInterfaceSelectionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessInterfaceSelectionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessInterfaceValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessInterfaceValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessMaskValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessMaskValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPostExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPostExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPostGlobalExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPostGlobalExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPostReadingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPostReadingRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPostWritingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPostWritingRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPreExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPreExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPreGlobalExecutionRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPreGlobalExecutionRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPreReadingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPreReadingRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessPreWritingRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessPreWritingRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessScriptComment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessScriptComment";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessSetServerHandleRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessSetServerHandleRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessTestClonationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessTestClonationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessUndoModeRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessUndoModeRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessValidationRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessValidationRoutine";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetProcessVersion(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetProcessVersion";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetScientificNotation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetScientificNotation";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetStringAllowedCharacters(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetStringAllowedCharacters";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetStringLengthLimits(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetStringLengthLimits";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetTableRowLimits(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetTableRowLimits";
    }
    return nullptr;
}

static void* stub_ProcessDefinition_SetValidNumericRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ProcessDefinition/SetValidNumericRange";
    }
    return nullptr;
}

static void* stub_RealTimePreview_CloseRealTimePreviewProgressDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/CloseRealTimePreviewProgressDialog";
    }
    return nullptr;
}

static void* stub_RealTimePreview_IsRealTimePreviewProgressDialogVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/IsRealTimePreviewProgressDialogVisible";
    }
    return nullptr;
}

static void* stub_RealTimePreview_IsRealTimePreviewUpdating(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/IsRealTimePreviewUpdating";
    }
    return nullptr;
}

static void* stub_RealTimePreview_SetRealTimePreviewOwner(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/SetRealTimePreviewOwner";
    }
    return nullptr;
}

static void* stub_RealTimePreview_SetRealTimePreviewProgressCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/SetRealTimePreviewProgressCount";
    }
    return nullptr;
}

static void* stub_RealTimePreview_SetRealTimePreviewProgressText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/SetRealTimePreviewProgressText";
    }
    return nullptr;
}

static void* stub_RealTimePreview_ShowRealTimePreviewProgressDialog(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/ShowRealTimePreviewProgressDialog";
    }
    return nullptr;
}

static void* stub_RealTimePreview_UpdateRealTimePreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "RealTimePreview/UpdateRealTimePreview";
    }
    return nullptr;
}

static void* stub_SVG_CreateSVGBuffer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/CreateSVGBuffer";
    }
    return nullptr;
}

static void* stub_SVG_CreateSVGFile(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/CreateSVGFile";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGDataBuffer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGDataBuffer";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGDescription";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGDimensions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGDimensions";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGFilePath(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGFilePath";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGResolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGResolution";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGTitle";
    }
    return nullptr;
}

static void* stub_SVG_GetSVGViewBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/GetSVGViewBox";
    }
    return nullptr;
}

static void* stub_SVG_IsSVGPainting(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/IsSVGPainting";
    }
    return nullptr;
}

static void* stub_SVG_SetSVGDescription(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/SetSVGDescription";
    }
    return nullptr;
}

static void* stub_SVG_SetSVGDimensions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/SetSVGDimensions";
    }
    return nullptr;
}

static void* stub_SVG_SetSVGResolution(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/SetSVGResolution";
    }
    return nullptr;
}

static void* stub_SVG_SetSVGTitle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/SetSVGTitle";
    }
    return nullptr;
}

static void* stub_SVG_SetSVGViewBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SVG/SetSVGViewBox";
    }
    return nullptr;
}

static void* stub_ScrollBox_CreateScrollBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/CreateScrollBox";
    }
    return nullptr;
}

static void* stub_ScrollBox_CreateScrollBoxViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/CreateScrollBoxViewport";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBarsVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBarsVisible";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxAutoScrollEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxAutoScrollEnabled";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxHorizontalRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxHorizontalRange";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxLineSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxLineSize";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxPageSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxPageSize";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxPosition";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxTrackingEnabled";
    }
    return nullptr;
}

static void* stub_ScrollBox_GetScrollBoxVerticalRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/GetScrollBoxVerticalRange";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBarsVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBarsVisible";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxAutoScrollEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxAutoScrollEnabled";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxHorizontalPosUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxHorizontalPosUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxHorizontalRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxHorizontalRange";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxHorizontalRangeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxHorizontalRangeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxLineSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxLineSize";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxPageSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxPageSize";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxPosition";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxTrackingEnabled";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxVerticalPosUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxVerticalPosUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxVerticalRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxVerticalRange";
    }
    return nullptr;
}

static void* stub_ScrollBox_SetScrollBoxVerticalRangeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ScrollBox/SetScrollBoxVerticalRangeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_SharedImage_AttachToImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/AttachToImage";
    }
    return nullptr;
}

static void* stub_SharedImage_CreateImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/CreateImage";
    }
    return nullptr;
}

static void* stub_SharedImage_DetachFromImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/DetachFromImage";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageColorSpace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageColorSpace";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageFormat(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageFormat";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageGeometry(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageGeometry";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageOwner(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageOwner";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImagePixelData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImagePixelData";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageRGBWS";
    }
    return nullptr;
}

static void* stub_SharedImage_GetImageRefCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/GetImageRefCount";
    }
    return nullptr;
}

static void* stub_SharedImage_IsValidImageHandle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/IsValidImageHandle";
    }
    return nullptr;
}

static void* stub_SharedImage_SetImageColorSpace(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/SetImageColorSpace";
    }
    return nullptr;
}

static void* stub_SharedImage_SetImageGeometry(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/SetImageGeometry";
    }
    return nullptr;
}

static void* stub_SharedImage_SetImagePixelData(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/SetImagePixelData";
    }
    return nullptr;
}

static void* stub_SharedImage_SetImageRGBWS(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SharedImage/SetImageRGBWS";
    }
    return nullptr;
}

static void* stub_Sizer_CreateSizer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/CreateSizer";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerControlIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerControlIndex";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerCount";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerDevicePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerDevicePixelRatio";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerDisplayPixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerDisplayPixelRatio";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerIndex";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerMargin";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerOrientation(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerOrientation";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerParentControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerParentControl";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerResourcePixelRatio(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerResourcePixelRatio";
    }
    return nullptr;
}

static void* stub_Sizer_GetSizerSpacing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/GetSizerSpacing";
    }
    return nullptr;
}

static void* stub_Sizer_InsertSizer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/InsertSizer";
    }
    return nullptr;
}

static void* stub_Sizer_InsertSizerControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/InsertSizerControl";
    }
    return nullptr;
}

static void* stub_Sizer_InsertSizerSpacing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/InsertSizerSpacing";
    }
    return nullptr;
}

static void* stub_Sizer_InsertSizerStretch(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/InsertSizerStretch";
    }
    return nullptr;
}

static void* stub_Sizer_RemoveSizer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/RemoveSizer";
    }
    return nullptr;
}

static void* stub_Sizer_RemoveSizerControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/RemoveSizerControl";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerAlignment";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerControlAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerControlAlignment";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerControlStretchFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerControlStretchFactor";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerMargin(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerMargin";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerSpacing(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerSpacing";
    }
    return nullptr;
}

static void* stub_Sizer_SetSizerStretchFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Sizer/SetSizerStretchFactor";
    }
    return nullptr;
}

static void* stub_Slider_CreateSlider(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/CreateSlider";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderPageSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderPageSize";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderRange";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderStepSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderStepSize";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderTickInterval(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderTickInterval";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderTickStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderTickStyle";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderTrackingEnabled";
    }
    return nullptr;
}

static void* stub_Slider_GetSliderValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/GetSliderValue";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderPageSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderPageSize";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderRange";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderRangeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderRangeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderStepSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderStepSize";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderTickInterval(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderTickInterval";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderTickStyle(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderTickStyle";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderTrackingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderTrackingEnabled";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderValue";
    }
    return nullptr;
}

static void* stub_Slider_SetSliderValueUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Slider/SetSliderValueUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_SpinBox_CreateSpinBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/CreateSpinBox";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxAlignment";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxEditable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxEditable";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxMinimumValueText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxMinimumValueText";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxPrefix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxPrefix";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxRange";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxStepSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxStepSize";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxSuffix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxSuffix";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxValue";
    }
    return nullptr;
}

static void* stub_SpinBox_GetSpinBoxWrappingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/GetSpinBoxWrappingEnabled";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxAlignment";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxEditable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxEditable";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxMinimumValueText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxMinimumValueText";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxPrefix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxPrefix";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxRange(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxRange";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxRangeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxRangeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxStepSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxStepSize";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxSuffix(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxSuffix";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxValue";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxValueUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxValueUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_SpinBox_SetSpinBoxWrappingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "SpinBox/SetSpinBoxWrappingEnabled";
    }
    return nullptr;
}

static void* stub_TabBox_CreateTabBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/CreateTabBox";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxCurrentPageIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxCurrentPageIndex";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxLeftControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxLeftControl";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxLength(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxLength";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPageByIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPageByIndex";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPageEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPageEnabled";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPageIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPageIcon";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPageLabel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPageLabel";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPageToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPageToolTip";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxPosition";
    }
    return nullptr;
}

static void* stub_TabBox_GetTabBoxRightControl(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/GetTabBoxRightControl";
    }
    return nullptr;
}

static void* stub_TabBox_InsertTabBoxPage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/InsertTabBoxPage";
    }
    return nullptr;
}

static void* stub_TabBox_RemoveTabBoxPage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/RemoveTabBoxPage";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxControls(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxControls";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxCurrentPageIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxCurrentPageIndex";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPageEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPageEnabled";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPageIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPageIcon";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPageLabel(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPageLabel";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPageSelectedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPageSelectedEventRoutine";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPageToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPageToolTip";
    }
    return nullptr;
}

static void* stub_TabBox_SetTabBoxPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TabBox/SetTabBoxPosition";
    }
    return nullptr;
}

static void* stub_TextBox_CreateTextBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/CreateTextBox";
    }
    return nullptr;
}

static void* stub_TextBox_DeleteTextBoxText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/DeleteTextBoxText";
    }
    return nullptr;
}

static void* stub_TextBox_GetTextBoxCaretPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/GetTextBoxCaretPosition";
    }
    return nullptr;
}

static void* stub_TextBox_GetTextBoxReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/GetTextBoxReadOnly";
    }
    return nullptr;
}

static void* stub_TextBox_GetTextBoxSelectedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/GetTextBoxSelectedText";
    }
    return nullptr;
}

static void* stub_TextBox_GetTextBoxSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/GetTextBoxSelection";
    }
    return nullptr;
}

static void* stub_TextBox_GetTextBoxText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/GetTextBoxText";
    }
    return nullptr;
}

static void* stub_TextBox_InsertTextBoxText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/InsertTextBoxText";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxCaretPosition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxCaretPosition";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxCaretPositionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxCaretPositionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxReadOnly(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxReadOnly";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxSelected(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxSelected";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxSelection";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxSelectionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxSelectionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxText";
    }
    return nullptr;
}

static void* stub_TextBox_SetTextBoxUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TextBox/SetTextBoxUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_Thread_AppendThreadConsoleOutputText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/AppendThreadConsoleOutputText";
    }
    return nullptr;
}

static void* stub_Thread_ClearThreadConsoleOutputText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/ClearThreadConsoleOutputText";
    }
    return nullptr;
}

static void* stub_Thread_CreateThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/CreateThread";
    }
    return nullptr;
}

static void* stub_Thread_GetCurrentThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetCurrentThread";
    }
    return nullptr;
}

static void* stub_Thread_GetThreadConsoleOutputText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetThreadConsoleOutputText";
    }
    return nullptr;
}

static void* stub_Thread_GetThreadPriority(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetThreadPriority";
    }
    return nullptr;
}

static void* stub_Thread_GetThreadStackSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetThreadStackSize";
    }
    return nullptr;
}

static void* stub_Thread_GetThreadStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetThreadStatus";
    }
    return nullptr;
}

static void* stub_Thread_GetThreadStatusEx(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/GetThreadStatusEx";
    }
    return nullptr;
}

static void* stub_Thread_IsThreadActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/IsThreadActive";
    }
    return nullptr;
}

static void* stub_Thread_KillThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/KillThread";
    }
    return nullptr;
}

static void* stub_Thread_PerformanceAnalysisValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/PerformanceAnalysisValue";
    }
    return nullptr;
}

static void* stub_Thread_SetThreadExecRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/SetThreadExecRoutine";
    }
    return nullptr;
}

static void* stub_Thread_SetThreadPriority(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/SetThreadPriority";
    }
    return nullptr;
}

static void* stub_Thread_SetThreadStackSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/SetThreadStackSize";
    }
    return nullptr;
}

static void* stub_Thread_SetThreadStatus(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/SetThreadStatus";
    }
    return nullptr;
}

static void* stub_Thread_SleepThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/SleepThread";
    }
    return nullptr;
}

static void* stub_Thread_StartThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/StartThread";
    }
    return nullptr;
}

static void* stub_Thread_WaitThread(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Thread/WaitThread";
    }
    return nullptr;
}

static void* stub_Timer_CreateTimer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/CreateTimer";
    }
    return nullptr;
}

static void* stub_Timer_GetTimerInterval(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/GetTimerInterval";
    }
    return nullptr;
}

static void* stub_Timer_GetTimerSingleShot(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/GetTimerSingleShot";
    }
    return nullptr;
}

static void* stub_Timer_IsTimerActive(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/IsTimerActive";
    }
    return nullptr;
}

static void* stub_Timer_SetTimerInterval(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/SetTimerInterval";
    }
    return nullptr;
}

static void* stub_Timer_SetTimerNotifyEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/SetTimerNotifyEventRoutine";
    }
    return nullptr;
}

static void* stub_Timer_SetTimerSingleShot(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/SetTimerSingleShot";
    }
    return nullptr;
}

static void* stub_Timer_StartTimer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/StartTimer";
    }
    return nullptr;
}

static void* stub_Timer_StopTimer(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "Timer/StopTimer";
    }
    return nullptr;
}

static void* stub_TreeBox_AdjustTreeBoxColumnWidthToContents(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/AdjustTreeBoxColumnWidthToContents";
    }
    return nullptr;
}

static void* stub_TreeBox_BeginTreeBoxNodeEdition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/BeginTreeBoxNodeEdition";
    }
    return nullptr;
}

static void* stub_TreeBox_ClearTreeBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/ClearTreeBox";
    }
    return nullptr;
}

static void* stub_TreeBox_CreateTreeBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/CreateTreeBox";
    }
    return nullptr;
}

static void* stub_TreeBox_CreateTreeBoxNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/CreateTreeBoxNode";
    }
    return nullptr;
}

static void* stub_TreeBox_CreateTreeBoxViewport(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/CreateTreeBoxViewport";
    }
    return nullptr;
}

static void* stub_TreeBox_EditTreeBoxNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/EditTreeBoxNode";
    }
    return nullptr;
}

static void* stub_TreeBox_EndTreeBoxNodeEdition(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/EndTreeBoxNodeEdition";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxAlternateRowColorEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxAlternateRowColorEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxChild(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxChild";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxChildCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxChildCount";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxChildIndex(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxChildIndex";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxColumnCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxColumnCount";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxColumnVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxColumnVisible";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxColumnWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxColumnWidth";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxCurrentNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxCurrentNode";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxHeaderAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxHeaderAlignment";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxHeaderIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxHeaderIcon";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxHeaderSortingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxHeaderSortingEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxHeaderText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxHeaderText";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxHeaderVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxHeaderVisible";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxIconSize";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxIndentSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxIndentSize";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxMultipleNodeSelectionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxMultipleNodeSelectionEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeByPos(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeByPos";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeCheckable";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeChecked";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeChild(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeChild";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeChildCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeChildCount";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColAlignment";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColBackgroundColor";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColFont";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColIcon";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColText";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColTextColor";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeColToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeColToolTip";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeDraggingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeDraggingEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeEditable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeEditable";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeExpanded(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeExpanded";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeExpansionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeExpansionEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeFirstColumnSpanned(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeFirstColumnSpanned";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeParent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeParent";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeParentBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeParentBox";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeRect(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeRect";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeSelectable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeSelectable";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxNodeSelected(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxNodeSelected";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxRootDecorationEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxRootDecorationEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxSelectedNodes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxSelectedNodes";
    }
    return nullptr;
}

static void* stub_TreeBox_GetTreeBoxUniformRowHeightEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/GetTreeBoxUniformRowHeightEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_InsertTreeBoxNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/InsertTreeBoxNode";
    }
    return nullptr;
}

static void* stub_TreeBox_InsertTreeBoxNodeChild(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/InsertTreeBoxNodeChild";
    }
    return nullptr;
}

static void* stub_TreeBox_RemoveTreeBoxNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/RemoveTreeBoxNode";
    }
    return nullptr;
}

static void* stub_TreeBox_RemoveTreeBoxNodeChild(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/RemoveTreeBoxNodeChild";
    }
    return nullptr;
}

static void* stub_TreeBox_SelectAllTreeBoxNodes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SelectAllTreeBoxNodes";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxAlternateRowColorEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxAlternateRowColorEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxColumnCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxColumnCount";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxColumnVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxColumnVisible";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxColumnWidth(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxColumnWidth";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxCurrentNode(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxCurrentNode";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxCurrentNodeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxCurrentNodeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxHeaderAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxHeaderAlignment";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxHeaderIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxHeaderIcon";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxHeaderSortingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxHeaderSortingEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxHeaderText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxHeaderText";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxHeaderVisible(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxHeaderVisible";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxIconSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxIconSize";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxIndentSize(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxIndentSize";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxMultipleNodeSelectionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxMultipleNodeSelectionEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeActivatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeActivatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeCheckable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeCheckable";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeChecked(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeChecked";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeClickedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeClickedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColAlignment(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColAlignment";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColBackgroundColor";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColFont(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColFont";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColIcon(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColIcon";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColText";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColTextColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColTextColor";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeColToolTip(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeColToolTip";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeCollapsedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeCollapsedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeDoubleClickedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeDoubleClickedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeDraggingEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeDraggingEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeEditable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeEditable";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeEnteredEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeEnteredEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeExpanded(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeExpanded";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeExpandedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeExpandedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeExpansionEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeExpansionEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeFirstColumnSpanned(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeFirstColumnSpanned";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeIntoView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeIntoView";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeSelectable(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeSelectable";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeSelected(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeSelected";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeSelectionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeSelectionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxNodeUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxNodeUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxRootDecorationEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxRootDecorationEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SetTreeBoxUniformRowHeightEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SetTreeBoxUniformRowHeightEnabled";
    }
    return nullptr;
}

static void* stub_TreeBox_SortTreeBox(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "TreeBox/SortTreeBox";
    }
    return nullptr;
}

static void* stub_UI_AttachToUIObject(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/AttachToUIObject";
    }
    return nullptr;
}

static void* stub_UI_DetachFromUIObject(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/DetachFromUIObject";
    }
    return nullptr;
}

static void* stub_UI_GetUIObjectId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/GetUIObjectId";
    }
    return nullptr;
}

static void* stub_UI_GetUIObjectModule(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/GetUIObjectModule";
    }
    return nullptr;
}

static void* stub_UI_GetUIObjectRefCount(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/GetUIObjectRefCount";
    }
    return nullptr;
}

static void* stub_UI_GetUIObjectType(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/GetUIObjectType";
    }
    return nullptr;
}

static void* stub_UI_SetHandleDestroyedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/SetHandleDestroyedEventRoutine";
    }
    return nullptr;
}

static void* stub_UI_SetUIObjectId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "UI/SetUIObjectId";
    }
    return nullptr;
}

static void* stub_View_AddViewToDynamicTargets(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/AddViewToDynamicTargets";
    }
    return nullptr;
}

static void* stub_View_ComputeViewProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/ComputeViewProperty";
    }
    return nullptr;
}

static void* stub_View_DeleteViewProperty(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/DeleteViewProperty";
    }
    return nullptr;
}

static void* stub_View_DestroyViewScreenTransferFunctions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/DestroyViewScreenTransferFunctions";
    }
    return nullptr;
}

static void* stub_View_EnumerateViewProperties(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/EnumerateViewProperties";
    }
    return nullptr;
}

static void* stub_View_EnumerateViews(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/EnumerateViews";
    }
    return nullptr;
}

static void* stub_View_GetViewById(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewById";
    }
    return nullptr;
}

static void* stub_View_GetViewDimensions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewDimensions";
    }
    return nullptr;
}

static void* stub_View_GetViewFullId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewFullId";
    }
    return nullptr;
}

static void* stub_View_GetViewId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewId";
    }
    return nullptr;
}

static void* stub_View_GetViewImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewImage";
    }
    return nullptr;
}

static void* stub_View_GetViewLocks(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewLocks";
    }
    return nullptr;
}

static void* stub_View_GetViewParentWindow(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewParentWindow";
    }
    return nullptr;
}

static void* stub_View_GetViewPropertyAttributes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewPropertyAttributes";
    }
    return nullptr;
}

static void* stub_View_GetViewPropertyExists(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewPropertyExists";
    }
    return nullptr;
}

static void* stub_View_GetViewPropertyValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewPropertyValue";
    }
    return nullptr;
}

static void* stub_View_GetViewScreenTransferFunctions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewScreenTransferFunctions";
    }
    return nullptr;
}

static void* stub_View_GetViewScreenTransferFunctionsEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/GetViewScreenTransferFunctionsEnabled";
    }
    return nullptr;
}

static void* stub_View_IsPreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsPreview";
    }
    return nullptr;
}

static void* stub_View_IsReservedViewPropertyId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsReservedViewPropertyId";
    }
    return nullptr;
}

static void* stub_View_IsStoredPreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsStoredPreview";
    }
    return nullptr;
}

static void* stub_View_IsViewColorImage(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsViewColorImage";
    }
    return nullptr;
}

static void* stub_View_IsViewDynamicTarget(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsViewDynamicTarget";
    }
    return nullptr;
}

static void* stub_View_IsVolatilePreview(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/IsVolatilePreview";
    }
    return nullptr;
}

static void* stub_View_LockView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/LockView";
    }
    return nullptr;
}

static void* stub_View_RemoveViewFromDynamicTargets(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/RemoveViewFromDynamicTargets";
    }
    return nullptr;
}

static void* stub_View_SetViewId(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/SetViewId";
    }
    return nullptr;
}

static void* stub_View_SetViewPropertyAttributes(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/SetViewPropertyAttributes";
    }
    return nullptr;
}

static void* stub_View_SetViewPropertyValue(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/SetViewPropertyValue";
    }
    return nullptr;
}

static void* stub_View_SetViewScreenTransferFunctions(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/SetViewScreenTransferFunctions";
    }
    return nullptr;
}

static void* stub_View_SetViewScreenTransferFunctionsEnabled(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/SetViewScreenTransferFunctionsEnabled";
    }
    return nullptr;
}

static void* stub_View_UnlockView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "View/UnlockView";
    }
    return nullptr;
}

static void* stub_ViewList_CreateViewList(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/CreateViewList";
    }
    return nullptr;
}

static void* stub_ViewList_FindViewListView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/FindViewListView";
    }
    return nullptr;
}

static void* stub_ViewList_GetViewListContents(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/GetViewListContents";
    }
    return nullptr;
}

static void* stub_ViewList_GetViewListCurrentView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/GetViewListCurrentView";
    }
    return nullptr;
}

static void* stub_ViewList_GetViewListExcludedView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/GetViewListExcludedView";
    }
    return nullptr;
}

static void* stub_ViewList_RegenerateViewList(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/RegenerateViewList";
    }
    return nullptr;
}

static void* stub_ViewList_RemoveViewListView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/RemoveViewListView";
    }
    return nullptr;
}

static void* stub_ViewList_SetViewListCurrentView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/SetViewListCurrentView";
    }
    return nullptr;
}

static void* stub_ViewList_SetViewListCurrentViewUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/SetViewListCurrentViewUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_ViewList_SetViewListExcludedView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/SetViewListExcludedView";
    }
    return nullptr;
}

static void* stub_ViewList_SetViewListViewSelectedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "ViewList/SetViewListViewSelectedEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_CreateWebView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/CreateWebView";
    }
    return nullptr;
}

static void* stub_WebView_EvaluateWebViewScript(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/EvaluateWebViewScript";
    }
    return nullptr;
}

static void* stub_WebView_GetWebViewBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/GetWebViewBackgroundColor";
    }
    return nullptr;
}

static void* stub_WebView_GetWebViewHasSelection(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/GetWebViewHasSelection";
    }
    return nullptr;
}

static void* stub_WebView_GetWebViewSelectedText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/GetWebViewSelectedText";
    }
    return nullptr;
}

static void* stub_WebView_GetWebViewZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/GetWebViewZoomFactor";
    }
    return nullptr;
}

static void* stub_WebView_LoadWebViewContent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/LoadWebViewContent";
    }
    return nullptr;
}

static void* stub_WebView_ReloadWebView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/ReloadWebView";
    }
    return nullptr;
}

static void* stub_WebView_RequestWebViewHTML(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/RequestWebViewHTML";
    }
    return nullptr;
}

static void* stub_WebView_RequestWebViewPlainText(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/RequestWebViewPlainText";
    }
    return nullptr;
}

static void* stub_WebView_SaveWebViewAsPDF(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SaveWebViewAsPDF";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewBackgroundColor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewBackgroundColor";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewContent(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewContent";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewHTMLAvailableEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewHTMLAvailableEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewLoadFinishedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewLoadFinishedEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewLoadProgressEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewLoadProgressEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewLoadStartedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewLoadStartedEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewPlainTextAvailableEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewPlainTextAvailableEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewScriptResultAvailableEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewScriptResultAvailableEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewSelectionUpdatedEventRoutine(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewSelectionUpdatedEventRoutine";
    }
    return nullptr;
}

static void* stub_WebView_SetWebViewZoomFactor(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/SetWebViewZoomFactor";
    }
    return nullptr;
}

static void* stub_WebView_StopWebView(...) {
    static int callCount = 0;
    if (callCount++ < 5) {
        qWarning() << "UNIMPLEMENTED PCL API CALL:" << "WebView/StopWebView";
    }
    return nullptr;
}

void initPCLStubs() {
    s_stubMap["Action/CreateAction"] = reinterpret_cast<void*>(stub_Action_CreateAction);
    s_stubMap["Action/CreateActionSVG"] = reinterpret_cast<void*>(stub_Action_CreateActionSVG);
    s_stubMap["Action/CreateActionSVGFile"] = reinterpret_cast<void*>(stub_Action_CreateActionSVGFile);
    s_stubMap["Action/GetActionAccelerator"] = reinterpret_cast<void*>(stub_Action_GetActionAccelerator);
    s_stubMap["Action/GetActionIcon"] = reinterpret_cast<void*>(stub_Action_GetActionIcon);
    s_stubMap["Action/GetActionMenuItem"] = reinterpret_cast<void*>(stub_Action_GetActionMenuItem);
    s_stubMap["Action/GetActionMenuText"] = reinterpret_cast<void*>(stub_Action_GetActionMenuText);
    s_stubMap["Action/GetActionToolBar"] = reinterpret_cast<void*>(stub_Action_GetActionToolBar);
    s_stubMap["Action/GetActionToolTip"] = reinterpret_cast<void*>(stub_Action_GetActionToolTip);
    s_stubMap["Action/SetActionAccelerator"] = reinterpret_cast<void*>(stub_Action_SetActionAccelerator);
    s_stubMap["Action/SetActionExecutionRoutine"] = reinterpret_cast<void*>(stub_Action_SetActionExecutionRoutine);
    s_stubMap["Action/SetActionIcon"] = reinterpret_cast<void*>(stub_Action_SetActionIcon);
    s_stubMap["Action/SetActionIconSVG"] = reinterpret_cast<void*>(stub_Action_SetActionIconSVG);
    s_stubMap["Action/SetActionIconSVGFile"] = reinterpret_cast<void*>(stub_Action_SetActionIconSVGFile);
    s_stubMap["Action/SetActionMenuText"] = reinterpret_cast<void*>(stub_Action_SetActionMenuText);
    s_stubMap["Action/SetActionStateQueryRoutine"] = reinterpret_cast<void*>(stub_Action_SetActionStateQueryRoutine);
    s_stubMap["Action/SetActionToolTip"] = reinterpret_cast<void*>(stub_Action_SetActionToolTip);
    s_stubMap["Bitmap/AndBitmap"] = reinterpret_cast<void*>(stub_Bitmap_AndBitmap);
    s_stubMap["Bitmap/AndBitmaps"] = reinterpret_cast<void*>(stub_Bitmap_AndBitmaps);
    s_stubMap["Bitmap/CloneBitmap"] = reinterpret_cast<void*>(stub_Bitmap_CloneBitmap);
    s_stubMap["Bitmap/CloneBitmapRect"] = reinterpret_cast<void*>(stub_Bitmap_CloneBitmapRect);
    s_stubMap["Bitmap/CopyBitmap"] = reinterpret_cast<void*>(stub_Bitmap_CopyBitmap);
    s_stubMap["Bitmap/CreateBitmap"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmap);
    s_stubMap["Bitmap/CreateBitmapFromData"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapFromData);
    s_stubMap["Bitmap/CreateBitmapFromFile"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapFromFile);
    s_stubMap["Bitmap/CreateBitmapFromFile8"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapFromFile8);
    s_stubMap["Bitmap/CreateBitmapFromSVG"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapFromSVG);
    s_stubMap["Bitmap/CreateBitmapFromSVGFile"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapFromSVGFile);
    s_stubMap["Bitmap/CreateBitmapXPM"] = reinterpret_cast<void*>(stub_Bitmap_CreateBitmapXPM);
    s_stubMap["Bitmap/CreateEmptyBitmap"] = reinterpret_cast<void*>(stub_Bitmap_CreateEmptyBitmap);
    s_stubMap["Bitmap/FillBitmap"] = reinterpret_cast<void*>(stub_Bitmap_FillBitmap);
    s_stubMap["Bitmap/GetBitmapDevicePixelRatio"] = reinterpret_cast<void*>(stub_Bitmap_GetBitmapDevicePixelRatio);
    s_stubMap["Bitmap/GetBitmapDimensions"] = reinterpret_cast<void*>(stub_Bitmap_GetBitmapDimensions);
    s_stubMap["Bitmap/GetBitmapFormat"] = reinterpret_cast<void*>(stub_Bitmap_GetBitmapFormat);
    s_stubMap["Bitmap/GetBitmapPixel"] = reinterpret_cast<void*>(stub_Bitmap_GetBitmapPixel);
    s_stubMap["Bitmap/GetBitmapScanLine"] = reinterpret_cast<void*>(stub_Bitmap_GetBitmapScanLine);
    s_stubMap["Bitmap/IsEmptyBitmap"] = reinterpret_cast<void*>(stub_Bitmap_IsEmptyBitmap);
    s_stubMap["Bitmap/LoadBitmap"] = reinterpret_cast<void*>(stub_Bitmap_LoadBitmap);
    s_stubMap["Bitmap/LoadBitmapData"] = reinterpret_cast<void*>(stub_Bitmap_LoadBitmapData);
    s_stubMap["Bitmap/MirroredBitmap"] = reinterpret_cast<void*>(stub_Bitmap_MirroredBitmap);
    s_stubMap["Bitmap/OrBitmap"] = reinterpret_cast<void*>(stub_Bitmap_OrBitmap);
    s_stubMap["Bitmap/OrBitmaps"] = reinterpret_cast<void*>(stub_Bitmap_OrBitmaps);
    s_stubMap["Bitmap/ReplaceBitmapColor"] = reinterpret_cast<void*>(stub_Bitmap_ReplaceBitmapColor);
    s_stubMap["Bitmap/RotatedBitmap"] = reinterpret_cast<void*>(stub_Bitmap_RotatedBitmap);
    s_stubMap["Bitmap/SaveBitmap"] = reinterpret_cast<void*>(stub_Bitmap_SaveBitmap);
    s_stubMap["Bitmap/ScaledBitmap"] = reinterpret_cast<void*>(stub_Bitmap_ScaledBitmap);
    s_stubMap["Bitmap/SetBitmapAlpha"] = reinterpret_cast<void*>(stub_Bitmap_SetBitmapAlpha);
    s_stubMap["Bitmap/SetBitmapDevicePixelRatio"] = reinterpret_cast<void*>(stub_Bitmap_SetBitmapDevicePixelRatio);
    s_stubMap["Bitmap/SetBitmapFormat"] = reinterpret_cast<void*>(stub_Bitmap_SetBitmapFormat);
    s_stubMap["Bitmap/SetBitmapPixel"] = reinterpret_cast<void*>(stub_Bitmap_SetBitmapPixel);
    s_stubMap["Bitmap/XorBitmap"] = reinterpret_cast<void*>(stub_Bitmap_XorBitmap);
    s_stubMap["Bitmap/XorBitmapRect"] = reinterpret_cast<void*>(stub_Bitmap_XorBitmapRect);
    s_stubMap["Bitmap/XorBitmaps"] = reinterpret_cast<void*>(stub_Bitmap_XorBitmaps);
    s_stubMap["BitmapBox/CreateBitmapBox"] = reinterpret_cast<void*>(stub_BitmapBox_CreateBitmapBox);
    s_stubMap["BitmapBox/GetBitmapBoxAutoFitEnabled"] = reinterpret_cast<void*>(stub_BitmapBox_GetBitmapBoxAutoFitEnabled);
    s_stubMap["BitmapBox/GetBitmapBoxBitmap"] = reinterpret_cast<void*>(stub_BitmapBox_GetBitmapBoxBitmap);
    s_stubMap["BitmapBox/GetBitmapBoxMargin"] = reinterpret_cast<void*>(stub_BitmapBox_GetBitmapBoxMargin);
    s_stubMap["BitmapBox/SetBitmapBoxAutoFitEnabled"] = reinterpret_cast<void*>(stub_BitmapBox_SetBitmapBoxAutoFitEnabled);
    s_stubMap["BitmapBox/SetBitmapBoxBitmap"] = reinterpret_cast<void*>(stub_BitmapBox_SetBitmapBoxBitmap);
    s_stubMap["BitmapBox/SetBitmapBoxMargin"] = reinterpret_cast<void*>(stub_BitmapBox_SetBitmapBoxMargin);
    s_stubMap["Brush/CloneBrush"] = reinterpret_cast<void*>(stub_Brush_CloneBrush);
    s_stubMap["Brush/CreateBitmapBrush"] = reinterpret_cast<void*>(stub_Brush_CreateBitmapBrush);
    s_stubMap["Brush/CreateBrush"] = reinterpret_cast<void*>(stub_Brush_CreateBrush);
    s_stubMap["Brush/CreateConicalGradientBrush"] = reinterpret_cast<void*>(stub_Brush_CreateConicalGradientBrush);
    s_stubMap["Brush/CreateLinearGradientBrush"] = reinterpret_cast<void*>(stub_Brush_CreateLinearGradientBrush);
    s_stubMap["Brush/CreateRadialGradientBrush"] = reinterpret_cast<void*>(stub_Brush_CreateRadialGradientBrush);
    s_stubMap["Brush/GetBrushBitmap"] = reinterpret_cast<void*>(stub_Brush_GetBrushBitmap);
    s_stubMap["Brush/GetBrushColor"] = reinterpret_cast<void*>(stub_Brush_GetBrushColor);
    s_stubMap["Brush/GetBrushConicalGradientParameters"] = reinterpret_cast<void*>(stub_Brush_GetBrushConicalGradientParameters);
    s_stubMap["Brush/GetBrushGradientSpread"] = reinterpret_cast<void*>(stub_Brush_GetBrushGradientSpread);
    s_stubMap["Brush/GetBrushGradientStops"] = reinterpret_cast<void*>(stub_Brush_GetBrushGradientStops);
    s_stubMap["Brush/GetBrushGradientType"] = reinterpret_cast<void*>(stub_Brush_GetBrushGradientType);
    s_stubMap["Brush/GetBrushLinearGradientParameters"] = reinterpret_cast<void*>(stub_Brush_GetBrushLinearGradientParameters);
    s_stubMap["Brush/GetBrushRadialGradientParameters"] = reinterpret_cast<void*>(stub_Brush_GetBrushRadialGradientParameters);
    s_stubMap["Brush/GetBrushStyle"] = reinterpret_cast<void*>(stub_Brush_GetBrushStyle);
    s_stubMap["Brush/SetBrushBitmap"] = reinterpret_cast<void*>(stub_Brush_SetBrushBitmap);
    s_stubMap["Brush/SetBrushColor"] = reinterpret_cast<void*>(stub_Brush_SetBrushColor);
    s_stubMap["Brush/SetBrushStyle"] = reinterpret_cast<void*>(stub_Brush_SetBrushStyle);
    s_stubMap["Button/CreateCheckBox"] = reinterpret_cast<void*>(stub_Button_CreateCheckBox);
    s_stubMap["Button/CreatePushButton"] = reinterpret_cast<void*>(stub_Button_CreatePushButton);
    s_stubMap["Button/CreateRadioButton"] = reinterpret_cast<void*>(stub_Button_CreateRadioButton);
    s_stubMap["Button/CreateToolButton"] = reinterpret_cast<void*>(stub_Button_CreateToolButton);
    s_stubMap["Button/GetButtonChecked"] = reinterpret_cast<void*>(stub_Button_GetButtonChecked);
    s_stubMap["Button/GetButtonDefaultEnabled"] = reinterpret_cast<void*>(stub_Button_GetButtonDefaultEnabled);
    s_stubMap["Button/GetButtonIcon"] = reinterpret_cast<void*>(stub_Button_GetButtonIcon);
    s_stubMap["Button/GetButtonIconSize"] = reinterpret_cast<void*>(stub_Button_GetButtonIconSize);
    s_stubMap["Button/GetButtonPushed"] = reinterpret_cast<void*>(stub_Button_GetButtonPushed);
    s_stubMap["Button/GetButtonText"] = reinterpret_cast<void*>(stub_Button_GetButtonText);
    s_stubMap["Button/GetButtonTristateEnabled"] = reinterpret_cast<void*>(stub_Button_GetButtonTristateEnabled);
    s_stubMap["Button/GetToolButtonCheckable"] = reinterpret_cast<void*>(stub_Button_GetToolButtonCheckable);
    s_stubMap["Button/SetButtonCheckEventRoutine"] = reinterpret_cast<void*>(stub_Button_SetButtonCheckEventRoutine);
    s_stubMap["Button/SetButtonChecked"] = reinterpret_cast<void*>(stub_Button_SetButtonChecked);
    s_stubMap["Button/SetButtonClickEventRoutine"] = reinterpret_cast<void*>(stub_Button_SetButtonClickEventRoutine);
    s_stubMap["Button/SetButtonDefaultEnabled"] = reinterpret_cast<void*>(stub_Button_SetButtonDefaultEnabled);
    s_stubMap["Button/SetButtonIcon"] = reinterpret_cast<void*>(stub_Button_SetButtonIcon);
    s_stubMap["Button/SetButtonIconSize"] = reinterpret_cast<void*>(stub_Button_SetButtonIconSize);
    s_stubMap["Button/SetButtonPressEventRoutine"] = reinterpret_cast<void*>(stub_Button_SetButtonPressEventRoutine);
    s_stubMap["Button/SetButtonPushed"] = reinterpret_cast<void*>(stub_Button_SetButtonPushed);
    s_stubMap["Button/SetButtonReleaseEventRoutine"] = reinterpret_cast<void*>(stub_Button_SetButtonReleaseEventRoutine);
    s_stubMap["Button/SetButtonText"] = reinterpret_cast<void*>(stub_Button_SetButtonText);
    s_stubMap["Button/SetButtonTristateEnabled"] = reinterpret_cast<void*>(stub_Button_SetButtonTristateEnabled);
    s_stubMap["Button/SetToolButtonCheckable"] = reinterpret_cast<void*>(stub_Button_SetToolButtonCheckable);
    s_stubMap["CodeEditor/ClearEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_ClearEditorText);
    s_stubMap["CodeEditor/CreateCodeEditor"] = reinterpret_cast<void*>(stub_CodeEditor_CreateCodeEditor);
    s_stubMap["CodeEditor/CreateEditorLineNumbersControl"] = reinterpret_cast<void*>(stub_CodeEditor_CreateEditorLineNumbersControl);
    s_stubMap["CodeEditor/EditorClearMatches"] = reinterpret_cast<void*>(stub_CodeEditor_EditorClearMatches);
    s_stubMap["CodeEditor/EditorCopy"] = reinterpret_cast<void*>(stub_CodeEditor_EditorCopy);
    s_stubMap["CodeEditor/EditorCut"] = reinterpret_cast<void*>(stub_CodeEditor_EditorCut);
    s_stubMap["CodeEditor/EditorDelete"] = reinterpret_cast<void*>(stub_CodeEditor_EditorDelete);
    s_stubMap["CodeEditor/EditorFind"] = reinterpret_cast<void*>(stub_CodeEditor_EditorFind);
    s_stubMap["CodeEditor/EditorGotoMatchedParenthesis"] = reinterpret_cast<void*>(stub_CodeEditor_EditorGotoMatchedParenthesis);
    s_stubMap["CodeEditor/EditorHighlightAllMatches"] = reinterpret_cast<void*>(stub_CodeEditor_EditorHighlightAllMatches);
    s_stubMap["CodeEditor/EditorPaste"] = reinterpret_cast<void*>(stub_CodeEditor_EditorPaste);
    s_stubMap["CodeEditor/EditorRedo"] = reinterpret_cast<void*>(stub_CodeEditor_EditorRedo);
    s_stubMap["CodeEditor/EditorReplace"] = reinterpret_cast<void*>(stub_CodeEditor_EditorReplace);
    s_stubMap["CodeEditor/EditorReplaceAll"] = reinterpret_cast<void*>(stub_CodeEditor_EditorReplaceAll);
    s_stubMap["CodeEditor/EditorSelectAll"] = reinterpret_cast<void*>(stub_CodeEditor_EditorSelectAll);
    s_stubMap["CodeEditor/EditorUndo"] = reinterpret_cast<void*>(stub_CodeEditor_EditorUndo);
    s_stubMap["CodeEditor/EditorUnselect"] = reinterpret_cast<void*>(stub_CodeEditor_EditorUnselect);
    s_stubMap["CodeEditor/GetEditorBlockSelectionMode"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorBlockSelectionMode);
    s_stubMap["CodeEditor/GetEditorCharacterCount"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorCharacterCount);
    s_stubMap["CodeEditor/GetEditorCursorCoordinates"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorCursorCoordinates);
    s_stubMap["CodeEditor/GetEditorDynamicWordWrapMode"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorDynamicWordWrapMode);
    s_stubMap["CodeEditor/GetEditorEncodedText"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorEncodedText);
    s_stubMap["CodeEditor/GetEditorFilePath"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorFilePath);
    s_stubMap["CodeEditor/GetEditorHasSelection"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorHasSelection);
    s_stubMap["CodeEditor/GetEditorInsertMode"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorInsertMode);
    s_stubMap["CodeEditor/GetEditorLineCount"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorLineCount);
    s_stubMap["CodeEditor/GetEditorReadOnly"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorReadOnly);
    s_stubMap["CodeEditor/GetEditorRedoSteps"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorRedoSteps);
    s_stubMap["CodeEditor/GetEditorSelectedText"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorSelectedText);
    s_stubMap["CodeEditor/GetEditorSelectionCoordinates"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorSelectionCoordinates);
    s_stubMap["CodeEditor/GetEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorText);
    s_stubMap["CodeEditor/GetEditorUndoSteps"] = reinterpret_cast<void*>(stub_CodeEditor_GetEditorUndoSteps);
    s_stubMap["CodeEditor/InsertEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_InsertEditorText);
    s_stubMap["CodeEditor/LoadEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_LoadEditorText);
    s_stubMap["CodeEditor/SaveEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_SaveEditorText);
    s_stubMap["CodeEditor/SetEditorBlockSelectionMode"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorBlockSelectionMode);
    s_stubMap["CodeEditor/SetEditorCursorCoordinates"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorCursorCoordinates);
    s_stubMap["CodeEditor/SetEditorCursorPositionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorCursorPositionUpdatedEventRoutine);
    s_stubMap["CodeEditor/SetEditorDynamicWordWrapMode"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorDynamicWordWrapMode);
    s_stubMap["CodeEditor/SetEditorDynamicWordWrapModeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorDynamicWordWrapModeUpdatedEventRoutine);
    s_stubMap["CodeEditor/SetEditorEncodedText"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorEncodedText);
    s_stubMap["CodeEditor/SetEditorFilePath"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorFilePath);
    s_stubMap["CodeEditor/SetEditorInsertMode"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorInsertMode);
    s_stubMap["CodeEditor/SetEditorOverwriteModeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorOverwriteModeUpdatedEventRoutine);
    s_stubMap["CodeEditor/SetEditorReadOnly"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorReadOnly);
    s_stubMap["CodeEditor/SetEditorSelectionCoordinates"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorSelectionCoordinates);
    s_stubMap["CodeEditor/SetEditorSelectionModeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorSelectionModeUpdatedEventRoutine);
    s_stubMap["CodeEditor/SetEditorSelectionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorSelectionUpdatedEventRoutine);
    s_stubMap["CodeEditor/SetEditorText"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorText);
    s_stubMap["CodeEditor/SetEditorTextUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_CodeEditor_SetEditorTextUpdatedEventRoutine);
    s_stubMap["ComboBox/ClearComboBox"] = reinterpret_cast<void*>(stub_ComboBox_ClearComboBox);
    s_stubMap["ComboBox/CreateComboBox"] = reinterpret_cast<void*>(stub_ComboBox_CreateComboBox);
    s_stubMap["ComboBox/FindComboBoxItem"] = reinterpret_cast<void*>(stub_ComboBox_FindComboBoxItem);
    s_stubMap["ComboBox/GetComboBoxAutoCompletionEnabled"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxAutoCompletionEnabled);
    s_stubMap["ComboBox/GetComboBoxCurrentItem"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxCurrentItem);
    s_stubMap["ComboBox/GetComboBoxEditEnabled"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxEditEnabled);
    s_stubMap["ComboBox/GetComboBoxEditText"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxEditText);
    s_stubMap["ComboBox/GetComboBoxIconSize"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxIconSize);
    s_stubMap["ComboBox/GetComboBoxItemIcon"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxItemIcon);
    s_stubMap["ComboBox/GetComboBoxItemText"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxItemText);
    s_stubMap["ComboBox/GetComboBoxLength"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxLength);
    s_stubMap["ComboBox/GetComboBoxMaxVisibleItemCount"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxMaxVisibleItemCount);
    s_stubMap["ComboBox/GetComboBoxMinItemCharWidth"] = reinterpret_cast<void*>(stub_ComboBox_GetComboBoxMinItemCharWidth);
    s_stubMap["ComboBox/InsertComboBoxItem"] = reinterpret_cast<void*>(stub_ComboBox_InsertComboBoxItem);
    s_stubMap["ComboBox/RemoveComboBoxItem"] = reinterpret_cast<void*>(stub_ComboBox_RemoveComboBoxItem);
    s_stubMap["ComboBox/SetComboBoxAutoCompletionEnabled"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxAutoCompletionEnabled);
    s_stubMap["ComboBox/SetComboBoxCurrentItem"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxCurrentItem);
    s_stubMap["ComboBox/SetComboBoxEditEnabled"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxEditEnabled);
    s_stubMap["ComboBox/SetComboBoxEditText"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxEditText);
    s_stubMap["ComboBox/SetComboBoxEditTextUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxEditTextUpdatedEventRoutine);
    s_stubMap["ComboBox/SetComboBoxIconSize"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxIconSize);
    s_stubMap["ComboBox/SetComboBoxItemHighlightedEventRoutine"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxItemHighlightedEventRoutine);
    s_stubMap["ComboBox/SetComboBoxItemIcon"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxItemIcon);
    s_stubMap["ComboBox/SetComboBoxItemSelectedEventRoutine"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxItemSelectedEventRoutine);
    s_stubMap["ComboBox/SetComboBoxItemText"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxItemText);
    s_stubMap["ComboBox/SetComboBoxListVisible"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxListVisible);
    s_stubMap["ComboBox/SetComboBoxMaxVisibleItemCount"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxMaxVisibleItemCount);
    s_stubMap["ComboBox/SetComboBoxMinItemCharWidth"] = reinterpret_cast<void*>(stub_ComboBox_SetComboBoxMinItemCharWidth);
    s_stubMap["Control/ActivateWindow"] = reinterpret_cast<void*>(stub_Control_ActivateWindow);
    s_stubMap["Control/AdjustControlToContents"] = reinterpret_cast<void*>(stub_Control_AdjustControlToContents);
    s_stubMap["Control/BringControlToFront"] = reinterpret_cast<void*>(stub_Control_BringControlToFront);
    s_stubMap["Control/ControlToLocal"] = reinterpret_cast<void*>(stub_Control_ControlToLocal);
    s_stubMap["Control/CreateControl"] = reinterpret_cast<void*>(stub_Control_CreateControl);
    s_stubMap["Control/EnsureControlLayoutUpdated"] = reinterpret_cast<void*>(stub_Control_EnsureControlLayoutUpdated);
    s_stubMap["Control/GetChildByPos"] = reinterpret_cast<void*>(stub_Control_GetChildByPos);
    s_stubMap["Control/GetChildControlToFocus"] = reinterpret_cast<void*>(stub_Control_GetChildControlToFocus);
    s_stubMap["Control/GetChildrenRect"] = reinterpret_cast<void*>(stub_Control_GetChildrenRect);
    s_stubMap["Control/GetClientRect"] = reinterpret_cast<void*>(stub_Control_GetClientRect);
    s_stubMap["Control/GetControlAlternateCanvasColor"] = reinterpret_cast<void*>(stub_Control_GetControlAlternateCanvasColor);
    s_stubMap["Control/GetControlAncestry"] = reinterpret_cast<void*>(stub_Control_GetControlAncestry);
    s_stubMap["Control/GetControlBackgroundColor"] = reinterpret_cast<void*>(stub_Control_GetControlBackgroundColor);
    s_stubMap["Control/GetControlButtonColor"] = reinterpret_cast<void*>(stub_Control_GetControlButtonColor);
    s_stubMap["Control/GetControlButtonTextColor"] = reinterpret_cast<void*>(stub_Control_GetControlButtonTextColor);
    s_stubMap["Control/GetControlCanvasColor"] = reinterpret_cast<void*>(stub_Control_GetControlCanvasColor);
    s_stubMap["Control/GetControlCursor"] = reinterpret_cast<void*>(stub_Control_GetControlCursor);
    s_stubMap["Control/GetControlDevicePixelRatio"] = reinterpret_cast<void*>(stub_Control_GetControlDevicePixelRatio);
    s_stubMap["Control/GetControlDisplayPixelRatio"] = reinterpret_cast<void*>(stub_Control_GetControlDisplayPixelRatio);
    s_stubMap["Control/GetControlEnabled"] = reinterpret_cast<void*>(stub_Control_GetControlEnabled);
    s_stubMap["Control/GetControlExpansionEnabled"] = reinterpret_cast<void*>(stub_Control_GetControlExpansionEnabled);
    s_stubMap["Control/GetControlFocus"] = reinterpret_cast<void*>(stub_Control_GetControlFocus);
    s_stubMap["Control/GetControlFocusStyle"] = reinterpret_cast<void*>(stub_Control_GetControlFocusStyle);
    s_stubMap["Control/GetControlFont"] = reinterpret_cast<void*>(stub_Control_GetControlFont);
    s_stubMap["Control/GetControlForegroundColor"] = reinterpret_cast<void*>(stub_Control_GetControlForegroundColor);
    s_stubMap["Control/GetControlHighlightColor"] = reinterpret_cast<void*>(stub_Control_GetControlHighlightColor);
    s_stubMap["Control/GetControlHighlightedTextColor"] = reinterpret_cast<void*>(stub_Control_GetControlHighlightedTextColor);
    s_stubMap["Control/GetControlMaxSize"] = reinterpret_cast<void*>(stub_Control_GetControlMaxSize);
    s_stubMap["Control/GetControlMinSize"] = reinterpret_cast<void*>(stub_Control_GetControlMinSize);
    s_stubMap["Control/GetControlMouseTrackingEnabled"] = reinterpret_cast<void*>(stub_Control_GetControlMouseTrackingEnabled);
    s_stubMap["Control/GetControlParent"] = reinterpret_cast<void*>(stub_Control_GetControlParent);
    s_stubMap["Control/GetControlPosition"] = reinterpret_cast<void*>(stub_Control_GetControlPosition);
    s_stubMap["Control/GetControlResourcePixelRatio"] = reinterpret_cast<void*>(stub_Control_GetControlResourcePixelRatio);
    s_stubMap["Control/GetControlSizer"] = reinterpret_cast<void*>(stub_Control_GetControlSizer);
    s_stubMap["Control/GetControlStyleSheet"] = reinterpret_cast<void*>(stub_Control_GetControlStyleSheet);
    s_stubMap["Control/GetControlTextColor"] = reinterpret_cast<void*>(stub_Control_GetControlTextColor);
    s_stubMap["Control/GetControlUnderMouseStatus"] = reinterpret_cast<void*>(stub_Control_GetControlUnderMouseStatus);
    s_stubMap["Control/GetControlUpdatesEnabled"] = reinterpret_cast<void*>(stub_Control_GetControlUpdatesEnabled);
    s_stubMap["Control/GetControlVisible"] = reinterpret_cast<void*>(stub_Control_GetControlVisible);
    s_stubMap["Control/GetControlVisibleRect"] = reinterpret_cast<void*>(stub_Control_GetControlVisibleRect);
    s_stubMap["Control/GetControlWindow"] = reinterpret_cast<void*>(stub_Control_GetControlWindow);
    s_stubMap["Control/GetFocusChildControl"] = reinterpret_cast<void*>(stub_Control_GetFocusChildControl);
    s_stubMap["Control/GetFrameRect"] = reinterpret_cast<void*>(stub_Control_GetFrameRect);
    s_stubMap["Control/GetInfoText"] = reinterpret_cast<void*>(stub_Control_GetInfoText);
    s_stubMap["Control/GetNextSiblingControlToFocus"] = reinterpret_cast<void*>(stub_Control_GetNextSiblingControlToFocus);
    s_stubMap["Control/GetRealTimePreviewActive"] = reinterpret_cast<void*>(stub_Control_GetRealTimePreviewActive);
    s_stubMap["Control/GetTrackViewActive"] = reinterpret_cast<void*>(stub_Control_GetTrackViewActive);
    s_stubMap["Control/GetWindowOpacity"] = reinterpret_cast<void*>(stub_Control_GetWindowOpacity);
    s_stubMap["Control/GetWindowState"] = reinterpret_cast<void*>(stub_Control_GetWindowState);
    s_stubMap["Control/GetWindowTitle"] = reinterpret_cast<void*>(stub_Control_GetWindowTitle);
    s_stubMap["Control/GetWindowToolTip"] = reinterpret_cast<void*>(stub_Control_GetWindowToolTip);
    s_stubMap["Control/GlobalToLocal"] = reinterpret_cast<void*>(stub_Control_GlobalToLocal);
    s_stubMap["Control/LocalToControl"] = reinterpret_cast<void*>(stub_Control_LocalToControl);
    s_stubMap["Control/LocalToGlobal"] = reinterpret_cast<void*>(stub_Control_LocalToGlobal);
    s_stubMap["Control/LocalToParent"] = reinterpret_cast<void*>(stub_Control_LocalToParent);
    s_stubMap["Control/ParentToLocal"] = reinterpret_cast<void*>(stub_Control_ParentToLocal);
    s_stubMap["Control/RepaintControl"] = reinterpret_cast<void*>(stub_Control_RepaintControl);
    s_stubMap["Control/RepaintControlRect"] = reinterpret_cast<void*>(stub_Control_RepaintControlRect);
    s_stubMap["Control/RestyleControl"] = reinterpret_cast<void*>(stub_Control_RestyleControl);
    s_stubMap["Control/ScrollControl"] = reinterpret_cast<void*>(stub_Control_ScrollControl);
    s_stubMap["Control/ScrollControlRect"] = reinterpret_cast<void*>(stub_Control_ScrollControlRect);
    s_stubMap["Control/SendControlToBack"] = reinterpret_cast<void*>(stub_Control_SendControlToBack);
    s_stubMap["Control/SetChildControlToFocus"] = reinterpret_cast<void*>(stub_Control_SetChildControlToFocus);
    s_stubMap["Control/SetChildCreateEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetChildCreateEventRoutine);
    s_stubMap["Control/SetChildDestroyEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetChildDestroyEventRoutine);
    s_stubMap["Control/SetClientRect"] = reinterpret_cast<void*>(stub_Control_SetClientRect);
    s_stubMap["Control/SetCloseEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetCloseEventRoutine);
    s_stubMap["Control/SetControlAlternateCanvasColor"] = reinterpret_cast<void*>(stub_Control_SetControlAlternateCanvasColor);
    s_stubMap["Control/SetControlBackgroundColor"] = reinterpret_cast<void*>(stub_Control_SetControlBackgroundColor);
    s_stubMap["Control/SetControlButtonColor"] = reinterpret_cast<void*>(stub_Control_SetControlButtonColor);
    s_stubMap["Control/SetControlButtonTextColor"] = reinterpret_cast<void*>(stub_Control_SetControlButtonTextColor);
    s_stubMap["Control/SetControlCanvasColor"] = reinterpret_cast<void*>(stub_Control_SetControlCanvasColor);
    s_stubMap["Control/SetControlCursor"] = reinterpret_cast<void*>(stub_Control_SetControlCursor);
    s_stubMap["Control/SetControlCursorToParent"] = reinterpret_cast<void*>(stub_Control_SetControlCursorToParent);
    s_stubMap["Control/SetControlEnabled"] = reinterpret_cast<void*>(stub_Control_SetControlEnabled);
    s_stubMap["Control/SetControlExpansionEnabled"] = reinterpret_cast<void*>(stub_Control_SetControlExpansionEnabled);
    s_stubMap["Control/SetControlFixedSize"] = reinterpret_cast<void*>(stub_Control_SetControlFixedSize);
    s_stubMap["Control/SetControlFocus"] = reinterpret_cast<void*>(stub_Control_SetControlFocus);
    s_stubMap["Control/SetControlFocusStyle"] = reinterpret_cast<void*>(stub_Control_SetControlFocusStyle);
    s_stubMap["Control/SetControlFont"] = reinterpret_cast<void*>(stub_Control_SetControlFont);
    s_stubMap["Control/SetControlForegroundColor"] = reinterpret_cast<void*>(stub_Control_SetControlForegroundColor);
    s_stubMap["Control/SetControlHighlightColor"] = reinterpret_cast<void*>(stub_Control_SetControlHighlightColor);
    s_stubMap["Control/SetControlHighlightedTextColor"] = reinterpret_cast<void*>(stub_Control_SetControlHighlightedTextColor);
    s_stubMap["Control/SetControlMaxSize"] = reinterpret_cast<void*>(stub_Control_SetControlMaxSize);
    s_stubMap["Control/SetControlMinSize"] = reinterpret_cast<void*>(stub_Control_SetControlMinSize);
    s_stubMap["Control/SetControlMouseTrackingEnabled"] = reinterpret_cast<void*>(stub_Control_SetControlMouseTrackingEnabled);
    s_stubMap["Control/SetControlParent"] = reinterpret_cast<void*>(stub_Control_SetControlParent);
    s_stubMap["Control/SetControlPosition"] = reinterpret_cast<void*>(stub_Control_SetControlPosition);
    s_stubMap["Control/SetControlSize"] = reinterpret_cast<void*>(stub_Control_SetControlSize);
    s_stubMap["Control/SetControlSizer"] = reinterpret_cast<void*>(stub_Control_SetControlSizer);
    s_stubMap["Control/SetControlStyleSheet"] = reinterpret_cast<void*>(stub_Control_SetControlStyleSheet);
    s_stubMap["Control/SetControlTextColor"] = reinterpret_cast<void*>(stub_Control_SetControlTextColor);
    s_stubMap["Control/SetControlUpdatesEnabled"] = reinterpret_cast<void*>(stub_Control_SetControlUpdatesEnabled);
    s_stubMap["Control/SetControlVisible"] = reinterpret_cast<void*>(stub_Control_SetControlVisible);
    s_stubMap["Control/SetDestroyEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetDestroyEventRoutine);
    s_stubMap["Control/SetEnterEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetEnterEventRoutine);
    s_stubMap["Control/SetFileDragEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetFileDragEventRoutine);
    s_stubMap["Control/SetFileDropEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetFileDropEventRoutine);
    s_stubMap["Control/SetGetFocusEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetGetFocusEventRoutine);
    s_stubMap["Control/SetHideEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetHideEventRoutine);
    s_stubMap["Control/SetInfoText"] = reinterpret_cast<void*>(stub_Control_SetInfoText);
    s_stubMap["Control/SetKeyPressEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetKeyPressEventRoutine);
    s_stubMap["Control/SetKeyReleaseEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetKeyReleaseEventRoutine);
    s_stubMap["Control/SetLeaveEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetLeaveEventRoutine);
    s_stubMap["Control/SetLoseFocusEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetLoseFocusEventRoutine);
    s_stubMap["Control/SetMouseDoubleClickEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetMouseDoubleClickEventRoutine);
    s_stubMap["Control/SetMouseMoveEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetMouseMoveEventRoutine);
    s_stubMap["Control/SetMousePressEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetMousePressEventRoutine);
    s_stubMap["Control/SetMouseReleaseEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetMouseReleaseEventRoutine);
    s_stubMap["Control/SetMoveEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetMoveEventRoutine);
    s_stubMap["Control/SetNextSiblingControlToFocus"] = reinterpret_cast<void*>(stub_Control_SetNextSiblingControlToFocus);
    s_stubMap["Control/SetPaintEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetPaintEventRoutine);
    s_stubMap["Control/SetRealTimePreviewActive"] = reinterpret_cast<void*>(stub_Control_SetRealTimePreviewActive);
    s_stubMap["Control/SetResizeEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetResizeEventRoutine);
    s_stubMap["Control/SetShowEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetShowEventRoutine);
    s_stubMap["Control/SetTrackViewActive"] = reinterpret_cast<void*>(stub_Control_SetTrackViewActive);
    s_stubMap["Control/SetViewDragEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetViewDragEventRoutine);
    s_stubMap["Control/SetViewDropEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetViewDropEventRoutine);
    s_stubMap["Control/SetWheelEventRoutine"] = reinterpret_cast<void*>(stub_Control_SetWheelEventRoutine);
    s_stubMap["Control/SetWindowOpacity"] = reinterpret_cast<void*>(stub_Control_SetWindowOpacity);
    s_stubMap["Control/SetWindowTitle"] = reinterpret_cast<void*>(stub_Control_SetWindowTitle);
    s_stubMap["Control/SetWindowToolTip"] = reinterpret_cast<void*>(stub_Control_SetWindowToolTip);
    s_stubMap["Control/StackControls"] = reinterpret_cast<void*>(stub_Control_StackControls);
    s_stubMap["Control/UpdateControl"] = reinterpret_cast<void*>(stub_Control_UpdateControl);
    s_stubMap["Control/UpdateControlRect"] = reinterpret_cast<void*>(stub_Control_UpdateControlRect);
    s_stubMap["Cursor/CloneCursor"] = reinterpret_cast<void*>(stub_Cursor_CloneCursor);
    s_stubMap["Cursor/CreateBitmapCursor"] = reinterpret_cast<void*>(stub_Cursor_CreateBitmapCursor);
    s_stubMap["Cursor/CreateCursor"] = reinterpret_cast<void*>(stub_Cursor_CreateCursor);
    s_stubMap["Cursor/GetCursorHotSpot"] = reinterpret_cast<void*>(stub_Cursor_GetCursorHotSpot);
    s_stubMap["Dialog/CreateDialog"] = reinterpret_cast<void*>(stub_Dialog_CreateDialog);
    s_stubMap["Dialog/ExecuteDialog"] = reinterpret_cast<void*>(stub_Dialog_ExecuteDialog);
    s_stubMap["Dialog/ExecuteGetDirectoryDialog"] = reinterpret_cast<void*>(stub_Dialog_ExecuteGetDirectoryDialog);
    s_stubMap["Dialog/ExecuteOpenFileDialog"] = reinterpret_cast<void*>(stub_Dialog_ExecuteOpenFileDialog);
    s_stubMap["Dialog/ExecuteOpenMultipleFilesDialog"] = reinterpret_cast<void*>(stub_Dialog_ExecuteOpenMultipleFilesDialog);
    s_stubMap["Dialog/ExecuteSaveFileDialog"] = reinterpret_cast<void*>(stub_Dialog_ExecuteSaveFileDialog);
    s_stubMap["Dialog/GetDialogResizable"] = reinterpret_cast<void*>(stub_Dialog_GetDialogResizable);
    s_stubMap["Dialog/OpenDialog"] = reinterpret_cast<void*>(stub_Dialog_OpenDialog);
    s_stubMap["Dialog/ReturnDialog"] = reinterpret_cast<void*>(stub_Dialog_ReturnDialog);
    s_stubMap["Dialog/SetDialogResizable"] = reinterpret_cast<void*>(stub_Dialog_SetDialogResizable);
    s_stubMap["Dialog/SetExecuteDialogEventRoutine"] = reinterpret_cast<void*>(stub_Dialog_SetExecuteDialogEventRoutine);
    s_stubMap["Dialog/SetReturnDialogEventRoutine"] = reinterpret_cast<void*>(stub_Dialog_SetReturnDialogEventRoutine);
    s_stubMap["Edit/CreateEdit"] = reinterpret_cast<void*>(stub_Edit_CreateEdit);
    s_stubMap["Edit/GetEditAlignment"] = reinterpret_cast<void*>(stub_Edit_GetEditAlignment);
    s_stubMap["Edit/GetEditCaretPosition"] = reinterpret_cast<void*>(stub_Edit_GetEditCaretPosition);
    s_stubMap["Edit/GetEditMask"] = reinterpret_cast<void*>(stub_Edit_GetEditMask);
    s_stubMap["Edit/GetEditMaxLength"] = reinterpret_cast<void*>(stub_Edit_GetEditMaxLength);
    s_stubMap["Edit/GetEditModified"] = reinterpret_cast<void*>(stub_Edit_GetEditModified);
    s_stubMap["Edit/GetEditPasswordEnabled"] = reinterpret_cast<void*>(stub_Edit_GetEditPasswordEnabled);
    s_stubMap["Edit/GetEditReadOnly"] = reinterpret_cast<void*>(stub_Edit_GetEditReadOnly);
    s_stubMap["Edit/GetEditSelectedText"] = reinterpret_cast<void*>(stub_Edit_GetEditSelectedText);
    s_stubMap["Edit/GetEditSelection"] = reinterpret_cast<void*>(stub_Edit_GetEditSelection);
    s_stubMap["Edit/GetEditText"] = reinterpret_cast<void*>(stub_Edit_GetEditText);
    s_stubMap["Edit/GetEditValid"] = reinterpret_cast<void*>(stub_Edit_GetEditValid);
    s_stubMap["Edit/GetEditValidatingRegExp"] = reinterpret_cast<void*>(stub_Edit_GetEditValidatingRegExp);
    s_stubMap["Edit/SetCaretPositionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_Edit_SetCaretPositionUpdatedEventRoutine);
    s_stubMap["Edit/SetEditAlignment"] = reinterpret_cast<void*>(stub_Edit_SetEditAlignment);
    s_stubMap["Edit/SetEditCaretPosition"] = reinterpret_cast<void*>(stub_Edit_SetEditCaretPosition);
    s_stubMap["Edit/SetEditCompletedEventRoutine"] = reinterpret_cast<void*>(stub_Edit_SetEditCompletedEventRoutine);
    s_stubMap["Edit/SetEditMask"] = reinterpret_cast<void*>(stub_Edit_SetEditMask);
    s_stubMap["Edit/SetEditMaxLength"] = reinterpret_cast<void*>(stub_Edit_SetEditMaxLength);
    s_stubMap["Edit/SetEditModified"] = reinterpret_cast<void*>(stub_Edit_SetEditModified);
    s_stubMap["Edit/SetEditPasswordEnabled"] = reinterpret_cast<void*>(stub_Edit_SetEditPasswordEnabled);
    s_stubMap["Edit/SetEditReadOnly"] = reinterpret_cast<void*>(stub_Edit_SetEditReadOnly);
    s_stubMap["Edit/SetEditSelected"] = reinterpret_cast<void*>(stub_Edit_SetEditSelected);
    s_stubMap["Edit/SetEditSelection"] = reinterpret_cast<void*>(stub_Edit_SetEditSelection);
    s_stubMap["Edit/SetEditText"] = reinterpret_cast<void*>(stub_Edit_SetEditText);
    s_stubMap["Edit/SetEditValidatingRegExp"] = reinterpret_cast<void*>(stub_Edit_SetEditValidatingRegExp);
    s_stubMap["Edit/SetReturnPressedEventRoutine"] = reinterpret_cast<void*>(stub_Edit_SetReturnPressedEventRoutine);
    s_stubMap["Edit/SetSelectionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_Edit_SetSelectionUpdatedEventRoutine);
    s_stubMap["Edit/SetTextUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_Edit_SetTextUpdatedEventRoutine);
    s_stubMap["ExternalProcess/CloseExternalProcessStream"] = reinterpret_cast<void*>(stub_ExternalProcess_CloseExternalProcessStream);
    s_stubMap["ExternalProcess/CreateExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_CreateExternalProcess);
    s_stubMap["ExternalProcess/EnumerateExternalProcessEnvironment"] = reinterpret_cast<void*>(stub_ExternalProcess_EnumerateExternalProcessEnvironment);
    s_stubMap["ExternalProcess/ExecuteProgram"] = reinterpret_cast<void*>(stub_ExternalProcess_ExecuteProgram);
    s_stubMap["ExternalProcess/GetExternalProcessBytesAvailable"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessBytesAvailable);
    s_stubMap["ExternalProcess/GetExternalProcessBytesToWrite"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessBytesToWrite);
    s_stubMap["ExternalProcess/GetExternalProcessErrorCode"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessErrorCode);
    s_stubMap["ExternalProcess/GetExternalProcessExitCode"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessExitCode);
    s_stubMap["ExternalProcess/GetExternalProcessExitStatus"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessExitStatus);
    s_stubMap["ExternalProcess/GetExternalProcessIsRunning"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessIsRunning);
    s_stubMap["ExternalProcess/GetExternalProcessIsStarting"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessIsStarting);
    s_stubMap["ExternalProcess/GetExternalProcessPID"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessPID);
    s_stubMap["ExternalProcess/GetExternalProcessWorkingDirectory"] = reinterpret_cast<void*>(stub_ExternalProcess_GetExternalProcessWorkingDirectory);
    s_stubMap["ExternalProcess/KillExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_KillExternalProcess);
    s_stubMap["ExternalProcess/PipeExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_PipeExternalProcess);
    s_stubMap["ExternalProcess/ReadFromExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_ReadFromExternalProcess);
    s_stubMap["ExternalProcess/RedirectExternalProcessToFile"] = reinterpret_cast<void*>(stub_ExternalProcess_RedirectExternalProcessToFile);
    s_stubMap["ExternalProcess/SetExternalProcessEnvironment"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessEnvironment);
    s_stubMap["ExternalProcess/SetExternalProcessErrorEventRoutine"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessErrorEventRoutine);
    s_stubMap["ExternalProcess/SetExternalProcessFinishedEventRoutine"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessFinishedEventRoutine);
    s_stubMap["ExternalProcess/SetExternalProcessStandardErrorDataAvailableEventRoutine"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessStandardErrorDataAvailableEventRoutine);
    s_stubMap["ExternalProcess/SetExternalProcessStandardOutputDataAvailableEventRoutine"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessStandardOutputDataAvailableEventRoutine);
    s_stubMap["ExternalProcess/SetExternalProcessStartedEventRoutine"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessStartedEventRoutine);
    s_stubMap["ExternalProcess/SetExternalProcessWorkingDirectory"] = reinterpret_cast<void*>(stub_ExternalProcess_SetExternalProcessWorkingDirectory);
    s_stubMap["ExternalProcess/StartExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_StartExternalProcess);
    s_stubMap["ExternalProcess/StartProgram"] = reinterpret_cast<void*>(stub_ExternalProcess_StartProgram);
    s_stubMap["ExternalProcess/TerminateExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_TerminateExternalProcess);
    s_stubMap["ExternalProcess/WaitForExternalProcessDataAvailable"] = reinterpret_cast<void*>(stub_ExternalProcess_WaitForExternalProcessDataAvailable);
    s_stubMap["ExternalProcess/WaitForExternalProcessDataWritten"] = reinterpret_cast<void*>(stub_ExternalProcess_WaitForExternalProcessDataWritten);
    s_stubMap["ExternalProcess/WaitForExternalProcessFinished"] = reinterpret_cast<void*>(stub_ExternalProcess_WaitForExternalProcessFinished);
    s_stubMap["ExternalProcess/WaitForExternalProcessStarted"] = reinterpret_cast<void*>(stub_ExternalProcess_WaitForExternalProcessStarted);
    s_stubMap["ExternalProcess/WriteToExternalProcess"] = reinterpret_cast<void*>(stub_ExternalProcess_WriteToExternalProcess);
    s_stubMap["FileFormat/AddKeyword"] = reinterpret_cast<void*>(stub_FileFormat_AddKeyword);
    s_stubMap["FileFormat/BeginColorFilterArrayEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginColorFilterArrayEmbedding);
    s_stubMap["FileFormat/BeginColorFilterArrayExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginColorFilterArrayExtraction);
    s_stubMap["FileFormat/BeginDisplayFunctionEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginDisplayFunctionEmbedding);
    s_stubMap["FileFormat/BeginDisplayFunctionExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginDisplayFunctionExtraction);
    s_stubMap["FileFormat/BeginICCProfileEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginICCProfileEmbedding);
    s_stubMap["FileFormat/BeginICCProfileExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginICCProfileExtraction);
    s_stubMap["FileFormat/BeginImagePropertyEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginImagePropertyEmbedding);
    s_stubMap["FileFormat/BeginImagePropertyExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginImagePropertyExtraction);
    s_stubMap["FileFormat/BeginKeywordEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginKeywordEmbedding);
    s_stubMap["FileFormat/BeginKeywordExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginKeywordExtraction);
    s_stubMap["FileFormat/BeginPropertyEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginPropertyEmbedding);
    s_stubMap["FileFormat/BeginPropertyExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginPropertyExtraction);
    s_stubMap["FileFormat/BeginRGBWSEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginRGBWSEmbedding);
    s_stubMap["FileFormat/BeginRGBWSExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginRGBWSExtraction);
    s_stubMap["FileFormat/BeginThumbnailEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_BeginThumbnailEmbedding);
    s_stubMap["FileFormat/BeginThumbnailExtraction"] = reinterpret_cast<void*>(stub_FileFormat_BeginThumbnailExtraction);
    s_stubMap["FileFormat/CanReadIncrementally"] = reinterpret_cast<void*>(stub_FileFormat_CanReadIncrementally);
    s_stubMap["FileFormat/CanWriteIncrementally"] = reinterpret_cast<void*>(stub_FileFormat_CanWriteIncrementally);
    s_stubMap["FileFormat/CloseImage"] = reinterpret_cast<void*>(stub_FileFormat_CloseImage);
    s_stubMap["FileFormat/CloseImageFile"] = reinterpret_cast<void*>(stub_FileFormat_CloseImageFile);
    s_stubMap["FileFormat/CreateFileFormatInstance"] = reinterpret_cast<void*>(stub_FileFormat_CreateFileFormatInstance);
    s_stubMap["FileFormat/CreateImage"] = reinterpret_cast<void*>(stub_FileFormat_CreateImage);
    s_stubMap["FileFormat/CreateImageFile"] = reinterpret_cast<void*>(stub_FileFormat_CreateImageFile);
    s_stubMap["FileFormat/CreateImageFileEx"] = reinterpret_cast<void*>(stub_FileFormat_CreateImageFileEx);
    s_stubMap["FileFormat/DisposeFormatSpecificData"] = reinterpret_cast<void*>(stub_FileFormat_DisposeFormatSpecificData);
    s_stubMap["FileFormat/EditFileFormatPreferences"] = reinterpret_cast<void*>(stub_FileFormat_EditFileFormatPreferences);
    s_stubMap["FileFormat/EndColorFilterArrayEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndColorFilterArrayEmbedding);
    s_stubMap["FileFormat/EndColorFilterArrayExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndColorFilterArrayExtraction);
    s_stubMap["FileFormat/EndDisplayFunctionEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndDisplayFunctionEmbedding);
    s_stubMap["FileFormat/EndDisplayFunctionExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndDisplayFunctionExtraction);
    s_stubMap["FileFormat/EndICCProfileEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndICCProfileEmbedding);
    s_stubMap["FileFormat/EndICCProfileExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndICCProfileExtraction);
    s_stubMap["FileFormat/EndImagePropertyEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndImagePropertyEmbedding);
    s_stubMap["FileFormat/EndImagePropertyExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndImagePropertyExtraction);
    s_stubMap["FileFormat/EndKeywordEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndKeywordEmbedding);
    s_stubMap["FileFormat/EndKeywordExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndKeywordExtraction);
    s_stubMap["FileFormat/EndPropertyEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndPropertyEmbedding);
    s_stubMap["FileFormat/EndPropertyExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndPropertyExtraction);
    s_stubMap["FileFormat/EndRGBWSEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndRGBWSEmbedding);
    s_stubMap["FileFormat/EndRGBWSExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndRGBWSExtraction);
    s_stubMap["FileFormat/EndThumbnailEmbedding"] = reinterpret_cast<void*>(stub_FileFormat_EndThumbnailEmbedding);
    s_stubMap["FileFormat/EndThumbnailExtraction"] = reinterpret_cast<void*>(stub_FileFormat_EndThumbnailExtraction);
    s_stubMap["FileFormat/EnumerateFileFormats"] = reinterpret_cast<void*>(stub_FileFormat_EnumerateFileFormats);
    s_stubMap["FileFormat/EnumerateImageProperties"] = reinterpret_cast<void*>(stub_FileFormat_EnumerateImageProperties);
    s_stubMap["FileFormat/EnumerateProperties"] = reinterpret_cast<void*>(stub_FileFormat_EnumerateProperties);
    s_stubMap["FileFormat/GetFileFormatByFileExtension"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatByFileExtension);
    s_stubMap["FileFormat/GetFileFormatByMimeType"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatByMimeType);
    s_stubMap["FileFormat/GetFileFormatByName"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatByName);
    s_stubMap["FileFormat/GetFileFormatCapabilities"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatCapabilities);
    s_stubMap["FileFormat/GetFileFormatDescription"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatDescription);
    s_stubMap["FileFormat/GetFileFormatFileExtensions"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatFileExtensions);
    s_stubMap["FileFormat/GetFileFormatIcon"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatIcon);
    s_stubMap["FileFormat/GetFileFormatImplementation"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatImplementation);
    s_stubMap["FileFormat/GetFileFormatInstanceFormat"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatInstanceFormat);
    s_stubMap["FileFormat/GetFileFormatMimeTypes"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatMimeTypes);
    s_stubMap["FileFormat/GetFileFormatName"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatName);
    s_stubMap["FileFormat/GetFileFormatSmallIcon"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatSmallIcon);
    s_stubMap["FileFormat/GetFileFormatStatus"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatStatus);
    s_stubMap["FileFormat/GetFileFormatVersion"] = reinterpret_cast<void*>(stub_FileFormat_GetFileFormatVersion);
    s_stubMap["FileFormat/GetFormatSpecificData"] = reinterpret_cast<void*>(stub_FileFormat_GetFormatSpecificData);
    s_stubMap["FileFormat/GetICCProfile"] = reinterpret_cast<void*>(stub_FileFormat_GetICCProfile);
    s_stubMap["FileFormat/GetImageColorFilterArray"] = reinterpret_cast<void*>(stub_FileFormat_GetImageColorFilterArray);
    s_stubMap["FileFormat/GetImageCount"] = reinterpret_cast<void*>(stub_FileFormat_GetImageCount);
    s_stubMap["FileFormat/GetImageDescription"] = reinterpret_cast<void*>(stub_FileFormat_GetImageDescription);
    s_stubMap["FileFormat/GetImageDisplayFunction"] = reinterpret_cast<void*>(stub_FileFormat_GetImageDisplayFunction);
    s_stubMap["FileFormat/GetImageFilePath"] = reinterpret_cast<void*>(stub_FileFormat_GetImageFilePath);
    s_stubMap["FileFormat/GetImageFormatInfo"] = reinterpret_cast<void*>(stub_FileFormat_GetImageFormatInfo);
    s_stubMap["FileFormat/GetImageId"] = reinterpret_cast<void*>(stub_FileFormat_GetImageId);
    s_stubMap["FileFormat/GetImageProperty"] = reinterpret_cast<void*>(stub_FileFormat_GetImageProperty);
    s_stubMap["FileFormat/GetImageRGBWS"] = reinterpret_cast<void*>(stub_FileFormat_GetImageRGBWS);
    s_stubMap["FileFormat/GetKeywordCount"] = reinterpret_cast<void*>(stub_FileFormat_GetKeywordCount);
    s_stubMap["FileFormat/GetNextKeyword"] = reinterpret_cast<void*>(stub_FileFormat_GetNextKeyword);
    s_stubMap["FileFormat/GetProperty"] = reinterpret_cast<void*>(stub_FileFormat_GetProperty);
    s_stubMap["FileFormat/GetSelectedImageIndex"] = reinterpret_cast<void*>(stub_FileFormat_GetSelectedImageIndex);
    s_stubMap["FileFormat/GetThumbnail"] = reinterpret_cast<void*>(stub_FileFormat_GetThumbnail);
    s_stubMap["FileFormat/IsImageFileOpen"] = reinterpret_cast<void*>(stub_FileFormat_IsImageFileOpen);
    s_stubMap["FileFormat/OpenImageFile"] = reinterpret_cast<void*>(stub_FileFormat_OpenImageFile);
    s_stubMap["FileFormat/OpenImageFileEx"] = reinterpret_cast<void*>(stub_FileFormat_OpenImageFileEx);
    s_stubMap["FileFormat/QueryImageFileOptions"] = reinterpret_cast<void*>(stub_FileFormat_QueryImageFileOptions);
    s_stubMap["FileFormat/ReadImage"] = reinterpret_cast<void*>(stub_FileFormat_ReadImage);
    s_stubMap["FileFormat/ReadSamples"] = reinterpret_cast<void*>(stub_FileFormat_ReadSamples);
    s_stubMap["FileFormat/SelectImage"] = reinterpret_cast<void*>(stub_FileFormat_SelectImage);
    s_stubMap["FileFormat/SetFormatSpecificData"] = reinterpret_cast<void*>(stub_FileFormat_SetFormatSpecificData);
    s_stubMap["FileFormat/SetICCProfile"] = reinterpret_cast<void*>(stub_FileFormat_SetICCProfile);
    s_stubMap["FileFormat/SetImageColorFilterArray"] = reinterpret_cast<void*>(stub_FileFormat_SetImageColorFilterArray);
    s_stubMap["FileFormat/SetImageDisplayFunction"] = reinterpret_cast<void*>(stub_FileFormat_SetImageDisplayFunction);
    s_stubMap["FileFormat/SetImageId"] = reinterpret_cast<void*>(stub_FileFormat_SetImageId);
    s_stubMap["FileFormat/SetImageOptions"] = reinterpret_cast<void*>(stub_FileFormat_SetImageOptions);
    s_stubMap["FileFormat/SetImageProperty"] = reinterpret_cast<void*>(stub_FileFormat_SetImageProperty);
    s_stubMap["FileFormat/SetImageRGBWS"] = reinterpret_cast<void*>(stub_FileFormat_SetImageRGBWS);
    s_stubMap["FileFormat/SetProperty"] = reinterpret_cast<void*>(stub_FileFormat_SetProperty);
    s_stubMap["FileFormat/SetThumbnail"] = reinterpret_cast<void*>(stub_FileFormat_SetThumbnail);
    s_stubMap["FileFormat/ValidateFormatSpecificData"] = reinterpret_cast<void*>(stub_FileFormat_ValidateFormatSpecificData);
    s_stubMap["FileFormat/WasInexactRead"] = reinterpret_cast<void*>(stub_FileFormat_WasInexactRead);
    s_stubMap["FileFormat/WasLossyWrite"] = reinterpret_cast<void*>(stub_FileFormat_WasLossyWrite);
    s_stubMap["FileFormat/WriteImage"] = reinterpret_cast<void*>(stub_FileFormat_WriteImage);
    s_stubMap["FileFormat/WriteSamples"] = reinterpret_cast<void*>(stub_FileFormat_WriteSamples);
    s_stubMap["FileFormatDefinition/BeginFileFormatDefinition"] = reinterpret_cast<void*>(stub_FileFormatDefinition_BeginFileFormatDefinition);
    s_stubMap["FileFormatDefinition/EndFileFormatDefinition"] = reinterpret_cast<void*>(stub_FileFormatDefinition_EndFileFormatDefinition);
    s_stubMap["FileFormatDefinition/EnterFileFormatDefinitionContext"] = reinterpret_cast<void*>(stub_FileFormatDefinition_EnterFileFormatDefinitionContext);
    s_stubMap["FileFormatDefinition/ExitFileFormatDefinitionContext"] = reinterpret_cast<void*>(stub_FileFormatDefinition_ExitFileFormatDefinitionContext);
    s_stubMap["FileFormatDefinition/GetFileFormatBeingDefined"] = reinterpret_cast<void*>(stub_FileFormatDefinition_GetFileFormatBeingDefined);
    s_stubMap["FileFormatDefinition/IsFileFormatDefinitionContextActive"] = reinterpret_cast<void*>(stub_FileFormatDefinition_IsFileFormatDefinitionContextActive);
    s_stubMap["FileFormatDefinition/SetFileFormatAddKeywordRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatAddKeywordRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatAllowIncrementalReadRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatAllowIncrementalReadRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatAllowIncrementalWriteRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatAllowIncrementalWriteRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginColorFilterArrayEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginColorFilterArrayEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginColorFilterArrayExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginColorFilterArrayExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginDisplayFunctionEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginDisplayFunctionEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginDisplayFunctionExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginDisplayFunctionExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginICCProfileEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginICCProfileEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginICCProfileExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginICCProfileExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginImagePropertyEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginImagePropertyEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginImagePropertyExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginImagePropertyExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginKeywordEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginKeywordEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginKeywordExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginKeywordExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginPropertyEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginPropertyEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginPropertyExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginPropertyExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginRGBWSEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginRGBWSEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginRGBWSExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginRGBWSExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginThumbnailEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginThumbnailEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatBeginThumbnailExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatBeginThumbnailExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatCaps"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCaps);
    s_stubMap["FileFormatDefinition/SetFileFormatCloseImageRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCloseImageRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatCloseRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCloseRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatCreateImageRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCreateImageRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatCreateRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCreateRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatCreationRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatCreationRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatDescription"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatDescription);
    s_stubMap["FileFormatDefinition/SetFileFormatDestructionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatDestructionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatDisposeFormatSpecificDataRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatDisposeFormatSpecificDataRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEditPreferencesRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEditPreferencesRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndColorFilterArrayEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndColorFilterArrayEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndColorFilterArrayExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndColorFilterArrayExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndDisplayFunctionEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndDisplayFunctionEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndDisplayFunctionExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndDisplayFunctionExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndICCProfileEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndICCProfileEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndICCProfileExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndICCProfileExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndImagePropertyEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndImagePropertyEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndImagePropertyExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndImagePropertyExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndKeywordEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndKeywordEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndKeywordExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndKeywordExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndPropertyEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndPropertyEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndPropertyExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndPropertyExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndRGBWSEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndRGBWSEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndRGBWSExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndRGBWSExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndThumbnailEmbeddingRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndThumbnailEmbeddingRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEndThumbnailExtractionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEndThumbnailExtractionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEnumerateImagePropertiesRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEnumerateImagePropertiesRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatEnumeratePropertiesRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatEnumeratePropertiesRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetFilePathRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetFilePathRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetFormatSpecificDataRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetFormatSpecificDataRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetICCProfileRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetICCProfileRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageColorFilterArrayRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageColorFilterArrayRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageCountRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageCountRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageDescriptionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageDescriptionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageDisplayFunctionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageDisplayFunctionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageFormatInfoRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageFormatInfoRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageIdRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageIdRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImagePropertyRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImagePropertyRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetImageRGBWSRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetImageRGBWSRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetKeywordCountRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetKeywordCountRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetNextKeywordRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetNextKeywordRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetPropertyRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetPropertyRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetSelectedImageIndexRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetSelectedImageIndexRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatGetThumbnailRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatGetThumbnailRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatIconImage"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconImage);
    s_stubMap["FileFormatDefinition/SetFileFormatIconImageFile"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconImageFile);
    s_stubMap["FileFormatDefinition/SetFileFormatIconSVG"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconSVG);
    s_stubMap["FileFormatDefinition/SetFileFormatIconSVGFile"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconSVGFile);
    s_stubMap["FileFormatDefinition/SetFileFormatIconSmallImage"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconSmallImage);
    s_stubMap["FileFormatDefinition/SetFileFormatIconSmallImageFile"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIconSmallImageFile);
    s_stubMap["FileFormatDefinition/SetFileFormatImplementation"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatImplementation);
    s_stubMap["FileFormatDefinition/SetFileFormatIsOpenRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatIsOpenRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatOpenRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatOpenRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatQueryFormatStatusRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatQueryFormatStatusRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatQueryInexactReadRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatQueryInexactReadRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatQueryLossyWriteRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatQueryLossyWriteRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatQueryOptionsRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatQueryOptionsRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatReadImageRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatReadImageRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatReadSamplesRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatReadSamplesRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetFormatSpecificDataRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetFormatSpecificDataRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetICCProfileRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetICCProfileRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImageColorFilterArrayRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImageColorFilterArrayRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImageDisplayFunctionRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImageDisplayFunctionRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImageIdRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImageIdRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImageOptionsRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImageOptionsRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImagePropertyRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImagePropertyRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetImageRGBWSRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetImageRGBWSRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetPropertyRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetPropertyRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetSelectedImageIndexRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetSelectedImageIndexRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatSetThumbnailRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatSetThumbnailRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatValidateFormatSpecificDataRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatValidateFormatSpecificDataRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatVersion"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatVersion);
    s_stubMap["FileFormatDefinition/SetFileFormatWriteImageRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatWriteImageRoutine);
    s_stubMap["FileFormatDefinition/SetFileFormatWriteSamplesRoutine"] = reinterpret_cast<void*>(stub_FileFormatDefinition_SetFileFormatWriteSamplesRoutine);
    s_stubMap["Font/CloneFont"] = reinterpret_cast<void*>(stub_Font_CloneFont);
    s_stubMap["Font/CreateFontByFace"] = reinterpret_cast<void*>(stub_Font_CreateFontByFace);
    s_stubMap["Font/CreateFontByFamily"] = reinterpret_cast<void*>(stub_Font_CreateFontByFamily);
    s_stubMap["Font/EnumerateFontStyles"] = reinterpret_cast<void*>(stub_Font_EnumerateFontStyles);
    s_stubMap["Font/EnumerateFonts"] = reinterpret_cast<void*>(stub_Font_EnumerateFonts);
    s_stubMap["Font/EnumerateOptimalFontPointSizes"] = reinterpret_cast<void*>(stub_Font_EnumerateOptimalFontPointSizes);
    s_stubMap["Font/EnumerateWritingSystems"] = reinterpret_cast<void*>(stub_Font_EnumerateWritingSystems);
    s_stubMap["Font/GetCharPixelWidth"] = reinterpret_cast<void*>(stub_Font_GetCharPixelWidth);
    s_stubMap["Font/GetFontAscent"] = reinterpret_cast<void*>(stub_Font_GetFontAscent);
    s_stubMap["Font/GetFontCharDefined"] = reinterpret_cast<void*>(stub_Font_GetFontCharDefined);
    s_stubMap["Font/GetFontDescent"] = reinterpret_cast<void*>(stub_Font_GetFontDescent);
    s_stubMap["Font/GetFontExactMatch"] = reinterpret_cast<void*>(stub_Font_GetFontExactMatch);
    s_stubMap["Font/GetFontFace"] = reinterpret_cast<void*>(stub_Font_GetFontFace);
    s_stubMap["Font/GetFontFixedPitch"] = reinterpret_cast<void*>(stub_Font_GetFontFixedPitch);
    s_stubMap["Font/GetFontHeight"] = reinterpret_cast<void*>(stub_Font_GetFontHeight);
    s_stubMap["Font/GetFontItalic"] = reinterpret_cast<void*>(stub_Font_GetFontItalic);
    s_stubMap["Font/GetFontKerning"] = reinterpret_cast<void*>(stub_Font_GetFontKerning);
    s_stubMap["Font/GetFontLineSpacing"] = reinterpret_cast<void*>(stub_Font_GetFontLineSpacing);
    s_stubMap["Font/GetFontMaxWidth"] = reinterpret_cast<void*>(stub_Font_GetFontMaxWidth);
    s_stubMap["Font/GetFontOverline"] = reinterpret_cast<void*>(stub_Font_GetFontOverline);
    s_stubMap["Font/GetFontPixelSize"] = reinterpret_cast<void*>(stub_Font_GetFontPixelSize);
    s_stubMap["Font/GetFontPointSize"] = reinterpret_cast<void*>(stub_Font_GetFontPointSize);
    s_stubMap["Font/GetFontScalable"] = reinterpret_cast<void*>(stub_Font_GetFontScalable);
    s_stubMap["Font/GetFontStretchFactor"] = reinterpret_cast<void*>(stub_Font_GetFontStretchFactor);
    s_stubMap["Font/GetFontStrikeOut"] = reinterpret_cast<void*>(stub_Font_GetFontStrikeOut);
    s_stubMap["Font/GetFontUnderline"] = reinterpret_cast<void*>(stub_Font_GetFontUnderline);
    s_stubMap["Font/GetFontWeight"] = reinterpret_cast<void*>(stub_Font_GetFontWeight);
    s_stubMap["Font/GetNominalFontFixedPitch"] = reinterpret_cast<void*>(stub_Font_GetNominalFontFixedPitch);
    s_stubMap["Font/GetNominalFontItalic"] = reinterpret_cast<void*>(stub_Font_GetNominalFontItalic);
    s_stubMap["Font/GetNominalFontWeight"] = reinterpret_cast<void*>(stub_Font_GetNominalFontWeight);
    s_stubMap["Font/GetStringPixelRect"] = reinterpret_cast<void*>(stub_Font_GetStringPixelRect);
    s_stubMap["Font/GetStringPixelWidth"] = reinterpret_cast<void*>(stub_Font_GetStringPixelWidth);
    s_stubMap["Font/SetFontFace"] = reinterpret_cast<void*>(stub_Font_SetFontFace);
    s_stubMap["Font/SetFontFixedPitch"] = reinterpret_cast<void*>(stub_Font_SetFontFixedPitch);
    s_stubMap["Font/SetFontItalic"] = reinterpret_cast<void*>(stub_Font_SetFontItalic);
    s_stubMap["Font/SetFontKerning"] = reinterpret_cast<void*>(stub_Font_SetFontKerning);
    s_stubMap["Font/SetFontOverline"] = reinterpret_cast<void*>(stub_Font_SetFontOverline);
    s_stubMap["Font/SetFontPixelSize"] = reinterpret_cast<void*>(stub_Font_SetFontPixelSize);
    s_stubMap["Font/SetFontPointSize"] = reinterpret_cast<void*>(stub_Font_SetFontPointSize);
    s_stubMap["Font/SetFontStretchFactor"] = reinterpret_cast<void*>(stub_Font_SetFontStretchFactor);
    s_stubMap["Font/SetFontStrikeOut"] = reinterpret_cast<void*>(stub_Font_SetFontStrikeOut);
    s_stubMap["Font/SetFontUnderline"] = reinterpret_cast<void*>(stub_Font_SetFontUnderline);
    s_stubMap["Font/SetFontWeight"] = reinterpret_cast<void*>(stub_Font_SetFontWeight);
    s_stubMap["Frame/CreateFrame"] = reinterpret_cast<void*>(stub_Frame_CreateFrame);
    s_stubMap["Frame/GetFrameBorderWidth"] = reinterpret_cast<void*>(stub_Frame_GetFrameBorderWidth);
    s_stubMap["Frame/GetFrameLineWidth"] = reinterpret_cast<void*>(stub_Frame_GetFrameLineWidth);
    s_stubMap["Frame/GetFrameStyle"] = reinterpret_cast<void*>(stub_Frame_GetFrameStyle);
    s_stubMap["Frame/SetFrameLineWidth"] = reinterpret_cast<void*>(stub_Frame_SetFrameLineWidth);
    s_stubMap["Frame/SetFrameStyle"] = reinterpret_cast<void*>(stub_Frame_SetFrameStyle);
    s_stubMap["GPU/EnumerateCUDADevices"] = reinterpret_cast<void*>(stub_GPU_EnumerateCUDADevices);
    s_stubMap["GPU/GetCUDADeviceMaxThreadsPerBlock"] = reinterpret_cast<void*>(stub_GPU_GetCUDADeviceMaxThreadsPerBlock);
    s_stubMap["GPU/GetCUDADeviceProperties"] = reinterpret_cast<void*>(stub_GPU_GetCUDADeviceProperties);
    s_stubMap["GPU/GetCUDADeviceSharedMemoryPerBlock"] = reinterpret_cast<void*>(stub_GPU_GetCUDADeviceSharedMemoryPerBlock);
    s_stubMap["GPU/GetCUDADeviceTotalGlobalMem"] = reinterpret_cast<void*>(stub_GPU_GetCUDADeviceTotalGlobalMem);
    s_stubMap["GPU/GetCUDASelectedDevice"] = reinterpret_cast<void*>(stub_GPU_GetCUDASelectedDevice);
    s_stubMap["GPU/InitCUDARuntime"] = reinterpret_cast<void*>(stub_GPU_InitCUDARuntime);
    s_stubMap["GPU/IsCUDADeviceAvailable"] = reinterpret_cast<void*>(stub_GPU_IsCUDADeviceAvailable);
    s_stubMap["Global/Abort"] = reinterpret_cast<void*>(stub_Global_Abort);
    s_stubMap["Global/Allocate"] = reinterpret_cast<void*>(stub_Global_Allocate);
    s_stubMap["Global/ApplicationInstanceSlot"] = reinterpret_cast<void*>(stub_Global_ApplicationInstanceSlot);
    s_stubMap["Global/BroadcastGlobalFiltersUpdated"] = reinterpret_cast<void*>(stub_Global_BroadcastGlobalFiltersUpdated);
    s_stubMap["Global/BroadcastImageUpdated"] = reinterpret_cast<void*>(stub_Global_BroadcastImageUpdated);
    s_stubMap["Global/BrowseProcessDocumentation"] = reinterpret_cast<void*>(stub_Global_BrowseProcessDocumentation);
    s_stubMap["Global/CancelGlobalSettingsUpdate"] = reinterpret_cast<void*>(stub_Global_CancelGlobalSettingsUpdate);
    s_stubMap["Global/ClearError"] = reinterpret_cast<void*>(stub_Global_ClearError);
    s_stubMap["Global/Deallocate"] = reinterpret_cast<void*>(stub_Global_Deallocate);
    s_stubMap["Global/DeleteSettingsItem"] = reinterpret_cast<void*>(stub_Global_DeleteSettingsItem);
    s_stubMap["Global/DisableAbort"] = reinterpret_cast<void*>(stub_Global_DisableAbort);
    s_stubMap["Global/EnableAbort"] = reinterpret_cast<void*>(stub_Global_EnableAbort);
    s_stubMap["Global/EnterGlobalSettingsUpdateContext"] = reinterpret_cast<void*>(stub_Global_EnterGlobalSettingsUpdateContext);
    s_stubMap["Global/ErrorMessage"] = reinterpret_cast<void*>(stub_Global_ErrorMessage);
    s_stubMap["Global/ExecuteCommand"] = reinterpret_cast<void*>(stub_Global_ExecuteCommand);
    s_stubMap["Global/ExitGlobalSettingsUpdateContext"] = reinterpret_cast<void*>(stub_Global_ExitGlobalSettingsUpdateContext);
    s_stubMap["Global/FlushConsole"] = reinterpret_cast<void*>(stub_Global_FlushConsole);
    s_stubMap["Global/GetApplicationConfigurationDirectory"] = reinterpret_cast<void*>(stub_Global_GetApplicationConfigurationDirectory);
    s_stubMap["Global/GetConsole"] = reinterpret_cast<void*>(stub_Global_GetConsole);
    s_stubMap["Global/GetConsoleText"] = reinterpret_cast<void*>(stub_Global_GetConsoleText);
    s_stubMap["Global/GetCursorPosition"] = reinterpret_cast<void*>(stub_Global_GetCursorPosition);
    s_stubMap["Global/GetGlobalColor"] = reinterpret_cast<void*>(stub_Global_GetGlobalColor);
    s_stubMap["Global/GetGlobalFlag"] = reinterpret_cast<void*>(stub_Global_GetGlobalFlag);
    s_stubMap["Global/GetGlobalFont"] = reinterpret_cast<void*>(stub_Global_GetGlobalFont);
    s_stubMap["Global/GetGlobalInteger"] = reinterpret_cast<void*>(stub_Global_GetGlobalInteger);
    s_stubMap["Global/GetGlobalReal"] = reinterpret_cast<void*>(stub_Global_GetGlobalReal);
    s_stubMap["Global/GetGlobalString"] = reinterpret_cast<void*>(stub_Global_GetGlobalString);
    s_stubMap["Global/GetKeyboardModifiers"] = reinterpret_cast<void*>(stub_Global_GetKeyboardModifiers);
    s_stubMap["Global/GetLastScriptResult"] = reinterpret_cast<void*>(stub_Global_GetLastScriptResult);
    s_stubMap["Global/GetPixInsightCodename"] = reinterpret_cast<void*>(stub_Global_GetPixInsightCodename);
    s_stubMap["Global/GetPixInsightVersion"] = reinterpret_cast<void*>(stub_Global_GetPixInsightVersion);
    s_stubMap["Global/GetPixelTraitsLUT"] = reinterpret_cast<void*>(stub_Global_GetPixelTraitsLUT);
    s_stubMap["Global/GetProcessStatus"] = reinterpret_cast<void*>(stub_Global_GetProcessStatus);
    s_stubMap["Global/GetProfilesDirectory"] = reinterpret_cast<void*>(stub_Global_GetProfilesDirectory);
    s_stubMap["Global/GetReadoutOptions"] = reinterpret_cast<void*>(stub_Global_GetReadoutOptions);
    s_stubMap["Global/GetSettingsItemGlobalAccess"] = reinterpret_cast<void*>(stub_Global_GetSettingsItemGlobalAccess);
    s_stubMap["Global/GetThreadWindowId"] = reinterpret_cast<void*>(stub_Global_GetThreadWindowId);
    s_stubMap["Global/GetToolTipWindowText"] = reinterpret_cast<void*>(stub_Global_GetToolTipWindowText);
    s_stubMap["Global/HideToolTipWindow"] = reinterpret_cast<void*>(stub_Global_HideToolTipWindow);
    s_stubMap["Global/IsGlobalSettingsUpdateContextActive"] = reinterpret_cast<void*>(stub_Global_IsGlobalSettingsUpdateContextActive);
    s_stubMap["Global/IsRealTimePreviewUpdating"] = reinterpret_cast<void*>(stub_Global_IsRealTimePreviewUpdating);
    s_stubMap["Global/LastError"] = reinterpret_cast<void*>(stub_Global_LastError);
    s_stubMap["Global/LaunchProcessInstance"] = reinterpret_cast<void*>(stub_Global_LaunchProcessInstance);
    s_stubMap["Global/LaunchProcessInstanceOnView"] = reinterpret_cast<void*>(stub_Global_LaunchProcessInstanceOnView);
    s_stubMap["Global/LaunchProcessInterface"] = reinterpret_cast<void*>(stub_Global_LaunchProcessInterface);
    s_stubMap["Global/MaxProcessorsAllowedForModule"] = reinterpret_cast<void*>(stub_Global_MaxProcessorsAllowedForModule);
    s_stubMap["Global/MessageBox"] = reinterpret_cast<void*>(stub_Global_MessageBox);
    s_stubMap["Global/ProcessEvents"] = reinterpret_cast<void*>(stub_Global_ProcessEvents);
    s_stubMap["Global/ReadConsoleChar"] = reinterpret_cast<void*>(stub_Global_ReadConsoleChar);
    s_stubMap["Global/ReadConsoleString"] = reinterpret_cast<void*>(stub_Global_ReadConsoleString);
    s_stubMap["Global/ReadSettingsBlock"] = reinterpret_cast<void*>(stub_Global_ReadSettingsBlock);
    s_stubMap["Global/ReadSettingsFlag"] = reinterpret_cast<void*>(stub_Global_ReadSettingsFlag);
    s_stubMap["Global/ReadSettingsInteger"] = reinterpret_cast<void*>(stub_Global_ReadSettingsInteger);
    s_stubMap["Global/ReadSettingsReal"] = reinterpret_cast<void*>(stub_Global_ReadSettingsReal);
    s_stubMap["Global/ReadSettingsString"] = reinterpret_cast<void*>(stub_Global_ReadSettingsString);
    s_stubMap["Global/ReadSettingsUnsignedInteger"] = reinterpret_cast<void*>(stub_Global_ReadSettingsUnsignedInteger);
    s_stubMap["Global/ResetProcessStatus"] = reinterpret_cast<void*>(stub_Global_ResetProcessStatus);
    s_stubMap["Global/SetCursorPosition"] = reinterpret_cast<void*>(stub_Global_SetCursorPosition);
    s_stubMap["Global/SetGlobalColor"] = reinterpret_cast<void*>(stub_Global_SetGlobalColor);
    s_stubMap["Global/SetGlobalFlag"] = reinterpret_cast<void*>(stub_Global_SetGlobalFlag);
    s_stubMap["Global/SetGlobalFont"] = reinterpret_cast<void*>(stub_Global_SetGlobalFont);
    s_stubMap["Global/SetGlobalInteger"] = reinterpret_cast<void*>(stub_Global_SetGlobalInteger);
    s_stubMap["Global/SetGlobalReal"] = reinterpret_cast<void*>(stub_Global_SetGlobalReal);
    s_stubMap["Global/SetGlobalString"] = reinterpret_cast<void*>(stub_Global_SetGlobalString);
    s_stubMap["Global/SetReadoutOptions"] = reinterpret_cast<void*>(stub_Global_SetReadoutOptions);
    s_stubMap["Global/SetRealTimePreviewOwner"] = reinterpret_cast<void*>(stub_Global_SetRealTimePreviewOwner);
    s_stubMap["Global/SetSettingsItemGlobalAccess"] = reinterpret_cast<void*>(stub_Global_SetSettingsItemGlobalAccess);
    s_stubMap["Global/ShowConsole"] = reinterpret_cast<void*>(stub_Global_ShowConsole);
    s_stubMap["Global/ShowToolTipWindow"] = reinterpret_cast<void*>(stub_Global_ShowToolTipWindow);
    s_stubMap["Global/UpdateRealTimePreview"] = reinterpret_cast<void*>(stub_Global_UpdateRealTimePreview);
    s_stubMap["Global/ValidateConsole"] = reinterpret_cast<void*>(stub_Global_ValidateConsole);
    s_stubMap["Global/WriteConsole"] = reinterpret_cast<void*>(stub_Global_WriteConsole);
    s_stubMap["Global/WriteSettingsBlock"] = reinterpret_cast<void*>(stub_Global_WriteSettingsBlock);
    s_stubMap["Global/WriteSettingsFlag"] = reinterpret_cast<void*>(stub_Global_WriteSettingsFlag);
    s_stubMap["Global/WriteSettingsInteger"] = reinterpret_cast<void*>(stub_Global_WriteSettingsInteger);
    s_stubMap["Global/WriteSettingsReal"] = reinterpret_cast<void*>(stub_Global_WriteSettingsReal);
    s_stubMap["Global/WriteSettingsString"] = reinterpret_cast<void*>(stub_Global_WriteSettingsString);
    s_stubMap["Global/WriteSettingsUnsignedInteger"] = reinterpret_cast<void*>(stub_Global_WriteSettingsUnsignedInteger);
    s_stubMap["Graphics/BeginBitmapPaint"] = reinterpret_cast<void*>(stub_Graphics_BeginBitmapPaint);
    s_stubMap["Graphics/BeginControlPaint"] = reinterpret_cast<void*>(stub_Graphics_BeginControlPaint);
    s_stubMap["Graphics/BeginSVGPaint"] = reinterpret_cast<void*>(stub_Graphics_BeginSVGPaint);
    s_stubMap["Graphics/CreateGraphics"] = reinterpret_cast<void*>(stub_Graphics_CreateGraphics);
    s_stubMap["Graphics/DrawArc"] = reinterpret_cast<void*>(stub_Graphics_DrawArc);
    s_stubMap["Graphics/DrawArcD"] = reinterpret_cast<void*>(stub_Graphics_DrawArcD);
    s_stubMap["Graphics/DrawBitmap"] = reinterpret_cast<void*>(stub_Graphics_DrawBitmap);
    s_stubMap["Graphics/DrawBitmapD"] = reinterpret_cast<void*>(stub_Graphics_DrawBitmapD);
    s_stubMap["Graphics/DrawBitmapRect"] = reinterpret_cast<void*>(stub_Graphics_DrawBitmapRect);
    s_stubMap["Graphics/DrawBitmapRectD"] = reinterpret_cast<void*>(stub_Graphics_DrawBitmapRectD);
    s_stubMap["Graphics/DrawChord"] = reinterpret_cast<void*>(stub_Graphics_DrawChord);
    s_stubMap["Graphics/DrawChordD"] = reinterpret_cast<void*>(stub_Graphics_DrawChordD);
    s_stubMap["Graphics/DrawEllipse"] = reinterpret_cast<void*>(stub_Graphics_DrawEllipse);
    s_stubMap["Graphics/DrawEllipseD"] = reinterpret_cast<void*>(stub_Graphics_DrawEllipseD);
    s_stubMap["Graphics/DrawLine"] = reinterpret_cast<void*>(stub_Graphics_DrawLine);
    s_stubMap["Graphics/DrawLineD"] = reinterpret_cast<void*>(stub_Graphics_DrawLineD);
    s_stubMap["Graphics/DrawPie"] = reinterpret_cast<void*>(stub_Graphics_DrawPie);
    s_stubMap["Graphics/DrawPieD"] = reinterpret_cast<void*>(stub_Graphics_DrawPieD);
    s_stubMap["Graphics/DrawPoint"] = reinterpret_cast<void*>(stub_Graphics_DrawPoint);
    s_stubMap["Graphics/DrawPointD"] = reinterpret_cast<void*>(stub_Graphics_DrawPointD);
    s_stubMap["Graphics/DrawPolygon"] = reinterpret_cast<void*>(stub_Graphics_DrawPolygon);
    s_stubMap["Graphics/DrawPolygonD"] = reinterpret_cast<void*>(stub_Graphics_DrawPolygonD);
    s_stubMap["Graphics/DrawPolyline"] = reinterpret_cast<void*>(stub_Graphics_DrawPolyline);
    s_stubMap["Graphics/DrawPolylineD"] = reinterpret_cast<void*>(stub_Graphics_DrawPolylineD);
    s_stubMap["Graphics/DrawRect"] = reinterpret_cast<void*>(stub_Graphics_DrawRect);
    s_stubMap["Graphics/DrawRectD"] = reinterpret_cast<void*>(stub_Graphics_DrawRectD);
    s_stubMap["Graphics/DrawRoundedRect"] = reinterpret_cast<void*>(stub_Graphics_DrawRoundedRect);
    s_stubMap["Graphics/DrawRoundedRectD"] = reinterpret_cast<void*>(stub_Graphics_DrawRoundedRectD);
    s_stubMap["Graphics/DrawScaledBitmap"] = reinterpret_cast<void*>(stub_Graphics_DrawScaledBitmap);
    s_stubMap["Graphics/DrawScaledBitmapD"] = reinterpret_cast<void*>(stub_Graphics_DrawScaledBitmapD);
    s_stubMap["Graphics/DrawScaledBitmapRect"] = reinterpret_cast<void*>(stub_Graphics_DrawScaledBitmapRect);
    s_stubMap["Graphics/DrawScaledBitmapRectD"] = reinterpret_cast<void*>(stub_Graphics_DrawScaledBitmapRectD);
    s_stubMap["Graphics/DrawText"] = reinterpret_cast<void*>(stub_Graphics_DrawText);
    s_stubMap["Graphics/DrawTextD"] = reinterpret_cast<void*>(stub_Graphics_DrawTextD);
    s_stubMap["Graphics/DrawTextRect"] = reinterpret_cast<void*>(stub_Graphics_DrawTextRect);
    s_stubMap["Graphics/DrawTextRectD"] = reinterpret_cast<void*>(stub_Graphics_DrawTextRectD);
    s_stubMap["Graphics/DrawTiledBitmap"] = reinterpret_cast<void*>(stub_Graphics_DrawTiledBitmap);
    s_stubMap["Graphics/DrawTiledBitmapD"] = reinterpret_cast<void*>(stub_Graphics_DrawTiledBitmapD);
    s_stubMap["Graphics/EnableGraphicsAntialiasing"] = reinterpret_cast<void*>(stub_Graphics_EnableGraphicsAntialiasing);
    s_stubMap["Graphics/EnableGraphicsClipping"] = reinterpret_cast<void*>(stub_Graphics_EnableGraphicsClipping);
    s_stubMap["Graphics/EnableGraphicsSmoothInterpolation"] = reinterpret_cast<void*>(stub_Graphics_EnableGraphicsSmoothInterpolation);
    s_stubMap["Graphics/EnableGraphicsTextAntialiasing"] = reinterpret_cast<void*>(stub_Graphics_EnableGraphicsTextAntialiasing);
    s_stubMap["Graphics/EnableGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_EnableGraphicsTransformation);
    s_stubMap["Graphics/EndPaint"] = reinterpret_cast<void*>(stub_Graphics_EndPaint);
    s_stubMap["Graphics/FillChord"] = reinterpret_cast<void*>(stub_Graphics_FillChord);
    s_stubMap["Graphics/FillChordD"] = reinterpret_cast<void*>(stub_Graphics_FillChordD);
    s_stubMap["Graphics/FillEllipse"] = reinterpret_cast<void*>(stub_Graphics_FillEllipse);
    s_stubMap["Graphics/FillEllipseD"] = reinterpret_cast<void*>(stub_Graphics_FillEllipseD);
    s_stubMap["Graphics/FillPie"] = reinterpret_cast<void*>(stub_Graphics_FillPie);
    s_stubMap["Graphics/FillPieD"] = reinterpret_cast<void*>(stub_Graphics_FillPieD);
    s_stubMap["Graphics/FillPolygon"] = reinterpret_cast<void*>(stub_Graphics_FillPolygon);
    s_stubMap["Graphics/FillPolygonD"] = reinterpret_cast<void*>(stub_Graphics_FillPolygonD);
    s_stubMap["Graphics/FillRect"] = reinterpret_cast<void*>(stub_Graphics_FillRect);
    s_stubMap["Graphics/FillRectD"] = reinterpret_cast<void*>(stub_Graphics_FillRectD);
    s_stubMap["Graphics/FillRoundedRect"] = reinterpret_cast<void*>(stub_Graphics_FillRoundedRect);
    s_stubMap["Graphics/FillRoundedRectD"] = reinterpret_cast<void*>(stub_Graphics_FillRoundedRectD);
    s_stubMap["Graphics/GetGraphicsAntialiasingEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsAntialiasingEnabled);
    s_stubMap["Graphics/GetGraphicsBackgroundBrush"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsBackgroundBrush);
    s_stubMap["Graphics/GetGraphicsBrush"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsBrush);
    s_stubMap["Graphics/GetGraphicsBrushOrigin"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsBrushOrigin);
    s_stubMap["Graphics/GetGraphicsBrushOriginD"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsBrushOriginD);
    s_stubMap["Graphics/GetGraphicsClipRect"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsClipRect);
    s_stubMap["Graphics/GetGraphicsClipRectD"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsClipRectD);
    s_stubMap["Graphics/GetGraphicsClippingEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsClippingEnabled);
    s_stubMap["Graphics/GetGraphicsCompositionOperator"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsCompositionOperator);
    s_stubMap["Graphics/GetGraphicsFont"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsFont);
    s_stubMap["Graphics/GetGraphicsOpacity"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsOpacity);
    s_stubMap["Graphics/GetGraphicsPen"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsPen);
    s_stubMap["Graphics/GetGraphicsSmoothInterpolationEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsSmoothInterpolationEnabled);
    s_stubMap["Graphics/GetGraphicsStatus"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsStatus);
    s_stubMap["Graphics/GetGraphicsTextAntialiasingEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsTextAntialiasingEnabled);
    s_stubMap["Graphics/GetGraphicsTransformationEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsTransformationEnabled);
    s_stubMap["Graphics/GetGraphicsTransformationMatrix"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsTransformationMatrix);
    s_stubMap["Graphics/GetGraphicsTransparentBackgroundEnabled"] = reinterpret_cast<void*>(stub_Graphics_GetGraphicsTransparentBackgroundEnabled);
    s_stubMap["Graphics/GetTextRect"] = reinterpret_cast<void*>(stub_Graphics_GetTextRect);
    s_stubMap["Graphics/GetTextRectD"] = reinterpret_cast<void*>(stub_Graphics_GetTextRectD);
    s_stubMap["Graphics/MultiplyGraphicsTransformationMatrix"] = reinterpret_cast<void*>(stub_Graphics_MultiplyGraphicsTransformationMatrix);
    s_stubMap["Graphics/PopGraphicsState"] = reinterpret_cast<void*>(stub_Graphics_PopGraphicsState);
    s_stubMap["Graphics/PushGraphicsState"] = reinterpret_cast<void*>(stub_Graphics_PushGraphicsState);
    s_stubMap["Graphics/ResetGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_ResetGraphicsTransformation);
    s_stubMap["Graphics/RotateGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_RotateGraphicsTransformation);
    s_stubMap["Graphics/ScaleGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_ScaleGraphicsTransformation);
    s_stubMap["Graphics/SetGraphicsBackgroundBrush"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsBackgroundBrush);
    s_stubMap["Graphics/SetGraphicsBrush"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsBrush);
    s_stubMap["Graphics/SetGraphicsBrushOrigin"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsBrushOrigin);
    s_stubMap["Graphics/SetGraphicsBrushOriginD"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsBrushOriginD);
    s_stubMap["Graphics/SetGraphicsClipRect"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsClipRect);
    s_stubMap["Graphics/SetGraphicsClipRectD"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsClipRectD);
    s_stubMap["Graphics/SetGraphicsCompositionOperator"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsCompositionOperator);
    s_stubMap["Graphics/SetGraphicsFont"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsFont);
    s_stubMap["Graphics/SetGraphicsOpacity"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsOpacity);
    s_stubMap["Graphics/SetGraphicsPen"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsPen);
    s_stubMap["Graphics/SetGraphicsTransformationMatrix"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsTransformationMatrix);
    s_stubMap["Graphics/SetGraphicsTransparentBackground"] = reinterpret_cast<void*>(stub_Graphics_SetGraphicsTransparentBackground);
    s_stubMap["Graphics/ShearGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_ShearGraphicsTransformation);
    s_stubMap["Graphics/StrokeChord"] = reinterpret_cast<void*>(stub_Graphics_StrokeChord);
    s_stubMap["Graphics/StrokeChordD"] = reinterpret_cast<void*>(stub_Graphics_StrokeChordD);
    s_stubMap["Graphics/StrokeEllipse"] = reinterpret_cast<void*>(stub_Graphics_StrokeEllipse);
    s_stubMap["Graphics/StrokeEllipseD"] = reinterpret_cast<void*>(stub_Graphics_StrokeEllipseD);
    s_stubMap["Graphics/StrokePie"] = reinterpret_cast<void*>(stub_Graphics_StrokePie);
    s_stubMap["Graphics/StrokePieD"] = reinterpret_cast<void*>(stub_Graphics_StrokePieD);
    s_stubMap["Graphics/StrokePolygon"] = reinterpret_cast<void*>(stub_Graphics_StrokePolygon);
    s_stubMap["Graphics/StrokePolygonD"] = reinterpret_cast<void*>(stub_Graphics_StrokePolygonD);
    s_stubMap["Graphics/StrokeRect"] = reinterpret_cast<void*>(stub_Graphics_StrokeRect);
    s_stubMap["Graphics/StrokeRectD"] = reinterpret_cast<void*>(stub_Graphics_StrokeRectD);
    s_stubMap["Graphics/StrokeRoundedRect"] = reinterpret_cast<void*>(stub_Graphics_StrokeRoundedRect);
    s_stubMap["Graphics/StrokeRoundedRectD"] = reinterpret_cast<void*>(stub_Graphics_StrokeRoundedRectD);
    s_stubMap["Graphics/TransformPoints"] = reinterpret_cast<void*>(stub_Graphics_TransformPoints);
    s_stubMap["Graphics/TranslateGraphicsTransformation"] = reinterpret_cast<void*>(stub_Graphics_TranslateGraphicsTransformation);
    s_stubMap["GroupBox/CreateGroupBox"] = reinterpret_cast<void*>(stub_GroupBox_CreateGroupBox);
    s_stubMap["GroupBox/GetGroupBoxCheckable"] = reinterpret_cast<void*>(stub_GroupBox_GetGroupBoxCheckable);
    s_stubMap["GroupBox/GetGroupBoxChecked"] = reinterpret_cast<void*>(stub_GroupBox_GetGroupBoxChecked);
    s_stubMap["GroupBox/GetGroupBoxTitle"] = reinterpret_cast<void*>(stub_GroupBox_GetGroupBoxTitle);
    s_stubMap["GroupBox/SetGroupBoxCheckEventRoutine"] = reinterpret_cast<void*>(stub_GroupBox_SetGroupBoxCheckEventRoutine);
    s_stubMap["GroupBox/SetGroupBoxCheckable"] = reinterpret_cast<void*>(stub_GroupBox_SetGroupBoxCheckable);
    s_stubMap["GroupBox/SetGroupBoxChecked"] = reinterpret_cast<void*>(stub_GroupBox_SetGroupBoxChecked);
    s_stubMap["GroupBox/SetGroupBoxTitle"] = reinterpret_cast<void*>(stub_GroupBox_SetGroupBoxTitle);
    s_stubMap["ImageView/BeginViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_BeginViewportSelection);
    s_stubMap["ImageView/CancelViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_CancelViewportSelection);
    s_stubMap["ImageView/CommitViewportUpdates"] = reinterpret_cast<void*>(stub_ImageView_CommitViewportUpdates);
    s_stubMap["ImageView/CreateImageView"] = reinterpret_cast<void*>(stub_ImageView_CreateImageView);
    s_stubMap["ImageView/CreateImageViewViewport"] = reinterpret_cast<void*>(stub_ImageView_CreateImageViewViewport);
    s_stubMap["ImageView/DeleteImageViewICCProfile"] = reinterpret_cast<void*>(stub_ImageView_DeleteImageViewICCProfile);
    s_stubMap["ImageView/EndViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_EndViewportSelection);
    s_stubMap["ImageView/GetImageViewCMEnabled"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewCMEnabled);
    s_stubMap["ImageView/GetImageViewDisplayChannel"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewDisplayChannel);
    s_stubMap["ImageView/GetImageViewICCProfile"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewICCProfile);
    s_stubMap["ImageView/GetImageViewICCProfileLength"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewICCProfileLength);
    s_stubMap["ImageView/GetImageViewImage"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewImage);
    s_stubMap["ImageView/GetImageViewImageGeometry"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewImageGeometry);
    s_stubMap["ImageView/GetImageViewMode"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewMode);
    s_stubMap["ImageView/GetImageViewRGBWS"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewRGBWS);
    s_stubMap["ImageView/GetImageViewSampleFormat"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewSampleFormat);
    s_stubMap["ImageView/GetImageViewTransparencyMode"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewTransparencyMode);
    s_stubMap["ImageView/GetImageViewViewportOrigin"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewViewportOrigin);
    s_stubMap["ImageView/GetImageViewViewportPosition"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewViewportPosition);
    s_stubMap["ImageView/GetImageViewViewportSize"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewViewportSize);
    s_stubMap["ImageView/GetImageViewVisibleViewportRect"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewVisibleViewportRect);
    s_stubMap["ImageView/GetImageViewZoomFactor"] = reinterpret_cast<void*>(stub_ImageView_GetImageViewZoomFactor);
    s_stubMap["ImageView/GetViewportBitmap"] = reinterpret_cast<void*>(stub_ImageView_GetViewportBitmap);
    s_stubMap["ImageView/GetViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_GetViewportSelection);
    s_stubMap["ImageView/GetViewportUpdateRect"] = reinterpret_cast<void*>(stub_ImageView_GetViewportUpdateRect);
    s_stubMap["ImageView/GlobalToViewport"] = reinterpret_cast<void*>(stub_ImageView_GlobalToViewport);
    s_stubMap["ImageView/ImageScalarToViewport"] = reinterpret_cast<void*>(stub_ImageView_ImageScalarToViewport);
    s_stubMap["ImageView/ImageScalarToViewportArray"] = reinterpret_cast<void*>(stub_ImageView_ImageScalarToViewportArray);
    s_stubMap["ImageView/ImageScalarToViewportArrayD"] = reinterpret_cast<void*>(stub_ImageView_ImageScalarToViewportArrayD);
    s_stubMap["ImageView/ImageScalarToViewportD"] = reinterpret_cast<void*>(stub_ImageView_ImageScalarToViewportD);
    s_stubMap["ImageView/ImageToViewport"] = reinterpret_cast<void*>(stub_ImageView_ImageToViewport);
    s_stubMap["ImageView/ImageToViewportArray"] = reinterpret_cast<void*>(stub_ImageView_ImageToViewportArray);
    s_stubMap["ImageView/ImageToViewportArrayD"] = reinterpret_cast<void*>(stub_ImageView_ImageToViewportArrayD);
    s_stubMap["ImageView/ImageToViewportD"] = reinterpret_cast<void*>(stub_ImageView_ImageToViewportD);
    s_stubMap["ImageView/IsImageViewColorImage"] = reinterpret_cast<void*>(stub_ImageView_IsImageViewColorImage);
    s_stubMap["ImageView/LoadImageViewICCProfile"] = reinterpret_cast<void*>(stub_ImageView_LoadImageViewICCProfile);
    s_stubMap["ImageView/ModifyViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_ModifyViewportSelection);
    s_stubMap["ImageView/RegenerateImageRect"] = reinterpret_cast<void*>(stub_ImageView_RegenerateImageRect);
    s_stubMap["ImageView/RegenerateImageViewViewport"] = reinterpret_cast<void*>(stub_ImageView_RegenerateImageViewViewport);
    s_stubMap["ImageView/RegenerateViewportRect"] = reinterpret_cast<void*>(stub_ImageView_RegenerateViewportRect);
    s_stubMap["ImageView/SetImageViewCMEnabled"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewCMEnabled);
    s_stubMap["ImageView/SetImageViewDisplayChannel"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewDisplayChannel);
    s_stubMap["ImageView/SetImageViewICCProfile"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewICCProfile);
    s_stubMap["ImageView/SetImageViewMode"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewMode);
    s_stubMap["ImageView/SetImageViewRGBWS"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewRGBWS);
    s_stubMap["ImageView/SetImageViewSampleFormat"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewSampleFormat);
    s_stubMap["ImageView/SetImageViewScrollEventRoutine"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewScrollEventRoutine);
    s_stubMap["ImageView/SetImageViewTransparencyMode"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewTransparencyMode);
    s_stubMap["ImageView/SetImageViewViewport"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewViewport);
    s_stubMap["ImageView/SetImageViewViewportPosition"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewViewportPosition);
    s_stubMap["ImageView/SetImageViewZoomFactor"] = reinterpret_cast<void*>(stub_ImageView_SetImageViewZoomFactor);
    s_stubMap["ImageView/UpdateImageRect"] = reinterpret_cast<void*>(stub_ImageView_UpdateImageRect);
    s_stubMap["ImageView/UpdateImageViewViewport"] = reinterpret_cast<void*>(stub_ImageView_UpdateImageViewViewport);
    s_stubMap["ImageView/UpdateViewportRect"] = reinterpret_cast<void*>(stub_ImageView_UpdateViewportRect);
    s_stubMap["ImageView/UpdateViewportSelection"] = reinterpret_cast<void*>(stub_ImageView_UpdateViewportSelection);
    s_stubMap["ImageView/ViewportScalarToImage"] = reinterpret_cast<void*>(stub_ImageView_ViewportScalarToImage);
    s_stubMap["ImageView/ViewportScalarToImageArray"] = reinterpret_cast<void*>(stub_ImageView_ViewportScalarToImageArray);
    s_stubMap["ImageView/ViewportScalarToImageArrayD"] = reinterpret_cast<void*>(stub_ImageView_ViewportScalarToImageArrayD);
    s_stubMap["ImageView/ViewportScalarToImageD"] = reinterpret_cast<void*>(stub_ImageView_ViewportScalarToImageD);
    s_stubMap["ImageView/ViewportToGlobal"] = reinterpret_cast<void*>(stub_ImageView_ViewportToGlobal);
    s_stubMap["ImageView/ViewportToImage"] = reinterpret_cast<void*>(stub_ImageView_ViewportToImage);
    s_stubMap["ImageView/ViewportToImageArray"] = reinterpret_cast<void*>(stub_ImageView_ViewportToImageArray);
    s_stubMap["ImageView/ViewportToImageArrayD"] = reinterpret_cast<void*>(stub_ImageView_ViewportToImageArrayD);
    s_stubMap["ImageView/ViewportToImageD"] = reinterpret_cast<void*>(stub_ImageView_ViewportToImageD);
    s_stubMap["ImageWindow/AddImageWindowKeyword"] = reinterpret_cast<void*>(stub_ImageWindow_AddImageWindowKeyword);
    s_stubMap["ImageWindow/BeginViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_BeginViewportSelection);
    s_stubMap["ImageWindow/BringImageWindowToFront"] = reinterpret_cast<void*>(stub_ImageWindow_BringImageWindowToFront);
    s_stubMap["ImageWindow/CancelViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_CancelViewportSelection);
    s_stubMap["ImageWindow/CelestialToImage"] = reinterpret_cast<void*>(stub_ImageWindow_CelestialToImage);
    s_stubMap["ImageWindow/ClearImageWindowAstrometricSolution"] = reinterpret_cast<void*>(stub_ImageWindow_ClearImageWindowAstrometricSolution);
    s_stubMap["ImageWindow/CloseImageWindow"] = reinterpret_cast<void*>(stub_ImageWindow_CloseImageWindow);
    s_stubMap["ImageWindow/CommitViewportUpdates"] = reinterpret_cast<void*>(stub_ImageWindow_CommitViewportUpdates);
    s_stubMap["ImageWindow/CopyImageWindowAstrometricSolution"] = reinterpret_cast<void*>(stub_ImageWindow_CopyImageWindowAstrometricSolution);
    s_stubMap["ImageWindow/CreateImageWindow"] = reinterpret_cast<void*>(stub_ImageWindow_CreateImageWindow);
    s_stubMap["ImageWindow/CreatePreview"] = reinterpret_cast<void*>(stub_ImageWindow_CreatePreview);
    s_stubMap["ImageWindow/DeleteImageWindowICCProfile"] = reinterpret_cast<void*>(stub_ImageWindow_DeleteImageWindowICCProfile);
    s_stubMap["ImageWindow/DeletePreview"] = reinterpret_cast<void*>(stub_ImageWindow_DeletePreview);
    s_stubMap["ImageWindow/DeletePreviews"] = reinterpret_cast<void*>(stub_ImageWindow_DeletePreviews);
    s_stubMap["ImageWindow/EndViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_EndViewportSelection);
    s_stubMap["ImageWindow/EnumerateImageWindows"] = reinterpret_cast<void*>(stub_ImageWindow_EnumerateImageWindows);
    s_stubMap["ImageWindow/EnumeratePreviews"] = reinterpret_cast<void*>(stub_ImageWindow_EnumeratePreviews);
    s_stubMap["ImageWindow/FitImageWindow"] = reinterpret_cast<void*>(stub_ImageWindow_FitImageWindow);
    s_stubMap["ImageWindow/GetActiveDynamicInterface"] = reinterpret_cast<void*>(stub_ImageWindow_GetActiveDynamicInterface);
    s_stubMap["ImageWindow/GetActiveImageWindow"] = reinterpret_cast<void*>(stub_ImageWindow_GetActiveImageWindow);
    s_stubMap["ImageWindow/GetCursorTolerance"] = reinterpret_cast<void*>(stub_ImageWindow_GetCursorTolerance);
    s_stubMap["ImageWindow/GetDefaultICCProfileEmbedding"] = reinterpret_cast<void*>(stub_ImageWindow_GetDefaultICCProfileEmbedding);
    s_stubMap["ImageWindow/GetDefaultPropertiesEmbedding"] = reinterpret_cast<void*>(stub_ImageWindow_GetDefaultPropertiesEmbedding);
    s_stubMap["ImageWindow/GetDefaultResolution"] = reinterpret_cast<void*>(stub_ImageWindow_GetDefaultResolution);
    s_stubMap["ImageWindow/GetDefaultThumbnailEmbedding"] = reinterpret_cast<void*>(stub_ImageWindow_GetDefaultThumbnailEmbedding);
    s_stubMap["ImageWindow/GetDynamicCursorBitmap"] = reinterpret_cast<void*>(stub_ImageWindow_GetDynamicCursorBitmap);
    s_stubMap["ImageWindow/GetDynamicCursorHotSpot"] = reinterpret_cast<void*>(stub_ImageWindow_GetDynamicCursorHotSpot);
    s_stubMap["ImageWindow/GetGlobalRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_GetGlobalRGBWS);
    s_stubMap["ImageWindow/GetImageType"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageType);
    s_stubMap["ImageWindow/GetImageWindowByFilePath"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowByFilePath);
    s_stubMap["ImageWindow/GetImageWindowById"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowById);
    s_stubMap["ImageWindow/GetImageWindowCMEnabled"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowCMEnabled);
    s_stubMap["ImageWindow/GetImageWindowCopyFlag"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowCopyFlag);
    s_stubMap["ImageWindow/GetImageWindowCurrentView"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowCurrentView);
    s_stubMap["ImageWindow/GetImageWindowDevicePixelRatio"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowDevicePixelRatio);
    s_stubMap["ImageWindow/GetImageWindowDisplayChannel"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowDisplayChannel);
    s_stubMap["ImageWindow/GetImageWindowDisplayPixelRatio"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowDisplayPixelRatio);
    s_stubMap["ImageWindow/GetImageWindowFileInfo"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowFileInfo);
    s_stubMap["ImageWindow/GetImageWindowFilePath"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowFilePath);
    s_stubMap["ImageWindow/GetImageWindowFileURL"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowFileURL);
    s_stubMap["ImageWindow/GetImageWindowGlobalRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowGlobalRGBWS);
    s_stubMap["ImageWindow/GetImageWindowHasAstrometricSolution"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowHasAstrometricSolution);
    s_stubMap["ImageWindow/GetImageWindowICCProfile"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowICCProfile);
    s_stubMap["ImageWindow/GetImageWindowICCProfileLength"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowICCProfileLength);
    s_stubMap["ImageWindow/GetImageWindowIconic"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowIconic);
    s_stubMap["ImageWindow/GetImageWindowKeyword"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowKeyword);
    s_stubMap["ImageWindow/GetImageWindowKeywordCount"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowKeywordCount);
    s_stubMap["ImageWindow/GetImageWindowMainView"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMainView);
    s_stubMap["ImageWindow/GetImageWindowMask"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMask);
    s_stubMap["ImageWindow/GetImageWindowMaskEnabled"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMaskEnabled);
    s_stubMap["ImageWindow/GetImageWindowMaskMode"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMaskMode);
    s_stubMap["ImageWindow/GetImageWindowMaskVisible"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMaskVisible);
    s_stubMap["ImageWindow/GetImageWindowMode"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowMode);
    s_stubMap["ImageWindow/GetImageWindowModifyCount"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowModifyCount);
    s_stubMap["ImageWindow/GetImageWindowNewFlag"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowNewFlag);
    s_stubMap["ImageWindow/GetImageWindowRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowRGBWS);
    s_stubMap["ImageWindow/GetImageWindowResolution"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowResolution);
    s_stubMap["ImageWindow/GetImageWindowResourcePixelRatio"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowResourcePixelRatio);
    s_stubMap["ImageWindow/GetImageWindowSampleFormat"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowSampleFormat);
    s_stubMap["ImageWindow/GetImageWindowTransparencyMode"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowTransparencyMode);
    s_stubMap["ImageWindow/GetImageWindowViewportOrigin"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowViewportOrigin);
    s_stubMap["ImageWindow/GetImageWindowViewportPosition"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowViewportPosition);
    s_stubMap["ImageWindow/GetImageWindowViewportSize"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowViewportSize);
    s_stubMap["ImageWindow/GetImageWindowVisible"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowVisible);
    s_stubMap["ImageWindow/GetImageWindowVisibleViewportRect"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowVisibleViewportRect);
    s_stubMap["ImageWindow/GetImageWindowZoomFactor"] = reinterpret_cast<void*>(stub_ImageWindow_GetImageWindowZoomFactor);
    s_stubMap["ImageWindow/GetMaskReferenceCount"] = reinterpret_cast<void*>(stub_ImageWindow_GetMaskReferenceCount);
    s_stubMap["ImageWindow/GetPreviewById"] = reinterpret_cast<void*>(stub_ImageWindow_GetPreviewById);
    s_stubMap["ImageWindow/GetPreviewCount"] = reinterpret_cast<void*>(stub_ImageWindow_GetPreviewCount);
    s_stubMap["ImageWindow/GetPreviewRect"] = reinterpret_cast<void*>(stub_ImageWindow_GetPreviewRect);
    s_stubMap["ImageWindow/GetSelectedPreview"] = reinterpret_cast<void*>(stub_ImageWindow_GetSelectedPreview);
    s_stubMap["ImageWindow/GetSwapDirectory"] = reinterpret_cast<void*>(stub_ImageWindow_GetSwapDirectory);
    s_stubMap["ImageWindow/GetTransparencyBackgroundBrush"] = reinterpret_cast<void*>(stub_ImageWindow_GetTransparencyBackgroundBrush);
    s_stubMap["ImageWindow/GetViewportBitmap"] = reinterpret_cast<void*>(stub_ImageWindow_GetViewportBitmap);
    s_stubMap["ImageWindow/GetViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_GetViewportSelection);
    s_stubMap["ImageWindow/GetViewportUpdateRect"] = reinterpret_cast<void*>(stub_ImageWindow_GetViewportUpdateRect);
    s_stubMap["ImageWindow/GlobalToViewport"] = reinterpret_cast<void*>(stub_ImageWindow_GlobalToViewport);
    s_stubMap["ImageWindow/ImageScalarToViewport"] = reinterpret_cast<void*>(stub_ImageWindow_ImageScalarToViewport);
    s_stubMap["ImageWindow/ImageScalarToViewportArray"] = reinterpret_cast<void*>(stub_ImageWindow_ImageScalarToViewportArray);
    s_stubMap["ImageWindow/ImageScalarToViewportArrayD"] = reinterpret_cast<void*>(stub_ImageWindow_ImageScalarToViewportArrayD);
    s_stubMap["ImageWindow/ImageScalarToViewportD"] = reinterpret_cast<void*>(stub_ImageWindow_ImageScalarToViewportD);
    s_stubMap["ImageWindow/ImageToCelestial"] = reinterpret_cast<void*>(stub_ImageWindow_ImageToCelestial);
    s_stubMap["ImageWindow/ImageToViewport"] = reinterpret_cast<void*>(stub_ImageWindow_ImageToViewport);
    s_stubMap["ImageWindow/ImageToViewportArray"] = reinterpret_cast<void*>(stub_ImageWindow_ImageToViewportArray);
    s_stubMap["ImageWindow/ImageToViewportArrayD"] = reinterpret_cast<void*>(stub_ImageWindow_ImageToViewportArrayD);
    s_stubMap["ImageWindow/ImageToViewportD"] = reinterpret_cast<void*>(stub_ImageWindow_ImageToViewportD);
    s_stubMap["ImageWindow/LoadImageWindowICCProfile"] = reinterpret_cast<void*>(stub_ImageWindow_LoadImageWindowICCProfile);
    s_stubMap["ImageWindow/LoadImageWindows"] = reinterpret_cast<void*>(stub_ImageWindow_LoadImageWindows);
    s_stubMap["ImageWindow/ModifyPreview"] = reinterpret_cast<void*>(stub_ImageWindow_ModifyPreview);
    s_stubMap["ImageWindow/ModifyViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_ModifyViewportSelection);
    s_stubMap["ImageWindow/PurgeImageWindowProperties"] = reinterpret_cast<void*>(stub_ImageWindow_PurgeImageWindowProperties);
    s_stubMap["ImageWindow/RegenerateImageRect"] = reinterpret_cast<void*>(stub_ImageWindow_RegenerateImageRect);
    s_stubMap["ImageWindow/RegenerateImageWindowAstrometricSolution"] = reinterpret_cast<void*>(stub_ImageWindow_RegenerateImageWindowAstrometricSolution);
    s_stubMap["ImageWindow/RegenerateImageWindowViewport"] = reinterpret_cast<void*>(stub_ImageWindow_RegenerateImageWindowViewport);
    s_stubMap["ImageWindow/RegenerateViewportRect"] = reinterpret_cast<void*>(stub_ImageWindow_RegenerateViewportRect);
    s_stubMap["ImageWindow/RemoveImageWindowMaskReferences"] = reinterpret_cast<void*>(stub_ImageWindow_RemoveImageWindowMaskReferences);
    s_stubMap["ImageWindow/ResetImageWindowKeywords"] = reinterpret_cast<void*>(stub_ImageWindow_ResetImageWindowKeywords);
    s_stubMap["ImageWindow/SelectPreview"] = reinterpret_cast<void*>(stub_ImageWindow_SelectPreview);
    s_stubMap["ImageWindow/SendImageWindowToBack"] = reinterpret_cast<void*>(stub_ImageWindow_SendImageWindowToBack);
    s_stubMap["ImageWindow/SetDynamicCursor"] = reinterpret_cast<void*>(stub_ImageWindow_SetDynamicCursor);
    s_stubMap["ImageWindow/SetDynamicCursorXPM"] = reinterpret_cast<void*>(stub_ImageWindow_SetDynamicCursorXPM);
    s_stubMap["ImageWindow/SetGlobalRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_SetGlobalRGBWS);
    s_stubMap["ImageWindow/SetImageType"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageType);
    s_stubMap["ImageWindow/SetImageWindowCMEnabled"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowCMEnabled);
    s_stubMap["ImageWindow/SetImageWindowCurrentView"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowCurrentView);
    s_stubMap["ImageWindow/SetImageWindowDisplayChannel"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowDisplayChannel);
    s_stubMap["ImageWindow/SetImageWindowGlobalRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowGlobalRGBWS);
    s_stubMap["ImageWindow/SetImageWindowICCProfile"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowICCProfile);
    s_stubMap["ImageWindow/SetImageWindowIconic"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowIconic);
    s_stubMap["ImageWindow/SetImageWindowMask"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowMask);
    s_stubMap["ImageWindow/SetImageWindowMaskEnabled"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowMaskEnabled);
    s_stubMap["ImageWindow/SetImageWindowMaskMode"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowMaskMode);
    s_stubMap["ImageWindow/SetImageWindowMaskVisible"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowMaskVisible);
    s_stubMap["ImageWindow/SetImageWindowMode"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowMode);
    s_stubMap["ImageWindow/SetImageWindowRGBWS"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowRGBWS);
    s_stubMap["ImageWindow/SetImageWindowResolution"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowResolution);
    s_stubMap["ImageWindow/SetImageWindowSampleFormat"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowSampleFormat);
    s_stubMap["ImageWindow/SetImageWindowTransparencyMode"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowTransparencyMode);
    s_stubMap["ImageWindow/SetImageWindowViewport"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowViewport);
    s_stubMap["ImageWindow/SetImageWindowViewportPosition"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowViewportPosition);
    s_stubMap["ImageWindow/SetImageWindowVisible"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowVisible);
    s_stubMap["ImageWindow/SetImageWindowZoomFactor"] = reinterpret_cast<void*>(stub_ImageWindow_SetImageWindowZoomFactor);
    s_stubMap["ImageWindow/SetSwapDirectories"] = reinterpret_cast<void*>(stub_ImageWindow_SetSwapDirectories);
    s_stubMap["ImageWindow/SetTransparencyBackgroundBrush"] = reinterpret_cast<void*>(stub_ImageWindow_SetTransparencyBackgroundBrush);
    s_stubMap["ImageWindow/TerminateDynamicSession"] = reinterpret_cast<void*>(stub_ImageWindow_TerminateDynamicSession);
    s_stubMap["ImageWindow/UpdateImageRect"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateImageRect);
    s_stubMap["ImageWindow/UpdateImageWindowAstrometryMetadata"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateImageWindowAstrometryMetadata);
    s_stubMap["ImageWindow/UpdateImageWindowMaskReferences"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateImageWindowMaskReferences);
    s_stubMap["ImageWindow/UpdateImageWindowViewport"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateImageWindowViewport);
    s_stubMap["ImageWindow/UpdateViewportRect"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateViewportRect);
    s_stubMap["ImageWindow/UpdateViewportSelection"] = reinterpret_cast<void*>(stub_ImageWindow_UpdateViewportSelection);
    s_stubMap["ImageWindow/ValidateImageWindowMask"] = reinterpret_cast<void*>(stub_ImageWindow_ValidateImageWindowMask);
    s_stubMap["ImageWindow/ValidateImageWindowView"] = reinterpret_cast<void*>(stub_ImageWindow_ValidateImageWindowView);
    s_stubMap["ImageWindow/ViewportScalarToImage"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportScalarToImage);
    s_stubMap["ImageWindow/ViewportScalarToImageArray"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportScalarToImageArray);
    s_stubMap["ImageWindow/ViewportScalarToImageArrayD"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportScalarToImageArrayD);
    s_stubMap["ImageWindow/ViewportScalarToImageD"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportScalarToImageD);
    s_stubMap["ImageWindow/ViewportToGlobal"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportToGlobal);
    s_stubMap["ImageWindow/ViewportToImage"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportToImage);
    s_stubMap["ImageWindow/ViewportToImageArray"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportToImageArray);
    s_stubMap["ImageWindow/ViewportToImageArrayD"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportToImageArrayD);
    s_stubMap["ImageWindow/ViewportToImageD"] = reinterpret_cast<void*>(stub_ImageWindow_ViewportToImageD);
    s_stubMap["ImageWindow/ZoomImageWindowToFit"] = reinterpret_cast<void*>(stub_ImageWindow_ZoomImageWindowToFit);
    s_stubMap["InterfaceDefinition/BeginInterfaceDefinition"] = reinterpret_cast<void*>(stub_InterfaceDefinition_BeginInterfaceDefinition);
    s_stubMap["InterfaceDefinition/EndInterfaceDefinition"] = reinterpret_cast<void*>(stub_InterfaceDefinition_EndInterfaceDefinition);
    s_stubMap["InterfaceDefinition/EnterInterfaceDefinitionContext"] = reinterpret_cast<void*>(stub_InterfaceDefinition_EnterInterfaceDefinitionContext);
    s_stubMap["InterfaceDefinition/ExitInterfaceDefinitionContext"] = reinterpret_cast<void*>(stub_InterfaceDefinition_ExitInterfaceDefinitionContext);
    s_stubMap["InterfaceDefinition/GetInterfaceBeingDefined"] = reinterpret_cast<void*>(stub_InterfaceDefinition_GetInterfaceBeingDefined);
    s_stubMap["InterfaceDefinition/IsInterfaceDefinitionContextActive"] = reinterpret_cast<void*>(stub_InterfaceDefinition_IsInterfaceDefinitionContextActive);
    s_stubMap["InterfaceDefinition/SetBeginReadoutNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetBeginReadoutNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetEndReadoutNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetEndReadoutNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalCMDisabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalCMDisabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalCMEnabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalCMEnabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalCMUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalCMUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalFiltersUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalFiltersUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalPreferencesUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalPreferencesUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetGlobalRGBWSUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetGlobalRGBWSUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageCMDisabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageCMDisabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageCMEnabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageCMEnabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageCMUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageCMUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageCreatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageCreatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageDeletedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageDeletedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageFocusedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageFocusedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageLockedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageLockedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageRGBWSUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageRGBWSUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageRenamedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageRenamedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageSTFDisabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageSTFDisabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageSTFEnabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageSTFEnabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageSTFUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageSTFUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageSavedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageSavedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageUnlockedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageUnlockedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetImageUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetImageUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceAliasIdentifiers"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceAliasIdentifiers);
    s_stubMap["InterfaceDefinition/SetInterfaceApplyGlobalRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceApplyGlobalRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceApplyRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceApplyRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceBrowseDocumentationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceBrowseDocumentationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceCancelRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceCancelRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDescription"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDescription);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicKeyPressRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicKeyPressRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicKeyReleaseRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicKeyReleaseRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicModeEnterRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicModeEnterRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicModeExitRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicModeExitRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseDoubleClickRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseDoubleClickRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseEnterRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseEnterRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseLeaveRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseLeaveRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseMoveRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseMoveRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMousePressRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMousePressRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseReleaseRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseReleaseRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicMouseWheelRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicMouseWheelRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicPaintRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicPaintRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceDynamicUpdateQueryRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceDynamicUpdateQueryRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceEditPreferencesRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceEditPreferencesRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceExecuteRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceExecuteRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceFeatures"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceFeatures);
    s_stubMap["InterfaceDefinition/SetInterfaceIconImage"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconImage);
    s_stubMap["InterfaceDefinition/SetInterfaceIconImageFile"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconImageFile);
    s_stubMap["InterfaceDefinition/SetInterfaceIconSVG"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconSVG);
    s_stubMap["InterfaceDefinition/SetInterfaceIconSVGFile"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconSVGFile);
    s_stubMap["InterfaceDefinition/SetInterfaceIconSmallImage"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconSmallImage);
    s_stubMap["InterfaceDefinition/SetInterfaceIconSmallImageFile"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceIconSmallImageFile);
    s_stubMap["InterfaceDefinition/SetInterfaceInitializationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceInitializationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceLaunchRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceLaunchRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceProcessImportRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceProcessImportRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceProcessInstantiationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceProcessInstantiationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceProcessTestInstantiationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceProcessTestInstantiationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceProcessValidationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceProcessValidationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceRealTimeCancelRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceRealTimeCancelRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceRealTimeGenerationFlagsRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceRealTimeGenerationFlagsRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceRealTimeGenerationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceRealTimeGenerationRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceRealTimePreviewUpdatedRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceRealTimePreviewUpdatedRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceRealTimeUpdateQueryRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceRealTimeUpdateQueryRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceResetRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceResetRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceTrackViewUpdatedRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceTrackViewUpdatedRoutine);
    s_stubMap["InterfaceDefinition/SetInterfaceVersion"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetInterfaceVersion);
    s_stubMap["InterfaceDefinition/SetMaskDisabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetMaskDisabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetMaskEnabledNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetMaskEnabledNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetMaskHiddenNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetMaskHiddenNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetMaskShownNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetMaskShownNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetMaskUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetMaskUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetProcessCreatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetProcessCreatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetProcessDeletedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetProcessDeletedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetProcessSavedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetProcessSavedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetProcessUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetProcessUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetReadoutOptionsUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetReadoutOptionsUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetRealTimePreviewGenerationFinishNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetRealTimePreviewGenerationFinishNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetRealTimePreviewGenerationStartNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetRealTimePreviewGenerationStartNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetRealTimePreviewLUTUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetRealTimePreviewLUTUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetRealTimePreviewOwnerChangeNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetRealTimePreviewOwnerChangeNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetTransparencyHiddenNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetTransparencyHiddenNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetTransparencyModeUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetTransparencyModeUpdatedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetUpdateReadoutNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetUpdateReadoutNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetViewPropertyDeletedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetViewPropertyDeletedNotificationRoutine);
    s_stubMap["InterfaceDefinition/SetViewPropertyUpdatedNotificationRoutine"] = reinterpret_cast<void*>(stub_InterfaceDefinition_SetViewPropertyUpdatedNotificationRoutine);
    s_stubMap["Label/CreateLabel"] = reinterpret_cast<void*>(stub_Label_CreateLabel);
    s_stubMap["Label/GetLabelAlignment"] = reinterpret_cast<void*>(stub_Label_GetLabelAlignment);
    s_stubMap["Label/GetLabelMargin"] = reinterpret_cast<void*>(stub_Label_GetLabelMargin);
    s_stubMap["Label/GetLabelRichTextEnabled"] = reinterpret_cast<void*>(stub_Label_GetLabelRichTextEnabled);
    s_stubMap["Label/GetLabelText"] = reinterpret_cast<void*>(stub_Label_GetLabelText);
    s_stubMap["Label/GetLabelWordWrappingEnabled"] = reinterpret_cast<void*>(stub_Label_GetLabelWordWrappingEnabled);
    s_stubMap["Label/SetLabelAlignment"] = reinterpret_cast<void*>(stub_Label_SetLabelAlignment);
    s_stubMap["Label/SetLabelMargin"] = reinterpret_cast<void*>(stub_Label_SetLabelMargin);
    s_stubMap["Label/SetLabelRichTextEnabled"] = reinterpret_cast<void*>(stub_Label_SetLabelRichTextEnabled);
    s_stubMap["Label/SetLabelText"] = reinterpret_cast<void*>(stub_Label_SetLabelText);
    s_stubMap["Label/SetLabelWordWrappingEnabled"] = reinterpret_cast<void*>(stub_Label_SetLabelWordWrappingEnabled);
    s_stubMap["Module/EvaluateScript"] = reinterpret_cast<void*>(stub_Module_EvaluateScript);
    s_stubMap["Module/HasEntitlement"] = reinterpret_cast<void*>(stub_Module_HasEntitlement);
    s_stubMap["Module/LoadResource"] = reinterpret_cast<void*>(stub_Module_LoadResource);
    s_stubMap["Module/UnloadResource"] = reinterpret_cast<void*>(stub_Module_UnloadResource);
    s_stubMap["ModuleDefinition/EnterModuleDefinitionContext"] = reinterpret_cast<void*>(stub_ModuleDefinition_EnterModuleDefinitionContext);
    s_stubMap["ModuleDefinition/ExitModuleDefinitionContext"] = reinterpret_cast<void*>(stub_ModuleDefinition_ExitModuleDefinitionContext);
    s_stubMap["ModuleDefinition/IsModuleDefinitionContextActive"] = reinterpret_cast<void*>(stub_ModuleDefinition_IsModuleDefinitionContextActive);
    s_stubMap["ModuleDefinition/SetModuleAllocationRoutine"] = reinterpret_cast<void*>(stub_ModuleDefinition_SetModuleAllocationRoutine);
    s_stubMap["ModuleDefinition/SetModuleDeallocationRoutine"] = reinterpret_cast<void*>(stub_ModuleDefinition_SetModuleDeallocationRoutine);
    s_stubMap["ModuleDefinition/SetModuleOnLoadRoutine"] = reinterpret_cast<void*>(stub_ModuleDefinition_SetModuleOnLoadRoutine);
    s_stubMap["ModuleDefinition/SetModuleOnUnloadRoutine"] = reinterpret_cast<void*>(stub_ModuleDefinition_SetModuleOnUnloadRoutine);
    s_stubMap["Mutex/CreateMutex"] = reinterpret_cast<void*>(stub_Mutex_CreateMutex);
    s_stubMap["Mutex/CreateReadWriteMutex"] = reinterpret_cast<void*>(stub_Mutex_CreateReadWriteMutex);
    s_stubMap["Mutex/GetLockState"] = reinterpret_cast<void*>(stub_Mutex_GetLockState);
    s_stubMap["Mutex/Lock"] = reinterpret_cast<void*>(stub_Mutex_Lock);
    s_stubMap["Mutex/LockForRead"] = reinterpret_cast<void*>(stub_Mutex_LockForRead);
    s_stubMap["Mutex/LockForWrite"] = reinterpret_cast<void*>(stub_Mutex_LockForWrite);
    s_stubMap["Mutex/Unlock"] = reinterpret_cast<void*>(stub_Mutex_Unlock);
    s_stubMap["NetworkTransfer/CloseNetworkTransferConnection"] = reinterpret_cast<void*>(stub_NetworkTransfer_CloseNetworkTransferConnection);
    s_stubMap["NetworkTransfer/CreateNetworkTransfer"] = reinterpret_cast<void*>(stub_NetworkTransfer_CreateNetworkTransfer);
    s_stubMap["NetworkTransfer/GetNetworkTransferBytesTransferred"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferBytesTransferred);
    s_stubMap["NetworkTransfer/GetNetworkTransferContentType"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferContentType);
    s_stubMap["NetworkTransfer/GetNetworkTransferCustomHTTPHeaders"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferCustomHTTPHeaders);
    s_stubMap["NetworkTransfer/GetNetworkTransferErrorInformation"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferErrorInformation);
    s_stubMap["NetworkTransfer/GetNetworkTransferIsAborted"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferIsAborted);
    s_stubMap["NetworkTransfer/GetNetworkTransferProxyURL"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferProxyURL);
    s_stubMap["NetworkTransfer/GetNetworkTransferResponseCode"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferResponseCode);
    s_stubMap["NetworkTransfer/GetNetworkTransferStatus"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferStatus);
    s_stubMap["NetworkTransfer/GetNetworkTransferTotalSpeed"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferTotalSpeed);
    s_stubMap["NetworkTransfer/GetNetworkTransferTotalTime"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferTotalTime);
    s_stubMap["NetworkTransfer/GetNetworkTransferURL"] = reinterpret_cast<void*>(stub_NetworkTransfer_GetNetworkTransferURL);
    s_stubMap["NetworkTransfer/PerformNetworkTransferDownload"] = reinterpret_cast<void*>(stub_NetworkTransfer_PerformNetworkTransferDownload);
    s_stubMap["NetworkTransfer/PerformNetworkTransferPOST"] = reinterpret_cast<void*>(stub_NetworkTransfer_PerformNetworkTransferPOST);
    s_stubMap["NetworkTransfer/PerformNetworkTransferSMTP"] = reinterpret_cast<void*>(stub_NetworkTransfer_PerformNetworkTransferSMTP);
    s_stubMap["NetworkTransfer/PerformNetworkTransferUpload"] = reinterpret_cast<void*>(stub_NetworkTransfer_PerformNetworkTransferUpload);
    s_stubMap["NetworkTransfer/SetNetworkTransferConnectionTimeout"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferConnectionTimeout);
    s_stubMap["NetworkTransfer/SetNetworkTransferCustomHTTPHeaders"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferCustomHTTPHeaders);
    s_stubMap["NetworkTransfer/SetNetworkTransferDownloadEventRoutine"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferDownloadEventRoutine);
    s_stubMap["NetworkTransfer/SetNetworkTransferProgressEventRoutine"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferProgressEventRoutine);
    s_stubMap["NetworkTransfer/SetNetworkTransferProxyURL"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferProxyURL);
    s_stubMap["NetworkTransfer/SetNetworkTransferSSL"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferSSL);
    s_stubMap["NetworkTransfer/SetNetworkTransferURL"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferURL);
    s_stubMap["NetworkTransfer/SetNetworkTransferUploadEventRoutine"] = reinterpret_cast<void*>(stub_NetworkTransfer_SetNetworkTransferUploadEventRoutine);
    s_stubMap["Numerical/CubicSplineGenerateD"] = reinterpret_cast<void*>(stub_Numerical_CubicSplineGenerateD);
    s_stubMap["Numerical/CubicSplineGenerateF"] = reinterpret_cast<void*>(stub_Numerical_CubicSplineGenerateF);
    s_stubMap["Numerical/CubicSplineInterpolateD"] = reinterpret_cast<void*>(stub_Numerical_CubicSplineInterpolateD);
    s_stubMap["Numerical/CubicSplineInterpolateF"] = reinterpret_cast<void*>(stub_Numerical_CubicSplineInterpolateF);
    s_stubMap["Numerical/FFTComplexInverseTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexInverseTransformD);
    s_stubMap["Numerical/FFTComplexInverseTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexInverseTransformF);
    s_stubMap["Numerical/FFTComplexOptimizedLengthD"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexOptimizedLengthD);
    s_stubMap["Numerical/FFTComplexOptimizedLengthF"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexOptimizedLengthF);
    s_stubMap["Numerical/FFTComplexTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexTransformD);
    s_stubMap["Numerical/FFTComplexTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTComplexTransformF);
    s_stubMap["Numerical/FFTCreateComplexInverseTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateComplexInverseTransformD);
    s_stubMap["Numerical/FFTCreateComplexInverseTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateComplexInverseTransformF);
    s_stubMap["Numerical/FFTCreateComplexTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateComplexTransformD);
    s_stubMap["Numerical/FFTCreateComplexTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateComplexTransformF);
    s_stubMap["Numerical/FFTCreateRealInverseTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateRealInverseTransformD);
    s_stubMap["Numerical/FFTCreateRealInverseTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateRealInverseTransformF);
    s_stubMap["Numerical/FFTCreateRealTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateRealTransformD);
    s_stubMap["Numerical/FFTCreateRealTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTCreateRealTransformF);
    s_stubMap["Numerical/FFTDestroyTransform"] = reinterpret_cast<void*>(stub_Numerical_FFTDestroyTransform);
    s_stubMap["Numerical/FFTRealInverseTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTRealInverseTransformD);
    s_stubMap["Numerical/FFTRealInverseTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTRealInverseTransformF);
    s_stubMap["Numerical/FFTRealOptimizedLengthD"] = reinterpret_cast<void*>(stub_Numerical_FFTRealOptimizedLengthD);
    s_stubMap["Numerical/FFTRealOptimizedLengthF"] = reinterpret_cast<void*>(stub_Numerical_FFTRealOptimizedLengthF);
    s_stubMap["Numerical/FFTRealTransformD"] = reinterpret_cast<void*>(stub_Numerical_FFTRealTransformD);
    s_stubMap["Numerical/FFTRealTransformF"] = reinterpret_cast<void*>(stub_Numerical_FFTRealTransformF);
    s_stubMap["Numerical/GaussJordanInPlaceD"] = reinterpret_cast<void*>(stub_Numerical_GaussJordanInPlaceD);
    s_stubMap["Numerical/GaussJordanInPlaceF"] = reinterpret_cast<void*>(stub_Numerical_GaussJordanInPlaceF);
    s_stubMap["Numerical/LinearFitD"] = reinterpret_cast<void*>(stub_Numerical_LinearFitD);
    s_stubMap["Numerical/LinearFitF"] = reinterpret_cast<void*>(stub_Numerical_LinearFitF);
    s_stubMap["Numerical/NaturalCubicSplineGenerateD"] = reinterpret_cast<void*>(stub_Numerical_NaturalCubicSplineGenerateD);
    s_stubMap["Numerical/NaturalCubicSplineGenerateF"] = reinterpret_cast<void*>(stub_Numerical_NaturalCubicSplineGenerateF);
    s_stubMap["Numerical/NaturalGridCubicSplineGenerateD"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineGenerateD);
    s_stubMap["Numerical/NaturalGridCubicSplineGenerateF"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineGenerateF);
    s_stubMap["Numerical/NaturalGridCubicSplineGenerateUI16"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineGenerateUI16);
    s_stubMap["Numerical/NaturalGridCubicSplineGenerateUI32"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineGenerateUI32);
    s_stubMap["Numerical/NaturalGridCubicSplineGenerateUI8"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineGenerateUI8);
    s_stubMap["Numerical/NaturalGridCubicSplineInterpolateD"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineInterpolateD);
    s_stubMap["Numerical/NaturalGridCubicSplineInterpolateF"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineInterpolateF);
    s_stubMap["Numerical/NaturalGridCubicSplineInterpolateUI16"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineInterpolateUI16);
    s_stubMap["Numerical/NaturalGridCubicSplineInterpolateUI32"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineInterpolateUI32);
    s_stubMap["Numerical/NaturalGridCubicSplineInterpolateUI8"] = reinterpret_cast<void*>(stub_Numerical_NaturalGridCubicSplineInterpolateUI8);
    s_stubMap["Numerical/SVDInPlaceD"] = reinterpret_cast<void*>(stub_Numerical_SVDInPlaceD);
    s_stubMap["Numerical/SVDInPlaceF"] = reinterpret_cast<void*>(stub_Numerical_SVDInPlaceF);
    s_stubMap["Numerical/SurfaceSplineCreateD"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineCreateD);
    s_stubMap["Numerical/SurfaceSplineCreateF"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineCreateF);
    s_stubMap["Numerical/SurfaceSplineDeserialize"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineDeserialize);
    s_stubMap["Numerical/SurfaceSplineDestroy"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineDestroy);
    s_stubMap["Numerical/SurfaceSplineDuplicate"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineDuplicate);
    s_stubMap["Numerical/SurfaceSplineEvaluate"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineEvaluate);
    s_stubMap["Numerical/SurfaceSplineEvaluateVectorD"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineEvaluateVectorD);
    s_stubMap["Numerical/SurfaceSplineEvaluateVectorF"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineEvaluateVectorF);
    s_stubMap["Numerical/SurfaceSplineSerialize"] = reinterpret_cast<void*>(stub_Numerical_SurfaceSplineSerialize);
    s_stubMap["Pen/ClonePen"] = reinterpret_cast<void*>(stub_Pen_ClonePen);
    s_stubMap["Pen/CreatePen"] = reinterpret_cast<void*>(stub_Pen_CreatePen);
    s_stubMap["Pen/GetPenBrush"] = reinterpret_cast<void*>(stub_Pen_GetPenBrush);
    s_stubMap["Pen/GetPenCap"] = reinterpret_cast<void*>(stub_Pen_GetPenCap);
    s_stubMap["Pen/GetPenColor"] = reinterpret_cast<void*>(stub_Pen_GetPenColor);
    s_stubMap["Pen/GetPenJoin"] = reinterpret_cast<void*>(stub_Pen_GetPenJoin);
    s_stubMap["Pen/GetPenStyle"] = reinterpret_cast<void*>(stub_Pen_GetPenStyle);
    s_stubMap["Pen/GetPenWidth"] = reinterpret_cast<void*>(stub_Pen_GetPenWidth);
    s_stubMap["Pen/SetPenBrush"] = reinterpret_cast<void*>(stub_Pen_SetPenBrush);
    s_stubMap["Pen/SetPenCap"] = reinterpret_cast<void*>(stub_Pen_SetPenCap);
    s_stubMap["Pen/SetPenColor"] = reinterpret_cast<void*>(stub_Pen_SetPenColor);
    s_stubMap["Pen/SetPenJoin"] = reinterpret_cast<void*>(stub_Pen_SetPenJoin);
    s_stubMap["Pen/SetPenStyle"] = reinterpret_cast<void*>(stub_Pen_SetPenStyle);
    s_stubMap["Pen/SetPenWidth"] = reinterpret_cast<void*>(stub_Pen_SetPenWidth);
    s_stubMap["Process/AllocateTableRows"] = reinterpret_cast<void*>(stub_Process_AllocateTableRows);
    s_stubMap["Process/AssignProcessInstance"] = reinterpret_cast<void*>(stub_Process_AssignProcessInstance);
    s_stubMap["Process/BrowseProcessDocumentation"] = reinterpret_cast<void*>(stub_Process_BrowseProcessDocumentation);
    s_stubMap["Process/CloneProcessInstance"] = reinterpret_cast<void*>(stub_Process_CloneProcessInstance);
    s_stubMap["Process/CreateProcessInstance"] = reinterpret_cast<void*>(stub_Process_CreateProcessInstance);
    s_stubMap["Process/CreateProcessInstanceFromIcon"] = reinterpret_cast<void*>(stub_Process_CreateProcessInstanceFromIcon);
    s_stubMap["Process/CreateProcessInstanceFromSourceCode"] = reinterpret_cast<void*>(stub_Process_CreateProcessInstanceFromSourceCode);
    s_stubMap["Process/EditProcessPreferences"] = reinterpret_cast<void*>(stub_Process_EditProcessPreferences);
    s_stubMap["Process/EnumerateProcessCategories"] = reinterpret_cast<void*>(stub_Process_EnumerateProcessCategories);
    s_stubMap["Process/EnumerateProcessIcons"] = reinterpret_cast<void*>(stub_Process_EnumerateProcessIcons);
    s_stubMap["Process/EnumerateProcessParameters"] = reinterpret_cast<void*>(stub_Process_EnumerateProcessParameters);
    s_stubMap["Process/EnumerateProcesses"] = reinterpret_cast<void*>(stub_Process_EnumerateProcesses);
    s_stubMap["Process/EnumerateTableColumns"] = reinterpret_cast<void*>(stub_Process_EnumerateTableColumns);
    s_stubMap["Process/ExecuteGlobal"] = reinterpret_cast<void*>(stub_Process_ExecuteGlobal);
    s_stubMap["Process/ExecuteOnImage"] = reinterpret_cast<void*>(stub_Process_ExecuteOnImage);
    s_stubMap["Process/ExecuteOnView"] = reinterpret_cast<void*>(stub_Process_ExecuteOnView);
    s_stubMap["Process/GetExecutionTimes"] = reinterpret_cast<void*>(stub_Process_GetExecutionTimes);
    s_stubMap["Process/GetInterface"] = reinterpret_cast<void*>(stub_Process_GetInterface);
    s_stubMap["Process/GetParameterAliasIdentifiers"] = reinterpret_cast<void*>(stub_Process_GetParameterAliasIdentifiers);
    s_stubMap["Process/GetParameterAllowedCharacters"] = reinterpret_cast<void*>(stub_Process_GetParameterAllowedCharacters);
    s_stubMap["Process/GetParameterByName"] = reinterpret_cast<void*>(stub_Process_GetParameterByName);
    s_stubMap["Process/GetParameterDefaultElementIndex"] = reinterpret_cast<void*>(stub_Process_GetParameterDefaultElementIndex);
    s_stubMap["Process/GetParameterDefaultValue"] = reinterpret_cast<void*>(stub_Process_GetParameterDefaultValue);
    s_stubMap["Process/GetParameterDescription"] = reinterpret_cast<void*>(stub_Process_GetParameterDescription);
    s_stubMap["Process/GetParameterElementAliasIdentifiers"] = reinterpret_cast<void*>(stub_Process_GetParameterElementAliasIdentifiers);
    s_stubMap["Process/GetParameterElementCount"] = reinterpret_cast<void*>(stub_Process_GetParameterElementCount);
    s_stubMap["Process/GetParameterElementIdentifier"] = reinterpret_cast<void*>(stub_Process_GetParameterElementIdentifier);
    s_stubMap["Process/GetParameterElementValue"] = reinterpret_cast<void*>(stub_Process_GetParameterElementValue);
    s_stubMap["Process/GetParameterIdentifier"] = reinterpret_cast<void*>(stub_Process_GetParameterIdentifier);
    s_stubMap["Process/GetParameterLengthLimits"] = reinterpret_cast<void*>(stub_Process_GetParameterLengthLimits);
    s_stubMap["Process/GetParameterPrecision"] = reinterpret_cast<void*>(stub_Process_GetParameterPrecision);
    s_stubMap["Process/GetParameterProcess"] = reinterpret_cast<void*>(stub_Process_GetParameterProcess);
    s_stubMap["Process/GetParameterRange"] = reinterpret_cast<void*>(stub_Process_GetParameterRange);
    s_stubMap["Process/GetParameterReadOnly"] = reinterpret_cast<void*>(stub_Process_GetParameterReadOnly);
    s_stubMap["Process/GetParameterRequired"] = reinterpret_cast<void*>(stub_Process_GetParameterRequired);
    s_stubMap["Process/GetParameterScientificNotation"] = reinterpret_cast<void*>(stub_Process_GetParameterScientificNotation);
    s_stubMap["Process/GetParameterScriptComment"] = reinterpret_cast<void*>(stub_Process_GetParameterScriptComment);
    s_stubMap["Process/GetParameterTable"] = reinterpret_cast<void*>(stub_Process_GetParameterTable);
    s_stubMap["Process/GetParameterType"] = reinterpret_cast<void*>(stub_Process_GetParameterType);
    s_stubMap["Process/GetParameterValue"] = reinterpret_cast<void*>(stub_Process_GetParameterValue);
    s_stubMap["Process/GetProcessAliasIdentifiers"] = reinterpret_cast<void*>(stub_Process_GetProcessAliasIdentifiers);
    s_stubMap["Process/GetProcessByName"] = reinterpret_cast<void*>(stub_Process_GetProcessByName);
    s_stubMap["Process/GetProcessCategory"] = reinterpret_cast<void*>(stub_Process_GetProcessCategory);
    s_stubMap["Process/GetProcessDefaultInterface"] = reinterpret_cast<void*>(stub_Process_GetProcessDefaultInterface);
    s_stubMap["Process/GetProcessDescription"] = reinterpret_cast<void*>(stub_Process_GetProcessDescription);
    s_stubMap["Process/GetProcessIcon"] = reinterpret_cast<void*>(stub_Process_GetProcessIcon);
    s_stubMap["Process/GetProcessIdentifier"] = reinterpret_cast<void*>(stub_Process_GetProcessIdentifier);
    s_stubMap["Process/GetProcessInstanceDescription"] = reinterpret_cast<void*>(stub_Process_GetProcessInstanceDescription);
    s_stubMap["Process/GetProcessInstanceProcess"] = reinterpret_cast<void*>(stub_Process_GetProcessInstanceProcess);
    s_stubMap["Process/GetProcessInstanceSourceCode"] = reinterpret_cast<void*>(stub_Process_GetProcessInstanceSourceCode);
    s_stubMap["Process/GetProcessInstanceVersion"] = reinterpret_cast<void*>(stub_Process_GetProcessInstanceVersion);
    s_stubMap["Process/GetProcessProperties"] = reinterpret_cast<void*>(stub_Process_GetProcessProperties);
    s_stubMap["Process/GetProcessScriptComment"] = reinterpret_cast<void*>(stub_Process_GetProcessScriptComment);
    s_stubMap["Process/GetProcessSmallIcon"] = reinterpret_cast<void*>(stub_Process_GetProcessSmallIcon);
    s_stubMap["Process/GetProcessVersion"] = reinterpret_cast<void*>(stub_Process_GetProcessVersion);
    s_stubMap["Process/GetTableColumnByName"] = reinterpret_cast<void*>(stub_Process_GetTableColumnByName);
    s_stubMap["Process/GetTableRowCount"] = reinterpret_cast<void*>(stub_Process_GetTableRowCount);
    s_stubMap["Process/GetUpdatesViewHistory"] = reinterpret_cast<void*>(stub_Process_GetUpdatesViewHistory);
    s_stubMap["Process/LaunchInterface"] = reinterpret_cast<void*>(stub_Process_LaunchInterface);
    s_stubMap["Process/LaunchProcess"] = reinterpret_cast<void*>(stub_Process_LaunchProcess);
    s_stubMap["Process/LaunchProcessInstance"] = reinterpret_cast<void*>(stub_Process_LaunchProcessInstance);
    s_stubMap["Process/RunProcessCommandLine"] = reinterpret_cast<void*>(stub_Process_RunProcessCommandLine);
    s_stubMap["Process/SetParameterValue"] = reinterpret_cast<void*>(stub_Process_SetParameterValue);
    s_stubMap["Process/SetProcessInstanceDescription"] = reinterpret_cast<void*>(stub_Process_SetProcessInstanceDescription);
    s_stubMap["Process/ValidateGlobalExecution"] = reinterpret_cast<void*>(stub_Process_ValidateGlobalExecution);
    s_stubMap["Process/ValidateImageExecution"] = reinterpret_cast<void*>(stub_Process_ValidateImageExecution);
    s_stubMap["Process/ValidateInterface"] = reinterpret_cast<void*>(stub_Process_ValidateInterface);
    s_stubMap["Process/ValidateInterfaceLaunch"] = reinterpret_cast<void*>(stub_Process_ValidateInterfaceLaunch);
    s_stubMap["Process/ValidateProcessInstance"] = reinterpret_cast<void*>(stub_Process_ValidateProcessInstance);
    s_stubMap["Process/ValidateViewExecution"] = reinterpret_cast<void*>(stub_Process_ValidateViewExecution);
    s_stubMap["Process/ValidateViewExecutionMask"] = reinterpret_cast<void*>(stub_Process_ValidateViewExecutionMask);
    s_stubMap["ProcessDefinition/BeginParameterDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_BeginParameterDefinition);
    s_stubMap["ProcessDefinition/BeginProcessDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_BeginProcessDefinition);
    s_stubMap["ProcessDefinition/BeginTableColumnDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_BeginTableColumnDefinition);
    s_stubMap["ProcessDefinition/DefineEnumerationAlias"] = reinterpret_cast<void*>(stub_ProcessDefinition_DefineEnumerationAlias);
    s_stubMap["ProcessDefinition/DefineEnumerationElement"] = reinterpret_cast<void*>(stub_ProcessDefinition_DefineEnumerationElement);
    s_stubMap["ProcessDefinition/EndParameterDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_EndParameterDefinition);
    s_stubMap["ProcessDefinition/EndProcessDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_EndProcessDefinition);
    s_stubMap["ProcessDefinition/EndTableColumnDefinition"] = reinterpret_cast<void*>(stub_ProcessDefinition_EndTableColumnDefinition);
    s_stubMap["ProcessDefinition/EnterProcessDefinitionContext"] = reinterpret_cast<void*>(stub_ProcessDefinition_EnterProcessDefinitionContext);
    s_stubMap["ProcessDefinition/ExitProcessDefinitionContext"] = reinterpret_cast<void*>(stub_ProcessDefinition_ExitProcessDefinitionContext);
    s_stubMap["ProcessDefinition/GetParameterBeingDefined"] = reinterpret_cast<void*>(stub_ProcessDefinition_GetParameterBeingDefined);
    s_stubMap["ProcessDefinition/GetProcessBeingDefined"] = reinterpret_cast<void*>(stub_ProcessDefinition_GetProcessBeingDefined);
    s_stubMap["ProcessDefinition/IsProcessDefinitionContextActive"] = reinterpret_cast<void*>(stub_ProcessDefinition_IsProcessDefinitionContextActive);
    s_stubMap["ProcessDefinition/SetBlockSizeLimits"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetBlockSizeLimits);
    s_stubMap["ProcessDefinition/SetDefaultBooleanValue"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetDefaultBooleanValue);
    s_stubMap["ProcessDefinition/SetDefaultEnumerationValueIndex"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetDefaultEnumerationValueIndex);
    s_stubMap["ProcessDefinition/SetDefaultNumericValue"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetDefaultNumericValue);
    s_stubMap["ProcessDefinition/SetDefaultStringValue"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetDefaultStringValue);
    s_stubMap["ProcessDefinition/SetParameterAliasIdentifiers"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterAliasIdentifiers);
    s_stubMap["ProcessDefinition/SetParameterAllocationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterAllocationRoutine);
    s_stubMap["ProcessDefinition/SetParameterDescription"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterDescription);
    s_stubMap["ProcessDefinition/SetParameterLengthQueryRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterLengthQueryRoutine);
    s_stubMap["ProcessDefinition/SetParameterLockRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterLockRoutine);
    s_stubMap["ProcessDefinition/SetParameterProcessVersionRange"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterProcessVersionRange);
    s_stubMap["ProcessDefinition/SetParameterReadOnly"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterReadOnly);
    s_stubMap["ProcessDefinition/SetParameterRequired"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterRequired);
    s_stubMap["ProcessDefinition/SetParameterScriptComment"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterScriptComment);
    s_stubMap["ProcessDefinition/SetParameterUnlockRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterUnlockRoutine);
    s_stubMap["ProcessDefinition/SetParameterValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetParameterValidationRoutine);
    s_stubMap["ProcessDefinition/SetPrecision"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetPrecision);
    s_stubMap["ProcessDefinition/SetProcessAliasIdentifiers"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessAliasIdentifiers);
    s_stubMap["ProcessDefinition/SetProcessAssignmentRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessAssignmentRoutine);
    s_stubMap["ProcessDefinition/SetProcessBrowseDocumentationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessBrowseDocumentationRoutine);
    s_stubMap["ProcessDefinition/SetProcessCategory"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessCategory);
    s_stubMap["ProcessDefinition/SetProcessClassInitializationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessClassInitializationRoutine);
    s_stubMap["ProcessDefinition/SetProcessClonationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessClonationRoutine);
    s_stubMap["ProcessDefinition/SetProcessCommandLineProcessingRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessCommandLineProcessingRoutine);
    s_stubMap["ProcessDefinition/SetProcessCreationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessCreationRoutine);
    s_stubMap["ProcessDefinition/SetProcessDefaultInterfaceSelectionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessDefaultInterfaceSelectionRoutine);
    s_stubMap["ProcessDefinition/SetProcessDescription"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessDescription);
    s_stubMap["ProcessDefinition/SetProcessDestructionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessDestructionRoutine);
    s_stubMap["ProcessDefinition/SetProcessEditPreferencesRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessEditPreferencesRoutine);
    s_stubMap["ProcessDefinition/SetProcessExecutionPreferencesRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessExecutionPreferencesRoutine);
    s_stubMap["ProcessDefinition/SetProcessExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessExecutionValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessExecutionValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessGlobalExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessGlobalExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessGlobalExecutionValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessGlobalExecutionValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessHistoryUpdateValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessHistoryUpdateValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessIPCGetStatusRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIPCGetStatusRoutine);
    s_stubMap["ProcessDefinition/SetProcessIPCSetParametersRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIPCSetParametersRoutine);
    s_stubMap["ProcessDefinition/SetProcessIPCStartRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIPCStartRoutine);
    s_stubMap["ProcessDefinition/SetProcessIPCStopRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIPCStopRoutine);
    s_stubMap["ProcessDefinition/SetProcessIconImage"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconImage);
    s_stubMap["ProcessDefinition/SetProcessIconImageFile"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconImageFile);
    s_stubMap["ProcessDefinition/SetProcessIconSVG"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconSVG);
    s_stubMap["ProcessDefinition/SetProcessIconSVGFile"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconSVGFile);
    s_stubMap["ProcessDefinition/SetProcessIconSmallImage"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconSmallImage);
    s_stubMap["ProcessDefinition/SetProcessIconSmallImageFile"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessIconSmallImageFile);
    s_stubMap["ProcessDefinition/SetProcessImageExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessImageExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessImageExecutionValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessImageExecutionValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessInitializationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessInitializationRoutine);
    s_stubMap["ProcessDefinition/SetProcessInterfaceSelectionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessInterfaceSelectionRoutine);
    s_stubMap["ProcessDefinition/SetProcessInterfaceValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessInterfaceValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessMaskValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessMaskValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessPostExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPostExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessPostGlobalExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPostGlobalExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessPostReadingRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPostReadingRoutine);
    s_stubMap["ProcessDefinition/SetProcessPostWritingRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPostWritingRoutine);
    s_stubMap["ProcessDefinition/SetProcessPreExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPreExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessPreGlobalExecutionRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPreGlobalExecutionRoutine);
    s_stubMap["ProcessDefinition/SetProcessPreReadingRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPreReadingRoutine);
    s_stubMap["ProcessDefinition/SetProcessPreWritingRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessPreWritingRoutine);
    s_stubMap["ProcessDefinition/SetProcessScriptComment"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessScriptComment);
    s_stubMap["ProcessDefinition/SetProcessSetServerHandleRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessSetServerHandleRoutine);
    s_stubMap["ProcessDefinition/SetProcessTestClonationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessTestClonationRoutine);
    s_stubMap["ProcessDefinition/SetProcessUndoModeRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessUndoModeRoutine);
    s_stubMap["ProcessDefinition/SetProcessValidationRoutine"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessValidationRoutine);
    s_stubMap["ProcessDefinition/SetProcessVersion"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetProcessVersion);
    s_stubMap["ProcessDefinition/SetScientificNotation"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetScientificNotation);
    s_stubMap["ProcessDefinition/SetStringAllowedCharacters"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetStringAllowedCharacters);
    s_stubMap["ProcessDefinition/SetStringLengthLimits"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetStringLengthLimits);
    s_stubMap["ProcessDefinition/SetTableRowLimits"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetTableRowLimits);
    s_stubMap["ProcessDefinition/SetValidNumericRange"] = reinterpret_cast<void*>(stub_ProcessDefinition_SetValidNumericRange);
    s_stubMap["RealTimePreview/CloseRealTimePreviewProgressDialog"] = reinterpret_cast<void*>(stub_RealTimePreview_CloseRealTimePreviewProgressDialog);
    s_stubMap["RealTimePreview/IsRealTimePreviewProgressDialogVisible"] = reinterpret_cast<void*>(stub_RealTimePreview_IsRealTimePreviewProgressDialogVisible);
    s_stubMap["RealTimePreview/IsRealTimePreviewUpdating"] = reinterpret_cast<void*>(stub_RealTimePreview_IsRealTimePreviewUpdating);
    s_stubMap["RealTimePreview/SetRealTimePreviewOwner"] = reinterpret_cast<void*>(stub_RealTimePreview_SetRealTimePreviewOwner);
    s_stubMap["RealTimePreview/SetRealTimePreviewProgressCount"] = reinterpret_cast<void*>(stub_RealTimePreview_SetRealTimePreviewProgressCount);
    s_stubMap["RealTimePreview/SetRealTimePreviewProgressText"] = reinterpret_cast<void*>(stub_RealTimePreview_SetRealTimePreviewProgressText);
    s_stubMap["RealTimePreview/ShowRealTimePreviewProgressDialog"] = reinterpret_cast<void*>(stub_RealTimePreview_ShowRealTimePreviewProgressDialog);
    s_stubMap["RealTimePreview/UpdateRealTimePreview"] = reinterpret_cast<void*>(stub_RealTimePreview_UpdateRealTimePreview);
    s_stubMap["SVG/CreateSVGBuffer"] = reinterpret_cast<void*>(stub_SVG_CreateSVGBuffer);
    s_stubMap["SVG/CreateSVGFile"] = reinterpret_cast<void*>(stub_SVG_CreateSVGFile);
    s_stubMap["SVG/GetSVGDataBuffer"] = reinterpret_cast<void*>(stub_SVG_GetSVGDataBuffer);
    s_stubMap["SVG/GetSVGDescription"] = reinterpret_cast<void*>(stub_SVG_GetSVGDescription);
    s_stubMap["SVG/GetSVGDimensions"] = reinterpret_cast<void*>(stub_SVG_GetSVGDimensions);
    s_stubMap["SVG/GetSVGFilePath"] = reinterpret_cast<void*>(stub_SVG_GetSVGFilePath);
    s_stubMap["SVG/GetSVGResolution"] = reinterpret_cast<void*>(stub_SVG_GetSVGResolution);
    s_stubMap["SVG/GetSVGTitle"] = reinterpret_cast<void*>(stub_SVG_GetSVGTitle);
    s_stubMap["SVG/GetSVGViewBox"] = reinterpret_cast<void*>(stub_SVG_GetSVGViewBox);
    s_stubMap["SVG/IsSVGPainting"] = reinterpret_cast<void*>(stub_SVG_IsSVGPainting);
    s_stubMap["SVG/SetSVGDescription"] = reinterpret_cast<void*>(stub_SVG_SetSVGDescription);
    s_stubMap["SVG/SetSVGDimensions"] = reinterpret_cast<void*>(stub_SVG_SetSVGDimensions);
    s_stubMap["SVG/SetSVGResolution"] = reinterpret_cast<void*>(stub_SVG_SetSVGResolution);
    s_stubMap["SVG/SetSVGTitle"] = reinterpret_cast<void*>(stub_SVG_SetSVGTitle);
    s_stubMap["SVG/SetSVGViewBox"] = reinterpret_cast<void*>(stub_SVG_SetSVGViewBox);
    s_stubMap["ScrollBox/CreateScrollBox"] = reinterpret_cast<void*>(stub_ScrollBox_CreateScrollBox);
    s_stubMap["ScrollBox/CreateScrollBoxViewport"] = reinterpret_cast<void*>(stub_ScrollBox_CreateScrollBoxViewport);
    s_stubMap["ScrollBox/GetScrollBarsVisible"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBarsVisible);
    s_stubMap["ScrollBox/GetScrollBoxAutoScrollEnabled"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxAutoScrollEnabled);
    s_stubMap["ScrollBox/GetScrollBoxHorizontalRange"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxHorizontalRange);
    s_stubMap["ScrollBox/GetScrollBoxLineSize"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxLineSize);
    s_stubMap["ScrollBox/GetScrollBoxPageSize"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxPageSize);
    s_stubMap["ScrollBox/GetScrollBoxPosition"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxPosition);
    s_stubMap["ScrollBox/GetScrollBoxTrackingEnabled"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxTrackingEnabled);
    s_stubMap["ScrollBox/GetScrollBoxVerticalRange"] = reinterpret_cast<void*>(stub_ScrollBox_GetScrollBoxVerticalRange);
    s_stubMap["ScrollBox/SetScrollBarsVisible"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBarsVisible);
    s_stubMap["ScrollBox/SetScrollBoxAutoScrollEnabled"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxAutoScrollEnabled);
    s_stubMap["ScrollBox/SetScrollBoxHorizontalPosUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxHorizontalPosUpdatedEventRoutine);
    s_stubMap["ScrollBox/SetScrollBoxHorizontalRange"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxHorizontalRange);
    s_stubMap["ScrollBox/SetScrollBoxHorizontalRangeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxHorizontalRangeUpdatedEventRoutine);
    s_stubMap["ScrollBox/SetScrollBoxLineSize"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxLineSize);
    s_stubMap["ScrollBox/SetScrollBoxPageSize"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxPageSize);
    s_stubMap["ScrollBox/SetScrollBoxPosition"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxPosition);
    s_stubMap["ScrollBox/SetScrollBoxTrackingEnabled"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxTrackingEnabled);
    s_stubMap["ScrollBox/SetScrollBoxVerticalPosUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxVerticalPosUpdatedEventRoutine);
    s_stubMap["ScrollBox/SetScrollBoxVerticalRange"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxVerticalRange);
    s_stubMap["ScrollBox/SetScrollBoxVerticalRangeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ScrollBox_SetScrollBoxVerticalRangeUpdatedEventRoutine);
    s_stubMap["SharedImage/AttachToImage"] = reinterpret_cast<void*>(stub_SharedImage_AttachToImage);
    s_stubMap["SharedImage/CreateImage"] = reinterpret_cast<void*>(stub_SharedImage_CreateImage);
    s_stubMap["SharedImage/DetachFromImage"] = reinterpret_cast<void*>(stub_SharedImage_DetachFromImage);
    s_stubMap["SharedImage/GetImageColorSpace"] = reinterpret_cast<void*>(stub_SharedImage_GetImageColorSpace);
    s_stubMap["SharedImage/GetImageFormat"] = reinterpret_cast<void*>(stub_SharedImage_GetImageFormat);
    s_stubMap["SharedImage/GetImageGeometry"] = reinterpret_cast<void*>(stub_SharedImage_GetImageGeometry);
    s_stubMap["SharedImage/GetImageOwner"] = reinterpret_cast<void*>(stub_SharedImage_GetImageOwner);
    s_stubMap["SharedImage/GetImagePixelData"] = reinterpret_cast<void*>(stub_SharedImage_GetImagePixelData);
    s_stubMap["SharedImage/GetImageRGBWS"] = reinterpret_cast<void*>(stub_SharedImage_GetImageRGBWS);
    s_stubMap["SharedImage/GetImageRefCount"] = reinterpret_cast<void*>(stub_SharedImage_GetImageRefCount);
    s_stubMap["SharedImage/IsValidImageHandle"] = reinterpret_cast<void*>(stub_SharedImage_IsValidImageHandle);
    s_stubMap["SharedImage/SetImageColorSpace"] = reinterpret_cast<void*>(stub_SharedImage_SetImageColorSpace);
    s_stubMap["SharedImage/SetImageGeometry"] = reinterpret_cast<void*>(stub_SharedImage_SetImageGeometry);
    s_stubMap["SharedImage/SetImagePixelData"] = reinterpret_cast<void*>(stub_SharedImage_SetImagePixelData);
    s_stubMap["SharedImage/SetImageRGBWS"] = reinterpret_cast<void*>(stub_SharedImage_SetImageRGBWS);
    s_stubMap["Sizer/CreateSizer"] = reinterpret_cast<void*>(stub_Sizer_CreateSizer);
    s_stubMap["Sizer/GetSizerControlIndex"] = reinterpret_cast<void*>(stub_Sizer_GetSizerControlIndex);
    s_stubMap["Sizer/GetSizerCount"] = reinterpret_cast<void*>(stub_Sizer_GetSizerCount);
    s_stubMap["Sizer/GetSizerDevicePixelRatio"] = reinterpret_cast<void*>(stub_Sizer_GetSizerDevicePixelRatio);
    s_stubMap["Sizer/GetSizerDisplayPixelRatio"] = reinterpret_cast<void*>(stub_Sizer_GetSizerDisplayPixelRatio);
    s_stubMap["Sizer/GetSizerIndex"] = reinterpret_cast<void*>(stub_Sizer_GetSizerIndex);
    s_stubMap["Sizer/GetSizerMargin"] = reinterpret_cast<void*>(stub_Sizer_GetSizerMargin);
    s_stubMap["Sizer/GetSizerOrientation"] = reinterpret_cast<void*>(stub_Sizer_GetSizerOrientation);
    s_stubMap["Sizer/GetSizerParentControl"] = reinterpret_cast<void*>(stub_Sizer_GetSizerParentControl);
    s_stubMap["Sizer/GetSizerResourcePixelRatio"] = reinterpret_cast<void*>(stub_Sizer_GetSizerResourcePixelRatio);
    s_stubMap["Sizer/GetSizerSpacing"] = reinterpret_cast<void*>(stub_Sizer_GetSizerSpacing);
    s_stubMap["Sizer/InsertSizer"] = reinterpret_cast<void*>(stub_Sizer_InsertSizer);
    s_stubMap["Sizer/InsertSizerControl"] = reinterpret_cast<void*>(stub_Sizer_InsertSizerControl);
    s_stubMap["Sizer/InsertSizerSpacing"] = reinterpret_cast<void*>(stub_Sizer_InsertSizerSpacing);
    s_stubMap["Sizer/InsertSizerStretch"] = reinterpret_cast<void*>(stub_Sizer_InsertSizerStretch);
    s_stubMap["Sizer/RemoveSizer"] = reinterpret_cast<void*>(stub_Sizer_RemoveSizer);
    s_stubMap["Sizer/RemoveSizerControl"] = reinterpret_cast<void*>(stub_Sizer_RemoveSizerControl);
    s_stubMap["Sizer/SetSizerAlignment"] = reinterpret_cast<void*>(stub_Sizer_SetSizerAlignment);
    s_stubMap["Sizer/SetSizerControlAlignment"] = reinterpret_cast<void*>(stub_Sizer_SetSizerControlAlignment);
    s_stubMap["Sizer/SetSizerControlStretchFactor"] = reinterpret_cast<void*>(stub_Sizer_SetSizerControlStretchFactor);
    s_stubMap["Sizer/SetSizerMargin"] = reinterpret_cast<void*>(stub_Sizer_SetSizerMargin);
    s_stubMap["Sizer/SetSizerSpacing"] = reinterpret_cast<void*>(stub_Sizer_SetSizerSpacing);
    s_stubMap["Sizer/SetSizerStretchFactor"] = reinterpret_cast<void*>(stub_Sizer_SetSizerStretchFactor);
    s_stubMap["Slider/CreateSlider"] = reinterpret_cast<void*>(stub_Slider_CreateSlider);
    s_stubMap["Slider/GetSliderPageSize"] = reinterpret_cast<void*>(stub_Slider_GetSliderPageSize);
    s_stubMap["Slider/GetSliderRange"] = reinterpret_cast<void*>(stub_Slider_GetSliderRange);
    s_stubMap["Slider/GetSliderStepSize"] = reinterpret_cast<void*>(stub_Slider_GetSliderStepSize);
    s_stubMap["Slider/GetSliderTickInterval"] = reinterpret_cast<void*>(stub_Slider_GetSliderTickInterval);
    s_stubMap["Slider/GetSliderTickStyle"] = reinterpret_cast<void*>(stub_Slider_GetSliderTickStyle);
    s_stubMap["Slider/GetSliderTrackingEnabled"] = reinterpret_cast<void*>(stub_Slider_GetSliderTrackingEnabled);
    s_stubMap["Slider/GetSliderValue"] = reinterpret_cast<void*>(stub_Slider_GetSliderValue);
    s_stubMap["Slider/SetSliderPageSize"] = reinterpret_cast<void*>(stub_Slider_SetSliderPageSize);
    s_stubMap["Slider/SetSliderRange"] = reinterpret_cast<void*>(stub_Slider_SetSliderRange);
    s_stubMap["Slider/SetSliderRangeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_Slider_SetSliderRangeUpdatedEventRoutine);
    s_stubMap["Slider/SetSliderStepSize"] = reinterpret_cast<void*>(stub_Slider_SetSliderStepSize);
    s_stubMap["Slider/SetSliderTickInterval"] = reinterpret_cast<void*>(stub_Slider_SetSliderTickInterval);
    s_stubMap["Slider/SetSliderTickStyle"] = reinterpret_cast<void*>(stub_Slider_SetSliderTickStyle);
    s_stubMap["Slider/SetSliderTrackingEnabled"] = reinterpret_cast<void*>(stub_Slider_SetSliderTrackingEnabled);
    s_stubMap["Slider/SetSliderValue"] = reinterpret_cast<void*>(stub_Slider_SetSliderValue);
    s_stubMap["Slider/SetSliderValueUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_Slider_SetSliderValueUpdatedEventRoutine);
    s_stubMap["SpinBox/CreateSpinBox"] = reinterpret_cast<void*>(stub_SpinBox_CreateSpinBox);
    s_stubMap["SpinBox/GetSpinBoxAlignment"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxAlignment);
    s_stubMap["SpinBox/GetSpinBoxEditable"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxEditable);
    s_stubMap["SpinBox/GetSpinBoxMinimumValueText"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxMinimumValueText);
    s_stubMap["SpinBox/GetSpinBoxPrefix"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxPrefix);
    s_stubMap["SpinBox/GetSpinBoxRange"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxRange);
    s_stubMap["SpinBox/GetSpinBoxStepSize"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxStepSize);
    s_stubMap["SpinBox/GetSpinBoxSuffix"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxSuffix);
    s_stubMap["SpinBox/GetSpinBoxValue"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxValue);
    s_stubMap["SpinBox/GetSpinBoxWrappingEnabled"] = reinterpret_cast<void*>(stub_SpinBox_GetSpinBoxWrappingEnabled);
    s_stubMap["SpinBox/SetSpinBoxAlignment"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxAlignment);
    s_stubMap["SpinBox/SetSpinBoxEditable"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxEditable);
    s_stubMap["SpinBox/SetSpinBoxMinimumValueText"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxMinimumValueText);
    s_stubMap["SpinBox/SetSpinBoxPrefix"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxPrefix);
    s_stubMap["SpinBox/SetSpinBoxRange"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxRange);
    s_stubMap["SpinBox/SetSpinBoxRangeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxRangeUpdatedEventRoutine);
    s_stubMap["SpinBox/SetSpinBoxStepSize"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxStepSize);
    s_stubMap["SpinBox/SetSpinBoxSuffix"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxSuffix);
    s_stubMap["SpinBox/SetSpinBoxValue"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxValue);
    s_stubMap["SpinBox/SetSpinBoxValueUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxValueUpdatedEventRoutine);
    s_stubMap["SpinBox/SetSpinBoxWrappingEnabled"] = reinterpret_cast<void*>(stub_SpinBox_SetSpinBoxWrappingEnabled);
    s_stubMap["TabBox/CreateTabBox"] = reinterpret_cast<void*>(stub_TabBox_CreateTabBox);
    s_stubMap["TabBox/GetTabBoxCurrentPageIndex"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxCurrentPageIndex);
    s_stubMap["TabBox/GetTabBoxLeftControl"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxLeftControl);
    s_stubMap["TabBox/GetTabBoxLength"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxLength);
    s_stubMap["TabBox/GetTabBoxPageByIndex"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPageByIndex);
    s_stubMap["TabBox/GetTabBoxPageEnabled"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPageEnabled);
    s_stubMap["TabBox/GetTabBoxPageIcon"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPageIcon);
    s_stubMap["TabBox/GetTabBoxPageLabel"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPageLabel);
    s_stubMap["TabBox/GetTabBoxPageToolTip"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPageToolTip);
    s_stubMap["TabBox/GetTabBoxPosition"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxPosition);
    s_stubMap["TabBox/GetTabBoxRightControl"] = reinterpret_cast<void*>(stub_TabBox_GetTabBoxRightControl);
    s_stubMap["TabBox/InsertTabBoxPage"] = reinterpret_cast<void*>(stub_TabBox_InsertTabBoxPage);
    s_stubMap["TabBox/RemoveTabBoxPage"] = reinterpret_cast<void*>(stub_TabBox_RemoveTabBoxPage);
    s_stubMap["TabBox/SetTabBoxControls"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxControls);
    s_stubMap["TabBox/SetTabBoxCurrentPageIndex"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxCurrentPageIndex);
    s_stubMap["TabBox/SetTabBoxPageEnabled"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPageEnabled);
    s_stubMap["TabBox/SetTabBoxPageIcon"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPageIcon);
    s_stubMap["TabBox/SetTabBoxPageLabel"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPageLabel);
    s_stubMap["TabBox/SetTabBoxPageSelectedEventRoutine"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPageSelectedEventRoutine);
    s_stubMap["TabBox/SetTabBoxPageToolTip"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPageToolTip);
    s_stubMap["TabBox/SetTabBoxPosition"] = reinterpret_cast<void*>(stub_TabBox_SetTabBoxPosition);
    s_stubMap["TextBox/CreateTextBox"] = reinterpret_cast<void*>(stub_TextBox_CreateTextBox);
    s_stubMap["TextBox/DeleteTextBoxText"] = reinterpret_cast<void*>(stub_TextBox_DeleteTextBoxText);
    s_stubMap["TextBox/GetTextBoxCaretPosition"] = reinterpret_cast<void*>(stub_TextBox_GetTextBoxCaretPosition);
    s_stubMap["TextBox/GetTextBoxReadOnly"] = reinterpret_cast<void*>(stub_TextBox_GetTextBoxReadOnly);
    s_stubMap["TextBox/GetTextBoxSelectedText"] = reinterpret_cast<void*>(stub_TextBox_GetTextBoxSelectedText);
    s_stubMap["TextBox/GetTextBoxSelection"] = reinterpret_cast<void*>(stub_TextBox_GetTextBoxSelection);
    s_stubMap["TextBox/GetTextBoxText"] = reinterpret_cast<void*>(stub_TextBox_GetTextBoxText);
    s_stubMap["TextBox/InsertTextBoxText"] = reinterpret_cast<void*>(stub_TextBox_InsertTextBoxText);
    s_stubMap["TextBox/SetTextBoxCaretPosition"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxCaretPosition);
    s_stubMap["TextBox/SetTextBoxCaretPositionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxCaretPositionUpdatedEventRoutine);
    s_stubMap["TextBox/SetTextBoxReadOnly"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxReadOnly);
    s_stubMap["TextBox/SetTextBoxSelected"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxSelected);
    s_stubMap["TextBox/SetTextBoxSelection"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxSelection);
    s_stubMap["TextBox/SetTextBoxSelectionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxSelectionUpdatedEventRoutine);
    s_stubMap["TextBox/SetTextBoxText"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxText);
    s_stubMap["TextBox/SetTextBoxUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TextBox_SetTextBoxUpdatedEventRoutine);
    s_stubMap["Thread/AppendThreadConsoleOutputText"] = reinterpret_cast<void*>(stub_Thread_AppendThreadConsoleOutputText);
    s_stubMap["Thread/ClearThreadConsoleOutputText"] = reinterpret_cast<void*>(stub_Thread_ClearThreadConsoleOutputText);
    s_stubMap["Thread/CreateThread"] = reinterpret_cast<void*>(stub_Thread_CreateThread);
    s_stubMap["Thread/GetCurrentThread"] = reinterpret_cast<void*>(stub_Thread_GetCurrentThread);
    s_stubMap["Thread/GetThreadConsoleOutputText"] = reinterpret_cast<void*>(stub_Thread_GetThreadConsoleOutputText);
    s_stubMap["Thread/GetThreadPriority"] = reinterpret_cast<void*>(stub_Thread_GetThreadPriority);
    s_stubMap["Thread/GetThreadStackSize"] = reinterpret_cast<void*>(stub_Thread_GetThreadStackSize);
    s_stubMap["Thread/GetThreadStatus"] = reinterpret_cast<void*>(stub_Thread_GetThreadStatus);
    s_stubMap["Thread/GetThreadStatusEx"] = reinterpret_cast<void*>(stub_Thread_GetThreadStatusEx);
    s_stubMap["Thread/IsThreadActive"] = reinterpret_cast<void*>(stub_Thread_IsThreadActive);
    s_stubMap["Thread/KillThread"] = reinterpret_cast<void*>(stub_Thread_KillThread);
    s_stubMap["Thread/PerformanceAnalysisValue"] = reinterpret_cast<void*>(stub_Thread_PerformanceAnalysisValue);
    s_stubMap["Thread/SetThreadExecRoutine"] = reinterpret_cast<void*>(stub_Thread_SetThreadExecRoutine);
    s_stubMap["Thread/SetThreadPriority"] = reinterpret_cast<void*>(stub_Thread_SetThreadPriority);
    s_stubMap["Thread/SetThreadStackSize"] = reinterpret_cast<void*>(stub_Thread_SetThreadStackSize);
    s_stubMap["Thread/SetThreadStatus"] = reinterpret_cast<void*>(stub_Thread_SetThreadStatus);
    s_stubMap["Thread/SleepThread"] = reinterpret_cast<void*>(stub_Thread_SleepThread);
    s_stubMap["Thread/StartThread"] = reinterpret_cast<void*>(stub_Thread_StartThread);
    s_stubMap["Thread/WaitThread"] = reinterpret_cast<void*>(stub_Thread_WaitThread);
    s_stubMap["Timer/CreateTimer"] = reinterpret_cast<void*>(stub_Timer_CreateTimer);
    s_stubMap["Timer/GetTimerInterval"] = reinterpret_cast<void*>(stub_Timer_GetTimerInterval);
    s_stubMap["Timer/GetTimerSingleShot"] = reinterpret_cast<void*>(stub_Timer_GetTimerSingleShot);
    s_stubMap["Timer/IsTimerActive"] = reinterpret_cast<void*>(stub_Timer_IsTimerActive);
    s_stubMap["Timer/SetTimerInterval"] = reinterpret_cast<void*>(stub_Timer_SetTimerInterval);
    s_stubMap["Timer/SetTimerNotifyEventRoutine"] = reinterpret_cast<void*>(stub_Timer_SetTimerNotifyEventRoutine);
    s_stubMap["Timer/SetTimerSingleShot"] = reinterpret_cast<void*>(stub_Timer_SetTimerSingleShot);
    s_stubMap["Timer/StartTimer"] = reinterpret_cast<void*>(stub_Timer_StartTimer);
    s_stubMap["Timer/StopTimer"] = reinterpret_cast<void*>(stub_Timer_StopTimer);
    s_stubMap["TreeBox/AdjustTreeBoxColumnWidthToContents"] = reinterpret_cast<void*>(stub_TreeBox_AdjustTreeBoxColumnWidthToContents);
    s_stubMap["TreeBox/BeginTreeBoxNodeEdition"] = reinterpret_cast<void*>(stub_TreeBox_BeginTreeBoxNodeEdition);
    s_stubMap["TreeBox/ClearTreeBox"] = reinterpret_cast<void*>(stub_TreeBox_ClearTreeBox);
    s_stubMap["TreeBox/CreateTreeBox"] = reinterpret_cast<void*>(stub_TreeBox_CreateTreeBox);
    s_stubMap["TreeBox/CreateTreeBoxNode"] = reinterpret_cast<void*>(stub_TreeBox_CreateTreeBoxNode);
    s_stubMap["TreeBox/CreateTreeBoxViewport"] = reinterpret_cast<void*>(stub_TreeBox_CreateTreeBoxViewport);
    s_stubMap["TreeBox/EditTreeBoxNode"] = reinterpret_cast<void*>(stub_TreeBox_EditTreeBoxNode);
    s_stubMap["TreeBox/EndTreeBoxNodeEdition"] = reinterpret_cast<void*>(stub_TreeBox_EndTreeBoxNodeEdition);
    s_stubMap["TreeBox/GetTreeBoxAlternateRowColorEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxAlternateRowColorEnabled);
    s_stubMap["TreeBox/GetTreeBoxChild"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxChild);
    s_stubMap["TreeBox/GetTreeBoxChildCount"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxChildCount);
    s_stubMap["TreeBox/GetTreeBoxChildIndex"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxChildIndex);
    s_stubMap["TreeBox/GetTreeBoxColumnCount"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxColumnCount);
    s_stubMap["TreeBox/GetTreeBoxColumnVisible"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxColumnVisible);
    s_stubMap["TreeBox/GetTreeBoxColumnWidth"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxColumnWidth);
    s_stubMap["TreeBox/GetTreeBoxCurrentNode"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxCurrentNode);
    s_stubMap["TreeBox/GetTreeBoxHeaderAlignment"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxHeaderAlignment);
    s_stubMap["TreeBox/GetTreeBoxHeaderIcon"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxHeaderIcon);
    s_stubMap["TreeBox/GetTreeBoxHeaderSortingEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxHeaderSortingEnabled);
    s_stubMap["TreeBox/GetTreeBoxHeaderText"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxHeaderText);
    s_stubMap["TreeBox/GetTreeBoxHeaderVisible"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxHeaderVisible);
    s_stubMap["TreeBox/GetTreeBoxIconSize"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxIconSize);
    s_stubMap["TreeBox/GetTreeBoxIndentSize"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxIndentSize);
    s_stubMap["TreeBox/GetTreeBoxMultipleNodeSelectionEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxMultipleNodeSelectionEnabled);
    s_stubMap["TreeBox/GetTreeBoxNodeByPos"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeByPos);
    s_stubMap["TreeBox/GetTreeBoxNodeCheckable"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeCheckable);
    s_stubMap["TreeBox/GetTreeBoxNodeChecked"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeChecked);
    s_stubMap["TreeBox/GetTreeBoxNodeChild"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeChild);
    s_stubMap["TreeBox/GetTreeBoxNodeChildCount"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeChildCount);
    s_stubMap["TreeBox/GetTreeBoxNodeColAlignment"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColAlignment);
    s_stubMap["TreeBox/GetTreeBoxNodeColBackgroundColor"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColBackgroundColor);
    s_stubMap["TreeBox/GetTreeBoxNodeColFont"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColFont);
    s_stubMap["TreeBox/GetTreeBoxNodeColIcon"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColIcon);
    s_stubMap["TreeBox/GetTreeBoxNodeColText"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColText);
    s_stubMap["TreeBox/GetTreeBoxNodeColTextColor"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColTextColor);
    s_stubMap["TreeBox/GetTreeBoxNodeColToolTip"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeColToolTip);
    s_stubMap["TreeBox/GetTreeBoxNodeDraggingEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeDraggingEnabled);
    s_stubMap["TreeBox/GetTreeBoxNodeEditable"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeEditable);
    s_stubMap["TreeBox/GetTreeBoxNodeEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeEnabled);
    s_stubMap["TreeBox/GetTreeBoxNodeExpanded"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeExpanded);
    s_stubMap["TreeBox/GetTreeBoxNodeExpansionEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeExpansionEnabled);
    s_stubMap["TreeBox/GetTreeBoxNodeFirstColumnSpanned"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeFirstColumnSpanned);
    s_stubMap["TreeBox/GetTreeBoxNodeParent"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeParent);
    s_stubMap["TreeBox/GetTreeBoxNodeParentBox"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeParentBox);
    s_stubMap["TreeBox/GetTreeBoxNodeRect"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeRect);
    s_stubMap["TreeBox/GetTreeBoxNodeSelectable"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeSelectable);
    s_stubMap["TreeBox/GetTreeBoxNodeSelected"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxNodeSelected);
    s_stubMap["TreeBox/GetTreeBoxRootDecorationEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxRootDecorationEnabled);
    s_stubMap["TreeBox/GetTreeBoxSelectedNodes"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxSelectedNodes);
    s_stubMap["TreeBox/GetTreeBoxUniformRowHeightEnabled"] = reinterpret_cast<void*>(stub_TreeBox_GetTreeBoxUniformRowHeightEnabled);
    s_stubMap["TreeBox/InsertTreeBoxNode"] = reinterpret_cast<void*>(stub_TreeBox_InsertTreeBoxNode);
    s_stubMap["TreeBox/InsertTreeBoxNodeChild"] = reinterpret_cast<void*>(stub_TreeBox_InsertTreeBoxNodeChild);
    s_stubMap["TreeBox/RemoveTreeBoxNode"] = reinterpret_cast<void*>(stub_TreeBox_RemoveTreeBoxNode);
    s_stubMap["TreeBox/RemoveTreeBoxNodeChild"] = reinterpret_cast<void*>(stub_TreeBox_RemoveTreeBoxNodeChild);
    s_stubMap["TreeBox/SelectAllTreeBoxNodes"] = reinterpret_cast<void*>(stub_TreeBox_SelectAllTreeBoxNodes);
    s_stubMap["TreeBox/SetTreeBoxAlternateRowColorEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxAlternateRowColorEnabled);
    s_stubMap["TreeBox/SetTreeBoxColumnCount"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxColumnCount);
    s_stubMap["TreeBox/SetTreeBoxColumnVisible"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxColumnVisible);
    s_stubMap["TreeBox/SetTreeBoxColumnWidth"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxColumnWidth);
    s_stubMap["TreeBox/SetTreeBoxCurrentNode"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxCurrentNode);
    s_stubMap["TreeBox/SetTreeBoxCurrentNodeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxCurrentNodeUpdatedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxHeaderAlignment"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxHeaderAlignment);
    s_stubMap["TreeBox/SetTreeBoxHeaderIcon"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxHeaderIcon);
    s_stubMap["TreeBox/SetTreeBoxHeaderSortingEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxHeaderSortingEnabled);
    s_stubMap["TreeBox/SetTreeBoxHeaderText"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxHeaderText);
    s_stubMap["TreeBox/SetTreeBoxHeaderVisible"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxHeaderVisible);
    s_stubMap["TreeBox/SetTreeBoxIconSize"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxIconSize);
    s_stubMap["TreeBox/SetTreeBoxIndentSize"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxIndentSize);
    s_stubMap["TreeBox/SetTreeBoxMultipleNodeSelectionEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxMultipleNodeSelectionEnabled);
    s_stubMap["TreeBox/SetTreeBoxNodeActivatedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeActivatedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeCheckable"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeCheckable);
    s_stubMap["TreeBox/SetTreeBoxNodeChecked"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeChecked);
    s_stubMap["TreeBox/SetTreeBoxNodeClickedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeClickedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeColAlignment"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColAlignment);
    s_stubMap["TreeBox/SetTreeBoxNodeColBackgroundColor"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColBackgroundColor);
    s_stubMap["TreeBox/SetTreeBoxNodeColFont"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColFont);
    s_stubMap["TreeBox/SetTreeBoxNodeColIcon"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColIcon);
    s_stubMap["TreeBox/SetTreeBoxNodeColText"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColText);
    s_stubMap["TreeBox/SetTreeBoxNodeColTextColor"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColTextColor);
    s_stubMap["TreeBox/SetTreeBoxNodeColToolTip"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeColToolTip);
    s_stubMap["TreeBox/SetTreeBoxNodeCollapsedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeCollapsedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeDoubleClickedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeDoubleClickedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeDraggingEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeDraggingEnabled);
    s_stubMap["TreeBox/SetTreeBoxNodeEditable"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeEditable);
    s_stubMap["TreeBox/SetTreeBoxNodeEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeEnabled);
    s_stubMap["TreeBox/SetTreeBoxNodeEnteredEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeEnteredEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeExpanded"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeExpanded);
    s_stubMap["TreeBox/SetTreeBoxNodeExpandedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeExpandedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeExpansionEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeExpansionEnabled);
    s_stubMap["TreeBox/SetTreeBoxNodeFirstColumnSpanned"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeFirstColumnSpanned);
    s_stubMap["TreeBox/SetTreeBoxNodeIntoView"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeIntoView);
    s_stubMap["TreeBox/SetTreeBoxNodeSelectable"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeSelectable);
    s_stubMap["TreeBox/SetTreeBoxNodeSelected"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeSelected);
    s_stubMap["TreeBox/SetTreeBoxNodeSelectionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeSelectionUpdatedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxNodeUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxNodeUpdatedEventRoutine);
    s_stubMap["TreeBox/SetTreeBoxRootDecorationEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxRootDecorationEnabled);
    s_stubMap["TreeBox/SetTreeBoxUniformRowHeightEnabled"] = reinterpret_cast<void*>(stub_TreeBox_SetTreeBoxUniformRowHeightEnabled);
    s_stubMap["TreeBox/SortTreeBox"] = reinterpret_cast<void*>(stub_TreeBox_SortTreeBox);
    s_stubMap["UI/AttachToUIObject"] = reinterpret_cast<void*>(stub_UI_AttachToUIObject);
    s_stubMap["UI/DetachFromUIObject"] = reinterpret_cast<void*>(stub_UI_DetachFromUIObject);
    s_stubMap["UI/GetUIObjectId"] = reinterpret_cast<void*>(stub_UI_GetUIObjectId);
    s_stubMap["UI/GetUIObjectModule"] = reinterpret_cast<void*>(stub_UI_GetUIObjectModule);
    s_stubMap["UI/GetUIObjectRefCount"] = reinterpret_cast<void*>(stub_UI_GetUIObjectRefCount);
    s_stubMap["UI/GetUIObjectType"] = reinterpret_cast<void*>(stub_UI_GetUIObjectType);
    s_stubMap["UI/SetHandleDestroyedEventRoutine"] = reinterpret_cast<void*>(stub_UI_SetHandleDestroyedEventRoutine);
    s_stubMap["UI/SetUIObjectId"] = reinterpret_cast<void*>(stub_UI_SetUIObjectId);
    s_stubMap["View/AddViewToDynamicTargets"] = reinterpret_cast<void*>(stub_View_AddViewToDynamicTargets);
    s_stubMap["View/ComputeViewProperty"] = reinterpret_cast<void*>(stub_View_ComputeViewProperty);
    s_stubMap["View/DeleteViewProperty"] = reinterpret_cast<void*>(stub_View_DeleteViewProperty);
    s_stubMap["View/DestroyViewScreenTransferFunctions"] = reinterpret_cast<void*>(stub_View_DestroyViewScreenTransferFunctions);
    s_stubMap["View/EnumerateViewProperties"] = reinterpret_cast<void*>(stub_View_EnumerateViewProperties);
    s_stubMap["View/EnumerateViews"] = reinterpret_cast<void*>(stub_View_EnumerateViews);
    s_stubMap["View/GetViewById"] = reinterpret_cast<void*>(stub_View_GetViewById);
    s_stubMap["View/GetViewDimensions"] = reinterpret_cast<void*>(stub_View_GetViewDimensions);
    s_stubMap["View/GetViewFullId"] = reinterpret_cast<void*>(stub_View_GetViewFullId);
    s_stubMap["View/GetViewId"] = reinterpret_cast<void*>(stub_View_GetViewId);
    s_stubMap["View/GetViewImage"] = reinterpret_cast<void*>(stub_View_GetViewImage);
    s_stubMap["View/GetViewLocks"] = reinterpret_cast<void*>(stub_View_GetViewLocks);
    s_stubMap["View/GetViewParentWindow"] = reinterpret_cast<void*>(stub_View_GetViewParentWindow);
    s_stubMap["View/GetViewPropertyAttributes"] = reinterpret_cast<void*>(stub_View_GetViewPropertyAttributes);
    s_stubMap["View/GetViewPropertyExists"] = reinterpret_cast<void*>(stub_View_GetViewPropertyExists);
    s_stubMap["View/GetViewPropertyValue"] = reinterpret_cast<void*>(stub_View_GetViewPropertyValue);
    s_stubMap["View/GetViewScreenTransferFunctions"] = reinterpret_cast<void*>(stub_View_GetViewScreenTransferFunctions);
    s_stubMap["View/GetViewScreenTransferFunctionsEnabled"] = reinterpret_cast<void*>(stub_View_GetViewScreenTransferFunctionsEnabled);
    s_stubMap["View/IsPreview"] = reinterpret_cast<void*>(stub_View_IsPreview);
    s_stubMap["View/IsReservedViewPropertyId"] = reinterpret_cast<void*>(stub_View_IsReservedViewPropertyId);
    s_stubMap["View/IsStoredPreview"] = reinterpret_cast<void*>(stub_View_IsStoredPreview);
    s_stubMap["View/IsViewColorImage"] = reinterpret_cast<void*>(stub_View_IsViewColorImage);
    s_stubMap["View/IsViewDynamicTarget"] = reinterpret_cast<void*>(stub_View_IsViewDynamicTarget);
    s_stubMap["View/IsVolatilePreview"] = reinterpret_cast<void*>(stub_View_IsVolatilePreview);
    s_stubMap["View/LockView"] = reinterpret_cast<void*>(stub_View_LockView);
    s_stubMap["View/RemoveViewFromDynamicTargets"] = reinterpret_cast<void*>(stub_View_RemoveViewFromDynamicTargets);
    s_stubMap["View/SetViewId"] = reinterpret_cast<void*>(stub_View_SetViewId);
    s_stubMap["View/SetViewPropertyAttributes"] = reinterpret_cast<void*>(stub_View_SetViewPropertyAttributes);
    s_stubMap["View/SetViewPropertyValue"] = reinterpret_cast<void*>(stub_View_SetViewPropertyValue);
    s_stubMap["View/SetViewScreenTransferFunctions"] = reinterpret_cast<void*>(stub_View_SetViewScreenTransferFunctions);
    s_stubMap["View/SetViewScreenTransferFunctionsEnabled"] = reinterpret_cast<void*>(stub_View_SetViewScreenTransferFunctionsEnabled);
    s_stubMap["View/UnlockView"] = reinterpret_cast<void*>(stub_View_UnlockView);
    s_stubMap["ViewList/CreateViewList"] = reinterpret_cast<void*>(stub_ViewList_CreateViewList);
    s_stubMap["ViewList/FindViewListView"] = reinterpret_cast<void*>(stub_ViewList_FindViewListView);
    s_stubMap["ViewList/GetViewListContents"] = reinterpret_cast<void*>(stub_ViewList_GetViewListContents);
    s_stubMap["ViewList/GetViewListCurrentView"] = reinterpret_cast<void*>(stub_ViewList_GetViewListCurrentView);
    s_stubMap["ViewList/GetViewListExcludedView"] = reinterpret_cast<void*>(stub_ViewList_GetViewListExcludedView);
    s_stubMap["ViewList/RegenerateViewList"] = reinterpret_cast<void*>(stub_ViewList_RegenerateViewList);
    s_stubMap["ViewList/RemoveViewListView"] = reinterpret_cast<void*>(stub_ViewList_RemoveViewListView);
    s_stubMap["ViewList/SetViewListCurrentView"] = reinterpret_cast<void*>(stub_ViewList_SetViewListCurrentView);
    s_stubMap["ViewList/SetViewListCurrentViewUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_ViewList_SetViewListCurrentViewUpdatedEventRoutine);
    s_stubMap["ViewList/SetViewListExcludedView"] = reinterpret_cast<void*>(stub_ViewList_SetViewListExcludedView);
    s_stubMap["ViewList/SetViewListViewSelectedEventRoutine"] = reinterpret_cast<void*>(stub_ViewList_SetViewListViewSelectedEventRoutine);
    s_stubMap["WebView/CreateWebView"] = reinterpret_cast<void*>(stub_WebView_CreateWebView);
    s_stubMap["WebView/EvaluateWebViewScript"] = reinterpret_cast<void*>(stub_WebView_EvaluateWebViewScript);
    s_stubMap["WebView/GetWebViewBackgroundColor"] = reinterpret_cast<void*>(stub_WebView_GetWebViewBackgroundColor);
    s_stubMap["WebView/GetWebViewHasSelection"] = reinterpret_cast<void*>(stub_WebView_GetWebViewHasSelection);
    s_stubMap["WebView/GetWebViewSelectedText"] = reinterpret_cast<void*>(stub_WebView_GetWebViewSelectedText);
    s_stubMap["WebView/GetWebViewZoomFactor"] = reinterpret_cast<void*>(stub_WebView_GetWebViewZoomFactor);
    s_stubMap["WebView/LoadWebViewContent"] = reinterpret_cast<void*>(stub_WebView_LoadWebViewContent);
    s_stubMap["WebView/ReloadWebView"] = reinterpret_cast<void*>(stub_WebView_ReloadWebView);
    s_stubMap["WebView/RequestWebViewHTML"] = reinterpret_cast<void*>(stub_WebView_RequestWebViewHTML);
    s_stubMap["WebView/RequestWebViewPlainText"] = reinterpret_cast<void*>(stub_WebView_RequestWebViewPlainText);
    s_stubMap["WebView/SaveWebViewAsPDF"] = reinterpret_cast<void*>(stub_WebView_SaveWebViewAsPDF);
    s_stubMap["WebView/SetWebViewBackgroundColor"] = reinterpret_cast<void*>(stub_WebView_SetWebViewBackgroundColor);
    s_stubMap["WebView/SetWebViewContent"] = reinterpret_cast<void*>(stub_WebView_SetWebViewContent);
    s_stubMap["WebView/SetWebViewHTMLAvailableEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewHTMLAvailableEventRoutine);
    s_stubMap["WebView/SetWebViewLoadFinishedEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewLoadFinishedEventRoutine);
    s_stubMap["WebView/SetWebViewLoadProgressEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewLoadProgressEventRoutine);
    s_stubMap["WebView/SetWebViewLoadStartedEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewLoadStartedEventRoutine);
    s_stubMap["WebView/SetWebViewPlainTextAvailableEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewPlainTextAvailableEventRoutine);
    s_stubMap["WebView/SetWebViewScriptResultAvailableEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewScriptResultAvailableEventRoutine);
    s_stubMap["WebView/SetWebViewSelectionUpdatedEventRoutine"] = reinterpret_cast<void*>(stub_WebView_SetWebViewSelectionUpdatedEventRoutine);
    s_stubMap["WebView/SetWebViewZoomFactor"] = reinterpret_cast<void*>(stub_WebView_SetWebViewZoomFactor);
    s_stubMap["WebView/StopWebView"] = reinterpret_cast<void*>(stub_WebView_StopWebView);
}

void* getPCLStub(const std::string& name) {
    auto it = s_stubMap.find(name);
    if (it != s_stubMap.end()) {
        return it->second;
    }
    qWarning() << "PCL STUB NOT FOUND FOR:" << name.c_str();
    return nullptr;
}

void overridePCLStub(const std::string& name, void* funcPtr) {
    s_stubMap[name] = funcPtr;
}

} // namespace blastro