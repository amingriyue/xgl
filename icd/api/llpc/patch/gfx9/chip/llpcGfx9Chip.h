/*
 *******************************************************************************
 *
 * Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/**
 ***********************************************************************************************************************
 * @file  llpcGfx9Chip.h
 * @brief LLPC header file: contains various definitions for Gfx9 chips.
 ***********************************************************************************************************************
 */
#pragma once

#include <cstdint>
#include <unordered_map>
#include "llpcElf.h"

namespace Llpc
{

namespace Gfx9
{

#undef CS_ENABLE

#include "gfx9_plus_merged_enum.h"
#include "gfx9_plus_merged_mask.h"
#include "gfx9_plus_merged_offset.h"
#include "gfx9_plus_merged_registers.h"
#include "gfx9_plus_merged_shift.h"
#include "gfx9_plus_merged_typedef.h"

// =====================================================================================================================
// Helper macros to operate registers

// Defines fields: register ID (byte-based) and its value
#define DEF_REG(_reg)               uint32_t _reg##_ID; reg##_reg _reg##_VAL;

// Defines GFX-dependent fields: register ID (byte-based) and its value
#define DEF_REG_ID_GFX9(_reg)       uint32_t            _reg##__GFX09_ID;
#define DEF_REG_ID(_reg)            uint32_t            _reg##_ID;

#define DEF_REG_VAL_GFX9(_reg)      reg##_reg##__GFX09  _reg##__GFX09_VAL;
#define DEF_REG_VAL(_reg)           struct { uint32_t u32All; } _reg##_VAL;

#define DEF_REG_ID_GFX10(_reg)
#define DEF_REG_VAL_GFX10(_reg)

#define DEF_REG_GFX(_reg) \
    union \
    { \
        DEF_REG_ID_GFX9(_reg)   \
        DEF_REG_ID_GFX10(_reg)  \
        DEF_REG_ID(_reg)        \
    }; \
    \
    union \
    { \
        DEF_REG_VAL_GFX9(_reg)  \
        DEF_REG_VAL_GFX10(_reg) \
        DEF_REG_VAL(_reg)       \
    };

// Initializes register ID and its value
#define INIT_REG(_reg)            { _reg##_ID = mm##_reg; _reg##_VAL.u32All = 0; }

// Initializes GFX-dependent register ID and its value
#define INIT_REG_GFX9(_reg)       { _reg##_ID = mm##_reg##__GFX09; _reg##_VAL.u32All = 0; }

// Case label for switch, set register value
#define CASE_SET_REG(_stage, _reg, _val)   case (mm##_reg * 4): { (_stage)->_reg##_VAL.u32All = (_val); break; }

// Adds an entry for the map from register ID to its name string
#define ADD_REG_MAP(_reg)           RegNameMap[mm##_reg * 4] = #_reg;
#define ADD_REG_MAP_GFX9(_reg)      RegNameMapGfx9[mm##_reg##__GFX09 * 4] = #_reg;

// Gets register value
#define GET_REG(_stage, _reg)                      ((_stage)->_reg##_VAL.u32All)

// Sets register value
#define SET_REG(_stage, _reg, _val)                (_stage)->_reg##_VAL.u32All = (_val);

// Adds and sets dynamic register value
#define SET_DYN_REG(_pipeline, _reg, _val) \
    LLPC_ASSERT((_pipeline)->m_dynRegCount < _pipeline->MaxDynamicRegs); \
    (_pipeline)->m_dynRegs[(_pipeline)->m_dynRegCount].key = (_reg);     \
    (_pipeline)->m_dynRegs[(_pipeline)->m_dynRegCount++].value = (_val);

// Gets register field value
#define GET_REG_FIELD(_stage, _reg, _field)        ((_stage)->_reg##_VAL.bits._field)

// Sets register field value
#define SET_REG_FIELD(_stage, _reg, _field, _val)  (_stage)->_reg##_VAL.bits._field = (_val);

// Gets GFX-dependent register field value
#define GET_REG_FIELD_GFX9(_stage, _reg, _field)    ((_stage)->_reg##__GFX09_VAL.bits._field)

// Sets GFX-dependent register field value
#define SET_REG_FIELD_GFX9(_stage, _reg, _field, _val)      (_stage)->_reg##__GFX09_VAL.bits._field = (_val);

// Preferred number of ES threads per GS thread.
constexpr uint32_t EsThreadsPerGsThread = 128;

// Preferred number of GS primitives per ES thread.
constexpr uint32_t GsPrimsPerEsThread = 256;

// Preferred number of GS threads per VS thread.
constexpr uint32_t GsThreadsPerVsThread = 2;

// The register headers don't specify an enum for the values of VGT_GS_MODE.ONCHIP.
enum VGT_GS_MODE_ONCHIP_TYPE : uint32_t
{
    VGT_GS_MODE_ONCHIP_OFF         = 0,
    VGT_GS_MODE_ONCHIP_ON          = 3,
};

// =====================================================================================================================
// Represents configuration of static registers relevant to hardware vertex shader.
struct VsRegConfig
{
    DEF_REG(SPI_SHADER_PGM_RSRC1_VS);
    DEF_REG_GFX(SPI_SHADER_PGM_RSRC2_VS);
    DEF_REG(SPI_SHADER_POS_FORMAT);
    DEF_REG(SPI_VS_OUT_CONFIG);
    DEF_REG_GFX(PA_CL_VS_OUT_CNTL);
    DEF_REG(PA_CL_CLIP_CNTL);
    DEF_REG(PA_CL_VTE_CNTL);
    DEF_REG(PA_SU_VTX_CNTL);
    DEF_REG(VGT_PRIMITIVEID_EN);
    //DEF_REG(VGT_STRMOUT_CONFIG);        // Always zero for LLPC
    DEF_REG(VGT_STRMOUT_BUFFER_CONFIG); // Always zero for LLPC
    DEF_REG(VGT_STRMOUT_VTX_STRIDE_0);  // Always zero for LLPC
    DEF_REG(VGT_STRMOUT_VTX_STRIDE_1);  // Always zero for LLPC
    DEF_REG(VGT_STRMOUT_VTX_STRIDE_2);  // Always zero for LLPC
    DEF_REG(VGT_STRMOUT_VTX_STRIDE_3);  // Always zero for LLPC
    DEF_REG(VGT_REUSE_OFF);
    DEF_REG(VGT_VERTEX_REUSE_BLOCK_CNTL);
    DEF_REG(VS_SCRATCH_SIZE);
    DEF_REG(VS_NUM_USED_VGPRS);
    DEF_REG(VS_NUM_USED_SGPRS);
    DEF_REG(USES_VIEWPORT_ARRAY_INDEX);

