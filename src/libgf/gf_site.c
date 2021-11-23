/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_site.c
** @brief Site the document file information.
*/
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_array.h>
#include <libgf/gf_path.h>
#include <libgf/gf_file_info.h>
#include <libgf/gf_site.h>

#include "gf_local.h"

/* -------------------------------------------------------------------------- */

/*!
** @brief A subject of entries
**
*/

struct gf_category {
  gf_string* id;    ///< String usable for a URL or an identifier etc.
  gf_string* name;  ///< Printable name
};

static gf_status
category_init(gf_category* cat) {
  gf_validate(cat);

  cat->id = NULL;
  cat->name = NULL;

  return GF_SUCCESS;
}

static gf_status
category_prepare(gf_category* cat) {
  gf_validate(cat);

  _(gf_string_new(&cat->id));
  _(gf_string_new(&cat->name));
  
  return GF_SUCCESS;
}

gf_status
gf_category_new(gf_category** cat) {
  gf_status rc = 0;
  gf_category* tmp = NULL;
  
  gf_validate(cat);
  
  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = category_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = category_prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_category_free(tmp);
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

void
gf_category_free(gf_category* cat) {
  if (cat) {
    if (cat->id) {
      gf_string_free(cat->id);
    }
    if (cat->name) {
      gf_string_free(cat->name);
    }
    (void)category_init(cat);
    gf_free(cat);
  }
}

static void
category_free(gf_any* any) {
  if (any) {
    gf_category_free((gf_category*)any->ptr);
  }
}

gf_status
gf_category_set_id(gf_category* cat, const gf_string* id) {
  gf_validate(cat);
  gf_validate(!gf_string_is_empty(id));

  _(gf_string_copy(cat->id, id));
  
  return GF_SUCCESS;
}

gf_status
gf_category_set_name(gf_category* cat, const gf_string* name) {
  gf_validate(cat);
  gf_validate(!gf_string_is_empty(name));

  _(gf_string_copy(cat->name, name));

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Site entry
**
*/

struct gf_entry {
  gf_entry_type  type;
  gf_entry_state state;
  gf_string*     title;
  gf_string*     author;
  gf_datetime    date;
  gf_array*      description; ///< A description composed with paragraph strings
  gf_file_info*  file_info;   ///< The info file. (site.gf, meta.gf ...)
  gf_string*     method;
  gf_site*       site;
  gf_path*       output_path;
  gf_array*      subject_set; ///< The array of gf_category objects
  gf_array*      keyword_set; ///< The array of gf_category objects
};

static gf_status
entry_init(gf_entry* entry) {
  gf_validate(entry);

  entry->type        = GF_ENTRY_TYPE_UNKNOWN;
  entry->state       = GF_ENTRY_STATE_UNKNOWN;
  entry->title       = NULL;
  entry->author      = NULL;
  entry->date        = 0;
  entry->description = NULL;
  entry->file_info   = NULL;
  entry->method      = NULL;
  entry->site        = NULL;
  entry->output_path = NULL;
  entry->subject_set = NULL;
  entry->keyword_set = NULL;
  
  return GF_SUCCESS;
}

static gf_status
entry_prepare(gf_entry* entry) {
  gf_validate(entry);

  _(gf_array_new(&entry->description));
  _(gf_array_set_free_fn(entry->description, gf_string_free_any));
  _(gf_string_new(&entry->method));
  _(gf_path_new(&entry->output_path, ""));
  _(gf_array_new(&entry->subject_set));
  _(gf_array_set_free_fn(entry->subject_set, category_free));
  _(gf_array_new(&entry->keyword_set));
  _(gf_array_set_free_fn(entry->keyword_set, category_free));

  return GF_SUCCESS;
}

gf_status
gf_entry_new(gf_entry** entry) {
  gf_status rc = 0;
  gf_entry* tmp = NULL;
  
  gf_validate(entry);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = entry_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = entry_prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_entry_free(tmp);
    gf_throw(rc);
  }

  *entry = tmp;
  
  return GF_SUCCESS;
}

void
gf_entry_free(gf_entry* entry) {
  if (entry) {
    if (entry->title) {
      gf_string_free(entry->title);
    }
    if (entry->author) {
      gf_string_free(entry->author);
    }
    if (entry->method) {
      gf_string_free(entry->method);
    }
    if (entry->output_path) {
      gf_path_free(entry->output_path);
    }
    if (entry->subject_set) {
      gf_array_free(entry->subject_set);
    }
    if (entry->keyword_set) {
      gf_array_free(entry->keyword_set);
    }
    entry_init(entry);
    gf_free(entry);
  }
}

static void
entry_free(gf_any* any) {
  if (any) {
    gf_entry_free((gf_entry*)any->ptr);
  }
}

gf_status
gf_entry_set_type(gf_entry* obj, gf_entry_type type) {
  gf_validate(obj);
  obj->type = type;
  return GF_SUCCESS;
}

gf_status
gf_entry_set_title(gf_entry* obj, const gf_string* title) {
  gf_validate(obj);
  gf_validate(title);

  _(gf_string_assign(&obj->title, title));
  
  return GF_SUCCESS;
}

gf_status
gf_entry_set_author(gf_entry* obj, const gf_string* author) {
  gf_validate(obj);
  gf_validate(author);

  _(gf_string_assign(&obj->author, author));
  
  return GF_SUCCESS;
}

gf_status
gf_entry_set_date(gf_entry* obj, gf_datetime date) {
  gf_validate(obj);

  obj->date = date;
  
  return GF_SUCCESS;
}

gf_status
gf_entry_set_file_info(gf_entry* entry, gf_file_info* file_info) {
  gf_validate(entry);
  gf_validate(file_info);

  entry->file_info = file_info;
  
  return GF_SUCCESS;
}

const gf_char*
gf_entry_get_full_path_string(gf_entry* entry) {
  gf_status rc = 0;
  const gf_char* path = NULL;
  
  if (!entry || !entry->file_info) {
    return NULL;
  }
  rc = gf_file_info_get_full_path(entry->file_info, &path);
  if (rc != GF_SUCCESS) {
    return NULL;
  }
  
  return path;
}

/* -------------------------------------------------------------------------- */

static gf_status
entry_read_xml_file(xmlDocPtr* doc, gf_entry* entry) {
  const gf_char* path = NULL;
  xmlDocPtr tmp = NULL;
  
  static const int option =
    XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_XINCLUDE;
  
  gf_validate(doc);
  gf_validate(entry);

  path = gf_entry_get_full_path_string(entry);
  assert(!gf_strnull(path));

  tmp = xmlReadFile(path, NULL, option);
  if (!tmp) {
    gf_raise(GF_E_API, "Failed to read an XML file.");
  }

  *doc = tmp;
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static gf_status
entry_set_title(gf_entry* entry, xmlNodePtr node) {
  gf_validate(entry);
  gf_validate(node);
  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (cur->type == XML_TEXT_NODE) {
      _(gf_string_append(entry->title, (char*)cur->content));
    } else {
      _(entry_set_title(entry, cur));
    }
  }
  return GF_SUCCESS;
}

static gf_status
entry_set_author(gf_entry* entry, xmlNodePtr node) {
  gf_validate(entry);
  gf_validate(node);
  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (cur->type == XML_TEXT_NODE) {
      // NOTE: By now, we don't care about the order of first and last name.
      // We concate the string components of an author in appearance order.
      if (gf_string_is_empty(entry->author) > 0) {
        _(gf_string_append(entry->author, " "));
      }
      _(gf_string_append(entry->author, (char*)cur->content));
    } else {
      _(entry_set_author(entry, cur));
    }
  }
  return GF_SUCCESS;
}

