#!lua

-- A solution contains projects, and defines the available configurations
solution "RaycastedTunnel"
   configurations { "Debug", "Release" }
   platforms { "native", "x64", "x32" }
   --startproject "iDK2_Test" -- Set this project as startup project
   location ( "./" ) -- Location of the solutions
         
    -- Project to test the library
    project "RaycastedTunnel"
      kind "ConsoleApp"
      language "C++"
      location ( "./RaycastedTunnel/" ) 
      targetdir ("./RaycastedTunnel/bin/")
      
      includedirs { 
           "./RaycastedTunnel/include", 
           "./RaycastedTunnel/dependencies/SFML/include"
      }
      
      libdirs {
          "./RaycastedTunnel/dependencies/SFML/lib", 
          "./RaycastedTunnel/dependencies/SFML/bin"
      }
      
      files{ group = "include", "./RaycastedTunnel/include/**.h" } -- include filter and get the files
      files{ group = "src", "./RaycastedTunnel/src/**.cc", "./RaycastedTunnel/src/**.cpp" } -- src filter and get the files
      


       configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "ExtraWarnings"}
         links {
          "sfml-system-d",
          "sfml-window-d",
          "sfml-graphics-d",
          "sfml-audio-d",
          "sfml-network-d",
         }

       configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "ExtraWarnings" }