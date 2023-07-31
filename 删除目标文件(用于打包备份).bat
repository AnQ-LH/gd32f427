rd /Q /S build
rd /Q /S .xmake
rd /Q /S Output\Objects
rd /Q /S Output\Listings
rd /Q /S Project\MDK-ARM(AC6)\DebugConfig
del /Q Project\MDK-ARM(AC6)\*.bak
del /Q Project\MDK-ARM(AC6)\*.dep
del /Q Project\MDK-ARM(AC6)\JLink*
del /Q Project\MDK-ARM(AC6)\project.uvgui.*
del /Q *.map