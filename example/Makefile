SOURCE=mandel.c
TARGET?=mandel
PLUGIN_DIR=../plugin/
DEMO_PLUGIN=$(PLUGIN_DIR)/attribute_demo_plugin.so
PLUGIN_DUMP=out.xml

$(PLUGIN_DUMP) : $(SOURCE) $(DEMO_PLUGIN)
	$(CC) -fplugin=$(DEMO_PLUGIN) -fplugin-arg-attribute_demo_plugin-out=$(PLUGIN_DUMP) $(SOURCE) -o /dev/null -lm

$(DEMO_PLUGIN) : 
	make -C $(PLUGIN_DIR)



