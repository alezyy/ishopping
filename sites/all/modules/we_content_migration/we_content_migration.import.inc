<?php

/**
 * Import structure and content
 *
 * @param string $path
 *  The name of the module to import.
 *
 *@return void
 */
function we_content_migration_import_load($path) {
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.entity_generator');
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.field_generator');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.field');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.taxonomy');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.mapping');

  $import_name = basename($path);

  _we_content_migration_import_entity($path, $import_name);
  _we_content_migration_import_taxonomy($path, $import_name);
  _we_content_migration_import_field_base($path, $import_name);
  _we_content_migration_import_field_instance($path, $import_name);

  we_content_migration_import_content($path, $import_name);
  we_content_migration_import_field_reference();
}

function _we_content_migration_import_entity($path, $import_name) {
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.node');
  $file_path = "{$path}/$import_name.entity.json";

  if(file_exists($file_path)) {
    $entities = file_get_contents($file_path);
    $entities = drupal_json_decode($entities);

    foreach ($entities as $entity => $entity_data) {
      if ($entity == 'node') {
        node_we_content_migration_import_bundle($entity_data['bundles']);
      }
    }
  }
}

function we_content_migration_import_content($path, $import_name) {
  $content_path = "{$path}/contents";

  foreach(file_scan_directory($content_path, '/\.json$/', array('recurse' => TRUE)) as $file) {
    list($entity_type, $bundle) = explode('-', $file->name);

    $entity_info = entity_get_info($entity_type);
    if ($entity_info && isset($bundle)) {
      $file_json_contents = file_get_contents($file->uri);
      $file_contents = drupal_json_decode($file_json_contents);
      foreach ($file_contents as $data) {
        // Remove entity indentifier
        $id_name = isset($entity_info['entity keys']['id']) ? $entity_info['entity keys']['id'] : NULL;
        $oeid = $data[$id_name];
        if (isset($data[$id_name]))
          unset($data[$id_name]);

        $revision_id_name = isset($entity_info['entity keys']['revision']) ? $entity_info['entity keys']['revision'] : NULL;
        if (isset($data[$revision_id_name]))
          unset($data[$revision_id_name]);

        $neid = we_content_migration_generate_entity($data, $entity_type, $bundle);
        if ($neid) {
          // Should save new record to DB
          we_content_migration_mapping_save($entity_type, $bundle, $oeid, $neid);
        }
      }
    }
  }
}

/**
 * Map all reference fields
 *
 * @return void
 */
function we_content_migration_import_field_reference() {
  $ref_field_list = we_content_migration_mapping_field_reference_list();
  foreach ($ref_field_list as $field_data) {
    $field_info = field_info_field($field_data->field_name);
    $table_name = "field_data_{$field_data->field_name}";
    $table_field_load = $field_info['storage']['details']['sql']['FIELD_LOAD_CURRENT'];
    $table_field_name = reset($table_field_load[$table_name]);

    $field_values = unserialize($field_data->field_value);
    foreach ($field_values as $delta => $field_value) {
      $ref_oid = $field_value['value'];
      switch ($field_info['module']) {
        case 'taxonomy':
          $ref_entity_type = 'taxonomy_term';
          $ref_bundle = array($field_info['settings']['allowed_values'][0]['vocabulary']);
          break;
        default:
          $ref_entity_type = $field_info['settings']['target_type'];
          $ref_bundle = array_keys($field_info['settings']['handler_settings']['target_bundles']);
          break;
      }

      $ref_nid = we_content_migration_mapping_get_new_id($ref_oid, $ref_entity_type, $ref_bundle);

      if ($ref_nid) {
        $field_data_id = db_insert($table_name)->fields(array(
          'entity_type' => $field_data->entity_type,
          'bundle' => $field_data->bundle,
          'deleted' => 0,
          'entity_id' => $field_data->eid,
          'revision_id' => 0,
          'language' => $field_data->language,
          'delta' => $delta,
          $table_field_name => $ref_nid,
        ))->execute();
      }

    }
  }
}

