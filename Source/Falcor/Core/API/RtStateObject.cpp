/***************************************************************************
 # Copyright (c) 2015-23, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
#include "RtStateObject.h"
#include "Device.h"
#include "GFXAPI.h"
#include "Core/Program/RtProgram.h"

namespace Falcor
{

ref<RtStateObject> RtStateObject::create(ref<Device> pDevice, const Desc& desc)
{
    return ref<RtStateObject>(new RtStateObject(pDevice, desc));
}

RtStateObject::RtStateObject(ref<Device> pDevice, const Desc& desc) : mpDevice(pDevice), mDesc(desc)
{
    auto pKernels = getKernels();
    gfx::RayTracingPipelineStateDesc rtpDesc = {};
    std::vector<gfx::HitGroupDesc> hitGroups;
    // Loop over the hitgroups
    for (const auto& pEntryPointGroup : pKernels->getUniqueEntryPointGroups())
    {
        if (pEntryPointGroup->getType() == EntryPointGroupKernels::Type::RtHitGroup)
        {
            const Shader* pIntersection = pEntryPointGroup->getShader(ShaderType::Intersection);
            const Shader* pAhs = pEntryPointGroup->getShader(ShaderType::AnyHit);
            const Shader* pChs = pEntryPointGroup->getShader(ShaderType::ClosestHit);

            gfx::HitGroupDesc hitgroupDesc = {};
            hitgroupDesc.anyHitEntryPoint = pAhs ? pAhs->getEntryPoint().c_str() : nullptr;
            hitgroupDesc.closestHitEntryPoint = pChs ? pChs->getEntryPoint().c_str() : nullptr;
            hitgroupDesc.intersectionEntryPoint = pIntersection ? pIntersection->getEntryPoint().c_str() : nullptr;
            hitgroupDesc.hitGroupName = pEntryPointGroup->getExportName().c_str();
            hitGroups.push_back(hitgroupDesc);
        }
    }

    rtpDesc.hitGroupCount = (uint32_t)hitGroups.size();
    rtpDesc.hitGroups = hitGroups.data();
    rtpDesc.maxRecursion = mDesc.maxTraceRecursionDepth;

    static_assert((uint32_t)gfx::RayTracingPipelineFlags::SkipProcedurals == (uint32_t)RtPipelineFlags::SkipProceduralPrimitives);
    static_assert((uint32_t)gfx::RayTracingPipelineFlags::SkipTriangles == (uint32_t)RtPipelineFlags::SkipTriangles);

    rtpDesc.flags = (gfx::RayTracingPipelineFlags::Enum)mDesc.pipelineFlags;
    auto rtProgram = dynamic_cast<RtProgram*>(mDesc.pKernels->getProgramVersion()->getProgram());
    FALCOR_ASSERT(rtProgram);
    rtpDesc.maxRayPayloadSize = rtProgram->getRtDesc().getMaxPayloadSize();
    rtpDesc.maxAttributeSizeInBytes = rtProgram->getRtDesc().getMaxAttributeSize();
    rtpDesc.program = mDesc.pKernels->getGfxProgram();

    FALCOR_GFX_CALL(mpDevice->getGfxDevice()->createRayTracingPipelineState(rtpDesc, mGfxPipelineState.writeRef()));

    // Get shader identifiers.
    // In GFX, a shader identifier is just the entry point group name.
    for (const auto& pEntryPointGroup : pKernels->getUniqueEntryPointGroups())
    {
        mEntryPointGroupExportNames.push_back(pEntryPointGroup->getExportName());
    }
}
} // namespace Falcor
