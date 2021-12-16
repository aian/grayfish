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
#include <libgf/gf_hash.h>
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
  *cat = tmp;
  
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
** @brief Site entry object.
**
** An entry is a resource of the website, which is determined to be processed by
** the Grayfish convertion processor. Not the all source files in the website
** are regarded as entry objects. Only the following files are processed as
** site entries.
**
**   - First, the directory including meta.gf file is treated as a section entry
**   - Second, the index.dbk is regarded as a main document entry.
*/

struct gf_entry {
  gf_entry_type  type;        ///< Type of a entry
  gf_entry_state state;       ///< State of a entry
  gf_string*     title;       ///< Entry title retrieved from an entry file
  gf_string*     subtitle;    ///< Entry subtitle retrieved from an entry file
  gf_string*     author;      ///< Name of an author read from an entry file
  gf_datetime    date;        ///< Published date read from an entry file
  gf_16u         issue;       ///< The number of an issue
  gf_16u         volume;      ///< The number of a volume
  gf_string*     series;      ///< Numbers of the volumes in a series.
  gf_string*     edition;     ///< The name or number of an edition
  gf_array*      description; ///< A description composed with paragraphs
  gf_file_info*  file_info;   ///< The info file. (meta.gf, index.dbk ...)
  gf_string*     method;      ///< The process type linked to the XSLT name
  gf_site*       site;        ///< The site object which includes this object
  gf_path*       output_path; ///< The output path 
  gf_array*      subject_set; ///< Array of gf_category objects
  gf_array*      keyword_set; ///< Array of gf_category objects
  gf_array*      file_set;    ///< Array of gf_file_info objects
  gf_array*      children;    ///< Entry children
};

static void entry_free(gf_any* any);

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
  entry->file_set    = NULL;
  entry->children    = NULL;
  
  return GF_SUCCESS;
}