static gf_status
entry_set_date(gf_entry* entry, xmlNodePtr node) {
  xmlNodePtr child = NULL;
  
  gf_validate(entry);
  gf_validate(node);

  child = node->children;
  if (child->type != XML_TEXT_NODE) {
    gf_raise(GF_E_DATA, "Invalid XML data.");
  }
  _(gf_date_parse((const char*)child->content, &entry->date));
  
  return GF_SUCCESS;
}

static gf_status
entry_set_description(gf_entry* entry, xmlNodePtr node) {
  gf_status rc = 0;
  
  gf_validate(entry);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (cur->type == XML_TEXT_NODE) {
      gf_string* s = NULL;
      
      _(gf_string_new(&s));
      rc = gf_string_set(s, (char*)cur->content);
      if (rc != GF_SUCCESS) {
        gf_string_free(s);
        gf_throw(rc);
      }
      rc = gf_array_add(entry->description, (gf_any){ .ptr = s });
      if (rc != GF_SUCCESS) {
        gf_string_free(s);
        gf_throw(rc);
      }
    } else {
      _(entry_set_description(entry, cur));
    }
  }

  return GF_SUCCESS;
}

static gf_status
entry_set_subject_set(gf_entry* entry, xmlNodePtr node) {
  gf_status rc = 0;
  xmlNodePtr subject_node = NULL;
  
  gf_validate(entry);
  gf_validate(node);

  // NOTE: We process the first child of the subject_set node.
  subject_node = node->children;
  if (subject_node->type != XML_ELEMENT_NODE) {
    gf_raise(GF_E_DATA, "Invalid XML data.");
  }
  if (!!xmlStrcmp(subject_node->name, BAD_CAST"subject")) {
    gf_raise(GF_E_DATA, "Invalid XML data.");
  }
  for (xmlNodePtr cur = subject_node->children; cur; cur = cur->next) {
    if (cur->type == XML_ELEMENT_NODE) {
      if (!xmlStrcmp(cur->name, BAD_CAST"subjectterm")) {
        gf_string* s = NULL;
        gf_category* cat = NULL;
        xmlChar* id = NULL;

        id = xmlGetProp(cur, BAD_CAST"id");
        if (!id) {
          gf_raise(GF_E_DATA, "Invalid XML data.");
        }
        if (cur->children->type != XML_TEXT_NODE) {
          gf_raise(GF_E_DATA, "Invalid XML data.");
        }
        _(gf_category_new(&cat));

        rc = gf_string_new(&s);
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_string_set(s, (const char*)id);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_category_set_id(cat, s);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_string_set(s, (const char*)cur->children->content);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_category_set_name(cat, s);
        gf_string_free(s);
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_array_add(entry->keyword_set, (gf_any){ .ptr = cat });
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
      }
    }
  }
  
  return GF_SUCCESS;
}

