C:\Factory\Tools\RDMD.exe /RC out

COPY /B Cipher\Release\Cipher.exe out\fCipher.exe
COPY /B AUTHORS out
COPY /B LICENSE out
COPY /B Readme_man.txt out\Readme.txt

rem C:\Factory\Tools\zcp.exe out C:\app\fCipher

C:\Factory\SubTools\zip.exe /O out fCipher

PAUSE
