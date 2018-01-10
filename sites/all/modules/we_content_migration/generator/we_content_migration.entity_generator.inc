<?php

/**
 * Handle all entity generator functions
 */

function we_content_migration_generate_entity($data, $entity_type, $bundle, $entity_id = NULL, $type = 'JSON') {
  // Create new entity or load entity existed
  if ($entity_id) {
    $entity = entity_load_single($entity_type, $entity_id);
  }
  else {
    $entity = entity_create($entity_type, array('type' => $bundle));
  }

  if ($entity) {
    $ewrapper = entity_metadata_wrapper($entity_type, $entity, array('bundle' => $bundle));
    $property_info = $ewrapper->getPropertyInfo();

    $field_data = array();
    foreach ($data as $key => $value) {
      if (!is_array($value) && isset($property_info[$key])) {
        $entity->{$key} = $value;
      }
      else if(is_array($value)) {
        $field_data[$key] = $value;
      }
    }

    // Need to change some config for taxonomy
    if ($entity_type == 'taxonomy_term') {
      $vocabulary = taxonomy_vocabulary_machine_name_load($bundle);

      if (!$vocabulary)
        return FALSE;
      $entity->vid = $vocabulary->vid;
    }

    // Save the entity first to get new entity id for field reference
    $ewrapper->save();

    if ($type == 'JSON') {
      we_content_migration_generate_fields($field_data, $entity, $entity_type, $bundle);
    }
    else if ($type == 'EXCEL') {
      we_content_migration_excel_generate_fields($field_data, $entity, $entity_type, $bundle);
    }

    // print_r($entity); exit;
    // Save all field data
    $ewrapper->save();
    return $ewrapper->getIdentifier();
  }

  return FALSE;
}

function we_content_migration_generate_fields($data, $entity, $entity_type, $bundle) {
  $field_types = field_info_field_types();
  $instances = field_info_instances($entity_type, $bundle);
  $ewrapper = entity_metadata_wrapper($entity_type, $entity);

  foreach ($instances as $instance) {
    $field_name = $instance['field_name'];
    if (isset($data[$field_name])) {
      $field = field_info_field($field_name);
      $field_data = $data[$field_name];
      $module = $field_types[$field['type']]['module'];

      foreach ($field_data as $lang => $field_values) {
        if(is_array($field_values)) {
          $db_store = false;
          foreach ($field_values as $field_value) {
            $generator_function = 'we_content_migration_field_generate_' . $module;
            if (function_exists($generator_function)) {
              $new_field_data = $generator_function($field_value, $instance, $field);
              if ($new_field_data && $new_field_data != 'db_store') {
                $entity->{$field_name}[$lang][] = $new_field_data;
              }
              else if ($new_field_data == 'db_store') {
                $db_store = true;
              }
            }
          }

          if ($db_store) {
            $field_values_encode = serialize($field_values);
            we_content_migration_mapping_save_reference($entity_type, $bundle, $ewrapper->getIdentifier(),
                                            $field_name, $field_values_encode, $lang);
          }
        }
      }
    }
  }
}

function we_content_migration_excel_generate_fields($data, $entity, $entity_type, $bundle) {
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.excel.field_generator');
  $field_types = field_info_field_types();
  $instances = field_info_instances($entity_type, $bundle);
  $ewrapper = entity_metadata_wrapper($entity_type, $entity);

  foreach ($instances as $instance) {
    $field_name = $instance['field_name'];
    if (isset($data[$field_name])) {
      $field = field_info_field($field_name);
      $field_data = $data[$field_name];
      $module = $field_types[$field['type']]['module'];

      if (empty($field_data)) {
        $entity->{$field_name} = array();
        continue;
      }

      $first_value = true;
      foreach ($field_data as $key => $field_value) {
        $db_store = false;
        $generator_function = 'we_content_migration_excel_field_generate_' . $module;
        if (function_exists($generator_function)) {
          $new_field_data = $generator_function($field_value, $instance, $field);
          if ($new_field_data && $new_field_data != 'db_store') {
            if (isset($entity->{$field_name}[LANGUAGE_NONE]) && $first_value) {
              $entity->{$field_name}[LANGUAGE_NONE] = array($new_field_data);
              $first_value = false;
            }
            else {
              $entity->{$field_name}[LANGUAGE_NONE][] = $new_field_data;
            }
          }
          else if ($new_field_data == 'db_store') {
            $db_store = true;
          }
        }

        if ($db_store) {
          $field_values_encode = serialize($field_data);
          we_content_migration_mapping_save_reference($entity_type, $bundle, $ewrapper->getIdentifier(),
                                          $field_name, $field_values_encode);
        }

      }
    }
  }
}