static gf_status
entry_set_keyword_set(gf_entry* entry, xmlNodePtr node) {
  gf_status rc = 0;
  
  gf_validate(entry);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (cur->type == XML_ELEMENT_NODE) {
      if (!xmlStrcmp(cur->name, BAD_CAST"keyword")) {
        gf_string* s = NULL;
        gf_category* cat = NULL;
        xmlChar* id = NULL;

        id = xmlGetProp(cur, BAD_CAST"id");
        if (!id) {
          gf_raise(GF_E_DATA, "Invalid XML data.");
        }
        if (cur->children->type != XML_TEXT_NODE) {
          gf_raise(GF_E_DATA, "Invalid XML data.");
        }
        _(gf_category_new(&cat));

        rc = gf_string_new(&s);
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_string_set(s, (const char*)id);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_category_set_id(cat, s);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_string_set(s, (const char*)cur->children->content);
        if (rc != GF_SUCCESS) {
          gf_string_free(s);
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_category_set_name(cat, s);
        gf_string_free(s);
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
        rc = gf_array_add(entry->keyword_set, (gf_any){ .ptr = cat });
        if (rc != GF_SUCCESS) {
          gf_category_free(cat);
          gf_throw(rc);
        }
      }
    }
  }
  
  return GF_SUCCESS;
}

