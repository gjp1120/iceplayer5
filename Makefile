.PHONY: all clean

all:
	@echo "编译开始"
	sh build.sh

clean:
	@echo "清理开始"
	find -name '#*#' -print -exec rm {} \;
	find -name '*.swp' -print -exec rm {} \;
	find -name '*~' -print -exec rm {} \;
	rm -rf build


