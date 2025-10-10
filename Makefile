.PHONY: format
format:
	@clang-format -i -style=file -- src/**.c include/**/*.h
