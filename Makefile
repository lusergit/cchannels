EXAMPLES_DIR = examples

.PHONY: make_examples

make_examples:
	@echo "Generating examples ... "
	$(MAKE) -C $(EXAMPLES_DIR)
	@echo "Done. Examples available in ./examples/ folder"

clean_examples:
	@echo "Deleting examples ... "
	$(MAKE) clean -C $(EXAMPLES_DIR)
	@echo "Done."
