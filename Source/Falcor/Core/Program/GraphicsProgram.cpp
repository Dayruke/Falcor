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
#include "GraphicsProgram.h"
#include "ProgramManager.h"
#include "Core/ObjectPython.h"
#include "Core/API/Device.h"
#include "Utils/Scripting/ScriptBindings.h"

namespace Falcor
{
ref<GraphicsProgram> GraphicsProgram::create(ref<Device> pDevice, const Desc& desc, const Program::DefineList& programDefines)
{
    return ref<GraphicsProgram>(new GraphicsProgram(pDevice, desc, programDefines));
}

ref<GraphicsProgram> GraphicsProgram::createFromFile(
    ref<Device> pDevice,
    const std::filesystem::path& path,
    const std::string& vsEntry,
    const std::string& psEntry,
    const DefineList& programDefines
)
{
    Desc d(path);
    d.vsEntry(vsEntry).psEntry(psEntry);
    return create(pDevice, d, programDefines);
}

GraphicsProgram::GraphicsProgram(ref<Device> pDevice, const Desc& desc, const Program::DefineList& programDefines)
    : Program(pDevice, desc, programDefines)
{}

FALCOR_SCRIPT_BINDING(GraphicsProgram)
{
    pybind11::class_<GraphicsProgram, ref<GraphicsProgram>>(m, "GraphicsProgram");
}
} // namespace Falcor
