:: Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

for /F "tokens=3 delims=: " %%H in ('sc query "defaultMediaServer" ^| findstr "        STATE"') do (
  if /I "%%H" NEQ "STOPPED" (
   REM Put your code you want to execute here
   REM For example, the following line
   net stop "defaultMediaServer"
  )
)

for /F "tokens=3 delims=: " %%H in ('sc query "defaultMediaServer" ^| findstr "        STATE"') do (
  if /I "%%H" NEQ "RUNNING" (
   REM Put your code you want to execute here
   REM For example, the following line
   net start "defaultMediaServer"
  )
)