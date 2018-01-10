<?php
/**
 * @file
 * All funtions handle excel export
 */

/**
 * Create a PHPExcel object template
 *
 * @param array $data
 * @return PHPExcel $excelObj
 */
function we_content_migration_form_generate_excel_file($entities, $directory = '') {
  $excelObj = new PHPExcel();

  foreach ($entities as $entity_type => $bundles) {
    foreach($bundles as $bundle) {
      $entity_info = entity_get_info($entity_type);
      $field_instances = field_info_instances($entity_type, $bundle);
      ksort($field_instances);

      $work_sheet_name = "{$entity_info['label']} - {$entity_info['bundles'][$bundle]['label']}";
      $work_sheet = new PHPExcel_Worksheet($excelObj, $work_sheet_name);

      // Set requirement header
      $data = array(
        'entity_type' => $entity_type,
        'bundle' => $bundle,
        'field_instances' => $field_instances
      );
      $field_map = we_content_migration_form_generate_excel_header($work_sheet, $data);

      // Write data
      $data['field_map'] = $field_map;
      we_content_migration_form_generate_excel_content($work_sheet, $data, $directory);

      $excelObj->addSheet($work_sheet);
    }
  }

  // Remove default sheet
  $excelObj->setActiveSheetIndexByName('Worksheet');
  $sheet_index = $excelObj->getActiveSheetIndex('Worksheet');
  $excelObj->removeSheetByIndex($sheet_index);

  return $excelObj;
}

/**
 * Insert export information
 *
 * @param PHPExcel_Worksheet $work_sheet
 * @param string $entity_type
 * @param string $bundle
 * @param array $field_instances
 * @return array $field_map
 */
function we_content_migration_form_generate_excel_header($work_sheet, $data) {
  $entity_type = $data['entity_type'];
  $bundle = $data['bundle'];
  $field_instances = $data['field_instances'];
  $field_map = array();

  // Set information
  $work_sheet->setCellValue('B1', t("Please don't change the information inside green area"));
  $work_sheet->setCellValue('B2', "entity");
  $work_sheet->setCellValue('C2', $entity_type);
  $work_sheet->setCellValue('B3', "bundle");
  $work_sheet->setCellValue('C3', $bundle);
  $work_sheet->setCellValue('B4', "Fields");
  $work_sheet->setCellValue('B5', "Title");
  $work_sheet->setCellValue("A5", 'entity_id');

  $start_col = 2;
  $row = 4;
  $field_number = 0;


  foreach ($field_instances as $field_name => $field_instance) {
    $current_col = $start_col + $field_number;

    // Set field machine name
    $work_sheet->setCellValueByColumnAndRow($current_col, $row, $field_name);

    // Set field name
    $work_sheet->setCellValueByColumnAndRow($current_col, $row + 1, $field_instance['label']);

    $field_number++;

    $field_map[$field_name] = $current_col;
  }

  return $field_map;
}

/**
 * Insert entity data
 *
 * @param PHPExcel_Worksheet $work_sheet
 * @param string $entity_type
 * @param string $bundle
 * @param array $field_instances
 * @return void
 */
function we_content_migration_form_generate_excel_content($work_sheet, $data, $directory) {
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.field_export.excel');

  $entity_type = $data['entity_type'];
  $bundle = $data['bundle'];
  $field_instances = $data['field_instances'];
  $field_map = $data['field_map'];

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type)
        ->entityCondition('bundle', $bundle);
  $result = $query->execute();

  if (isset($result[$entity_type])) {
    $eids = array_keys($result[$entity_type]);

    $start_row = 6;
    foreach ($eids as $eid) {
      $entity = entity_load_single($entity_type, $eid);
      $ewrapper = entity_metadata_wrapper($entity_type, $entity);

      $entity = (array) $entity;
      $entity_export_data = array(
        'entity_id' => $eid,
        'title' => $ewrapper->label()
      );
      $max_row = 0;

      foreach ($field_instances as $field_name => $field_instance) {
        $field_info = field_info_field($field_name);
        $field_module = $field_info['module'];
        $field_type = $field_info['type'];

        $handle_func = 'we_content_migration_excel_field_export_' . $field_module;
        $options = array('directory' => $directory);
        if (function_exists($handle_func))
          $field_data = $handle_func($entity[$field_name], $field_type, $field_module, $options);
        else
          $field_data = we_content_migration_excel_field_export($entity[$field_name], $field_type, $field_module, $options);

        if (count($field_data) > $max_row)
          $max_row = count($field_data);

        $entity_export_data[$field_name] = $field_data;
      }

      for ($i = 0; $i < $max_row; $i++) {
        $current_row = $start_row + $i;

        // Write entity id, title
        $work_sheet->setCellValueByColumnAndRow(0, $current_row, $entity_export_data['entity_id']);
        $work_sheet->setCellValueByColumnAndRow(1, $current_row, $entity_export_data['title']);

        foreach ($field_instances as $field_name => $field_instance) {
          if (isset($entity_export_data[$field_name][$i])) {
            $work_sheet->setCellValueByColumnAndRow(
              $field_map[$field_name],
              $current_row,
              $entity_export_data[$field_name][$i]
            );
          }
        }
      }

      $start_row += $max_row;
    }
  }
}