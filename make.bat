set CC65LIB=c:\cc65\lib
cl65 -DBDWGC=0 -DCC65 -W -t none -C sbc2rom.cfg -o lysp.65b -m lysp.map -l lysp.c gc.c %CC65LIB%\sbc2rom.lib
del *.o /Q >nul

