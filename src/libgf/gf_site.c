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

#include <libxml/tree.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_array.h>
#include <libgf/gf_path.h>
#include <libgf/gf_file_info.h>
#include <libgf/gf_site.h>

#include "gf_local.h"

/* -------------------------------------------------------------------------- */

/*!
** @brief Site object
**
** This is the base type of evry site objects.
*/

struct gf_object {
  gf_object_type  type;
  gf_object_state state;
  gf_string*      title;
  gf_string*      author;
  gf_datetime     update_date;
  gf_file_info*   file_info;  ///< The info file. (site.gf, meta.gf ...)
};

gf_status
gf_object_init(gf_object* obj) {
  gf_validate(obj);

  obj->type = GF_OBJECT_TYPE_UNKNOWN;
  obj->state = GF_OBJECT_STATE_UNKNOWN;
  obj->title = NULL;
  obj->author = NULL;
  obj->update_date = 0;
  obj->file_info = NULL;
  
  return GF_SUCCESS;
}

gf_status
gf_object_clear(gf_object* obj) {
  gf_validate(obj);

  obj->type = GF_OBJECT_TYPE_UNKNOWN;

  if (obj->title) {
    gf_string_free(obj->title);
  }
  if (obj->author) {
    gf_string_free(obj->author);
  }
  _(gf_object_init(obj));

  return GF_SUCCESS;
}

gf_status
gf_object_set_type(gf_object* obj, gf_object_type type) {
  gf_validate(obj);
  obj->type = type;
  return GF_SUCCESS;
}

gf_status
gf_object_set_title(gf_object* obj, const gf_string* title) {
  gf_validate(obj);
  gf_validate(title);

  _(gf_string_assign(&obj->title, title));
  
  return GF_SUCCESS;
}

gf_status
gf_object_set_author(gf_object* obj, const gf_string* author) {
  gf_validate(obj);
  gf_validate(author);

  _(gf_string_assign(&obj->author, author));
  
  return GF_SUCCESS;
}

gf_status
gf_object_set_update_date(gf_object* obj, gf_datetime date) {
  gf_validate(obj);

  obj->update_date = date;
  
  return GF_SUCCESS;
}

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
    category_init(cat);
    gf_free(cat);
  }
}

static void
category_free(gf_any* any) {
  gf_category_free((gf_category*)any->ptr);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Site entry
**
*/

struct gf_entry {
  gf_object base;
  gf_array* subject_set;  ///< The array of gf_category objects
  gf_array* keyword_set;  ///< The array of gf_category objects
};

static gf_status
entry_init(gf_entry* entry) {
  gf_validate(entry);

  _(gf_object_init(&entry->base));
  entry->subject_set = NULL;
  entry->keyword_set = NULL;
  
  return GF_SUCCESS;
}

static gf_status
entry_prepare(gf_entry* entry) {
  gf_validate(entry);

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
    gf_object_clear(&entry->base);
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
  gf_entry_free((gf_entry*)any->ptr);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Site database
*/

struct gf_site {
  gf_object     base;       ///< Base class
  gf_file_info* root;       ///< The root of a site tree. (directory)
  gf_array*     entry_set;  ///< Entries to process
};

static gf_status
site_init(gf_site* site) {
  gf_validate(site);

  _(gf_object_init(&site->base));
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

static gf_status
site_scan_directories(gf_site* site, gf_path* root) {
  gf_file_info* file_info = NULL;
  
  gf_validate(site);
  gf_validate(root);

  /* Scan files */
  _(gf_file_info_new(&file_info, root));
  site->root = file_info;
  
  
  return GF_SUCCESS;
}

gf_status
gf_site_scan(gf_site** site, const gf_path* path) {
  gf_status rc = 0;
  gf_path* root = NULL;
  gf_site* tmp = NULL;
  
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  /* Disposal root path */
  _(gf_path_new(&root, gf_path_get_string(path)));

  rc = gf_site_new(&tmp);
  if (rc != GF_SUCCESS) {
    gf_path_free(root);
    gf_throw(rc);
  }
  /* Traverse */
  rc = site_scan_directories(tmp, root);
  gf_path_free(root);
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
