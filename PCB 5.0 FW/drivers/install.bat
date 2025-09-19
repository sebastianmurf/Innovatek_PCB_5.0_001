@echo off
 
Set RegQry=HKLM\Hardware\Description\System\CentralProcessor\0
REG.exe Query %RegQry% > CheckOS.txt 
Find /i "x86" < CheckOS.txt > nul
If %ERRORLEVEL% == 0 (
    cls
    echo Please disconnect any Virtual Com Port 
    pause
    cls
    echo Deinstalling, please wait...
    dpinst_x86.exe /U fseuusbvcomm.inf /D /S > nul
    dpinst_x86.exe /LM /SE /SW /F > nul
    cls
    echo Please connect Virtual Com Port 
    pause
    cls
    echo Installing, please wait...
    dpinst_x86.exe /LM /SE /SW /F > nul
) ELSE (
    cls
    echo Please disconnect any Virtual Com Port 
    pause
    cls
    echo Deinstalling, please wait...
    dpinst_x64.exe /U fseuusbvcomm.inf /D /S > nul
    dpinst_x64.exe /LM /SE /SW /F > nul
    cls
    echo Please connect Virtual Com Port 
    pause
    cls
    echo Installing, please wait...
    dpinst_x64.exe /LM /SE /SW /F > nul
)
rm CheckOS.txt > nul