static gf_status
entry_set_document_info(gf_entry* entry) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlNodePtr cur = NULL;
  xmlChar* prop = NULL;

  gf_validate(entry);

  _(entry_read_xml_file(&doc, entry));
  assert(doc);

  root = xmlDocGetRootElement(doc);
  if (!root) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  cur = root->children;
  if (!cur) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  /* gf_entry::type */
  entry->type = GF_ENTRY_TYPE_DOCUMENT;
  /* gf_entry::state */
  entry->state = GF_ENTRY_STATE_PUBLISHED; // By now, draft mode is unavailable.
  /* gf_entry::method */
  prop = xmlGetProp(cur, BAD_CAST"role");
  if (prop) {
    rc = gf_string_set(entry->method, (const char*)prop);
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  } else {
    rc = gf_string_set(entry->method, (const char*)cur->name);
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  }
  /* "info" elements */
  cur = cur->children;
  if (!cur || !!xmlStrcmp(cur->name, BAD_CAST"info")) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  for (cur = cur->children; cur; cur = cur->next) {
    if (!xmlStrcmp(cur->name, BAD_CAST"title")) {
      rc = entry_set_title(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"author")) {
      rc = entry_set_author(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"pubdate")) {
      rc = entry_set_date(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"description")) {
      rc = entry_set_description(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"subjectset")) {
      rc = entry_set_subject_set(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"keywordset")) {
      rc = entry_set_keyword_set(entry, cur);
    } else {
      /* do nothing */
    }
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  }
  
  xmlFreeDoc(doc);
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static gf_status
entry_set_meta_info(gf_entry* entry) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlNodePtr cur = NULL;
  
  gf_validate(entry);

  _(entry_read_xml_file(&doc, entry));
  assert(doc);

  root = xmlDocGetRootElement(doc);
  if (!root) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  /* meta element*/
  cur = root->children;
  if (!cur) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  /* gf_entry::type */
  entry->type = GF_ENTRY_TYPE_SECTION;
  /* gf_entry::method */
  rc = gf_string_set(entry->method, "index");
  if (rc != GF_SUCCESS) {
    xmlFreeDoc(doc);
    gf_throw(rc);
  }
  /* children of meta element */
  for (cur = cur->children; cur; cur = cur->next) {
    if (!xmlStrcmp(cur->name, BAD_CAST"title")) {
      rc = entry_set_title(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"author")) {
      rc = entry_set_author(entry, cur);
    } else if (!xmlStrcmp(cur->name, BAD_CAST"description")) {
      rc = entry_set_description(entry, cur);
    } else {
      /* do nothing */
    }
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  }
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Site database
*/

struct gf_site {
  gf_file_info* root;       ///< The root of a site tree. (directory)
  gf_array*     entry_set;  ///< Entries to process
};

static gf_status
site_init(gf_site* site) {
  gf_validate(site);

  site->root = NULL;
  site->entry_set = NULL;
  
  return GF_SUCCESS;
}

static gf_status
site_prepare(gf_site* site) {
  _(gf_array_new(&site->entry_set));
  _(gf_array_set_free_fn(site->entry_set, entry_free));
  return GF_SUCCESS;
}

gf_status
gf_site_new(gf_site** site) {
  gf_status rc = 0;
  gf_site* tmp = NULL;

  _(gf_malloc((gf_ptr *)&tmp, sizeof(*tmp)));
  rc = site_init(tmp);
  if (rc != GF_SUCCESS ) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = site_prepare(tmp);
  if (rc != GF_SUCCESS ) {
    gf_site_free(tmp);
    gf_throw(rc);
  }
  
  *site = tmp;
  
  return GF_SUCCESS;
}

void
gf_site_free(gf_site* site) {
  if (site) {
    (void)gf_site_reset(site);
    gf_free(site);
  }
}

gf_status
gf_site_reset(gf_site* site) {
  gf_validate(site);

  if (site->root) {
    gf_file_info_free(site->root);
    site->root = NULL;
  }
  
  return GF_SUCCESS;
}

static gf_bool
site_does_file_name_equal(
  const gf_file_info* file_info, const gf_char* file_name) {
  gf_status rc = 0;
  const gf_char* name = NULL;
  gf_bool ret = GF_FALSE;

  if (!file_info || !file_name) {
    assert(0);
    return GF_FALSE;
  }

  rc = gf_file_info_get_file_name(file_info, &name);
  if (rc != GF_SUCCESS) {
    return GF_FALSE;
  }
  // TODO: strcmp must be wrapped by gf_strcmp
  if (!strcmp(name, file_name)) {
    ret = GF_TRUE;
  } else {
    ret = GF_FALSE;
  }

  return ret;
}

typedef gf_bool (*does_file_name_equal_fn)(const gf_file_info* file_info);

static gf_bool
site_is_document_file(const gf_file_info* file_info) {
  static const gf_char name[] = "index.dbk";
  return site_does_file_name_equal(file_info, name);
}

static gf_bool
site_is_meta_file(const gf_file_info* file_info) {
  static const gf_char name[] = "meta.gf";
  return site_does_file_name_equal(file_info, name);
}

