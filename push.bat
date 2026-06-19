@echo off
cd \source\frixos
date /t
time /t
echo pushing to v%1
copy build\frixos.bin c:\source\frixos-web\www\revE%1.bin /b/y
cd spiffs
del files.txt 2>nul
rem Build files.txt manifest recursively, as forward-slash relative paths
rem (e.g. js/core.js). SPIFFS is flat, so the path is the object name and
rem the OTA client downloads each line verbatim from update_dir/<line>.
setlocal enabledelayedexpansion
set "base=%CD%\"
(for /r %%F in (*) do (
  set "rel=%%F"
  set "rel=!rel:%base%=!"
  set "rel=!rel:\=/!"
  echo !rel!
)) > ..\files.txt
endlocal
copy ..\files.txt .\ > nul
echo re-created files.txt recursively
mkdir c:\source\frixos-web\www\%1 2>nul
rem /s+/e recurse into js, css, i18n and recreate the tree on the server
xcopy * c:\source\frixos-web\www\%1\ /s /e /y /i > nul
echo copied spiffs with subfolders
cd ..
