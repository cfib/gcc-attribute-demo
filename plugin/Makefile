CC=gcc
PLUGIN_NAME=attribute_demo_plugin
PLUGINDIR=$(shell $(CC) -print-file-name=plugin)

.PHONY: clean

$(PLUGIN_NAME).so : $(PLUGIN_NAME).o
	$(CC) -fPIC -shared -fno-rtti $< -o $@

$(PLUGIN_NAME).o : $(PLUGIN_NAME).c
	$(CC) -Wall -Wextra -Werror -pedantic -std=c++11 -I$(PLUGINDIR)/include -x c++ -fPIC -fno-rtti -c $< -o $@

clean :
	rm $(PLUGIN_NAME).o
