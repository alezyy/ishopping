<?php
/**
 * @file
 * we_content_migration.node.inc
 */

function node_we_content_migration_export_bundle($types) {
  $elements = array(
    'type' => TRUE,
    'name' => TRUE,
    'base' => FALSE,
    'description' => TRUE,
    'has_title' => FALSE,
    'title_label' => TRUE,
    'help' => TRUE,
  );

  $output = array();
  foreach ($types as $type) {
    if ($info = node_type_get_type($type)) {
      // Use this for
      foreach ($elements as $key => $t) {
        $output[$type][$key] = $info->$key;
      }
    }
  }

  return $output;
}

/**
 * 
 * Import node bundles
 */
function node_we_content_migration_import_bundle($bundles) {
  $existing_node = entity_get_info()['node']['bundles'];
  foreach ($bundles as $bundle => $value) {
    if (!isset($existing_node[$bundle])) {
      $node_type = node_type_set_defaults($value);
      node_type_save($node_type);
    }
  }
}

/**
 * 
 * Import node content
 */
function node_we_content_migration_import_content($contents) {
  module_load_include('inc', 'we_content_migration', 'we_content_migration.field');
  $field_exceptions = field_base_exception();

  foreach ($contents as $original_node) {
   $node = node_we_content_migration_node_clone((object) $original_node, $field_exceptions);
    entity_save('node', $node);
  }
}

/**
 * Prepare a clone of the node during import.
 */
function node_we_content_migration_node_clone($original_node, $field_exceptions) {
  global $user;

  $node = clone($original_node);

  // Remove field image, entity reference, term reference, files
  foreach ($field_exceptions as $exception) {
    if(isset($node->$exception)) {
      unset($node->$exception);
    }
  }

  $node->nid = NULL;
  $node->vid = NULL;
  $node->uid = 0;
  $node->created = NULL;
  $node->changed = NULL;
  $node->revision_timestamp = NULL;
  $node->last_comment_timestamp = NULL;
  $node->path = NULL;

  /*else {
    if (is_array($node->path) && isset($node->path['pid'])) {
      unset($node->path['pid']);
    }
    if (module_exists('pathauto')) {
      // Prevent pathauto from creating a new path alias.
      $node->path['pathauto'] = FALSE;
    }
  }*/

  $node->status = FALSE;
  $node->promote = FALSE;
  $node->sticky = FALSE;
  return $node;

}