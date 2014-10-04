/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SC_SOURCE_CORE_OPENCL_OPENCLWRAPPER_HXX
#define INCLUDED_SC_SOURCE_CORE_OPENCL_OPENCLWRAPPER_HXX

#include <config_features.h>
#include <formula/opcode.hxx>
#include <sal/detail/log.h>
#include <osl/file.hxx>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include "platforminfo.hxx"

#include <rtl/string.hxx>

#include "clcc/clew.h"

// CL_MAP_WRITE_INVALIDATE_REGION is new in OpenCL 1.2.

// When compiling against an older OpenCL, use CL_MAP_WRITE.

// FIXME: But what if this code has been compiled against OpenCL 1.2
// headers but then runs against an OpenCL 1.1 implementation?
// Probably the code should check at run-time the version of the
// OpenCL implementation and choose which flag to use based on that.
#ifdef CL_MAP_WRITE_INVALIDATE_REGION
#define OPENCLWRAPPER_CL_MAP_WRITE_FLAG CL_MAP_WRITE_INVALIDATE_REGION
#else
#define OPENCLWRAPPER_CL_MAP_WRITE_FLAG CL_MAP_WRITE
#endif

#define MaxTextExtent 4096
//support AMD opencl
#define CL_QUEUE_THREAD_HANDLE_AMD 0x403E
#define CL_MAP_WRITE_INVALIDATE_REGION (1 << 2)

#define CHECK_OPENCL(status,name)    \
if( status != CL_SUCCESS )    \
{    \
    printf ("OpenCL error code is %d at " SAL_DETAIL_WHERE " when %s .\n", status, name);    \
    return false;    \
}

#define CHECK_OPENCL_PTR(status,name)    \
if( status != CL_SUCCESS )    \
{    \
    printf ("OpenCL error code is %d at " SAL_DETAIL_WHERE " when %s .\n", status, name);    \
    return NULL;    \
}

#define CHECK_OPENCL_VOID(status,name)    \
if( status != CL_SUCCESS )    \
{    \
    printf ("OpenCL error code is %d at " SAL_DETAIL_WHERE " when %s .\n", status, name);    \
}

#define CHECK_OPENCL_RELEASE(status,name)    \
if ( name != NULL )    \
    clReleaseMemObject( name );    \
if( status != CL_SUCCESS )    \
{    \
    printf ("OpenCL error code is %d at " SAL_DETAIL_WHERE " when clReleaseMemObject( %s ).\n", status, #name);    \
}

#define MAX_KERNEL_STRING_LEN 64
#define MAX_CLFILE_NUM 50
#define MAX_CLKERNEL_NUM 200
#define MAX_KERNEL_NAME_LEN 64

#if defined(_MSC_VER)
#ifndef strcasecmp
#define strcasecmp strcmp
#endif
#endif

#include <cstdio>

typedef struct _KernelEnv
{
    cl_context mpkContext;
    cl_command_queue mpkCmdQueue;
    cl_program mpkProgram;
} KernelEnv;

extern "C" {

// user defined, this is function wrapper which is used to set the input
// parameters, launch kernel and copy data from GPU to CPU or CPU to GPU.
typedef int ( *cl_kernel_function )( void **userdata, KernelEnv *kenv );

}

namespace sc { namespace opencl {

typedef unsigned int uint;

struct OpenCLEnv
{
    cl_platform_id mpOclPlatformID;
    cl_context mpOclContext;
    cl_device_id mpOclDevsID;
    cl_command_queue mpOclCmdQueue;
};

struct GPUEnv
{
    //share vb in all modules in hb library
    cl_platform_id mpPlatformID;
    cl_device_type mDevType;
    cl_context mpContext;
    cl_device_id *mpArryDevsID;
    cl_device_id mpDevID;
    cl_command_queue mpCmdQueue;
    cl_program mpArryPrograms[MAX_CLFILE_NUM]; //one program object maps one kernel source file
    int mnIsUserCreated; // 1: created , 0:no create and needed to create by opencl wrapper
    bool mnKhrFp64Flag;
    bool mnAmdFp64Flag;
};

struct SingleVectorFormula
{
    const double *mdpInputLeftData;
    const double *mdpInputRightData;
    size_t mnInputLeftDataSize;
    size_t mnInputRightDataSize;
    uint mnInputLeftStartPosition;
    uint mnInputRightStartPosition;
    int mnInputLeftOffset;
    int mnInputRightOffset;
};

struct DoubleVectorFormula
{
    const double *mdpInputData;
    size_t mnInputDataSize;
    uint mnInputStartPosition;
    uint mnInputEndPosition;
    int mnInputStartOffset;
    int mnInputEndOffset;
};

class OpenclDevice
{
public:
    static GPUEnv gpuEnv;
    static bool bIsInited;
    static OString maCacheFolder;

    static void registOpenclKernel();
    static bool initOpenclRunEnv( GPUEnv *gpu );
    static void releaseOpenclEnv( GPUEnv *gpuInfo );
    static bool initOpenclRunEnv( int argc );
    static bool generatBinFromKernelSource( cl_program program, const char * clFileName );
    static bool writeBinaryToFile( const OString& rName, const char* birary, size_t numBytes );
    static std::vector<boost::shared_ptr<osl::File> > binaryGenerated( const char * clFileName, cl_context context);
    static bool buildProgramFromBinary(const char* buildOption, GPUEnv* gpuEnv, const char* filename, int idx);

    static bool initOpenclAttr( OpenCLEnv * env );
    static void setKernelEnv( KernelEnv *envInfo );
};

size_t getOpenCLPlatformCount();
const std::vector<OpenclPlatformInfo>& fillOpenCLInfo();

/**
 * Used to set or switch between OpenCL devices.
 *
 * @param pDeviceId the id of the opencl device of type cl_device_id, NULL means use software calculation
 * @param bAutoSelect use the algorithm to select the best OpenCL device
 *
 * @return returns true if there is a valid opencl device that has been set up
 */
bool switchOpenclDevice(const OUString* pDeviceId, bool bAutoSelect,
        bool bForceEvaluation);

void getOpenCLDeviceInfo(size_t& rDeviceId, size_t& rPlatformId);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