static gf_status
entry_prepare(gf_entry* entry) {
  gf_validate(entry);

  _(gf_string_new(&entry->title));
  _(gf_string_new(&entry->author));
  _(gf_array_new(&entry->description));
  _(gf_array_set_free_fn(entry->description, gf_string_free_any));
  _(gf_string_new(&entry->method));
  _(gf_path_new(&entry->output_path, ""));
  _(gf_array_new(&entry->subject_set));
  _(gf_array_set_free_fn(entry->subject_set, category_free));
  _(gf_array_new(&entry->keyword_set));
  _(gf_array_set_free_fn(entry->keyword_set, category_free));
  _(gf_array_new(&entry->file_set)); // this array doesn't free element objects
  _(gf_array_new(&entry->children));
  _(gf_array_set_free_fn(entry->children, entry_free));

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
    if (entry->file_info) {
      gf_file_info_free(entry->file_info);
    }
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
    if (entry->file_set) {
      gf_array_free(entry->file_set);
    }
    if (entry->children) {
      gf_array_free(entry->children);
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
gf_entry_set_file_info(gf_entry* entry, gf_file_info* info) {
  gf_validate(entry);
  gf_validate(info);

  if (entry->file_info) {
    gf_file_info_free(entry->file_info);
    entry->file_info = NULL;
  }
  _(gf_file_info_clone(&entry->file_info, info));
  
  return GF_SUCCESS;
}

const gf_char*
gf_entry_get_file_name_string(const gf_entry* entry) {
  gf_status rc = 0;
  const gf_char* path = NULL;
  
  if (!entry || !entry->file_info) {
    return NULL;
  }
  rc = gf_file_info_get_file_name(entry->file_info, &path);
  if (rc != GF_SUCCESS) {
    return NULL;
  }
  
  return path;
}

const gf_char*
gf_entry_get_full_path_string(const gf_entry* entry) {
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

/*!
** @warning The returned path must be freed.
*/
gf_path*
gf_entry_get_local_path(const gf_entry* entry, const gf_path* root) {
  gf_status rc = 0;
  const gf_char* full_path = NULL;
  gf_path* path = NULL;

  if (!entry || !root) {
    return NULL;
  }
  full_path = gf_entry_get_full_path_string(entry);
  if (!full_path) {
    return NULL;
  }
  rc = gf_path_append_string(&path, root, full_path);
  if (rc != GF_SUCCESS) {
    return NULL;
  }

  return path;
}


gf_bool
gf_entry_is_section(const gf_entry* entry) {
  return entry && entry->type == GF_ENTRY_TYPE_SECTION ? GF_TRUE : GF_FALSE;
}

gf_bool
gf_entry_is_document(const gf_entry* entry) {
  return entry && entry->type == GF_ENTRY_TYPE_DOCUMENT ? GF_TRUE : GF_FALSE;
}

gf_size_t
gf_entry_count_children(const gf_entry* entry) {
  return entry && entry->children ? gf_array_size(entry->children) : 0;
}

gf_status
gf_entry_get_child(gf_entry* entry, gf_size_t index, gf_entry** child) {
  gf_any any = { 0 };
  
  gf_validate(entry);
  gf_validate(child);

  _(gf_array_get(entry->children, index, &any));
  *child = (gf_entry*)(any.ptr);
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static gf_status
entry_read_xml_file(xmlDocPtr* doc, const gf_path* root, gf_entry* entry) {
  gf_path* path = NULL;
  xmlDocPtr tmp = NULL;
  
#ifdef GF_DEBUG_
  static const int option = 0;
#else
  static const int option = GF_XML_PARSE_OPTIONS;
#endif
  
  gf_validate(doc);
  gf_validate(entry);

  path = gf_entry_get_local_path(entry, root);
  if (!path) {
    gf_raise(GF_E_PATH, "Failed to create local path");
  }
  tmp = xmlReadFile(gf_path_get_string(path), NULL, option);
  gf_path_free(path);
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
    if (xmlNodeIsText(cur)) {
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
    if (xmlNodeIsText(cur)) {
      // NOTE: By now, we don't care about the order of first and last name.
      // We concate the string components of an author in appearance order.
      if (!gf_string_is_empty(entry->author)) {
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
  if (!xmlNodeIsText(child)) {
    gf_raise(GF_E_DATA, "Invalid XML data.");
  }
  _(gf_datetime_parse_iso8061_string((const char*)child->content, &entry->date));
  
  return GF_SUCCESS;
}

static gf_status
entry_set_description(gf_entry* entry, xmlNodePtr node) {
  gf_status rc = 0;
  
  gf_validate(entry);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (xmlNodeIsText(cur)) {
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
        if (!xmlNodeIsText(cur->children)) {
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
        rc = gf_array_add(entry->subject_set, (gf_any){ .ptr = cat });
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
        if (!xmlNodeIsText(cur->children)) {
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
entry_set_document_info(gf_entry* entry, const gf_path* root_path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlChar* prop = NULL;

  gf_validate(entry);

  _(entry_read_xml_file(&doc, root_path, entry));
  assert(doc);

  root = xmlDocGetRootElement(doc);
  if (!root) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  /* gf_entry::type */
  entry->type = GF_ENTRY_TYPE_DOCUMENT;
  /* gf_entry::state */
  entry->state = GF_ENTRY_STATE_PUBLISHED; // By now, draft mode is unavailable.
  /* gf_entry::method */
  prop = xmlGetProp(root, BAD_CAST"role");
  if (prop) {
    rc = gf_string_set(entry->method, (const char*)prop);
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  } else {
    rc = gf_string_set(entry->method, (const char*)root->name);
    if (rc != GF_SUCCESS) {
      xmlFreeDoc(doc);
      gf_throw(rc);
    }
  }
  /* "info" elements */
  root = root->children;
  if (!root || !!xmlStrcmp(root->name, BAD_CAST"info")) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_DATA, "Invalid XML document.");
  }
  for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
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
entry_set_meta_info(gf_entry* entry, const gf_path* root_path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  
  gf_validate(entry);

  _(entry_read_xml_file(&doc, root_path, entry));
  assert(doc);

  /* "meta" element */
  root = xmlDocGetRootElement(doc);
  if (!root) {
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
  for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
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
** @defgroup SiteStruct The website information and operations.
*/
/* @{ */

/*!
** @brief The website structure.
**
** This structure represents whole your website information. By this, you can
** update or build the website.
*/

struct gf_site {
  gf_array*     entry_set;  ///< Entries to process
};

/*!
** @brief Initialize a gf_site object.
**
** @param 
**
** @return GF_SUCCESS on success, GF_E_* otherwise. 
*/

static gf_status
site_init(gf_site* site) {
  gf_validate(site);

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

  if (site->entry_set) {
    _(gf_array_clear(site->entry_set));
  }
  
  return GF_SUCCESS;
}


gf_status
gf_site_get_root_entry(gf_site* site, gf_entry** entry) {
  gf_size_t cnt = 0;
  
  gf_validate(site);
  gf_validate(entry);

  cnt = gf_array_size(site->entry_set);
  if (cnt <= 0) {
    *entry = NULL;
  } else {
    gf_any any = { 0 };
    _(gf_array_get(site->entry_set, 0, &any));
    *entry = (gf_entry*)(any.ptr);
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
site_collect_document_info(
  gf_entry* entry, const gf_path* root, gf_file_info* file_info) {
  gf_size_t cnt = 0;
  gf_file_info* child_info = NULL;
  
  gf_validate(entry);
  gf_validate(file_info);

  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    _(gf_file_info_get_child(file_info, i, &child_info));
    if (site_is_document_file(child_info)) {
      _(gf_entry_set_file_info(entry, child_info));
      _(entry_set_document_info(entry, root));
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_collect_meta_info(
  gf_entry* entry, const gf_path* root, gf_file_info* file_info) {
  gf_size_t cnt = 0;
  gf_file_info* child_info = NULL;
  
  gf_validate(entry);
  gf_validate(file_info);

  cnt = gf_file_info_count_children(file_info);
  for (gf_size_t i = 0; i < cnt; i++) {
    _(gf_file_info_get_child(file_info, i, &child_info));
    if (site_is_meta_file(child_info)) {
      _(gf_entry_set_file_info(entry, child_info));
      _(entry_set_meta_info(entry, root));
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_collect_file_info(gf_array* file_set, gf_file_info* root) {
  gf_status rc = 0;
  gf_size_t cnt = 0;

  cnt = gf_file_info_count_children(root);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_file_info* child = NULL;
    
    _(gf_file_info_get_child(root, i, &child));

    if (gf_file_info_is_directory(child)) {
      if (!site_does_directory_have_document_file(child) &&
          !site_does_directory_have_meta_file(child) &&
          !gf_file_info_does_file_name_equal(child, GF_CONFIG_DIRECTORY)) {
        _(site_collect_file_info(file_set, child));
      }
    } else {
      gf_file_info* info = NULL;

      _(gf_file_info_clone(&info, child));
      rc = gf_array_add(file_set, (gf_any){ .ptr = info });
      if (rc != GF_SUCCESS) {
        gf_file_info_free(info);
        gf_throw(rc);
      }
    }
  }

  return GF_SUCCESS;
}


static gf_status
site_scan_directories(
  gf_array* entry_set, const gf_path* root, gf_file_info* file_info) {
  gf_status rc = 0;
  gf_size_t cnt = 0;
  gf_entry* entry = NULL;
  
  gf_validate(entry_set);
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
    _(gf_entry_new(&entry));
    rc = site_collect_document_info(entry, root, file_info);
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
    rc = gf_array_add(entry_set, (gf_any){ .ptr = entry });
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
  } else if (site_does_directory_have_meta_file(file_info)) {
    _(gf_entry_new(&entry));
    rc = site_collect_meta_info(entry, root, file_info);
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
    rc = gf_array_add(entry_set, (gf_any){ .ptr = entry });
    if (rc != GF_SUCCESS) {
      gf_entry_free(entry);
      gf_throw(rc);
    }
  } else {
    /* This directory is not our target */
    return GF_SUCCESS;
  }
  if (entry) {
    /* collect file info */
    rc = site_collect_file_info(entry->file_set, file_info);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    /* process children  */
    cnt = gf_file_info_count_children(file_info);
    for (gf_size_t i = 0; i < cnt; i++) {
      gf_file_info* child_info = NULL;
      
      _(gf_file_info_get_child(file_info, i, &child_info));
      _(site_scan_directories(entry->children, root, child_info));
    }
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

  /* Traverse */
  rc = site_scan_directories(tmp->entry_set, path, file_info);
  gf_file_info_free(file_info);
  if (rc != GF_SUCCESS) {
    gf_site_free(tmp);
    gf_throw(rc);
  }

  *site = tmp;

  return GF_SUCCESS;
}

/* @} */

/* -------------------------------------------------------------------------- */
/*!
** @defgroup WriteSite Writing gf_site object to an XML file.
*/
/* @{ */

static gf_status
site_write_xml_file(xmlDocPtr doc, const char* path) {
  FILE* fp = NULL;
  int ret = 0;
  
  gf_validate(doc);
  gf_validate(!gf_strnull(path));

  fp = fopen(path, "w");
  if (!fp) {
    gf_raise(GF_E_OPEN, "Failed to open file.");
  }
  ret = xmlDocFormatDump(fp, doc, 1);
  fclose(fp);
  if (ret < 0) {
    gf_raise(GF_E_OPEN, "Failed to open file.");
  }
  
  return GF_SUCCESS;
}

static gf_status
site_add_xml_int16u(xmlNodePtr node, xmlChar* name, gf_16u value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%d", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_int16u_hex(xmlNodePtr node, xmlChar* name, gf_16u value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%x", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_int16s(xmlNodePtr node, xmlChar* name, gf_16s value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%d", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_int32u(xmlNodePtr node, xmlChar* name, gf_32u value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%d", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_int64u(xmlNodePtr node, xmlChar* name, gf_64u value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%lld", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_int64u_hex(xmlNodePtr node, xmlChar* name, gf_64u value) {
  xmlNodePtr cur = NULL;
  char buf[256] = { 0 };
  
  gf_validate(node);
  gf_validate(name);

  snprintf(buf, 256, "%llx", value);
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST buf);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_string(xmlNodePtr node, xmlChar* name, const gf_string* value) {
  xmlNodePtr cur = NULL;

  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  cur = xmlNewTextChild(node, NULL, name, BAD_CAST gf_string_get(value));
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_date(xmlNodePtr node, xmlChar* name, gf_64u datetime) {
  gf_status rc = 0;
  gf_string* str = NULL;
  xmlNodePtr cur = NULL;

  gf_validate(node);
  gf_validate(name);

  _(gf_string_new(&str));
  if (datetime > 0) {
    rc = gf_datetime_make_iso8061_string(str, (gf_datetime)datetime);
    if (rc != GF_SUCCESS) {
      gf_string_free(str);
      gf_throw(rc);
    }
  }
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST gf_string_get(str));
  gf_string_free(str);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_description(
  xmlNodePtr node, xmlChar* name, const gf_array* value) {
  xmlNodePtr cur = NULL;
  gf_size_t cnt = 0;

  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  cur = xmlNewNode(NULL, BAD_CAST"description");
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  cur = xmlAddChild(node, cur);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to add an XML node.");
  }
  cnt = gf_array_size(value);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_any any = { 0 };
    gf_string* str = NULL;
    
    _(gf_array_get(value, i, &any));
    str = (gf_string*)(any.ptr);
    _(site_add_xml_string(cur, BAD_CAST"p", str));
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_path(xmlNodePtr node, xmlChar* name, const gf_path* value) {
  xmlNodePtr cur = NULL;

  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  cur = xmlNewTextChild(node, NULL, name, BAD_CAST gf_path_get_string(value));
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_string(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, const gf_char**)) {

  xmlNodePtr cur = NULL;
  const gf_char* str = NULL;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &str));
  
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST str);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_hash(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_size_t, gf_8u*)) {

  xmlNodePtr txt = NULL;
  xmlNodePtr cur = NULL;
  gf_16u hash_size = 0;
  gf_8u buf[256] = { 0 };
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, 256, buf));

  _(gf_file_info_get_hash_size(info, &hash_size));
  
  cur = xmlNewNode(NULL, name);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  cur = xmlAddChild(node, cur);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  txt = xmlNewTextLen(BAD_CAST buf, hash_size);
  if (!txt) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  txt = xmlAddChild(cur, txt);
  if (!txt) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int16u(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_16u*)) {
  gf_16u n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int16u(node, name, n));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int16u_hex(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_16u*)) {
  gf_16u n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int16u_hex(node, name, n));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int16s(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_16s*)) {
  gf_16s n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int16s(node, name, n));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int32u(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_32u*)) {
  gf_32u n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int32u(node, name, n));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int64u(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_64u*)) {
  gf_64u n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int64u(node, name, n));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_int64u_hex(
  xmlNodePtr node, const gf_file_info* info, xmlChar* name,
  gf_status (*fn)(const gf_file_info*, gf_64u*)) {
  gf_64u n = 0;
  
  gf_validate(info);
  gf_validate(name);
  gf_validate(fn);

  _(fn(info, &n));
  _(site_add_xml_int64u_hex(node, name, n));

  return GF_SUCCESS;
}

/*!
**
** We do not write values of user_data and user_flag.
**
**
*/

static gf_status
site_add_xml_file_info(
  xmlNodePtr node, xmlChar* name, const gf_file_info* value) {
  xmlNodePtr cur = NULL;
  
  gf_validate(node);
  gf_validate(value);

  cur = xmlNewNode(NULL, name);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
   }
  cur = xmlAddChild(node, cur);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to add an XML node.");
   }
  _(site_add_xml_file_info_string(
      cur, value, BAD_CAST"file-name", gf_file_info_get_file_name));
  _(site_add_xml_file_info_string(
      cur, value, BAD_CAST"full-path", gf_file_info_get_full_path));
  _(site_add_xml_file_info_hash(
      cur, value, BAD_CAST"hash", gf_file_info_get_hash_string));
  _(site_add_xml_file_info_int16u(
      cur, value, BAD_CAST"hash-size", gf_file_info_get_hash_size));
  _(site_add_xml_file_info_int16u(
      cur, value, BAD_CAST"inode", gf_file_info_get_inode));
  _(site_add_xml_file_info_int16u_hex(
      cur, value, BAD_CAST"mode", gf_file_info_get_mode));
  _(site_add_xml_file_info_int16s(
      cur, value, BAD_CAST"link-count", gf_file_info_get_link_count));
  _(site_add_xml_file_info_int16s(
      cur, value, BAD_CAST"uid", gf_file_info_get_uid));
  _(site_add_xml_file_info_int16s(
      cur, value, BAD_CAST"gid", gf_file_info_get_gid));
  _(site_add_xml_file_info_int32u(
      cur, value, BAD_CAST"device", gf_file_info_get_device));
  _(site_add_xml_file_info_int32u(
      cur, value, BAD_CAST"rdevice", gf_file_info_get_rdevice));
  _(site_add_xml_file_info_int64u(
      cur, value, BAD_CAST"file-size", gf_file_info_get_file_size));
  _(site_add_xml_file_info_int64u_hex(
      cur, value, BAD_CAST"access-time", gf_file_info_get_access_time));
  _(site_add_xml_file_info_int64u_hex(
      cur, value, BAD_CAST"modify-time", gf_file_info_get_modify_time));
  _(site_add_xml_file_info_int64u_hex(
      cur, value, BAD_CAST"create-time", gf_file_info_get_create_time));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_category(
  xmlNodePtr node, xmlChar* name, const gf_category* value) {
  xmlNodePtr cur = NULL;

  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  // NOTE: We directly refer to the member gf_category::name. It may be
  // preferable to access via getter functions.
  cur = xmlNewTextChild(node, NULL, name, BAD_CAST gf_string_get(value->name));
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  xmlSetProp(cur, BAD_CAST"xml:id", BAD_CAST gf_string_get(value->id));

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_set(xmlNodePtr node, xmlChar* name, const gf_array* value) {
  xmlNodePtr cur = NULL;
  gf_size_t cnt = 0;

  cur = xmlNewNode(NULL, name);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  cur = xmlAddChild(node, cur);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to add an XML node.");
  }
  cnt = gf_array_size(value);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_any any = { 0 };

    _(gf_array_get(value, i, &any));
    _(site_add_xml_file_info(cur, BAD_CAST"file-info", (gf_file_info*)any.ptr));
  }
  
  return GF_SUCCESS;
}

static gf_status
site_add_xml_category_set(
  xmlNodePtr node, xmlChar* name, xmlChar* child_name, const gf_array* value) {
  xmlNodePtr cur = NULL;
  gf_size_t cnt = 0;
  
  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  cur = xmlNewNode(NULL, name);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  cur = xmlAddChild(node, cur);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to add an XML node.");
  }
  cnt = gf_array_size(value);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_any any = { 0 };

    _(gf_array_get(value, i, &any));
    _(site_add_xml_category(cur, child_name, (gf_category*)any.ptr));
  }

  return GF_SUCCESS;
}

static gf_status
site_add_xml_file_info_set(
  xmlNodePtr node, xmlChar* name, const gf_array* value) {
  xmlNodePtr cur = NULL;
  gf_size_t cnt = 0;
  
  gf_validate(node);
  gf_validate(name);
  gf_validate(value);

  cur = xmlNewNode(NULL, name);
  if (!cur) {
    gf_raise(GF_E_API, "Failed to add an XML node.");
  }
  cnt = gf_array_size(value);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_any any = { 0 };

    _(gf_array_get(value, i, &any));
    _(site_add_xml_file_info(cur, BAD_CAST"file-info", (gf_file_info*)(any.ptr)));
  }
  
  return GF_SUCCESS;
}

static gf_status
site_make_entry_node(xmlNodePtr* node, gf_entry* entry) {
  xmlNodePtr tmp = NULL;
  xmlNodePtr children_node = NULL;
  gf_size_t cnt = 0;

  tmp = xmlNewNode(NULL, BAD_CAST"entry");
  if (!tmp) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }

  _(site_add_xml_int32u(tmp, BAD_CAST"type", entry->type));
  _(site_add_xml_int32u(tmp, BAD_CAST"state", entry->state));
  _(site_add_xml_string(tmp, BAD_CAST"title", entry->title));
  _(site_add_xml_string(tmp, BAD_CAST"author", entry->author));
  _(site_add_xml_date(tmp, BAD_CAST"date", entry->date));
  _(site_add_xml_description(tmp, BAD_CAST"description", entry->description));
  _(site_add_xml_file_info(tmp, BAD_CAST"file-info", entry->file_info));
  _(site_add_xml_string(tmp, BAD_CAST"method", entry->method));
  _(site_add_xml_path(tmp, BAD_CAST"output-path", entry->output_path));
  _(site_add_xml_file_set(tmp, BAD_CAST"file-set", entry->file_set));
  _(site_add_xml_category_set(
      tmp, BAD_CAST"subject-set", BAD_CAST"subject", entry->subject_set));
  _(site_add_xml_category_set(
      tmp, BAD_CAST"keyword-set", BAD_CAST"keyword", entry->keyword_set));
  _(site_add_xml_file_info_set(tmp, BAD_CAST"file-set", entry->file_set));

  /* Process children */
  children_node = xmlNewNode(NULL, BAD_CAST"children");
  if (!children_node) {
    gf_raise(GF_E_API, "Failed to create an XML node.");
  }
  children_node = xmlAddChild(tmp, children_node);
  if (!children_node) {
    xmlFreeNode(children_node);
    gf_raise(GF_E_API, "Failed to add an XML node.");
  }
  if (gf_entry_is_section(entry)) {
    cnt = gf_array_size(entry->children);
    for (gf_size_t i = 0; i < cnt; i++) {
      gf_any any = { 0 };
      xmlNodePtr child_node = NULL;
      
      _(gf_array_get(entry->children, i, &any));
      _(site_make_entry_node(&child_node, (gf_entry*)any.ptr));
    
      child_node = xmlAddChild(children_node, child_node);
      if (!children_node) {
        gf_raise(GF_E_API, "Failed to add an XML node.");
      }
    }
  }
  
  *node = tmp;
  
  return GF_SUCCESS;
}

static gf_status
site_write_content(xmlNodePtr root, const gf_site* site) {
  gf_size_t cnt = 0;

  cnt = gf_array_size(site->entry_set);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_any any = { 0 };

    _(gf_array_get(site->entry_set, i, &any));
    if (any.ptr) {
      xmlNodePtr entry_node = NULL;

      _(site_make_entry_node(&entry_node, (gf_entry*)any.ptr));
      
      entry_node = xmlAddChild(root, entry_node);
      if (!entry_node) {
        gf_raise(GF_E_API, "Failed to add an XML node.");
      }
    }
  }
  
  return GF_SUCCESS;
}

gf_status
gf_site_write_file(const gf_site* site, const gf_path* path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;

  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  doc = xmlNewDoc(BAD_CAST"1.0");
  if (!doc) {
    gf_raise(GF_E_API, "Failed to create an XML document on write.");
  }
  /* Root element "site" */
  // TODO: set namespace
  root = xmlNewNode(NULL, BAD_CAST"site");
  if (!root) {
    gf_raise(GF_E_API, "Failed to create an XML element on write.");
  }
  xmlDocSetRootElement(doc, root);
  /* Build site content */
  rc = site_write_content(root, site);
  if (rc != GF_SUCCESS) {
    xmlFreeDoc(doc);
    gf_throw(rc);
  }
  /* Write file */
  rc = site_write_xml_file(doc, gf_path_get_string(path));
  xmlFreeDoc(doc);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  return GF_SUCCESS;
}

/* @} */

/* -------------------------------------------------------------------------- */

static gf_status
site_read_xml_int16u(gf_16u* value, const xmlNodePtr node) {
  gf_16u n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);

  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtoul((gf_char*)node->content, &e, 10);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_16u)n;
  
  return GF_SUCCESS;
}

static gf_status
site_read_xml_int16u_hex(gf_16u* value, const xmlNodePtr node) {
  gf_16u n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);

  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtoul((gf_char*)node->content, &e, 16);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_16u)n;
  
  return GF_SUCCESS;
}

static gf_status
site_read_xml_int16s(gf_16s* value, const xmlNodePtr node) {
  gf_16s n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtol((gf_char*)node->content, &e, 10);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_16s)n;

  return GF_SUCCESS;
}

