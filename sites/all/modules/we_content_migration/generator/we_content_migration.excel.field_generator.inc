<?php
/**
 * Include all functions to generate entity field contents from excel file
 */

function we_content_migration_excel_field_generate($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return array('value' => $field_value);
}

function we_content_migration_excel_field_generate_text($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $field = array();

  $field['value'] = $field_value;

  if ($instance['widget']['type'] != 'text_textfield') {
    $field['format'] = 'full_html';
  }

  return $field;
}

function we_content_migration_excel_field_generate_entityreference($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return 'db_store';
}

function we_content_migration_excel_field_generate_taxonomy($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $object_field = array();
  if (!empty($field_value)) {
    $machine_name = $field_info['settings']['allowed_values'][0]['vocabulary'];
    $vocabulary = taxonomy_vocabulary_machine_name_load($machine_name);
    $query = db_select('taxonomy_term_data', 't');
    $tid = $query->fields('t', array('tid'))
                 ->condition('t.vid', $vocabulary->vid, '=')
                 ->condition('t.name', $field_value)
                 ->range(0, 1)
                 ->execute()
                 ->fetchField();
    if ($tid === FALSE) {
      $term_info = array(
        'name' => $field_value,
        'description' => $field_value,
        'vid' => $vocabulary->vid,
      );
      $term_object = (object) $term_info;
      taxonomy_term_save($term_object);
      $tid = $term_object->tid;
    }

    $object_field['tid'] = (int) $tid;
  }

  return $object_field;
}

function we_content_migration_excel_field_generate_image($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $destination = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH);
  $directory = $destination . '/import';

  $field_value = str_replace('public://', '', $field_value);
  $file_path = drupal_realpath($directory . '/files/' . $field_value);

  $object_field = array();

  $object_field = we_content_migration_excel_field_generate_save_image($file_path, array(), $field_info, $instance);

  return $object_field;
}

function we_content_migration_excel_field_generate_save_image($path, $info, $field, $instance) {
  if(empty($info)) {
    $info = pathinfo($path);
  }

  $filename = isset($info['filename']) ? $info['filename'] : "";
  $extension = isset($info['extension']) ? $info['extension'] : 'png';

  $object_field = array();
  $source = new stdClass();
  $source->uri = $path;
  $source->uid = 1;
  $source->filemime = 'image/' . pathinfo($path, PATHINFO_EXTENSION);
  $destination_dir = $field['settings']['uri_scheme'] . '://' . $instance['settings']['file_directory'];

  file_prepare_directory($destination_dir, FILE_CREATE_DIRECTORY);
  $counter = 0;
  $destination = $destination_dir . '/' . $filename . "." . $extension;
  $realpath = drupal_realpath($destination);

  while (file_exists($realpath)) {
    $counter ++;
    $destination = $destination_dir . '/' . $filename . ($counter ? "_" . $counter : "") . "." . $extension;
    $realpath = drupal_realpath($destination);
  }
  $source->filename = $filename . ($counter ? "_" . $counter : "") . "." . $extension;

  $file = file_move($source, $destination, FILE_CREATE_DIRECTORY);

  return (array)$file;
}

function we_content_migration_excel_field_generate_boolean($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return array('value' => $field_value);
}

function we_content_migration_excel_field_generate_email($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return array('email' => $field_value);
}

function we_content_migration_excel_field_generate_list($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $number_of_values = $field_info['cardinality'];
  $list_type = $field_info['type'];
  $object_field = array();
  switch ($list_type) {
    case 'list_boolean':
      $object_field = we_content_migration_excel_list_boolean($field_value);
      break;
    case 'list_text':
      $object_field = we_content_migration_excel_list_text($field_value, $number_of_values);
      break;
    case 'list_float':
      $object_field = array('value' => $field_value);
      break;
    case 'list_integer':
      $object_field = array('value' => $field_value);
      break;
  }

  return $object_field;
}

function we_content_migration_excel_list_boolean($field_value) {
  if ($field_value == 'true' || $field_value == 1) {
    return array('value' => 1);
  }
  else {
    return array('value' => 0);
  }
}

function we_content_migration_excel_list_text($field_value, $number_of_values) {
  return array('value' => $field_value);
}

function we_content_migration_excel_field_generate_date($field_value, $object, $field_info, $instance, $bundle) {

  if ($field_info['type'] == 'datestamp') {
    return array('value' => $field_value);
  }

  if ($field_value == '')
    return array();

  $field_value = str_replace('/', '-', $field_value);
  $time = strtotime($field_value);
  $str = date('Y-m-d H:i:s', $time);
  return array('value' => $str);
}

function we_content_migration_excel_field_generate_addressfield($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $address = explode(', ', $field_value);

  return array(
    'thoroughfare' => isset($address[0]) ? $address[0] : '',
    'locality' => isset($address[1]) ? $address[1] : '',
    'administrative_area' => isset($address[2]) ? $address[2] : '',
    'postal_code' => isset($address[3]) ? $address[3] : '',
  );
}

function we_content_migration_excel_field_generate_commerce_product_reference($field_value, $object, $field, $instance, $bundle) {
  return array('product_id' => $field_value);
}