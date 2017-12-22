/*
 *  Copyright (c) Christian Fibich, 2017 <christian.fibich@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  --------------------------------------------------------------------
 *  Implements a simple GCC plugin that dumps the position, name and type
 *  of some C elements when they have their __attribute__((ATTRIBUTE_DEMO))
 *  set.
 *  --------------------------------------------------------------------
 */

#include <gcc-plugin.h>
#include <plugin.h>
#include <stdlib.h>
#include <config.h>
#include <system.h>
#include <c-family/c-common.h>
#include <attribs.h>
#include <tree.h>
#include <stdio.h>

#define PLUGIN_NAME "ATTRIBUTE_DEMO"

int plugin_is_GPL_compatible;

char const *plugin_name = "";

/* Structure for passing data to functions */
struct attribute_demo_plugin_data {
    char const *output_file_name;
    FILE *outfp;
} *attribute_demo;

/* Attribute handler callback */
static tree handle_attribute_demo_attribute (tree *node, tree name, tree args,
        int flags, bool *no_add_attrs)
{
    int rv = 0;
    char const *attr = IDENTIFIER_POINTER(name);
    char const *func = NULL;
    int ok = 0;

    /* unused params */
    (void) args;
    (void) flags;
    (void) no_add_attrs;

    if (VAR_P(*node)) {
        ok = 1;
        rv = fputs("<variable",attribute_demo->outfp);
        if (rv == EOF) {
            error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        }

        if (cfun) {
            func = IDENTIFIER_POINTER(TREE_VALUE(cfun->decl));
            rv = fprintf(attribute_demo->outfp," func=\"%s\"", func);
            if (rv == EOF) {
                error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
            }
        }

    } else if (TREE_CODE(*node) == PARM_DECL) {
        ok = 1;

        rv = fputs("<parameter",attribute_demo->outfp);
        if (rv == EOF) {
            error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        }

        tree func_decl = DECL_CONTEXT(*node);

        if(func_decl != NULL && TREE_CODE(func_decl) == FUNCTION_DECL) {

            func = IDENTIFIER_POINTER(TREE_VALUE(func_decl));
            rv = fprintf(attribute_demo->outfp," func=\"%s\"", func);
            if (rv == EOF) {
                error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
            }
        }

    } else if (TREE_CODE(*node) == FUNCTION_DECL) {
        ok = 1;
        rv = fprintf(attribute_demo->outfp,"<function");
        if (rv == EOF) {
            error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        }
    } else if (TREE_CODE(*node) == LABEL_DECL) {
        ok = 1;
        rv = fprintf(attribute_demo->outfp,"<label");
        if (rv == EOF) {
            error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        }
    }
    if (ok) {

        rv = fprintf(attribute_demo->outfp," attribute=\"%s\" name=\"%s\" "
                     "file=\"%s\" line=\"%d\" />\n",attr, IDENTIFIER_POINTER(TREE_VALUE(*node)),
                     DECL_SOURCE_FILE(*node),DECL_SOURCE_LINE(*node));
        if (rv == EOF) {
            error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        }

    } else {
        warning(0,"Plugin %s: Ignored attribute %s for this construct: %d",plugin_name,attr,TREE_CODE(*node));
    }


    return NULL_TREE;
}

/* Attribute definition */
static struct attribute_spec attribute_demo_attrs[] = {
    { PLUGIN_NAME,              0, 0, false,  false, false, handle_attribute_demo_attribute, false },
    { NULL,                     0, 0, false,  false, false, NULL, false },
};

/* Plugin callback called during attribute registration.
Registered with register_callback (plugin_name, PLUGIN_ATTRIBUTES, register_attributes, NULL)
*/
static void register_attributes (void *event_data, void *data)
{
    /* unused params */
    (void) data;
    (void) event_data;

    for (unsigned int i = 0; attribute_demo_attrs[i].name != NULL ; i++)
        register_attribute (&attribute_demo_attrs[i]);
}

static void error_usage(const char *msg)
{
    error("Plugin %s: %s\n\nRequired argument(s):\n\t-fplugin-arg-%s-out=OUTPUT_FILE\n",plugin_name,msg,plugin_name);
    xexit(-1);
}

/* Cleanup function (write file footer, close file) */
static void cleanup (void *event_data, void *data)
{
    /* unused params */
    (void) event_data;
    struct attribute_demo_plugin_data *attribute_demo = (struct attribute_demo_plugin_data *) data;

    if(fprintf(attribute_demo->outfp,"</attributes>\n") < 0) {
        error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
    }

    if (fclose(attribute_demo->outfp) != 0) {
        error("Plugin %s: Could not close %s: %s\n",plugin_name,attribute_demo->output_file_name,xstrerror(errno));
    }
    free(attribute_demo);
}

/* Plugin initalization function */
int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{

    /* unused params */
    (void) version;

    attribute_demo = (struct attribute_demo_plugin_data *)xmalloc(sizeof(struct attribute_demo_plugin_data));

    attribute_demo->output_file_name = NULL;
    attribute_demo->outfp     = NULL;

    plugin_name = plugin_info->base_name;

    /* Check args */
    for (int optind = 0; optind < plugin_info->argc; optind++) {
        struct plugin_argument *arg = &plugin_info->argv[optind];

        if (strcmp(arg->key,"out") == 0) {
            attribute_demo->output_file_name = arg->value;
        }

    }

    if (attribute_demo->output_file_name == NULL) {
        error_usage("No output file given.");
    }

    if((attribute_demo->outfp = fopen(attribute_demo->output_file_name,"w")) == NULL) {
        error("Plugin %s: Could not open %s: %s.\n",plugin_name,attribute_demo->output_file_name,xstrerror(errno));
        xexit(-1);
    }

    if(fprintf(attribute_demo->outfp,"<?xml version=\"1.0\" ?>\n<attributes>\n") < 0) {
        error("Plugin %s: Could not write to %s: %s.\n", plugin_name, attribute_demo->output_file_name, xstrerror(errno));
        xexit(-1);
    }

    /* Register the attribute callbacks */
    register_callback(PLUGIN_NAME,PLUGIN_ATTRIBUTES,register_attributes,NULL);
    register_callback(PLUGIN_NAME,PLUGIN_FINISH,cleanup,attribute_demo);
    return 0;
}



