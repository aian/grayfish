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

#include <windows.h>

#include <libxml/tree.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_array.h>
#include <libgf/gf_path.h>
#include <libgf/gf_site.h>
#include <libgf/gf_local.h>

/* -------------------------------------------------------------------------- */

struct gf_author {
  gf_string* name;
  gf_string* mail;
};

static gf_status
author_init(gf_author* author) {
  gf_validate(author);

  author->name = NULL;
  author->mail = NULL;

  return GF_SUCCESS;
}

gf_status
gf_author_new(gf_author** author) {
  gf_status rc = 0;
  gf_author* tmp = NULL;

  gf_validate(author);
  _(gf_malloc((gf_ptr*)tmp, sizeof(*tmp)));

  rc = author_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  *author = tmp;
  
  return GF_SUCCESS;
}

void
gf_author_free(gf_author* author) {
  if (author) {
    if (author->name) {
      gf_string_free(author->name);
      author->name = NULL;
    }
    if (author->mail) {
      gf_string_free(author->mail);
      author->mail = NULL;
    }
    gf_free(author);
  }
}

gf_status
gf_author_set_name(gf_author* author, const gf_string* name) {
  gf_validate(author);
  gf_validate(name);

  _(gf_string_assign(&author->name, name));

  return GF_SUCCESS;
}

gf_status
gf_author_set_mail(gf_author* author, const gf_string* mail) {
  gf_validate(author);
  gf_validate(mail);

  _(gf_string_assign(&author->mail, mail));
  
  return GF_SUCCESS;
}