    // Initializer
    void Init()
    {
        INIT_REG(SPI_SHADER_PGM_RSRC1_VS);
        INIT_REG(SPI_SHADER_PGM_RSRC2_VS);
        INIT_REG(SPI_SHADER_POS_FORMAT);
        INIT_REG(SPI_VS_OUT_CONFIG);
        INIT_REG(PA_CL_VS_OUT_CNTL);
        INIT_REG(PA_CL_CLIP_CNTL);
        INIT_REG(PA_CL_VTE_CNTL);
        INIT_REG(PA_SU_VTX_CNTL);
        INIT_REG(VGT_PRIMITIVEID_EN);
        //INIT_REG(VGT_STRMOUT_CONFIG);
        INIT_REG(VGT_STRMOUT_BUFFER_CONFIG);
        INIT_REG(VGT_STRMOUT_VTX_STRIDE_0);
        INIT_REG(VGT_STRMOUT_VTX_STRIDE_1);
        INIT_REG(VGT_STRMOUT_VTX_STRIDE_2);
        INIT_REG(VGT_STRMOUT_VTX_STRIDE_3);
        INIT_REG(VGT_REUSE_OFF);
        INIT_REG(VGT_VERTEX_REUSE_BLOCK_CNTL);
        INIT_REG(VS_SCRATCH_SIZE);
        INIT_REG(VS_NUM_USED_VGPRS);
        INIT_REG(VS_NUM_USED_SGPRS);
        INIT_REG(USES_VIEWPORT_ARRAY_INDEX);
    }
};

// =====================================================================================================================
// Represents configuration of static registers relevant to hardware local-hull merged shader.
struct LsHsRegConfig
{
    DEF_REG(SPI_SHADER_PGM_RSRC1_HS);
    DEF_REG_GFX(SPI_SHADER_PGM_RSRC2_HS);
    DEF_REG(HS_SCRATCH_SIZE);
    DEF_REG(HS_NUM_USED_VGPRS);
    DEF_REG(HS_NUM_USED_SGPRS);
    DEF_REG(VGT_LS_HS_CONFIG);
    DEF_REG(VGT_HOS_MIN_TESS_LEVEL);
    DEF_REG(VGT_HOS_MAX_TESS_LEVEL);
    DEF_REG(VGT_TF_PARAM);