function we_content_migration_import_excel($directory, $type = 'xlsx') {
  $library_path = libraries_get_path('PHPExcel');
  require_once($library_path . '/Classes/PHPExcel.php');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.excel');
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.entity_generator');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.mapping');

  db_truncate('we_content_migration_mapping')->execute();
  db_truncate('we_content_migration_mapping_reference')->execute();

  $file_path = $directory . '/contents.' . $type;
  $inputFileName = drupal_realpath($file_path);
  $inputFileType = PHPExcel_IOFactory::identify($inputFileName);
  $objReader = PHPExcel_IOFactory::createReader($inputFileType);
  $objPHPExcel = $objReader->load($inputFileName);

  we_content_migration_import_excel_prepare($objPHPExcel);

  $number_of_sheet = $objPHPExcel->getSheetCount();
  $sheet_names = $objPHPExcel->getSheetNames();

  try {
    for ($i = 0; $i < $number_of_sheet; $i++) {
      $sheet = $objPHPExcel->setActiveSheetIndex($i);

      $entity_type = $sheet->getCell('C2')->getValue();
      $bundle = $sheet->getCell('C3')->getValue();
      $entity_info = entity_get_info($entity_type);
      $id_name = isset($entity_info['entity keys']['id']) ? $entity_info['entity keys']['id'] : NULL;

      $highest_row = $sheet->getHighestRow();
      $highest_column = $sheet->getHighestColumn();

      $field_map = we_content_migration_import_excel_get_field_map($sheet, $highest_row, $highest_column);

      $import_entities = we_content_migration_import_excel_get_data($sheet, $highest_row, $highest_column, $field_map);

      foreach ($import_entities as $old_entity_id => $data) {
        $o_entity_id = NULL;
        if ($data['entity_id']) {
          $o_entity_id = $data['entity_id'];
          $query = new EntityFieldQuery();
          $query->entityCondition('entity_type', $entity_type)
            ->entityCondition('bundle', $bundle)
            ->propertyCondition('title', $data['title'])
            ->propertyCondition($id_name, $old_entity_id)
            ->range(0, 1);

          $result = $query->execute();
          if (isset($result[$entity_type])) {
            $new_entity_id = we_content_migration_generate_entity($data, $entity_type, $bundle, $old_entity_id, 'EXCEL');
          }
          else {
            $new_entity_id = we_content_migration_generate_entity($data, $entity_type, $bundle, NULL, 'EXCEL');
          }

        } else {
          $new_entity_id = we_content_migration_generate_entity($data, $entity_type, $bundle, NULL, 'EXCEL');
        }
        if ($new_entity_id) {
          // Should save new record to DB
          we_content_migration_mapping_save($entity_type, $bundle, $o_entity_id, $new_entity_id);
        }
      }
    }  
  } catch (Exception $e) {
    echo $e->getMessage();
    exit;
  }



  
}

function we_content_migration_import_excel_prepare($objPHPExcel) {
  $sheet_names = $objPHPExcel->getSheetNames();
  // Remove empty sheet
  if (in_array('Worksheet', $sheet_names)) {
    $objPHPExcel->setActiveSheetIndexByName('Worksheet');
    $sheet_index = $objPHPExcel->getActiveSheetIndex('Worksheet');
    $objPHPExcel->removeSheetByIndex($sheet_index);
  }
}

function we_content_migration_import_excel_get_field_map($sheet, $highest_row, $highest_column) {
  $range = "C4:{$highest_column}4";
  $row_data = $sheet->rangeToArray($range, NULL, TRUE, TRUE, TRUE);

  return $row_data[4];
}

function we_content_migration_import_excel_get_data($sheet, $highest_row, $highest_column, $field_map) {
  $entity_id_column = 'A';
  $title_column = 'B';

  $data = array();
  $range = "A6:{$highest_column}{$highest_row}";
  $sheet_data = $sheet->rangeToArray($range, NULL, TRUE, TRUE, TRUE);

  foreach ($sheet_data as $row => $row_data) {
    // Set entity id and title
    $entity_id = empty($row_data['A']) ? $row_data['B'] : $row_data['A'];

    if (!isset($data[$entity_id])) {
      $data[$entity_id] = array('title' => $row_data['B']);
    }

    if ($entity_id == $row_data['A']) {
      $data[$entity_id]['entity_id'] = $entity_id;
    } else {
      $data[$entity_id]['entity_id'] = NULL;
    }

    unset($row_data['A']);
    unset($row_data['B']);

    // Get data
    foreach ($row_data as $column => $value) {
      $field_name = $field_map[$column];
      if ($value) {
        $data[$entity_id][$field_name][] = $value;
      }
      else if (empty($data[$entity_id][$field_name])) {
        $data[$entity_id][$field_name] = array();
      }
    }
  }

  return $data;
}