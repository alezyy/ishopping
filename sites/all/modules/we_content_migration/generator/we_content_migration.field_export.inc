<?php

/**
 * Handle field data export.
 */
function we_content_migration_field_export($data, $type, $module, $options = NULL) {
  return $data;
}

/**
 * Handle data export for text field
 */
function we_content_migration_field_export_text($data, $type, $module, $options = NULL) {
  return $data;
}

/**
 * Handle data export for image field
 */
function we_content_migration_field_export_image($data, $type, $module, $options = NULL) {
  $field_data = array();
  $path = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH) . '/' .$options;

  foreach ($data as $lang => $contents) {
    foreach ($contents as $key => $content) {
      $field_data[$lang][$key] = array(
        'filename' => $content['filename'],
        'uri' => $content['uri'],
        'alt' => $content['alt'],
        'title' => $content['title'],
      );
      $source_image = str_replace('public://', '', $content['uri']);
      $destination = $path . '/files/' . $source_image;
      $directory = dirname($destination); 
      file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
      file_unmanaged_copy($content['uri'], $directory);
    }
  }
  return $field_data;
}

/**
 * Handle data export for file field
 */
function we_content_migration_field_export_file($data, $type, $module, $options = NULL) {
  $field_data = array();

  foreach ($data as $lang => $contents) {
    foreach ($contents as $key => $content) {
      $field_data[$lang][$key] = array(
        'filename' => $content['filename'],
        'uri' => $content['uri'],
        'description' => $content['description'],
        'display' => $content['display'],
      );
    }
  }

  return $field_data;
}

/**
 * Handle data export for entityreference field
 */
function we_content_migration_field_export_entityreference($data, $type, $module, $options = NULL) {
  $field_data = array();

  foreach ($data as $lang => $contents) {
    foreach ($contents as $key => $content) {
      $field_data[$lang][$key] = array('value' => $content['target_id']);
    }
  }

  return $field_data;
}

/**
 * Handle data export for taxonomy field
 */
function we_content_migration_field_export_taxonomy($data, $type, $module, $options = NULL) {
  $field_data = array();

  foreach ($data as $lang => $contents) {
    foreach ($contents as $key => $content) {
      $field_data[$lang][$key] = array('value' => $content['tid']);
    }
  }

  return $field_data;
}