    // Initializer
    void Init()
    {
        INIT_REG(SPI_SHADER_PGM_RSRC1_HS);
        INIT_REG(SPI_SHADER_PGM_RSRC2_HS);
        INIT_REG(HS_SCRATCH_SIZE);
        INIT_REG(HS_NUM_USED_VGPRS);
        INIT_REG(HS_NUM_USED_SGPRS);
        INIT_REG(VGT_LS_HS_CONFIG);
        INIT_REG(VGT_HOS_MIN_TESS_LEVEL);
        INIT_REG(VGT_HOS_MAX_TESS_LEVEL);
        INIT_REG(VGT_TF_PARAM);
    }
};

// =====================================================================================================================
// Represents configuration of static registers relevant to hardware embedded-geometry merged shader.
struct EsGsRegConfig
{
    DEF_REG(SPI_SHADER_PGM_RSRC1_GS);
    DEF_REG_GFX(SPI_SHADER_PGM_RSRC2_GS);
    DEF_REG(GS_SCRATCH_SIZE);
    DEF_REG(GS_NUM_USED_VGPRS);
    DEF_REG(GS_NUM_USED_SGPRS);
    DEF_REG(VGT_GS_MAX_VERT_OUT);
    DEF_REG(VGT_GS_ONCHIP_CNTL);
    DEF_REG(VGT_ES_PER_GS);
    DEF_REG(VGT_GS_VERT_ITEMSIZE);
    DEF_REG(VGT_GS_INSTANCE_CNT);
    DEF_REG(VGT_GS_PER_VS);
    DEF_REG(VGT_GS_OUT_PRIM_TYPE);
    DEF_REG(VGT_GSVS_RING_ITEMSIZE);
    DEF_REG(VGT_GS_PER_ES);
    DEF_REG(VGT_GS_VERT_ITEMSIZE_1);
    DEF_REG(VGT_GS_VERT_ITEMSIZE_2);
    DEF_REG(VGT_GS_VERT_ITEMSIZE_3);
    DEF_REG(VGT_GSVS_RING_OFFSET_1);
    DEF_REG(VGT_GSVS_RING_OFFSET_2);
    DEF_REG(VGT_GSVS_RING_OFFSET_3);
    DEF_REG(VGT_GS_MODE);
    DEF_REG(VGT_ESGS_RING_ITEMSIZE);