static gf_status
site_read_xml_int32u(gf_32u* value, const xmlNodePtr node) {
  gf_32u n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }

  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtoul((gf_char*)node->content, &e, 10);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_32u)n;

  return GF_SUCCESS;
}

static gf_status
site_read_xml_int64u(gf_64u* value, const xmlNodePtr node) {
  gf_64u n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtoul((gf_char*)node->content, &e, 10);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_64u)n;

  return GF_SUCCESS;
}

static gf_status
site_read_xml_int64u_hex(gf_64u* value, const xmlNodePtr node) {
  gf_64u n = 0;
  gf_char* e = NULL;

  gf_validate(value);
  gf_validate(node);
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  n = strtoul((gf_char*)node->content, &e, 16);
  if (*e != '\0') {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  *value = (gf_64u)n;

  return GF_SUCCESS;
}

static gf_status
site_read_xml_string(gf_string* value, const xmlNodePtr node) {
  gf_validate(value);
  gf_validate(node);

  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  _(gf_string_set(value, (gf_char*)node->content));

  return GF_SUCCESS;
}

static gf_status
site_read_xml_path(gf_path* value, const xmlNodePtr node) {
  gf_validate(value);
  gf_validate(node);

  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  _(gf_path_set_string(value, (gf_char*)node->content));
  
  return GF_SUCCESS;
}

static gf_status
site_read_xml_date(gf_datetime* value, const xmlNodePtr node) {
  gf_validate(value);
  gf_validate(node);
  
  if (!xmlNodeIsText(node)) {
    gf_raise(GF_E_DATA, "Invalid site data.");
  }
  _(gf_datetime_parse_iso8061_string((const gf_char*)node->content, value));
  
  return GF_SUCCESS;
}

static gf_status
site_read_xml_description(gf_array* value, const xmlNodePtr node) {
  gf_validate(value);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    gf_status rc = 0;
    gf_string* str = NULL;
    xmlNodePtr txt = NULL;
    
    if (!xmlStrcmp(cur->name, BAD_CAST"p")) {
      assert(0);
      continue;
    }
    txt = cur->children;
    if (!xmlNodeIsText(txt)) {
      assert(0);
      continue;
    }
    _(gf_string_new(&str));
    rc = gf_string_set(str, (gf_char*)txt->content);
    if (rc != GF_SUCCESS) {
      gf_string_free(str);
      gf_throw(rc);
    }
    rc = gf_array_add(value, (gf_any){ .ptr = str });
    if (rc != GF_SUCCESS) {
      gf_string_free(str);
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_read_xml_category(
  gf_array* value, const xmlChar* name, const xmlNodePtr node) {
  gf_status rc = 0;

  gf_validate(value);
  gf_validate(name);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    gf_category* cat = NULL;
    gf_string* str = NULL;
    xmlNodePtr txt = NULL;
    xmlChar* id = NULL;
    
    if (!!xmlStrcmp(cur->name, name)) {
      assert(0);
      continue;
    }
    id = xmlGetProp(cur, BAD_CAST"id");
    if (!id) {
      assert(0);
      continue;
    }
    txt = cur->children;
    if (!txt || !xmlNodeIsText(txt)) {
      assert(0);
      continue;
    }

    _(gf_category_new(&cat));

    rc = gf_string_new(&str);
    if (rc != GF_SUCCESS) {
      gf_category_free(cat);
      gf_throw(rc);
    }
    gf_string_set(str, (gf_char*)id);
    if (rc != GF_SUCCESS) {
      gf_string_free(str);
      gf_category_free(cat);
      gf_throw(rc);
    }
    rc = gf_category_set_id(cat, str);
    if (rc != GF_SUCCESS) {
      gf_category_free(cat);
      gf_throw(rc);
    }

    gf_string_set(str, (gf_char*)txt->content);
    if (rc != GF_SUCCESS) {
      gf_string_free(str);
      gf_category_free(cat);
      gf_throw(rc);
    }
    rc = gf_category_set_name(cat, str);
    gf_string_free(str);
    if (rc != GF_SUCCESS) {
      gf_category_free(cat);
      gf_throw(rc);
    }
    rc = gf_array_add(value, (gf_any){ .ptr = cat });
    if (rc != GF_SUCCESS) {
      gf_category_free(cat);
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_string(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, const gf_char*)) {

  gf_status rc = 0;
  gf_string* str = NULL;

  gf_validate(info);
  gf_validate(str);
  gf_validate(fn);

  _(gf_string_new(&str));

  rc = site_read_xml_string(str, node);
  if (rc != GF_SUCCESS) {
    gf_string_free(str);
    gf_throw(rc);
  }
  rc = fn(info, gf_string_get(str));
  gf_string_free(str);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int16u(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_16u)) {
  gf_16u n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int16u(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int16u_hex(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_16u)) {
  gf_16u n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int16u_hex(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int16s(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_16s)) {
  gf_16s n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int16s(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int32u(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_32u)) {
  gf_32u n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int32u(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int64u(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_64u)) {
  gf_64u n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int64u(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_int64u_hex(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_64u)) {
  gf_64u n = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(site_read_xml_int64u_hex(&n, node));
  _(fn(info, n));

  return GF_SUCCESS;
}

gf_status
site_read_xml_file_info_hash(
  gf_file_info* info, const xmlNodePtr node,
  gf_status (*fn)(gf_file_info*, gf_size_t, gf_8u*)) {

  gf_16u size = 0;

  gf_validate(info);
  gf_validate(node);
  gf_validate(fn);

  _(gf_file_info_get_hash_size(info, &size));
  _(fn(info, size, (gf_8u*)node->content));

  return GF_SUCCESS;
}

static gf_status
site_read_xml_file_info(gf_file_info* info, const xmlNodePtr node) {
  gf_validate(info);
  gf_validate(node);

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (!xmlStrcmp(cur->name, BAD_CAST"file-name")) {
      _(site_read_xml_file_info_string(
          info, cur->children, gf_file_info_set_file_name));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"full-path")) {
      _(site_read_xml_file_info_string(
          info, cur->children, gf_file_info_set_full_path));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"hash")) {
      _(site_read_xml_file_info_hash(
          info, cur->children, gf_file_info_set_hash_string));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"hash-size")) {
      _(site_read_xml_file_info_int16u(
          info, cur->children, gf_file_info_set_hash_size));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"inode")) {
      _(site_read_xml_file_info_int16u(
          info, cur->children, gf_file_info_set_inode));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"mode")) {
      _(site_read_xml_file_info_int16u_hex(
          info, cur->children, gf_file_info_set_mode));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"link-count")) {
      _(site_read_xml_file_info_int16s(
          info, cur->children, gf_file_info_set_link_count));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"uid")) {
      _(site_read_xml_file_info_int16s(
          info, cur->children, gf_file_info_set_uid));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"gid")) {
      _(site_read_xml_file_info_int16s(
          info, cur->children, gf_file_info_set_gid));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"device")) {
      _(site_read_xml_file_info_int32u(
          info, cur->children, gf_file_info_set_device));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"rdevice")) {
      _(site_read_xml_file_info_int32u(
          info, cur->children, gf_file_info_set_rdevice));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"file-size")) {
      _(site_read_xml_file_info_int64u(
          info, cur->children, gf_file_info_set_file_size));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"access-time")) {
      _(site_read_xml_file_info_int64u_hex(
          info, cur->children, gf_file_info_set_access_time));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"modify-time")) {
      _(site_read_xml_file_info_int64u_hex(
          info, cur->children, gf_file_info_set_modify_time));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"create-time")) {
      _(site_read_xml_file_info_int64u_hex(
          info, cur->children, gf_file_info_set_create_time));
    } else {
      /* Unknown element - ignore */
      assert(0);
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_read_xml_file_set(gf_array* file_set, const xmlNodePtr node) {
  gf_status rc = 0;

  gf_validate(file_set);
  gf_validate(node);
  
  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (!xmlStrcmp(cur->name, BAD_CAST"file-info")) {
      gf_file_info* info = NULL;

      _(gf_file_info_new(&info, NULL, NULL));
      rc = gf_array_add(file_set, (gf_any){ .ptr = info });
      if (rc != GF_SUCCESS) {
        gf_file_info_free(info);
        gf_throw(rc);
      }
    }
  }

  return GF_SUCCESS;
}