gf_status
gf_author_copy(gf_author* dst, const gf_author* src) {
  gf_status rc = 0;
  
  gf_validate(dst);
  gf_validate(src);

  rc = gf_string_copy(dst->name, src->name);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_string_copy(dst->mail, src->mail);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_author_clone(gf_author** dst, const gf_author* src) {
  gf_status rc = 0;
  gf_author* tmp = NULL;
  
  gf_validate(dst);
  gf_validate(src);

  _(gf_author_new(&tmp));

  rc = gf_author_copy(tmp, src);
  if (rc != GF_SUCCESS) {
    gf_author_free(tmp);
    gf_throw(rc);
  }
  *dst = tmp;

  return GF_SUCCESS;
}

gf_status
gf_author_assign(gf_author** dst, const gf_author* src) {
  gf_author* tmp = NULL;
  
  gf_validate(dst);
  gf_validate(src);

  _(gf_author_clone(&tmp, src));
  
  if (*dst) {
    gf_author_free(*dst);
    *dst = NULL;
  }
  *dst = tmp;
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Site object
**
** This is the base type of evry site objects.
*/

struct gf_object {
  gf_object_type type;
  gf_string*     title;
  gf_author*     author;
  gf_date*       update_date;
  gf_date*       create_date;
};

gf_status
gf_object_init(gf_object* obj) {
  gf_validate(obj);

  obj->type = GF_OBJECT_TYPE_UNKNOWN;
  obj->title = NULL;
  obj->author = NULL;
  obj->update_date = NULL;
  obj->create_date = NULL;
  
  return GF_SUCCESS;
}

gf_status
gf_object_clear(gf_object* obj) {
  gf_validate(obj);

  obj->type = GF_OBJECT_TYPE_UNKNOWN;

  if (obj->title) {
    gf_string_free(obj->title);
    obj->title = NULL;
  }
  if (obj->author) {
    gf_author_free(obj->author);
    obj->author = NULL;
  }
  if (obj->update_date) {
    gf_date_free(obj->update_date);
    obj->update_date = NULL;
  }
  if (obj->create_date) {
    gf_date_free(obj->create_date);
    obj->create_date = NULL;
  }

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
gf_object_set_author(gf_object* obj, const gf_author* author) {
  gf_validate(obj);
  gf_validate(author);

  _(gf_author_assign(&obj->author, author));
  
  return GF_SUCCESS;
}

gf_status
gf_object_set_update_date(gf_object* obj, const gf_date* date) {
  gf_validate(obj);
  gf_validate(date);

  _(gf_date_copy(obj->update_date, date));
  
  return GF_SUCCESS;
}

gf_status
gf_object_set_create_date(gf_object* obj, const gf_date* date) {
  gf_validate(obj);
  gf_validate(date);

  _(gf_date_copy(obj->create_date, date));
  
  return GF_SUCCESS;
}


/* -------------------------------------------------------------------------- */

typedef struct gf_site gf_site;
typedef struct gf_section gf_section;
typedef struct gf_entry gf_entry;
typedef struct gf_file gf_file;

typedef struct gf_subject gf_subject;
typedef struct gf_keyword gf_keyword;
typedef struct gf_author gf_author;

/*!
** @brief Object type enumeration
*/


/*!
** @brief Site database
*/

struct gf_site {
  xmlDocPtr      doc;
  gf_string*     title;
  gf_string*     author;
  gf_string*     email;
  gf_string*     domain;
  gf_path*       remote_path;
  gf_path*       style_path;
  gf_array*      entry_set;
};

/*!
** @brief Site section
**
*/

struct gf_section {
  gf_object base_;  ///< the base class

};

/*!
** @brief Site entry
**
*/

struct gf_entry {
  gf_string*     name;
  gf_string*     title;
  gf_date*       modified;
  gf_date*       published;
  gf_array*      subject_set;  ///< The array of gf_site_subject objects
  gf_array*      keyword_set;  ///< The array of gf_site_keyword objects
  gf_array*      file_set;     ///< The array of gf_site_file objects
  xmlNodePtr     info;      ///<
};

/*!
** @brief A subject of entries
**
*/

struct gf_subject {
  gf_string* name;
  gf_string* note;
};

/*!
** @brief A keyword
**
*/

struct gf_keyword {
  gf_string* name;
};

/*!
** @brief A file
**
*/

struct gf_file {
  gf_string*     name;
  gf_string*     mime_type;
  gf_path*       path;
  gf_date*       update;
  gf_date*       create;
};

/* -------------------------------------------------------------------------- */

static gf_status
site_init(gf_site* site) {
  gf_validate(site);

  site->doc = NULL;

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
  
  if (site->doc) {
    xmlFreeDoc(site->doc);
    site->doc = NULL;
  }

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/* Update the site structure.
**
**
**
**
*/

#define SITE_XML_VERSION BAD_CAST "1.0"
#define SITE_NODE_ROOT   BAD_CAST "site"
#define SITE_NODE_DIR    BAD_CAST "dir"
#define SITE_NODE_FILE   BAD_CAST "file"

#define SITE_PROP_BASE           BAD_CAST "base"
#define SITE_PROP_NAME           BAD_CAST "name"
#define SITE_PROP_STATUS         BAD_CAST "status"
#define SITE_PROP_MODIFIED_DATE  BAD_CAST "md"
#define SITE_PROP_COMPILED_DATE  BAD_CAST "cd"
#define SITE_PROP_PUBLISHED_DATE BAD_CAST "pd"

#define SITE_DATE_ZERO     BAD_CAST "0"      ///< Default date value

/*!
** @brief 
**
*/

static xmlNodePtr
site_get_node(xmlNodePtr node, const xmlChar* elem, const xmlChar* name) {
  xmlNodePtr ret = NULL;
  xmlChar* prop = NULL;

  if (!node || gf_strnull((const char*)name)) {
    return NULL;
  }
  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    if (gf_strnull((const char*)elem) || !xmlStrcmp(cur->name, elem)) {
      prop = xmlGetProp(cur, SITE_PROP_NAME);
      if (prop && !xmlStrcmp(prop, name)) {
        ret = cur;
        break;
      }
    }
  }
  
  return ret;
}

/*!
** @brief 
**
*/

static xmlNodePtr
site_get_file_node(xmlNodePtr node, const xmlChar* name) {
  return site_get_node(node, SITE_NODE_FILE, name);
}

/*!
** @brief 
**
*/

static xmlNodePtr
site_add_file_node(xmlNodePtr node, const xmlChar* name) {
  xmlNodePtr file = NULL;
  xmlNodePtr ret = NULL;

  file = site_get_file_node(node, name);
  /*
  ** Create the new file node
  */
  if (!file) {
    file = xmlNewNode(NULL, SITE_NODE_FILE);
    if (!file) {
      return NULL;
    }
    ret = xmlAddChild(node, file);
    if (!ret) {
      xmlFreeNode(file);
      return NULL;
    }
    xmlSetProp(file, SITE_PROP_NAME, name);
    xmlSetProp(file, SITE_PROP_MODIFIED_DATE, SITE_DATE_ZERO);
    xmlSetProp(file, SITE_PROP_COMPILED_DATE, SITE_DATE_ZERO);
    xmlSetProp(file, SITE_PROP_PUBLISHED_DATE, SITE_DATE_ZERO);
  }
  
  return file;
}

/*!
** @brief 
**
*/

static xmlNodePtr
site_get_directory_node(xmlNodePtr node, const xmlChar* name) {
  return site_get_node(node, SITE_NODE_DIR, name);
}

/*!
** @brief 
**
*/

static xmlNodePtr
site_add_directory_node(xmlNodePtr node, const xmlChar* name) {
  xmlNodePtr dir = NULL;
  xmlNodePtr ret = NULL;

  dir = site_get_directory_node(node, name);
  /* New file node */
  if (!dir) {
    dir = xmlNewNode(NULL, SITE_NODE_DIR);
    if (!dir) {
      return NULL;
    }
    ret = xmlAddChild(node, dir);
    if (!ret) {
      xmlFreeNode(dir);
      return NULL;
    }
    xmlSetProp(dir, SITE_PROP_NAME, name);
  }
  
  return dir;
}

/*!
** @brief Make the file time string, which is compliant with ISO8601.
**
*/

static gf_status
site_make_file_time_string(
  xmlChar* buf, size_t size, const FILETIME* file_time) {
  SYSTEMTIME system_time = { 0 };

  if (!FileTimeToSystemTime(file_time, &system_time)) {
    gf_raise(GF_E_API, "Failed to make system time.");
  }
  snprintf((char*)buf, size, "%04d-%02d-%02dT%02d:%02d:%02d",
           system_time.wYear, system_time.wMonth, system_time.wDay,
           system_time.wHour, system_time.wMinute, system_time.wSecond);
  
  return GF_SUCCESS;
}

static gf_status
site_get_modified_time_string(char* buf, size_t size, const char* path) {
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;

  gf_validate(buf);
  gf_validate(size > 0);
  gf_validate(!gf_strnull(path));
  
  doc = xmlParseFile(path);
  if (!doc) {
    gf_warn("Failed to open file (%s).", path);
    buf[0] = '\0';
    return GF_SUCCESS;
  }
  root = xmlDocGetRootElement(doc);
  if (!root) {
    xmlFreeDoc(doc);
    gf_warn("Empty document (%s).", path);
  }
  
  for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
    if (cur->type != XML_ELEMENT_NODE) {
      continue;
    }
    if (xmlStrcmp(cur->name, BAD_CAST "info") == 0) {
      for (xmlNodePtr node = cur->children; node; node = node->next) {
        if (xmlStrcmp(node->name, BAD_CAST "date") == 0) {
          xmlChar* date = xmlNodeGetContent(node);
          if (gf_strnull((char*)date)) {
            continue;
          }
          strncpy(buf, (char*)date, size);
          xmlFreeDoc(doc);
          return GF_SUCCESS;
        }
      }
    }
  }
  
  /* Not found */
  gf_warn("'//*/info/date' element has not been found (%s).", path);
  buf[0] = '\0';
  xmlFreeDoc(doc);
  
  return GF_SUCCESS;
}

static gf_bool
site_is_docbook_file(const char* path) {
  return gf_uri_match_extension(path, ".dbk");
}

/*!
** @brief Set the file time to the file node
*/

static gf_status
site_set_file_status(
  xmlNodePtr file, WIN32_FIND_DATA* find_data, const char* path) {
  xmlChar* md = NULL;
  xmlChar fd[256] = { 0 };

  gf_validate(file);
  gf_validate(find_data);
  gf_validate(path);

  /* Node times */
  md = xmlGetProp(file, SITE_PROP_MODIFIED_DATE);
  
  /* File time */
  if (site_is_docbook_file(find_data->cFileName)) {
    _(site_get_modified_time_string((char*)fd, 256, path));
  }
  if (gf_strnull((char*)fd)) {
    _(site_make_file_time_string(fd, 256, &find_data->ftLastWriteTime));
  }
  /*
  ** Update the status of the current file
  */
  if (xmlStrcmp(fd, md) > 0) {
    /* Updated */
    xmlSetProp(file, SITE_PROP_MODIFIED_DATE, fd);
    md = fd;
  }
  
  return GF_SUCCESS;
}

/*!
** @brief 
*/

static gf_status
site_set_file_node(
  xmlNodePtr node, WIN32_FIND_DATA* find_data, const char* path) {
  xmlNodePtr file = NULL;
  
  gf_validate(node);
  gf_validate(find_data);

  /* File node */
  file = site_add_file_node(node, BAD_CAST find_data->cFileName);
  assert(file);
  /* Check the update time */
  _(site_set_file_status(file, find_data, path));
  
  return GF_SUCCESS;
}

/*!
** @brief
**
** 
*/

static gf_status
site_set_directory_node(
  xmlNodePtr* new_node, xmlNodePtr node, WIN32_FIND_DATA* find_data) {
  
  gf_validate(new_node);
  gf_validate(node);
  gf_validate(find_data);

  /* Directory node */
  *new_node = site_add_directory_node(node, BAD_CAST find_data->cFileName);
  assert(*new_node);

  return GF_SUCCESS;
}

/*!
** @brief 
**
*/

static gf_status
site_make_search_path(gf_path** search_path, const gf_path* path) {
  gf_status rc = 0;
  gf_path* wc = NULL;
  gf_path* tmp = NULL;

  rc = gf_path_new(&tmp, gf_path_get_string(path));
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_path_new(&wc, "*.*");
  if (rc != GF_SUCCESS) {
    gf_path_free(tmp);
    gf_throw(rc);
  }
  rc = gf_path_append(tmp, wc);
  gf_path_free(wc);
  if (rc != GF_SUCCESS) {
    gf_path_free(tmp);
    return rc;
  }
  *search_path = tmp;
  
  return GF_SUCCESS;
}

/*!
** @brief Traverse files on the specified path
**
** @param [in]      path The current path object
** @param [in, out] node The current XML node
**
*/

static gf_status
site_traverse_directories(const gf_path* path, xmlNodePtr node) {
  gf_status rc = 0;
  HANDLE hr = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA find_data = { 0 };
  gf_path* search_path = NULL;

  static const DWORD file_attr =
    FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE;
  
  _(site_make_search_path(&search_path, path));
  
  hr = FindFirstFile(gf_path_get_string(search_path), &find_data);
  gf_path_free(search_path);
  if (hr == INVALID_HANDLE_VALUE) {
    if (GetLastError() == ERROR_FILE_NOT_FOUND) {
      return GF_SUCCESS;
    } else {
      gf_raise(GF_E_API, "Failed to traverse files.");
    }
  }
  do {
    const char* file_name = find_data.cFileName;
    gf_path* child_path = NULL;

    /* Child path */
    rc = gf_path_append_string(&child_path, path, file_name);
    if (rc != GF_SUCCESS) {
      FindClose(hr);
      gf_throw(rc);
    }
    
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      /* On directory case */
      xmlNodePtr child_node = NULL;

      /* The directory whose name starts with comma is skipped.  */
      if (gf_strnull(file_name) || file_name[0] == '.') {
        continue;
      }
      
      child_node = site_get_node(node, BAD_CAST "", BAD_CAST file_name);
      if (!child_node) {
        /* Directory node */
        rc = site_set_directory_node(&child_node, node, &find_data);
        if (rc != GF_SUCCESS) {
          FindClose(hr);
          gf_raise(GF_E_API, "Failed to create a directory element.");
        }
      }
      /* Get into the next directory */
      rc = site_traverse_directories(child_path, child_node);
      gf_path_free(child_path);
      if (rc != GF_SUCCESS) {
        FindClose(hr);
        gf_throw(rc);
      }
    } else if (find_data.dwFileAttributes & file_attr) {
      /* On normal file case */
      rc = site_set_file_node(node, &find_data, gf_path_get_string(child_path));
      if (rc != GF_SUCCESS) {
        FindClose(hr);
        gf_throw(rc);
      }
    } else {
      /* The other case */
      /* skip */
    }
  } while (FindNextFile(hr, &find_data));

  FindClose(hr);
  
  return GF_SUCCESS;
}

/*!
** @brief Returns GF_TRUE if the specified node is the root.
*/

static gf_bool
site_is_node_root(const xmlNodePtr node) {
  return !xmlStrcmp(node->name, SITE_NODE_ROOT) ? GF_TRUE : GF_FALSE;
}

/*!
** @brief Returns GF_TRUE if the specified node is a directory.
*/

static gf_bool
site_is_node_dir(const xmlNodePtr node) {
  return !xmlStrcmp(node->name, SITE_NODE_DIR) ? GF_TRUE : GF_FALSE;
}

/*!
** @brief Returns GF_TRUE if the specified node is a file.
*/

static gf_bool
site_is_node_file(const xmlNodePtr node) {
  return !xmlStrcmp(node->name, SITE_NODE_FILE) ? GF_TRUE : GF_FALSE;
}

static gf_bool
site_is_node_docbook(const xmlNodePtr node) {
  xmlChar* name = NULL;

  if (!node) {
    return GF_FALSE;
  }
  name = xmlGetProp(node, BAD_CAST "name");
  if (!name) {
    return GF_FALSE;
  }

  return xmlStrcmp(name, BAD_CAST "index.dbk") ? GF_FALSE : GF_TRUE; 
}

/*!
** @brief Returns GF_TRUE if the specified node is the root or a directory.
*/

static gf_bool
site_is_node_root_or_dir(const xmlNodePtr node) {
  const gf_bool is_root = site_is_node_root(node);
  const gf_bool is_dir  = site_is_node_dir(node);
  
  return (is_root || is_dir) ? GF_TRUE : GF_FALSE;
}

static gf_status
site_set_docbook_info(xmlNodePtr node, const gf_path* path) {
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlNodePtr cur = NULL;
  xmlNodePtr info = NULL;
  
  gf_validate(node);
  gf_validate(path);
  
  doc = xmlParseFile(gf_path_get_string(path));
  if (!doc) {
    gf_warn("Failed to parse DocBook file. (%s)", gf_path_get_string(path));
    return GF_E_READ;
  }
  /* Get the root element */
  root = xmlDocGetRootElement(doc);
  if (!root) {
    gf_warn("Failed to parse DocBook file. (%s)", gf_path_get_string(path));
    return GF_E_READ;
  }
  /* Get the first element 'info' */
  for (cur = root->children; cur; cur = cur->next) {
    if (cur->type != XML_ELEMENT_NODE) {
      continue;
    }
    // NOTE: We don't check the namespace.
    if (!xmlStrcmp(cur->name, BAD_CAST "info")) {
      info = xmlCopyNode(cur, 1);
      if (node->children) {
        xmlUnlinkNode(node->children);
        xmlFreeNode(node->children);
        node->children = NULL;
      }
      xmlAddChild(node, info);
      return GF_SUCCESS;
    }
  }
  
  return GF_SUCCESS;
}

/*!
** @brief Traverse
**
*/

static gf_status
site_traverse_nodes(const gf_path* path, xmlNodePtr node) {
  gf_status rc = 0;
  const xmlChar* node_path = NULL;
  const char* file_path = NULL;
  
  gf_validate(path);
  gf_validate(node);

  file_path = gf_path_get_string(path);

  if (site_is_node_root_or_dir(node)) {
    if (site_is_node_root(node)) {
      node_path = xmlGetProp(node, SITE_PROP_BASE);
      if (xmlStrcmp((const xmlChar *)file_path, node_path)) {
        gf_raise(GF_E_PARAM, "Invalid site data condition.");
      }
      /* Check if the path exists just in case. */
      if (!gf_path_file_exists(path)) {
        gf_raise(GF_E_PARAM, "The @base path file does nott exist.");
      }
    } else if (site_is_node_dir(node)) {
      if (!gf_path_file_exists(path)) {
        /* Remove no existent directory node */
        xmlUnlinkNode(node);
        xmlFreeNode(node);
        return GF_SUCCESS;
      }
    } else {
      /* Do nothing */
    }
    /* Get into the child nodes */
    for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
      gf_path* child_path = NULL;
      const xmlChar* file_name = NULL;

      /* Skip non-element nodes */
      if (cur->type != XML_ELEMENT_NODE) {
        continue;
      }
      
      file_name = xmlGetProp(cur, SITE_PROP_NAME);
      _(gf_path_append_string(&child_path, path, (const char *)file_name));

      rc = site_traverse_nodes(child_path, cur);
      gf_path_free(child_path);
      if (rc != GF_SUCCESS) {
        gf_throw(rc);
      }
    }
  } else if (site_is_node_file(node)) {
    /* File node */
    if (gf_path_file_exists(path)) {
      if (site_is_node_docbook(node)) {
        rc = site_set_docbook_info(node, path);
        if (rc != GF_SUCCESS) {
          gf_warn("Failed to read the DocBook info (%s).", file_path);
        }
      }
    } else {
      /* Remove no existent file node */
      xmlUnlinkNode(node);
      xmlFreeNode(node);
      return GF_SUCCESS;
    }
  } else {
    /* Do nothing - Unknown node */
  }

  return GF_SUCCESS;
}

