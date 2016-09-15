PONG_DIR = path.getabsolute("..")
PONG_BUILD_DIR = path.join(PONG_DIR, ".build")
PONG_LIB_DIR = path.join(PONG_DIR, "3rdparty")

JN_DIR = path.getabsolute(path.join(PONG_DIR, "../jn"))

if not os.isdir(JN_DIR) then
  print ("jn not found at" .. JN_DIR)
  os.exit()
end

solution "multipong"
  configurations {
    "debug",
    "release"
  }

  platforms {
    "x32",
    "x64"
  }

  language "C++"
  startproject "client"

dofile (path.join(JN_DIR, "scripts/toolchain.lua"))
toolchain(PONG_BUILD_DIR, PONG_LIB_DIR)

dofile (path.join(JN_DIR, "scripts/jn.lua"))

project "client"
  kind "ConsoleApp"

  files {
    path.join(PONG_DIR, "client/**.cpp"),
    path.join(PONG_DIR, "client/**.h")
  }

  includedirs {
    path.join(JN_DIR, "include"),
    path.join(PONG_DIR, "client"),
  }

project "server"
  kind "ConsoleApp"

  files {
    path.join(PONG_DIR, "server/**.cpp"),
    path.join(PONG_DIR, "server/**.h")
  }

  includedirs {
    path.join(JN_DIR, "include"),
    path.join(PONG_DIR, "server"),
  }
