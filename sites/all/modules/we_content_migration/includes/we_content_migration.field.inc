<?php

/**
 * Handle field base export to get neccessary information.
 */
function field_base_we_content_migration_export($field_base) {
  $output = array();
  if(!empty($field_base)) {
    foreach ($field_base as $field_name) {
      if ($field = we_content_migration_field_base_load($field_name)) {
        unset($field['columns']);
        unset($field['foreign keys']);
        // Only remove the 'storage' declaration if the field is using the default
        // storage type.
        if ($field['storage']['type'] == variable_get('field_storage_default', 'field_sql_storage')) {
          unset($field['storage']);
        }
        // If we still have a storage declaration here it means that a non-default
        // storage type was altered into to the field definition. And no one would
        // never need to change the 'details' key, so don't render it.
        if (isset($field['storage']['details'])) {
          unset($field['storage']['details']);
        }

        _field_instance_we_content_migration_export_sort($field);
        $output[$field_name] = $field;
      }
    }
  }

  return $output;
}

/**
 * Load a field base configuration by a field_name identifier.
 */
function we_content_migration_field_base_load($field_name) {
  if ($field_info = field_info_field($field_name)) {
    unset($field_info['id']);
    unset($field_info['bundles']);
    return $field_info;
  }

  return FALSE;
}

function field_instance_we_content_migration_export($field_instance) {
  $output = array();
  if(!empty($field_instance)) {
    foreach ($field_instance as $identifier) {
      if ($instance = we_content_migration_field_instance_load($identifier)) {
        _field_instance_we_content_migration_export_sort($instance);
        $output[$identifier] = $instance;
      }
    }
  }

  return $output;
}

/**
 * Load a field instance configuration by a field_name identifier.
 */
function we_content_migration_field_instance_load($identifier) {
  list($entity_type, $bundle, $field_name) = explode('-', $identifier);
  if ($instance_info = field_info_instance($entity_type, $field_name, $bundle)) {
    unset($instance_info['id']);
    unset($instance_info['field_id']);
    return $instance_info;
  }
  return FALSE;
}

// Helper to enforce consistency in field export arrays.
function _field_instance_we_content_migration_export_sort(&$field, $sort = TRUE) {

  // Some arrays are not sorted to preserve order (for example allowed_values).
  static $sort_blacklist = array(
      'allowed_values',
      'format_handlers',
  );

  if ($sort) {
    uksort($field, 'strnatcmp');
  }
  foreach ($field as $k => $v) {
    if (is_array($v)) {
      _field_instance_we_content_migration_export_sort($field[$k], !in_array($k, $sort_blacklist));
    }
  }
}

/**
* Import structure : field base
*/
function _we_content_migration_import_field_base($path, $import_name) {
  $file_path = "{$path}/$import_name.field_base.json";
  if(file_exists($file_path)) {
    field_info_cache_clear();
    $field_base = file_get_contents($file_path);
    $field_base = drupal_json_decode($field_base);
    // Load all the existing field bases up-front so that we don't
    // have to rebuild the cache all the time.
    $existing_fields = field_info_fields();
    foreach ($field_base as $field) {
      // Create or update field.
      if (isset($existing_fields[$field['field_name']])) {
        $existing_field = $existing_fields[$field['field_name']];
        $array_diff_result = drupal_array_diff_assoc_recursive($field + $existing_field, $existing_field);
        if (!empty($array_diff_result)) {
          try {
            field_update_field($field);
          }
          catch (FieldException $e) {
            watchdog('we_content_migration', 'Attempt to update field %label failed: %message', array('%label' => $field['field_name'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
          }
        }
      }
      else {
        try {
          field_create_field($field);
        }
        catch (FieldException $e) {
          watchdog('we_content_migration', 'Attempt to create field %label failed: %message', array('%label' => $field['field_name'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
        }
        $existing_fields[$field['field_name']] = $field;
      }
    }
  }
}

/**
* Import structure : field instance
*/
function _we_content_migration_import_field_instance($path, $import_name) {
  $file_path = "{$path}/$import_name.field_instance.json";
  if(file_exists($file_path)) {
    $instances = file_get_contents($file_path);
    $instances = drupal_json_decode($instances);
    // Load all the existing instances up-front so that we don't
    // have to rebuild the cache all the time.
    $existing_instances = field_info_instances();

    foreach ($instances as $field_instance) {
      // If the field base information does not exist yet, cancel out.
      if (!field_info_field($field_instance['field_name'])) {
        continue;
      }

      // Create or update field instance.
      if (isset($existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']])) {
        $existing_instance = $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']];
        if ($field_instance + $existing_instance !== $existing_instance) {
          try {
            field_update_instance($field_instance);
          }
          catch (FieldException $e) {
            watchdog('we_content_migration', 'Attempt to update field instance %label (in %entity entity type %bundle bundle) failed: %message', array('%label' => $field_instance['field_name'], '%entity' => $field_instance['entity_type'], '%bundle' => $field_instance['bundle'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
          }
        }
      }
      else {
        try {
          field_create_instance($field_instance);
        }
        catch (FieldException $e) {
          watchdog('we_content_migration', 'Attempt to create field instance %label (in %entity entity type %bundle bundle) failed: %message', array('%label' => $field_instance['field_name'], '%entity' => $field_instance['entity_type'], '%bundle' => $field_instance['bundle'], '%message' => $e->getMessage()), WATCHDOG_ERROR);
        }
        $existing_instances[$field_instance['entity_type']][$field_instance['bundle']][$field_instance['field_name']] = $field_instance;
      }
    }
  }
}

/**
* return field_name which field_type = taxonomy term, entity reference, image, files
*/
function field_base_exception() {
  $exception = array('taxonomy_term_reference', 'image', 'entityreference', 'file');
  $field_exceptions = array();
  foreach (field_info_fields() as $field => $value) {
    if(in_array($value['type'], $exception)) {
      array_push($field_exceptions, $field);
    }
  }
  return $field_exceptions;
}