/*!
** @brief
**
*/

static gf_status
site_create_document_root(gf_site* site, const char* root_path) {
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;

  gf_validate(site);
  gf_validate(root_path);

  doc = xmlNewDoc(SITE_XML_VERSION);
  if (!doc) {
    gf_raise(GF_E_API, "Failed to create a site document.");
  }
  root = xmlNewNode(NULL, SITE_NODE_ROOT);
  if (!root) {
    xmlFreeDoc(doc);
    gf_raise(GF_E_API, "Failed to create a element of the site document.");
  }
  xmlSetProp(root, SITE_PROP_BASE, BAD_CAST root_path);

  (void)xmlDocSetRootElement(doc, root);
  /*
  ** Normally the pointer site->doc is expected to be NULL though we test if
  ** that has the xmlDoc object just in case.
  */
  if (site->doc) {
    xmlFreeDoc(site->doc);
    site->doc = NULL;
  }
  site->doc = doc;
  
  return GF_SUCCESS;
}

/*!
** @brief
*/

gf_status
gf_site_update(gf_site* site, const gf_path* root_path) {
  gf_status rc = 0;
  xmlNodePtr root_node = NULL;
  gf_path* root = NULL;
  
  gf_validate(site);
  gf_validate(!gf_path_is_empty(root_path));

  /* Root node */
  if (!site->doc) {
    _(site_create_document_root(site, gf_path_get_string(root_path)));
  }
  root_node = xmlDocGetRootElement(site->doc);

  /* Root path */
  _(gf_path_new(&root, gf_path_get_string(root_path)));

  /* Traverse */
  rc = site_traverse_directories(root, root_node);
  if (rc != GF_SUCCESS) {
    gf_path_free(root);
    gf_throw(rc);
  }
  rc = site_traverse_nodes(root, root_node);
  gf_path_free(root);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

static gf_status
site_scan_directories(gf_site* site, gf_path* root) {
  gf_validate(site);
  gf_validate(root);
  
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
  int ret = 0;
  const char* file_path = gf_path_get_string(path);

  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  if (!site->doc) {
    gf_raise(GF_E_PARAM, "File information is empty.");
  }
  ret = xmlSaveFormatFileEnc(file_path, site->doc, "UTF-8", 1);
  if (ret < 0) {
    gf_raise(GF_E_WRITE, "Failed to write document information file.");
  }

  return GF_SUCCESS;
}

gf_status
gf_site_read_file(gf_site** site, const gf_path* path) {
  gf_site* tmp = NULL;
  
  gf_validate(site);
  gf_validate(!gf_path_is_empty(path));

  _(gf_site_new(&tmp));
  
  tmp->doc = xmlParseFile(gf_path_get_string(path));
  if (!tmp->doc) {
    gf_raise(GF_E_API, "Failed to parse site file.");
  }
  *site = tmp;

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

struct gf_site_node {
  xmlNodePtr ptr;
};

gf_status
gf_site_get_root(gf_site_node** node, gf_site* site) {
  xmlNodePtr root = NULL;
  gf_site_node* tmp = NULL;

  gf_validate(node);
  gf_validate(site);
  
  if (!site->doc) {
    gf_raise(GF_E_STATE, "Site information is empty.");
  }
  root = xmlDocGetRootElement(site->doc);
  if (!root) {
    gf_raise(GF_E_STATE, "Site information is empty.");
  }

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  tmp->ptr = root;

  *node = tmp;
  
  return GF_SUCCESS;
}

void
gf_site_node_free(gf_site_node* node) {
  if (node) {
    node->ptr = NULL;
    gf_free(node);
  }
}

gf_bool
gf_site_node_is_null(const gf_site_node* node) {
  return node && node->ptr ? GF_FALSE : GF_TRUE;
}

gf_site_node*
gf_site_node_next(gf_site_node* node) {
  if (node && node->ptr) {
    node->ptr = node->ptr->next;
  } else {
    node->ptr = NULL;
  }
  return node;
}

gf_site_node*
gf_site_node_prev(gf_site_node* node) {
  if (node && node->ptr) {
    node->ptr = node->ptr->prev;
  } else {
    node->ptr = NULL;
  }
  return node;
}

gf_site_node*
gf_site_node_child(gf_site_node* node) {
  if (node && node->ptr) {
    node->ptr = node->ptr->children;
  } else {
    node->ptr = NULL;
  }
  return node;
}

gf_site_node*
gf_site_node_parent(gf_site_node* node) {
  if (node && node->ptr) {
    node->ptr = node->ptr->parent;
  } else {
    node->ptr = NULL;
  }
  return node;
}

static gf_bool
site_node_is_type(const gf_site_node* node, const xmlChar* type) {
  gf_bool ret = GF_FALSE;
  
  if (!node || !node->ptr) {
    return GF_FALSE;
  }
  if (!xmlStrcmp(node->ptr->name, type)) {
    ret = GF_TRUE;
  } else {
    ret = GF_FALSE;
  }

  return ret;
}

gf_bool
gf_site_node_is_root(const gf_site_node* node) {
  return site_node_is_type(node, SITE_NODE_ROOT);
}

gf_bool
gf_site_node_is_file(const gf_site_node* node) {
  return site_node_is_type(node, SITE_NODE_FILE);
}

gf_bool
gf_site_node_is_directory(const gf_site_node* node) {
  return site_node_is_type(node, SITE_NODE_DIR);
}

gf_status
gf_site_node_get_full_path(gf_path** full_path, const gf_site_node* node) {
  gf_status rc = 0;
  
  xmlChar* name = NULL;
  gf_path* base = NULL;
  gf_path* path = NULL;
  gf_site_node n = { .ptr = NULL };

  gf_validate(full_path);
  gf_validate(node);
  gf_validate(node->ptr);

  n.ptr = node->ptr;

  _(gf_path_new(&path, ""));
  /*
  ** Collect the path fragments
  */
  for ( ; !gf_site_node_is_root(&n); gf_site_node_parent(&n)) {
    name = xmlGetProp(n.ptr, SITE_PROP_NAME);
    if (!name) {
      assert(GF_FALSE);
      continue;
    }
    rc = gf_path_new(&base, (char*)name);
    if (rc != GF_SUCCESS) {
      gf_path_free(path);
      gf_throw(rc);
    }
    rc = gf_path_append(base, path);
    gf_path_swap(path, base);
    gf_path_free(base);
    base = NULL;
    if (rc != GF_SUCCESS) {
      gf_path_free(path);
      gf_throw(rc);
    }
  }
  /*
  ** Append the base path
  */
  assert(gf_site_node_is_root(&n));
  name = xmlGetProp(n.ptr, SITE_PROP_BASE);
  rc = gf_path_new(&base, (char*)name);
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    gf_throw(rc);
  }
  rc = gf_path_append(base, path);
  gf_path_swap(path, base);
  gf_path_free(base);
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    gf_throw(rc);
  }
  /*
  ** Assign
  */
  *full_path = path;

  return GF_SUCCESS;
}