static gf_status
site_read_xml_entry(gf_array* entry_set, const xmlNodePtr root) {
  gf_status rc = 0;
  gf_entry* entry = NULL;
  
  gf_validate(entry_set);
  gf_validate(root);

  if (!!xmlStrcmp(root->name, BAD_CAST"entry")) {
    gf_raise(GF_E_DATA, "Invalid site file.");
  }
  rc = gf_entry_new(&entry);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_array_add(entry_set, (gf_any){ .ptr = entry });
  if (rc != GF_SUCCESS) {
    gf_entry_free(entry);
    gf_throw(rc);
  }
  /* Process children */
  for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
    if (!xmlStrcmp(cur->name, BAD_CAST"type")) {
      _(site_read_xml_int32u(&entry->type, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"state")) {
      _(site_read_xml_int32u(&entry->state, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"title")) {
      _(site_read_xml_string(entry->title, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"author")) {
      _(site_read_xml_string(entry->author, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"date")) {
      _(site_read_xml_date(&entry->date, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"description")) {
      _(site_read_xml_description(entry->description, cur));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"file-info")) {
      _(site_read_xml_file_info(entry->file_info, cur));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"method")) {
      _(site_read_xml_string(entry->method, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"output-path")) {
      _(site_read_xml_path(entry->output_path, cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"file-set")) {
      _(site_read_xml_file_set(entry->file_set, cur));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"subject-set")) {
      _(site_read_xml_category(
          entry->subject_set, BAD_CAST"subject", cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"keyword-set")) {
      _(site_read_xml_category(
          entry->keyword_set, BAD_CAST"keyword", cur->children));
    } else if (!xmlStrcmp(cur->name, BAD_CAST"children")) {
      for (xmlNodePtr child = cur->children; child; child = child->next) {
        _(site_read_xml_entry(entry->children, child));
      }
    } else {
      /* Unknown element - ignore */
      assert(0);
    }
  }
  
  return GF_SUCCESS;
}


static gf_status
site_read_file(gf_site* site, const gf_path* path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;

#ifdef GF_DEBUG_
  static const int option = 0;
#else
  static const int option = GF_XML_PARSE_OPTIONS;
#endif

  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));
  
  doc = xmlReadFile(gf_path_get_string(path), NULL, option);
  if (!doc) {
    gf_raise(GF_E_API, "Failed to read an XML file.");
  }
  root = xmlDocGetRootElement(doc);
  if (!root) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_API, "Failed to read an XML file.");
  }
  if (!!xmlStrcmp(root->name, BAD_CAST"site")) {
    gf_raise(GF_E_DATA, "Invalid site file.");
  }
  root = root->children;
  assert(site->entry_set);
  rc = site_read_xml_entry(site->entry_set, root);
  xmlFreeDoc(doc);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_site_read_file(gf_site** site, const gf_path* path) {
  gf_status rc = 0;
  gf_site* tmp = NULL;
  
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));
  
  _(gf_site_new(&tmp));
  
  rc = site_read_file(tmp, path);
  gf_site_free(tmp);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  *site = tmp;

  return GF_SUCCESS;
}
