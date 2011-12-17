install(
	FILES
	iceplayer.png
	iceplayer_tray.png
	DESTINATION
	${datadir}/iceplayer
	)

install(
	FILES 
	iceplayer.desktop
	DESTINATION
	${datadir}/applications)

install(DIRECTORY skin
	    DESTINATION ${datadir}/iceplayer
		REGEX ".*.svn" EXCLUDE
		REGEX "Makefil.*" EXCLUDE)

