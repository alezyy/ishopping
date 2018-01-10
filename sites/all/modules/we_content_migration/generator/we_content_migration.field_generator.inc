<?php
/**
 * Include all functions to generate entity field contents
 */

function we_content_migration_field_generate_text($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $object_field = array();
  $object_field['format'] = 'full_html';
  if ($instance['widget']['type'] != 'text_textfield') {
    $object_field['value'] = isset($field_value['value']) ? $field_value['value'] : $field_value;
    $object_field['format'] = isset($field_value['format']) ? $field_value['format'] : 'full_html';
    if ($instance['widget']['type'] == 'text_textarea_with_summary') {
      $object_field['summary'] = is_array($field_value) && isset($field_value['summary']) ? $field_value['summary'] : "";
    }
  }
  else {
    $object_field['value'] = $field_value['value'];
  }

  return $object_field;
}

function we_content_migration_field_generate_entityreference($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return 'db_store';
}

function we_content_migration_field_generate_taxonomy($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return 'db_store';
}

function we_content_migration_field_generate_boolean($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return array('value' => $field_value);
}

function we_content_migration_field_generate_number($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $type_number = $field_info['type'];
  switch ($type_number) {
    case 'number_decimal':
      $object_field = $field_value;
      break;
    case 'number_float':
      $object_field = $field_value;
      break;
    case 'number_integer':
      $object_field = $field_value;
      break;
  }
  return $object_field;
}

function we_content_migration_field_generate_image($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $object_field = array();
  if(isset($field_value['fid']) && !empty($field_value['fid'])) {
    $file = file_load($field_value['fid']);
    $object_field['fid'] = $field_value['fid'];
    $object_field['alt'] = isset($field_value['alt']) ? $field_value['alt'] : "";
    $object_field['title'] = isset($field_value['alt']) ? $field_value['alt'] : "";
  }
  elseif(isset($field_value['path']) && !empty($field_value['path'])) {
    $info = isset($field_value['info']) ? $field_value['info'] : array();
    $object_field = we_content_migration_field_generate_save_image($field_value['path'], $info, $field_info, $instance);
    $object_field['alt'] = isset($field_value['alt']) ? $field_value['alt'] : "";
    $object_field['title'] = isset($field_value['alt']) ? $field_value['alt'] : "";
  }
  elseif(isset($field_value['uri']) && !empty($field_value['uri'])) {
    $info = isset($field_value['info']) ? $field_value['info'] : array();
    $object_field = we_content_migration_field_generate_save_image($field_value['uri'], $info, $field_info, $instance);
    $object_field['alt'] = isset($field_value['alt']) ? $field_value['alt'] : "";
    $object_field['title'] = isset($field_value['alt']) ? $field_value['alt'] : "";
  }
  elseif(isset($field_value['url']) && !empty($field_value['url'])) {
    $object_field = we_content_migration_field_generate_download_image($field_value['url'], $field_info, $instance);
    $object_field['alt'] = isset($field_value['alt']) ? $field_value['alt'] : "";
    $object_field['title'] = isset($field_value['alt']) ? $field_value['alt'] : "";
  }
  return $object_field;
}

function we_content_migration_field_generate_save_image($path, $info, $field, $instance) {
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

function we_content_migration_field_generate_download_image($url, $field, $instance) {
  $info = pathinfo($url);
  $extension = isset($info['extension']) ? $info['extension'] : 'png';

  $destination_dir = "public://import_images_tmp";
  file_prepare_directory($destination_dir, FILE_CREATE_DIRECTORY);

  $tmp_file = drupal_tempnam($destination_dir, 'imagefield_');
  $content = @file_get_contents($url);

  if(empty($content)) {
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_URL, $url);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($curl, CURLOPT_HEADER, false);
    $content = curl_exec($curl);
    curl_close($curl);
  }

  if(empty($content)) {
    $input_path = str_replace("http://", "https://", $input_path);
    $content = @file_get_contents($input_path);

    if(empty($content)) {
      $url = $input_path;
      $curl = curl_init();
      curl_setopt($curl, CURLOPT_URL, $url);
      curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($curl, CURLOPT_HEADER, false);
      $content = curl_exec($curl);
      curl_close($curl);
    }
  }

  if ($tmp_file && $content) {
    $destination = $tmp_file . '.' . $extension;
    file_put_contents($destination, $content);
    return general_utils_save_image($destination, $info, $object, $field, $instance, $bundle);
  }
  return NULL;
}

function we_content_migration_field_generate_date($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  return $field_value;
}

function we_content_migration_field_generate_link($field_value, $instance, $field_info, $language = LANGUAGE_NONE){
  $object_field = array();
  $object_field['url'] = isset($field_value['url']) ? $field_value['url'] : "";
  $object_field['title'] = isset($field_value['title']) ? $field_value['title'] : "";
  $object_field['attributes'] = isset($field_value['attributes']) ? $field_value['attributes'] : "";
  return $object_field;
}


