/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_VCL_OPENGL_WIN_WINDEVICEINFO_HXX
#define INCLUDED_VCL_OPENGL_WIN_WINDEVICEINFO_HXX

#include "opengl/DeviceInfo.hxx"
#include <rtl/ustring.hxx>
#include <vector>
#include <cstdint>

namespace wgl {

enum OperatingSystem {
    DRIVER_OS_UNKNOWN = 0,
    DRIVER_OS_WINDOWS_XP,
    DRIVER_OS_WINDOWS_SERVER_2003,
    DRIVER_OS_WINDOWS_VISTA,
    DRIVER_OS_WINDOWS_7,
    DRIVER_OS_WINDOWS_8,
    DRIVER_OS_WINDOWS_8_1,
    DRIVER_OS_LINUX,
    DRIVER_OS_OS_X_10_5,
    DRIVER_OS_OS_X_10_6,
    DRIVER_OS_OS_X_10_7,
    DRIVER_OS_OS_X_10_8,
    DRIVER_OS_ANDROID,
    DRIVER_OS_ALL
};

enum VersionComparisonOp {
    DRIVER_LESS_THAN,             // driver <  version
    DRIVER_LESS_THAN_OR_EQUAL,    // driver <= version
    DRIVER_GREATER_THAN,          // driver >  version
    DRIVER_GREATER_THAN_OR_EQUAL, // driver >= version
    DRIVER_EQUAL,                 // driver == version
    DRIVER_NOT_EQUAL,             // driver != version
    DRIVER_BETWEEN_EXCLUSIVE,     // driver > version && driver < versionMax
    DRIVER_BETWEEN_INCLUSIVE,     // driver >= version && driver <= versionMax
    DRIVER_BETWEEN_INCLUSIVE_START, // driver >= version && driver < versionMax
    DRIVER_COMPARISON_IGNORED
};

enum DeviceFamily {
    IntelGMA500,
    IntelGMA900,
    IntelGMA950,
    IntelGMA3150,
    IntelGMAX3000,
    IntelGMAX4500HD,
    IntelHD3000,
    IntelMobileHDGraphics,
    NvidiaBlockD3D9Layers,
    RadeonX1000,
    Geforce7300GT,
    Nvidia310M,
    DeviceFamilyMax
};

enum DeviceVendor {
    VendorAll,
    VendorIntel,
    VendorNVIDIA,
    VendorAMD,
    VendorATI,
    VendorMicrosoft,
    DeviceVendorMax
};

struct DriverInfo
{
    typedef std::vector<OUString> DeviceFamilyVector;

    // If |ownDevices| is true, you are transferring ownership of the devices
    // array, and it will be deleted when this GfxDriverInfo is destroyed.

    DriverInfo(OperatingSystem os, const OUString& vendor, DeviceFamilyVector* devices,
            VersionComparisonOp op,
            uint64_t driverVersion, bool bWhiteListed = false, const char *suggestedVersion = nullptr,
            bool ownDevices = false);

    DriverInfo();
    DriverInfo(const DriverInfo&);
    ~DriverInfo();

    OperatingSystem meOperatingSystem;
    uint32_t mnOperatingSystemVersion;

    OUString maAdapterVendor;

    static DeviceFamilyVector* const allDevices;
    DeviceFamilyVector* mpDevices;

    // Whether the mDevices array should be deleted when this structure is
    // deallocated. False by default.
    bool mbDeleteDevices;

    bool mbWhitelisted;

    VersionComparisonOp meComparisonOp;

    /* versions are assumed to be A.B.C.D packed as 0xAAAABBBBCCCCDDDD */
    uint64_t mnDriverVersion;
    uint64_t mnDriverVersionMax;
    static uint64_t allDriverVersions;

    static const DeviceFamilyVector* GetDeviceFamily(DeviceFamily id);
    static DeviceFamilyVector* mpDeviceFamilies[DeviceFamilyMax];

    OUString maSuggestedVersion;
};

#define GFX_DRIVER_VERSION(a,b,c,d) \
    ((uint64_t(a)<<48) | (uint64_t(b)<<32) | (uint64_t(c)<<16) | uint64_t(d))

inline uint64_t V(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    // We make sure every driver number is padded by 0s, this will allow us the
    // easiest 'compare as if decimals' approach. See ParseDriverVersion for a
    // more extensive explanation of this approach.
    while (b > 0 && b < 1000) {
        b *= 10;
    }
    while (c > 0 && c < 1000) {
        c *= 10;
    }
    while (d > 0 && d < 1000) {
        d *= 10;
    }
    return GFX_DRIVER_VERSION(a, b, c, d);
}

}

class WinOpenGLDeviceInfo : public OpenGLDeviceInfo
{
private:
    OUString maDriverVersion;
    OUString maDriverVersion2;

    OUString maDriverDate;
    OUString maDriverDate2;

    OUString maDeviceID;
    OUString maDeviceID2;

    OUString maAdapterVendorID;
    OUString maAdapterDeviceID;
    OUString maAdapterSubsysID;

    OUString maAdapterVendorID2;
    OUString maAdapterDeviceID2;
    OUString maAdapterSubsysID2;

    OUString maDeviceKey;
    OUString maDeviceKey2;

    OUString maDeviceString;
    OUString maDeviceString2;
    uint32_t mnWindowsVersion;

    bool mbHasDualGPU;
    bool mbHasDriverVersionMismatch;
    bool mbRDP;

    void GetData();
    void FillBlacklist();
    bool FindBlocklistedDeviceInList();

    static OUString* mpDeviceVendors[wgl::DeviceVendorMax];
    static std::vector<wgl::DriverInfo> maDriverInfo;

public:
    WinOpenGLDeviceInfo();

    static OUString GetDeviceVendor(wgl::DeviceVendor eVendor);
    virtual ~WinOpenGLDeviceInfo();

    virtual bool isDeviceBlocked();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
