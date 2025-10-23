--Default to gmake
if _ACTION ~= "gmake" and _ACTION ~= "clean" then
  _ACTION = "gmake"
end

local function file_exists(p)
  local f = io.open(p, "rb"); if f then f:close(); return true end
  return false
end

local function detect_cuda_home() --Detect CUDA installation path
  local env = os.getenv("CUDA_HOME") or os.getenv("CUDA_PATH")
  if env and file_exists(env .. "/bin/nvcc") then return env end

  local p = io.popen("command -v nvcc 2>/dev/null")
  local nvcc = p and p:read("*l") or nil
  if p then p:close() end
  if nvcc then
    local root = nvcc:gsub("/bin/nvcc$", "")
    if file_exists(root .. "/bin/nvcc") then return root end
  end

  local candidates = {}
  for _, d in ipairs(os.matchdirs("/usr/local/cuda-[0-9]*.[0-9]*")) do
    table.insert(candidates, d)
  end
  table.insert(candidates, "/usr/local/cuda")
  table.insert(candidates, "/opt/cuda")
  table.sort(candidates, function(a, b) return a > b end)

  for _, d in ipairs(candidates) do
    if file_exists(d .. "/bin/nvcc") then return d end
  end

  return nil
end

local function detect_nvidia_smi()
  local p = io.popen("command -v nvidia-smi 2>/dev/null")
  local smi = p and p:read("*l") or nil
  if p then p:close() end
  if smi and file_exists(smi) then return smi end

  for _, f in ipairs(os.matchfiles("/usr/bin/nvidia-smi")) do if file_exists(f) then return f end end
  for _, f in ipairs(os.matchfiles("/usr/lib/nvidia-*/bin/nvidia-smi")) do if file_exists(f) then return f end end
  for _, f in ipairs(os.matchfiles("/usr/lib/wsl/lib/nvidia-smi")) do if file_exists(f) then return f end end
  return nil
end

CUDA_HOME  = detect_cuda_home()
NVIDIA_SMI = detect_nvidia_smi()
HAVE_CUDA  = (CUDA_HOME ~= nil) and (NVIDIA_SMI ~= nil)  --Detects if CUDA is available

newoption({trigger="examples",
  description="Creates makefiles for LASS examples"})

DebugFlags = {"Symbols", "NoPCH", "NoManifest"}
ReleaseFlags = {"Optimize"}

solution "dissco"
  configurations {"Debug", "Release"}
  objdir "obj"

project "lass"
  location "make" 
  language "C++"
  flags {"StaticRuntime"}
  files {"LASS/src/*.cpp", "LASS/src/*.h"}
  excludes {"LASS/src/test/**"}
    includedirs {"/usr/local/include"}
  if(HAVE_CUDA) then 
      includedirs {CUDA_HOME .. "/include"}
    end
  kind "StaticLib"
  targetdir "lib"
  buildoptions {"-Wno-deprecated", "-Wall", "-Wextra", "-std=c++11"}
  if(HAVE_CUDA) then 
    defines { "HAVE_CUDA" }
    configuration "Debug" 
      flags(DebugFlags)
      buildoptions {"-g"}
      prebuildcommands {
        "mkdir -p $(OBJDIR)",
        "nvcc -c ../LASS/CUDA/FilterGPU.cu -Xcompiler -fPIC -g -G -o $(OBJDIR)/FilterGPU.o"
      }
      postbuildcommands {
        "ar rcs $(TARGET) $(OBJDIR)/FilterGPU.o",
        "ranlib $(TARGET)"
      }
    configuration "Release" 
      flags(ReleaseFlags)  
      prebuildcommands {
        "mkdir -p $(OBJDIR)",
        "nvcc -c ../LASS/CUDA/FilterGPU.cu -Xcompiler -fPIC -O3 -o $(OBJDIR)/FilterGPU.o"
      }
      postbuildcommands {
        "ar rcs $(TARGET) $(OBJDIR)/FilterGPU.o",
        "ranlib $(TARGET)"
      }

    configuration {}
  else
    configuration "Debug" 
      flags(DebugFlags)
      buildoptions {"-g"}
    configuration "Release" flags(ReleaseFlags)
  end

project "parser"
  location "make" 
  language "C"
  flags {"StaticRuntime"}
  files {"CMOD/src/parser/lex.yy.c"}
  kind "StaticLib"
  targetdir "lib"
  buildoptions {"-Wno-deprecated", "-std=c++11"}
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)

project "muparser"
  location "make" 
  language "C++"
  flags{"StaticRuntime"}
  files {"CMOD/src/muParser/**.cpp", "CMOD/src/muParser/**.h"}
  kind "StaticLib"
  targetdir "lib"
   buildoptions {"-Wno-deprecated", "-std=c++11"}
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)

project "lcmod"
  location "make" 
  language "C++"
  flags {"StaticRuntime"}
  files {"CMOD/src/**.cpp", "CMOD/src/**.h"}
  excludes {"CMOD/src/Main.*", "CMOD/src/test/**"}
  kind "StaticLib"
  targetdir "lib"
  buildoptions {"-Wno-deprecated", "-std=c++11"}
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)

project "cmod"
  location "make" 
  language "C++"
  flags {"StaticRuntime"}
  files {"CMOD/src/Main.*"}
  kind "ConsoleApp"
  libdirs { "lib", "/usr/local/lib"}
  links   { "lcmod", "lass", "parser","muparser", "pthread", "sndfile" }
  if (HAVE_CUDA) then 
    libdirs { "/usr/local/cuda/lib64" }
    links   { "cudart" }
  end
  linkoptions{"-lxerces-c", "-rdynamic"}
  buildoptions {"-Wno-deprecated", "-std=c++11"}
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)
  configuration "macosx"
    targetdir "bin"