static gf_bool
site_is_asset_directory(const gf_file_info* file_info) {
  static const gf_char name[] = "_";
  return site_does_file_name_equal(file_info, name);
}

static gf_bool
site_does_directory_have_file(
  const gf_file_info* file_info, does_file_name_equal_fn equal_fn) {
  gf_status rc = 0;
  gf_size_t cnt = 0;
  gf_bool ret = GF_FALSE;

  gf_validate(file_info);
  gf_validate(equal_fn);
  
  if (!file_info) {
    assert(0);
    return GF_FALSE;
  }
  ret = GF_FALSE;
  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_file_info* child_info = 0;

    rc = gf_file_info_get_child(file_info, i, &child_info);
    if (rc != GF_SUCCESS) {
      assert(0);
      return GF_FALSE;
    }
    if (equal_fn(child_info)) {
      ret = GF_TRUE;
      break;
    }
  }

  return ret;
}

static gf_bool
site_does_directory_have_meta_file(const gf_file_info* file_info) {
  static const does_file_name_equal_fn fn = site_is_meta_file;
  return site_does_directory_have_file(file_info, fn);
}

static gf_bool
site_does_directory_have_document_file(const gf_file_info* file_info) {
  static const does_file_name_equal_fn fn = site_is_document_file;
  return site_does_directory_have_file(file_info, fn);
}

static gf_status
site_collect_document_info(gf_entry* entry, gf_file_info* file_info) {
  gf_size_t cnt = 0;
  gf_file_info* child_info = NULL;
  
  gf_validate(entry);
  gf_validate(file_info);

  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    _(gf_file_info_get_child(file_info, i, &child_info));
    if (site_is_document_file(file_info)) {
      _(gf_entry_set_file_info(entry, child_info));
      _(entry_set_document_info(entry));
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_collect_meta_info(gf_entry* entry, gf_file_info* file_info) {
  gf_size_t cnt = 0;
  gf_file_info* child_info = NULL;
  
  gf_validate(entry);
  gf_validate(file_info);

  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    _(gf_file_info_get_child(file_info, i, &child_info));
    if (site_is_meta_file(file_info)) {
      _(gf_entry_set_file_info(entry, child_info));
      _(entry_set_meta_info(entry));
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_scan_directories(gf_site* site, gf_file_info* file_info) {
  gf_status rc = 0;
  gf_size_t cnt = 0;
  
  gf_validate(site);
  gf_validate(file_info);

  /* non-directory files are not our target */
  if (!gf_file_info_is_directory(file_info)) {
    return GF_SUCCESS;
  }
  /* asset directory is not our target */
  if (site_is_asset_directory(file_info)) {
    return GF_SUCCESS;
  }

  if (site_does_directory_have_document_file(file_info)) {
    gf_entry* entry = NULL;

    _(gf_entry_new(&entry));
    rc = site_collect_document_info(entry, file_info);
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
  } else if (site_does_directory_have_meta_file(file_info)) {
    gf_entry* entry = NULL;

    _(gf_entry_new(&entry));
    rc = site_collect_meta_info(entry, file_info);
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
  } else {
    /* This directory is not our target */
    return GF_SUCCESS;
  }

  /* process children  */
  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_file_info* child_info = 0;

    _(gf_file_info_get_child(file_info, i, &child_info));
    _(site_scan_directories(site, child_info));
  }
  
  return GF_SUCCESS;
}

gf_status
gf_site_scan(gf_site** site, const gf_path* path) {
  gf_status rc = 0;
  gf_site* tmp = NULL;
  gf_file_info* file_info = NULL;
  
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  rc = gf_site_new(&tmp);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  /* Scan files */
  _(gf_file_info_scan(&file_info, path));
  tmp->root = file_info;

  /* Traverse */
  rc = site_scan_directories(tmp, tmp->root);
  if (rc != GF_SUCCESS) {
    gf_site_free(tmp);
    gf_throw(rc);
  }

  *site = tmp;

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*
** File IO of the site structure.
**
**
*/

gf_status
gf_site_write_file(const gf_site* site, const gf_path* path) {
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  return GF_SUCCESS;
}

gf_status
gf_site_read_file(gf_site** site, const gf_path* path) {
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  return GF_SUCCESS;
}
