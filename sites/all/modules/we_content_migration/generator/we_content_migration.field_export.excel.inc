<?php

/**
 * Handle field data export.
 */
function we_content_migration_excel_field_export($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  // @To-Do: Support multiple languages
  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
    $field_data[] = $content['value'];
  }

  return $field_data;
}

/**
 * Handle data export for text field
 */
function we_content_migration_excel_field_export_text($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  // @To-Do: Support multiple languages
  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
      $field_data[] = $content['value'];
  }

  return $field_data;
}

/**
 * Handle data export for entityreference field
 */
function we_content_migration_excel_field_export_entityreference($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
    $field_data[] = $content['target_id'];
  }

  return $field_data;
}

/**
 * Handle data export for commerce_product_reference field
 */
function we_content_migration_excel_field_export_commerce_product_reference($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
    $field_data[] = $content['product_id'];
  }

  return $field_data;
}

/**
 * Handle data export for taxonomy field
 */
function we_content_migration_excel_field_export_taxonomy($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
    $term = taxonomy_term_load($content['tid']);
    if ($term) {
      $field_data[] = $term->name;
    }
  }

  return $field_data;
}

/**
 * Handle data export for address field
 */
function we_content_migration_excel_field_export_addressfield($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  $field_data = array();
  foreach ($data[LANGUAGE_NONE] as $content) {
    $field_data[] = "{$content['thoroughfare']}, {$content['locality']}, {$content['administrative_area']}, {$content['postal_code']}";
  }

  return $field_data;
}

/**
 * Handle data export for image field
 */
function we_content_migration_excel_field_export_image($data, $type, $module, $options = NULL) {
  if (empty($data))
    return array();

  $field_data = array();
  $path = $options['directory'];
  foreach ($data[LANGUAGE_NONE] as $content) {
    $source_image = str_replace('public://', '', $content['uri']);
    $destination = $path . '/files/' . $source_image;
    $directory = dirname($destination);
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    file_unmanaged_copy($content['uri'], $directory);

    $field_data[] = $content['uri'];
  }

  return $field_data;
}