    // Initializer
    void Init()
    {
        INIT_REG(SPI_SHADER_PGM_RSRC1_GS);
        INIT_REG(SPI_SHADER_PGM_RSRC2_GS);
        INIT_REG(GS_SCRATCH_SIZE);
        INIT_REG(GS_NUM_USED_VGPRS);
        INIT_REG(GS_NUM_USED_SGPRS);
        INIT_REG(VGT_GS_MAX_VERT_OUT);
        INIT_REG(VGT_GS_ONCHIP_CNTL);
        INIT_REG(VGT_ES_PER_GS);
        INIT_REG(VGT_GS_VERT_ITEMSIZE);
        INIT_REG(VGT_GS_INSTANCE_CNT);
        INIT_REG(VGT_GS_PER_VS);
        INIT_REG(VGT_GS_OUT_PRIM_TYPE);
        INIT_REG(VGT_GSVS_RING_ITEMSIZE);
        INIT_REG(VGT_GS_PER_ES);
        INIT_REG(VGT_GS_VERT_ITEMSIZE_1);
        INIT_REG(VGT_GS_VERT_ITEMSIZE_2);
        INIT_REG(VGT_GS_VERT_ITEMSIZE_3);
        INIT_REG(VGT_GSVS_RING_OFFSET_1);
        INIT_REG(VGT_GSVS_RING_OFFSET_2);
        INIT_REG(VGT_GSVS_RING_OFFSET_3);
        INIT_REG(VGT_GS_MODE);
        INIT_REG(VGT_ESGS_RING_ITEMSIZE);
    }
};

// =====================================================================================================================
// Represents configuration of static registers relevant to hardware pixel shader.
struct PsRegConfig
{
    DEF_REG(SPI_SHADER_PGM_RSRC1_PS);
    DEF_REG_GFX(SPI_SHADER_PGM_RSRC2_PS);
    DEF_REG(SPI_SHADER_Z_FORMAT);
    DEF_REG(SPI_SHADER_COL_FORMAT);
    DEF_REG(SPI_BARYC_CNTL);
    DEF_REG(SPI_PS_IN_CONTROL);
    DEF_REG(SPI_PS_INPUT_ENA);
    DEF_REG(SPI_PS_INPUT_ADDR);
    DEF_REG(SPI_INTERP_CONTROL_0);
    DEF_REG(PA_SC_MODE_CNTL_1);
    DEF_REG(DB_SHADER_CONTROL);
    DEF_REG(CB_SHADER_MASK);
    DEF_REG(PS_USES_UAVS);
    DEF_REG(PS_SCRATCH_SIZE);
    DEF_REG(PS_NUM_USED_VGPRS);
    DEF_REG(PS_NUM_USED_SGPRS);
    DEF_REG(PS_RUNS_AT_SAMPLE_RATE);
    DEF_REG(PA_SC_AA_CONFIG);
    DEF_REG(PA_SC_SHADER_CONTROL);
    DEF_REG(PA_SC_CONSERVATIVE_RASTERIZATION_CNTL);

    // Initializer
    void Init()
    {
        INIT_REG(SPI_SHADER_PGM_RSRC1_PS);
        INIT_REG(SPI_SHADER_PGM_RSRC2_PS);
        INIT_REG(SPI_SHADER_Z_FORMAT);
        INIT_REG(SPI_SHADER_COL_FORMAT);
        INIT_REG(SPI_BARYC_CNTL);
        INIT_REG(SPI_PS_IN_CONTROL);
        INIT_REG(SPI_PS_INPUT_ENA);
        INIT_REG(SPI_PS_INPUT_ADDR);
        INIT_REG(SPI_INTERP_CONTROL_0);
        INIT_REG(PA_SC_MODE_CNTL_1);
        INIT_REG(DB_SHADER_CONTROL);
        INIT_REG(CB_SHADER_MASK);
        INIT_REG(PS_USES_UAVS);
        INIT_REG(PS_SCRATCH_SIZE);
        INIT_REG(PS_NUM_USED_VGPRS);
        INIT_REG(PS_NUM_USED_SGPRS);
        INIT_REG(PS_RUNS_AT_SAMPLE_RATE);
        INIT_REG(PA_SC_AA_CONFIG);
        INIT_REG(PA_SC_SHADER_CONTROL);
        INIT_REG(PA_SC_CONSERVATIVE_RASTERIZATION_CNTL);
    }
};

// =====================================================================================================================
// Represents the common configuration of registers relevant to all pipeline.
struct PipelineRegConfig
{
    DEF_REG(USER_DATA_LIMIT);
    DEF_REG(SPILL_THRESHOLD);
    DEF_REG(PIPELINE_HASH_LO);
    DEF_REG(PIPELINE_HASH_HI);
    DEF_REG(API_HW_SHADER_MAPPING_LO);
    DEF_REG(API_HW_SHADER_MAPPING_HI);

