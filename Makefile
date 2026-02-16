DOCS_SRC := Doxyfile README.md src include examples manual
DOCS_WATCH_EVENTS := modify close_write move move_self create delete delete_self

.PHONY: format
format:
	@clang-format -i -style=file -- src/**.c include/**/*.h examples/**.c

.PHONY: watch_docs
watch_docs:
	@doxygen
	@while inotifywait -r $(foreach ev,$(DOCS_WATCH_EVENTS),-e $(ev)) $(DOCS_SRC); do \
		echo "Change detected, rebuilding docs..."; \
		doxygen; \
	done