function we_content_migration_field_generate_video_embed_field($field_value, $instance, $field_info, $language = LANGUAGE_NONE){

  return $field_value;
}

function we_content_migration_field_generate_addressfield($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  if(is_array($field_value)) {
    $object_field = array(
      'country' => $field_value['country'],
      'administrative_area' => $field_value['administrative_area'],
      'locality' => $field_value['locality'],
      'postal_code' => $field_value['postal_code'],
      'thoroughfare' => $field_value['thoroughfare'],
      'premise' => $field_value['premise'],
      'sub_premise' => $field_value['sub_premise'],
      'organisation_name' => $field_value['organisation_name'],
      'name_line' => $field_value['name_line'],
      'first_name' => $field_value['first_name'],
      'last_name' => $field_value['last_name'],
      'data' => $field_value['data']
    );
  }
  else {
    $states = array('ACT', 'NSW', 'NT', 'QLD', 'SA', 'TAS', 'VIC', 'WA');
    $object_field = array(
      'country' => 'AU',
    );
    $t1 = $field_value;
    $parts = explode(" ", $t1);
    $object_field['postal_code'] = trim($parts[count($parts) - 1]);

    $object_field['administrative_area'] = trim($parts[count($parts) - 2]);

    unset($parts[count($parts) - 1]);
    unset($parts[count($parts) - 1]);

    $t3 = implode(" ", $parts);
    $parts = explode(",", $t3);
    if(isset($parts[1])) {
      $object_field['locality'] = trim($parts[1]);
      $object_field['thoroughfare'] = trim($parts[0]);
    }
    else {
      $ps = explode(" ", $t3);
      $object_field['locality'] = trim($ps[count($ps) - 1]);
      unset($ps[count($ps) - 1]);
      $t4 = implode(" ", $ps);
      $object_field['thoroughfare'] = trim($t4);
    }
  }
  return $object_field;
}

function we_content_migration_field_generate_email($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $object_field = array(
    'email' => $field_value,
  );
  return $object_field;
}

function we_content_migration_field_generate_list($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $number_of_values = $field_info['cardinality'];
  $list_type = $field_info['type'];
  $object_field = array();
  switch ($list_type) {
    case 'list_boolean':
      $object_field = we_content_migration_list_boolean($field_value);
      break;
    case 'list_text':
      $object_field = we_content_migration_list_text($field_value, $number_of_values);
      break;
    case 'list_float':
      $object_field = $field_value;
      break;
    case 'list_integer':
      $object_field = $field_value;
      break;
  }

  return $object_field;
}

function we_content_migration_list_boolean($field_value) {
  if ($field_value['value'] == 'true' || $field_value['value'] == 1) {
    return array('value' => 1);
  }
  else {
    return array('value' => 0);
  }
}

function we_content_migration_list_text($field_value, $number_of_values) {

  return $field_value;
}

function we_content_migration_field_generate_file($field_value, $instance, $field_info, $language = LANGUAGE_NONE) {
  $object_field = array();

  if(isset($field_value['uri']) && !empty($field_value['uri'])) {
    $info = isset($field_value['info']) ? $field_value['info'] : array();
    $object_field = we_content_migration_field_generate_save_file($field_value['uri'], $field_info, $instance);
  }
  return $object_field;
}

function we_content_migration_field_generate_save_file($url, $field_info, $instance) {
  $info = pathinfo($url);
  $filename = isset($info['filename']) ? $info['filename'] : "";
  $extension = isset($info['extension']) ? $info['extension'] : 'png';

  $object_field = array();
  $path = we_content_migration_field_generate_download_file($url);
  if ($path) {
    $source = new stdClass();
    $source->uri = $path;
    $source->uid = 1;
    $source->filemime = 'field/file/' . pathinfo($path, PATHINFO_EXTENSION);
    $destination_dir = $field_info['settings']['uri_scheme'] . '://' . $instance['settings']['file_directory'];
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
  }
  else {
    return NULL;
  }
  $object_field['fid'] = $file->fid;
  $object_field['display'] = 1;
  $object_field['description'] = '';
  $object_field['upload_button'] = 'Upload';
  $object_field['remove_button'] = 'Remove';
  $object_field['upload'] = '';
  return $object_field;
}

function we_content_migration_field_generate_download_file($input_path) {
  $info = pathinfo($input_path);
  $extension = isset($info['extension']) ? $info['extension'] : 'png';
  $tmp_file = drupal_tempnam('temporary://', 'imagefield_');
  if ($tmp_file && file_exists($input_path)) {
    $content = file_get_contents($input_path);
    $destination = $tmp_file . '.' . $extension;
    $path = file_unmanaged_move($tmp_file, $destination, FILE_CREATE_DIRECTORY);
    file_put_contents(drupal_realpath($path), $content);
    return $destination;
  }
  return NULL;
}