    // Initializer
    void Init()
    {
        INIT_REG(USER_DATA_LIMIT);
        INIT_REG(SPILL_THRESHOLD);
        INIT_REG(PIPELINE_HASH_LO);
        INIT_REG(PIPELINE_HASH_HI);
        INIT_REG(API_HW_SHADER_MAPPING_LO);
        INIT_REG(API_HW_SHADER_MAPPING_HI);
        SET_REG(this, SPILL_THRESHOLD, UINT32_MAX);
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to graphics pipeline (VS-FS).
struct PipelineVsFsRegConfig: public PipelineRegConfig
{
    static const uint32_t MaxDynamicRegs = 32 + // mmSPI_SHADER_USER_DATA_VS_0~31
                                           32 + // mmSPI_SHADER_USER_DATA_PS_0~31
                                           32;  // mmSPI_PS_INPUT_CNTL_0~31

    VsRegConfig m_vsRegs;   // VS -> hardware VS
    PsRegConfig m_psRegs;   // FS -> hardware PS
    DEF_REG(VGT_SHADER_STAGES_EN);
    DEF_REG(API_VS_HASH_LO);
    DEF_REG(API_VS_HASH_HI);
    DEF_REG(API_PS_HASH_LO);
    DEF_REG(API_PS_HASH_HI);
    DEF_REG(INDIRECT_TABLE_ENTRY);
    DEF_REG(IA_MULTI_VGT_PARAM);

    Util::Abi::PalMetadataNoteEntry m_dynRegs[MaxDynamicRegs];  // Dynamic registers configuration
    uint32_t m_dynRegCount;                                     // Count of dynamic registers

    // Initializer
    void Init()
    {
        m_vsRegs.Init();
        m_psRegs.Init();
        PipelineRegConfig::Init();

        INIT_REG(VGT_SHADER_STAGES_EN);
        INIT_REG(API_VS_HASH_LO);
        INIT_REG(API_VS_HASH_HI);
        INIT_REG(API_PS_HASH_LO);
        INIT_REG(API_PS_HASH_HI);
        INIT_REG(INDIRECT_TABLE_ENTRY);
        INIT_REG_GFX9(IA_MULTI_VGT_PARAM);

        m_dynRegCount = 0;
    }

    // Get total register's count of this pipeline
    uint32_t GetRegCount() const
    {
        return  offsetof(PipelineVsFsRegConfig, m_dynRegs) / sizeof(Util::Abi::PalMetadataNoteEntry) +
                m_dynRegCount;
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to graphics pipeline (VS-TS-FS).
struct PipelineVsTsFsRegConfig: public PipelineRegConfig
{
    static const uint32_t MaxDynamicRegs = 32 + // mmSPI_SHADER_USER_DATA_LS_0~31
                                           32 + // mmSPI_SHADER_USER_DATA_VS_0~31
                                           32 + // mmSPI_SHADER_USER_DATA_PS_0~31
                                           32;  // mmSPI_PS_INPUT_CNTL_0~31

    LsHsRegConfig m_lsHsRegs; // VS-TCS -> hardware LS-HS
    VsRegConfig   m_vsRegs;   // TES    -> hardware VS
    PsRegConfig   m_psRegs;   // FS     -> hardware PS

    DEF_REG(VGT_SHADER_STAGES_EN);
    DEF_REG(API_VS_HASH_LO);
    DEF_REG(API_VS_HASH_HI);
    DEF_REG(API_HS_HASH_LO);
    DEF_REG(API_HS_HASH_HI);
    DEF_REG(API_DS_HASH_LO);
    DEF_REG(API_DS_HASH_HI);
    DEF_REG(API_PS_HASH_LO);
    DEF_REG(API_PS_HASH_HI);
    DEF_REG(INDIRECT_TABLE_ENTRY);
    DEF_REG(IA_MULTI_VGT_PARAM);

    Util::Abi::PalMetadataNoteEntry m_dynRegs[MaxDynamicRegs];  // Dynamic registers configuration
    uint32_t m_dynRegCount;                                     // Count of dynamic registers

    // Initializer
    void Init()
    {
        m_lsHsRegs.Init();
        m_vsRegs.Init();
        m_psRegs.Init();
        PipelineRegConfig::Init();

        INIT_REG(VGT_SHADER_STAGES_EN);
        INIT_REG(API_VS_HASH_LO);
        INIT_REG(API_VS_HASH_HI);
        INIT_REG(API_HS_HASH_LO);
        INIT_REG(API_HS_HASH_HI);
        INIT_REG(API_DS_HASH_LO);
        INIT_REG(API_DS_HASH_HI);
        INIT_REG(API_PS_HASH_LO);
        INIT_REG(API_PS_HASH_HI);
        INIT_REG(INDIRECT_TABLE_ENTRY);
        INIT_REG_GFX9(IA_MULTI_VGT_PARAM);

        m_dynRegCount = 0;
    }

    // Get total register's count of this pipeline
    uint32_t GetRegCount() const
    {
        return  offsetof(PipelineVsTsFsRegConfig, m_dynRegs) / sizeof(Util::Abi::PalMetadataNoteEntry) +
                m_dynRegCount;
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to graphics pipeline (VS-GS-FS).
struct PipelineVsGsFsRegConfig: public PipelineRegConfig
{
    static const uint32_t MaxDynamicRegs = 32 + // mmSPI_SHADER_USER_DATA_ES_0~31
                                           32 + // mmSPI_SHADER_USER_DATA_PS_0~31
                                           32 + // mmSPI_SHADER_USER_DATA_VS_0~31
                                           32;  // mmSPI_PS_INPUT_CNTL_0~31

    EsGsRegConfig m_esGsRegs;   // VS-GS -> hardware ES-GS
    PsRegConfig   m_psRegs;     // FS -> hardware PS
    VsRegConfig   m_vsRegs;     // Copy shader -> hardware VS

    DEF_REG(VGT_SHADER_STAGES_EN);
    DEF_REG(API_VS_HASH_LO);
    DEF_REG(API_VS_HASH_HI);
    DEF_REG(API_GS_HASH_LO);
    DEF_REG(API_GS_HASH_HI);
    DEF_REG(API_PS_HASH_LO);
    DEF_REG(API_PS_HASH_HI);
    DEF_REG(INDIRECT_TABLE_ENTRY);
    DEF_REG(IA_MULTI_VGT_PARAM);

    Util::Abi::PalMetadataNoteEntry m_dynRegs[MaxDynamicRegs];  // Dynamic registers configuration
    uint32_t m_dynRegCount;                                     // Count of dynamic registers

    // Initializer
    void Init()
    {
        m_esGsRegs.Init();
        m_vsRegs.Init();
        m_psRegs.Init();
        PipelineRegConfig::Init();

        INIT_REG(VGT_SHADER_STAGES_EN);
        INIT_REG(API_VS_HASH_LO);
        INIT_REG(API_VS_HASH_HI);
        INIT_REG(API_GS_HASH_LO);
        INIT_REG(API_GS_HASH_HI);
        INIT_REG(API_PS_HASH_LO);
        INIT_REG(API_PS_HASH_HI);
        INIT_REG(INDIRECT_TABLE_ENTRY);
        INIT_REG_GFX9(IA_MULTI_VGT_PARAM);

        SET_REG(this, SPILL_THRESHOLD, UINT32_MAX);

        m_dynRegCount = 0;
    }

    // Get total register's count of this pipeline
    uint32_t GetRegCount() const
    {
        return  offsetof(PipelineVsGsFsRegConfig, m_dynRegs) / sizeof(Util::Abi::PalMetadataNoteEntry) +
                m_dynRegCount;
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to graphics pipeline (VS-TS-GS-FS).
struct PipelineVsTsGsFsRegConfig: public PipelineRegConfig
{
    static const uint32_t MaxDynamicRegs = 32 + // mmSPI_SHADER_USER_DATA_LS_0~32
                                           32 + // mmSPI_SHADER_USER_DATA_ES_0~32
                                           32 + // mmSPI_SHADER_USER_DATA_PS_0~32
                                           32 + // mmSPI_SHADER_USER_DATA_VS_0~32
                                           32;  // mmSPI_PS_INPUT_CNTL_0~31

    LsHsRegConfig m_lsHsRegs;   // VS-TCS -> hardware LS-HS
    EsGsRegConfig m_esGsRegs;   // TES-GS -> hardware ES-GS
    PsRegConfig   m_psRegs;     // FS -> hardware PS
    VsRegConfig   m_vsRegs;     // Copy shader -> hardware VS

    DEF_REG(VGT_SHADER_STAGES_EN);
    DEF_REG(API_VS_HASH_LO);
    DEF_REG(API_VS_HASH_HI);
    DEF_REG(API_HS_HASH_LO);
    DEF_REG(API_HS_HASH_HI);
    DEF_REG(API_DS_HASH_LO);
    DEF_REG(API_DS_HASH_HI);
    DEF_REG(API_GS_HASH_LO);
    DEF_REG(API_GS_HASH_HI);
    DEF_REG(API_PS_HASH_LO);
    DEF_REG(API_PS_HASH_HI);
    DEF_REG(INDIRECT_TABLE_ENTRY);
    DEF_REG(IA_MULTI_VGT_PARAM);

    Util::Abi::PalMetadataNoteEntry m_dynRegs[MaxDynamicRegs];  // Dynamic registers configuration
    uint32_t m_dynRegCount;                                     // Count of dynamic registers

    // Initializer
    void Init()
    {
        m_lsHsRegs.Init();
        m_esGsRegs.Init();
        m_psRegs.Init();
        m_vsRegs.Init();
        PipelineRegConfig::Init();

        INIT_REG(VGT_SHADER_STAGES_EN);
        INIT_REG(API_VS_HASH_LO);
        INIT_REG(API_VS_HASH_HI);
        INIT_REG(API_HS_HASH_LO);
        INIT_REG(API_HS_HASH_HI);
        INIT_REG(API_DS_HASH_LO);
        INIT_REG(API_DS_HASH_HI);
        INIT_REG(API_GS_HASH_LO);
        INIT_REG(API_GS_HASH_HI);
        INIT_REG(API_PS_HASH_LO);
        INIT_REG(API_PS_HASH_HI);
        INIT_REG(INDIRECT_TABLE_ENTRY);
        INIT_REG_GFX9(IA_MULTI_VGT_PARAM);

        m_dynRegCount = 0;
    }

    // Get total register's count of this pipeline
    uint32_t GetRegCount() const
    {
        return  offsetof(PipelineVsTsGsFsRegConfig, m_dynRegs) / sizeof(Util::Abi::PalMetadataNoteEntry) +
                m_dynRegCount;
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to compute shader.
struct CsRegConfig
{
    DEF_REG(COMPUTE_PGM_RSRC1);
    DEF_REG(COMPUTE_PGM_RSRC2);
    DEF_REG(COMPUTE_NUM_THREAD_X);
    DEF_REG(COMPUTE_NUM_THREAD_Y);
    DEF_REG(COMPUTE_NUM_THREAD_Z);
    DEF_REG(CS_SCRATCH_SIZE);
    DEF_REG(CS_NUM_USED_VGPRS);
    DEF_REG(CS_NUM_USED_SGPRS);

    // Initializer
    void Init()
    {
        INIT_REG(COMPUTE_PGM_RSRC1);
        INIT_REG(COMPUTE_PGM_RSRC2);
        INIT_REG(COMPUTE_NUM_THREAD_X);
        INIT_REG(COMPUTE_NUM_THREAD_Y);
        INIT_REG(COMPUTE_NUM_THREAD_Z);
        INIT_REG(CS_SCRATCH_SIZE);
        INIT_REG(CS_NUM_USED_VGPRS);
        INIT_REG(CS_NUM_USED_SGPRS);
    }
};

// =====================================================================================================================
// Represents configuration of registers relevant to compute pipeline.
struct PipelineCsRegConfig: public PipelineRegConfig
{
    static const uint32_t MaxDynamicRegs = 16;  // mmCOMPUTE_USER_DATA_0~15

    CsRegConfig   m_csRegs;
    DEF_REG(API_CS_HASH_LO);
    DEF_REG(API_CS_HASH_HI);

    Util::Abi::PalMetadataNoteEntry m_dynRegs[MaxDynamicRegs];  // Dynamic registers configuration
    uint32_t m_dynRegCount;                                     // Count of dynamic registers

    // Initializer
    void Init()
    {
        m_csRegs.Init();
        PipelineRegConfig::Init();

        INIT_REG(API_CS_HASH_LO);
        INIT_REG(API_CS_HASH_HI);

        m_dynRegCount = 0;
    }

    // Get total register's count of this pipeline
    uint32_t GetRegCount() const
    {
        return  offsetof(PipelineCsRegConfig, m_dynRegs) / sizeof(Util::Abi::PalMetadataNoteEntry) +
                m_dynRegCount;
    }
};

// Map from register ID to its name string
static std::unordered_map<uint32_t, const char*>    RegNameMap;      // Common for both GFX9 and GFX10
static std::unordered_map<uint32_t, const char*>    RegNameMapGfx9;  // GFX9 specific

// Adds entries to register name map.
void InitRegisterNameMap(GfxIpVersion gfxIp);

// Gets the name string from byte-based ID of the register
const char* GetRegisterNameString(GfxIpVersion gfxIp, uint32_t regId);

} // Gfx9

} // Llpc