project "UpgradeProjectFormat"
  location "make" 
  language "C++"
  flags {"StaticRuntime"}
  files {"LASSIE/src/UpgradeProjectFormat.*"}
  kind "ConsoleApp"
  libdirs {"lib", "/usr/local/lib"}
  links {"lcmod", "lass", "parser","muparser", "pthread", "sndfile" }
  if( HAVE_CUDA ) then 
    libdirs { CUDA_HOME .. "/lib64", CUDA_HOME .. "/targets/x86_64-linux/lib"}
    links {"cudart"}
  end
  linkoptions{"-lxerces-c"}
  buildoptions {"-Wno-deprecated", "-Wno-register", "-std=c++11"}
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)
  configuration "macosx"
    targetdir "bin"

project "lassie"
  location "make" 
  language "C++"
  kind "ConsoleApp"
  files {"LASSIE/src/**.h", "LASSIE/src/**.cpp"}
  excludes {"LASSIE/src/UpgradeProjectFormat.*", "LASSIE/src/test/**"}
  buildoptions {"`pkg-config --cflags gtkmm-2.4`",
    "-Wno-deprecated-declarations", "-Wno-deprecated", "-std=c++11"}
  linkoptions {"`pkg-config --libs --cflags gtkmm-2.4`", "-Wno-deprecated", "-lxerces-c"}
  libdirs { "/usr/local/lib" }
  links   { "lcmod", "lass", "parser", "pthread", "sndfile" }
  if( HAVE_CUDA ) then 
    libdirs { CUDA_HOME .. "/lib64"}
    links {"cudart"}
  end
  configuration "Debug" 
    flags(DebugFlags)
    buildoptions {"-g"}
  configuration "Release" flags(ReleaseFlags)
  configuration "macosx"
    targetdir "bin"

    -- project "lassie_test"
    -- language "C++"
    -- kind "ConsoleApp"
    -- files {"LASSIE/src/**.h", "LASSIE/src/**.cpp"}
    -- excludes {"LASSIE/src/Main.cpp", "LASSIE/src/UpgradeProjectFormat.*"}
    -- buildoptions {"`pkg-config --cflags gtkmm-2.4`",
    --               "-Wno-deprecated", "-gstabs", "-g"}
    -- linkoptions {"`pkg-config --libs --cflags gtkmm-2.4`", "-Wno-deprecated", "-lxerces-c"}
    -- libdirs {"/usr/local/lib"}
    -- links {"lcmod", "lass", "parser", "pthread", "sndfile", "lassie", "boost_unit_test_framework"}
    -- configuration "Debug" flags(DebugFlags)
    -- postbuildcommands {"./lassie_test"}
    -- configuration "Release" flags(ReleaseFlags)
    -- configuration "macosx"
    -- targetdir "bin"

    -- project "lass_test"
    -- language "C++"
    -- kind "ConsoleApp"
    -- files {"LASS/src/test/**.h", "LASS/src/test/**.cpp"}
    -- buildoptions {"`pkg-config --cflags gtkmm-2.4`",
    --               "-Wno-deprecated", "-gstabs", "-g"}
    -- linkoptions {"`pkg-config --libs --cflags gtkmm-2.4`", "-Wno-deprecated", "-lxerces-c"}
    -- libdirs {"/usr/local/lib"}
    -- includedirs {"/usr/local/include"}
    -- links {"lass", "boost_unit_test_framework"}
    -- configuration "Debug" flags(DebugFlags)
    -- postbuildcommands {"./lass_test"}

    -- project "cmod_test"
    -- language "C++"
    -- kind "ConsoleApp"
    -- files {"CMOD/src/test/**.h", "CMOD/src/test/**.cpp"}
    -- buildoptions {"`pkg-config --cflags gtkmm-2.4`",
    --               "-Wno-deprecated", "-gstabs", "-g"}
    -- linkoptions {"`pkg-config --libs --cflags gtkmm-2.4`", "-Wno-deprecated", "-lxerces-c"}
    -- libdirs {"/usr/local/lib"}
    -- links {"lcmod", "lass", "parser", "pthread", "sndfile", "lassie", "boost_unit_test_framework"}
    -- configuration "Debug" flags(DebugFlags)
    -- postbuildcommands {"./cmod_test"}
    -- configuration "Release" flags(ReleaseFlags)

--------------------------------------------------------------------------------
--                       The LASS Examples Directory
--------------------------------------------------------------------------------
if _OPTIONS["examples"] then
  all_files = os.matchfiles("LASS/examples/*.cpp")
  print("Detected " .. table.getn(all_files) .. " LASS examples")
  print("  from individual .cpp files in 'LASS/examples'")
  for i = 1, table.getn(all_files) do
    example_path = "LASS/examples/"
    example_filename = all_files[i]
    example_name = path.getbasename(all_files[i])

    project(example_name)
      language "C++"
      kind "ConsoleApp"
      flags {"StaticRuntime"}
      files {example_path .. example_name .. ".cpp"}
      includedirs {"LASS/src/"}
      libdirs {"lib/"}
      links {"lass", "pthread"}
      buildoptions {"-Wno-deprecated"}
      postbuildcommands {"rm " .. example_name .. ".make"}
      targetdir("bin/")
      configuration "Debug"
        flags(DebugFlags)
        objdir("obj/Debug/" .. example_name)
        postbuildcommands {"rm -rf obj/Debug/" .. example_name}
      configuration "Release"
        flags(ReleaseFlags)
        objdir("obj/Release/" .. example_name)
        postbuildcommands {"rm -rf obj/Release/" .. example_name}
  end
end

if _ACTION == "clean" then
  print("Removing target and object directories.")
  os.rmdir("lib")
  os.rmdir("bin")
  os.rmdir("obj")
  os.rmdir("make")
end