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

static gf_bool
site_does_directory_have_file(
  const gf_file_info* file_info, const gf_char* file_name) {
  gf_status rc = 0;
  gf_size_t cnt = 0;
  gf_bool ret = GF_FALSE;

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
    if (site_does_file_name_equal(child_info, file_name)) {
      ret = GF_TRUE;
      break;
    }
  }

  return ret;
}

static gf_bool
site_does_directory_have_meta_file(const gf_file_info* file_info) {
  static const gf_char name[] = "meta.gf";
  return site_does_directory_have_file(file_info, name);
}

static gf_bool
site_does_directory_have_document_file(const gf_file_info* file_info) {
  static const gf_char name[] = "index.dbk";
  return site_does_directory_have_file(file_info, name);
}

static gf_bool
site_is_asset_directory(const gf_file_info* file_info) {
  static const gf_char name[] = "_";
  return site_does_file_name_equal(file_info, name);
}

static gf_status
site_scan_directories(gf_site* site, gf_file_info* file_info) {
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
    // TODO: imeprement me!
  } else if (site_does_directory_have_meta_file(file_info)) {
    // TODO: imeprement me!
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
