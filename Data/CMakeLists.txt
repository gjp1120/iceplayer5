install(
	FILES
	iceplayer.png
	iceplayer_tray.png
	DESTINATION
	${datadir}
	)

install(
	FILES 
	iceplayer.desktop
	DESTINATION
	${sharedir}/applications)

install(DIRECTORY skin
	    DESTINATION ${datadir}
		REGEX ".*.svn" EXCLUDE
		REGEX "Makefil.*" EXCLUDE)