gf_status
gf_site_node_get_tree_path(gf_path** tree_path, const gf_site_node* node) {
  gf_status rc = 0;
  
  xmlChar* name = NULL;
  gf_path* base = NULL;
  gf_path* path = NULL;
  gf_site_node n = { .ptr = NULL };

  gf_validate(tree_path);
  gf_validate(node);
  gf_validate(node->ptr);

  n.ptr = node->ptr;

  _(gf_path_new(&path, ""));
  /*
  ** Collect the path fragments
  */
  for ( ; !gf_site_node_is_root(&n); gf_site_node_parent(&n)) {
    name = xmlGetProp(n.ptr, SITE_PROP_NAME);
    if (!name) {
      assert(GF_FALSE);
      continue;
    }
    rc = gf_path_new(&base, (char*)name);
    if (rc != GF_SUCCESS) {
      gf_path_free(path);
      gf_throw(rc);
    }
    rc = gf_path_append(base, path);
    gf_path_swap(path, base);
    gf_path_free(base);
    base = NULL;
    if (rc != GF_SUCCESS) {
      gf_path_free(path);
      gf_throw(rc);
    }
  }
  /*
  ** Assign
  */
  *tree_path = path;

  return GF_SUCCESS;
}

gf_status
gf_site_node_get_file_name(char** file_name, const gf_site_node* node) {
  gf_validate(file_name);
  gf_validate(node);
  gf_validate(node->ptr);

  if (!gf_site_node_is_root(node)) {
    *file_name = (char*)xmlGetProp(node->ptr, SITE_PROP_NAME);
  } else {
    *file_name = NULL;
  }
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
** Document information
*/

struct gf_doc_info {
  char* title;
  char* path;
  char* date;
  char* status;
};

void
gf_doc_info_free(gf_doc_info* info) {
  if (info) {
    if (info->title) {
      gf_free(info->title);
      info->title = NULL;
    }
    if (info->path) {
      gf_free(info->path);
      info->path = NULL;
    }
    if (info->date) {
      gf_free(info->date);
      info->date = NULL;
    }
    if (info->status) {
      gf_free(info->status);
      info->status = NULL;
    }
    gf_free(info);
  }
}

static gf_status
doc_info_init(gf_doc_info* info) {
  gf_validate(info);

  info->title  = NULL;
  info->path   = NULL;
  info->date   = NULL;
  info->status = NULL;

  return GF_SUCCESS;
}

static gf_status
doc_info_set(gf_doc_info* info, const gf_site_node* node) {
  (void)info;
  (void)node;

  return GF_SUCCESS;
}

gf_status
gf_doc_info_get(gf_doc_info** info, const gf_site_node* node) {
  gf_status rc = 0;
  gf_doc_info* tmp = NULL;
  
  gf_validate(info);
  gf_validate(node);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = doc_info_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = doc_info_set(tmp, node);
  if (rc != GF_SUCCESS) {
    gf_doc_info_free(tmp);
    gf_throw(rc);
  }

  *info = tmp;

  return GF_SUCCESS;
}

const char*
gf_doc_info_title(const gf_doc_info* info) {
  static const char nul_[] = "NO TITLE";
  return info ? info->title : nul_;
}

const char*
gf_doc_info_path(const gf_doc_info* info) {
  static const char nul_[] = "NO NAME";
  return info ? info->path : nul_;
}

const char*
gf_doc_info_date(const gf_doc_info* info) {
  static const char nul_[] = "0000-00-00";
  return info ? info->date : nul_;
}

const char*
gf_doc_info_status(const gf_doc_info* info) {
  static const char nul_[] = "UNKNOWN";
  return info ? info->status : nul_;
}

gf_status
gf_doc_info_print(FILE* fp, const gf_site_node* node) {
  gf_doc_info* info = NULL;
  FILE* fpc = fp ? fp : stdout;

  _(gf_doc_info_get(&info, node));
  fprintf(fpc, "%s %10s %-16s %s",
          gf_doc_info_status(info), gf_doc_info_date(info),
          gf_doc_info_path(info), gf_doc_info_title(info));
  
  return GF_SUCCESS;
}
