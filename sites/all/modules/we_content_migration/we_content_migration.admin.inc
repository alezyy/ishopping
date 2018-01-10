<?php

/**
 * @file
 * Forms for admin screens
 */

/**
 * Settings form for migrate content
 */
function we_content_migration_admin_form($form, &$form_state) {
  $path = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH);

  $list_of_import = we_content_migration_get_import_list($path);

  $header = array('name' => t('name'), 'version' => t('version'));
  $form = array();
  if(!empty($list_of_import)) {
    $form['manage'] = array(
      '#tree' => FALSE,
      '#title' => 'Manage',
      '#description' => 'Loading all export file in ' . $path .'. You can change path load export file in setting',
      '#type' => 'fieldset',
      '#group' => 'packages',
    );
    $form['manage']['table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $list_of_import,
      '#default_value' => 0,
      '#empty' => t('No content available.'),
      '#multiple' => FALSE,

    );

    $form['manage']['remove_current'] = array(
      '#title' => t('Delete all current content'),
      '#description' => t('Delete all current content of site'),
      '#type' => 'checkbox',
    );
    $form['manage']['reset_default_properties'] = array(
      '#title' => t('Reset default properties'),
      '#description' => t('All content will be reset current properties. Example : Date created ...'),
      '#type' => 'checkbox',
    );
    $form['manage']['reset_pathauto'] = array(
      '#title' => t('Reset Path Auto'),
      '#description' => t('All content will be use current pattern of pathauto'),
      '#type' => 'checkbox',
    );

    $form['manage']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Import Content'),
      '#submit' => array('we_content_migration_import_form_submit'),
      '#validate' => array('we_content_migration_form_import_validate'),
    );
  }
  else {
      $form['manage'] = array(
      '#type' => 'fieldset',
      '#title' => t('No output in directory'),
      '#description' => t('Please make sure folder "output" exsits in mirate content module'),
    );
  }
  return $form;
}


/**
 * Form constructor for content migrate export form.
 */
function we_content_migration_export_form($form, $form_state) {
  drupal_add_css(drupal_get_path('module', 'we_content_migration') . '/we_content_migration.css');
  drupal_add_js(drupal_get_path('module', 'we_content_migration') . '/we_content_migration.js');

  $form = array(
    '#attributes' => array('class' => array('migrate-content-export-form', 'clearfix')),
  );
  /*
    Advanced setting area
  */
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Options'),
    '#tree' => FALSE,
    '#weight' => 10,
    '#prefix' => "<div id='migrate-content-export-advanced'>",
    '#suffix' => '</div>',
  );

  $form['advanced']['name'] = array(
    '#title' => t('Name'),
    '#description' => t('Example: Image gallery') . ' (' . t('Do not begin name with numbers.') . ')',
    '#type' => 'textfield',
    '#size' => 30,
  );

  $form['advanced']['machine_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine-readable name'),
    '#description' => t('Example: image_gallery') . '<br/>' . t('May only contain lowercase letters, numbers and underscores. <strong>Try to avoid conflicts with the names of existing Drupal projects.</strong>'),
    '#required' => TRUE,
    '#default_value' => '',
    '#machine_name' => array(
      'exists' => 'we_content_migration_export_form_module_name_exists',
      'source' => array('advanced', 'name'),
    ),
  );

  $form['advanced']['version'] = array(
    '#title' => t('Version'),
    '#description' => t('Examples: 7.x-1.0, 7.x-1.0-beta1'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#size' => 30,
    '#element_validate' => array('we_content_migration_export_form_validate_field'),
  );

  $form['advanced']['generate'] = array(
    '#title' => t('Generate in path'),
    '#type' => 'checkbox',
    '#default_value' => TRUE,
  );

  $form['advanced']['structure_only'] = array(
    '#title' => t('Generate structue only'),
    '#type' => 'checkbox',
  );

  $form['advanced']['generate_path'] = array(
    '#title' => t('Path to Generate Migrate Content module'),
    '#description' => t('File path to save json files'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#default_value' => variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH),
    // '#field_suffix' => '<span class="field-suffix">/migrate-content</span>',
    );

  $form['advanced']['generate_content'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Content'),
    '#submit' => array('we_content_migration_form_generate_content'),
    '#validate' => array('we_content_migration_form_validate'),
  );

  $form['advanced']['generate_excel_template'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Excel'),
    '#submit' => array('we_content_migration_form_generate_excel'),
    '#validate' => array('we_content_migration_form_validate'),
    '#file' => 'includes/we_content_migration.excel.inc'
  );

  //Export area
  $node_type = node_type_get_names();
  $form['export'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity'),
    '#description' => t('Expand each entity section and select which items should be included in this content export.'),
    '#tree' => TRUE,
    '#prefix' => "<div id='migrate-content-export-wrapper'>",
    '#suffix' => '</div>',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $entity = entity_get_info();
  $export_entity = array('node', 'taxonomy_term');

  foreach ($entity as $entity_type => $entity_item) {
    if(in_array($entity_type, $export_entity)) {
      $form['export'][$entity_type]['source'] = array(
        '#type' => 'fieldset',
        '#title' => $entity_item["label"] . ' (<span class="component-count">' . count($entity_item["bundles"]) . '</span>)',
        '#tree' => TRUE,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#attributes' => array('class' => array('migrate-content-export-component')),
        '#prefix' => "<div class='migrate-content-export-parent component-$entity_type' value='$entity_type'>",
      );
      $bundles = array();
      foreach ($entity_item['bundles'] as $key => $value) {
        $bundles[$key] = $value['label'];
      }
      $form['export'][$entity_type]['source']['selected'] = array(
      '#type' => 'checkboxes',
      '#id' => "edit-sources-$entity_type",
      '#options' => $bundles,
      '#attributes' => array(
          'class' => array('component-select'),
        ),
      '#validated' => TRUE,
      );
      $form['export'][$entity_type]['added'] = array(
        '#type' => 'checkboxes',
        '#options' => array(),
        '#validated' => TRUE,
        '#attributes' => array(
            'class' => array('component-added'),
          ),
      );
      $form['export'][$entity_type]['added']['#prefix'] ="<div class='component-list migrate-content-export-list'>";
      $form['export'][$entity_type]['added']['#suffix'] = '</div></div>';
    }
  }

  // Field Area

  $field_base = field_info_fields();
  $fields = array();

  foreach ($field_base as $field_name => $field) {
    $fields[$field_name] = $field_name;
  }

  $form['export']['field_base']['source'] = array(
      '#type' => 'fieldset',
      '#title' => 'Field Base (<span class="component-count">' . count($fields) . '</span>)',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array('class' => array('migrate-content-field-component migrate-content-export-component')),
      '#prefix' => "<div class='migrate-content-field-parent migrate-content-export-parent'>",
    );

  $form['export']['field_base']['source']['selected'] = array(
    '#type' => 'checkboxes',
    '#id' => "edit-sources-field-base",
    '#options' => $fields,
    '#attributes' => array(
        'class' => array('component-select'),
      ),
    '#validated' => TRUE,
    );
  $form['export']['field_base']['added'] = array(
    '#type' => 'checkboxes',
    '#options' => array(),
    '#validated' => TRUE,
    '#attributes' => array(
        'class' => array('component-added'),
      ),
  );
  $form['export']['field_base']['added']['#prefix'] ="<div class='component-list migrate-content-export-list'>";
  $form['export']['field_base']['added']['#suffix'] = '</div></div>';

  // Field Instance
  $field_instance = field_instance_we_content_migration_export_options();
  $form['export']['field_instance']['source'] = array(
      '#type' => 'fieldset',
      '#title' => 'Field Instance (<span class="component-count">' . count($field_instance) . '</span>)',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array('class' => array('migrate-content-field-component migrate-content-export-component')),
      '#prefix' => "<div class='migrate-content-field-parent migrate-content-export-parent'>",
    );

  $form['export']['field_instance']['source']['selected'] = array(
    '#type' => 'checkboxes',
    '#id' => "edit-sources-field-instance",
    '#options' => $field_instance,
    '#attributes' => array(
        'class' => array('component-select'),
      ),
    '#validated' => TRUE,
    );
  $form['export']['field_instance']['added'] = array(
    '#type' => 'checkboxes',
    '#options' => array(),
    '#validated' => TRUE,
    '#attributes' => array(
        'class' => array('component-added'),
      ),
  );
  $form['export']['field_instance']['added']['#prefix'] ="<div class='component-list migrate-content-export-list'>";
  $form['export']['field_instance']['added']['#suffix'] = '</div></div>';

  //dependencies
  $dependencies = _we_content_migration_get_dependencies();
  $form['export']['dependencies']['source'] = array(
      '#type' => 'fieldset',
      '#title' => 'Dependencies (<span class="component-count">' . count($dependencies) . '</span>)',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array('class' => array('migrate-content-field-component migrate-content-export-component')),
      '#prefix' => "<div class='migrate-content-dependencies-parent migrate-content-export-parent'>",
    );

  $form['export']['dependencies']['source']['selected'] = array(
    '#type' => 'checkboxes',
    '#id' => "edit-sources-dependencies",
    '#options' => $dependencies,
    '#attributes' => array(
        'class' => array('component-select'),
      ),
    '#validated' => TRUE,
    );
  $form['export']['dependencies']['added'] = array(
    '#type' => 'checkboxes',
    '#options' => array(),
    '#validated' => TRUE,
    '#attributes' => array(
        'class' => array('component-added'),
      ),
  );
  $form['export']['dependencies']['added']['#prefix'] ="<div class='component-list migrate-content-export-list'>";
  $form['export']['dependencies']['added']['#suffix'] = '</div></div>';
  return $form;
}

function we_content_migration_settings_form($form, $form_state) {

  $form['general'] = array(
    '#title' => t('General settings'),
    '#type' => 'fieldset',
  );
  $form['general']['we_content_migration_default_export_path'] = array(
    '#title' => t('Default export path'),
    '#type' => 'textfield',
    '#default_value' => variable_get('we_content_migration_default_export_path'),
    '#description' => t('All content exports will be automatically saved to this path.'),
  );
  return system_settings_form($form);
}


/**
 * Submit handler for the 'export content' form
 */
function we_content_migration_form_generate_content($form, &$form_state) {
  module_load_include('inc', 'we_content_migration', 'we_content_migration.export');
  $export = _we_content_migration_export_build($form, $form_state);

  $module_name = $export['machine_name'];
  $generate = $export['generate'];

  if ($files = we_content_migration_export_render($export, $module_name)) {
    $filename = (!empty($export['version']) ? "{$module_name}-{$export['version']}" : $module_name) . '.tar';

    if ($generate) {
      $success = TRUE;
      $destination = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH);
      $directory = (!empty($export['generate_path'])) ? $export['generate_path'] . '/' . $module_name :
        $destination . '/' . $module_name;

      $content_directory = "{$directory}/contents";

      if (file_prepare_directory($directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY) === FALSE
          || file_prepare_directory($content_directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY) === FALSE) {
        $success = FALSE;
      }
    }
    else {
      // Clear out output buffer to remove any garbage from tar output.
      if (ob_get_level()) {
        ob_end_clean();
      }

      drupal_add_http_header('Content-type', 'application/x-tar');
      drupal_add_http_header('Content-Disposition', 'attachment; filename="'. $filename .'"');
      drupal_send_headers();
    }

    // Write the content
    foreach ($files as $file => $file_contents) {
      if ($generate) {
        if (file_put_contents("{$directory}/{$file}", $file_contents) === FALSE) {
          $success = FALSE;
        }
      }
      else {
        print we_content_migration_tar_create("{$module_name}/{$file}", $file_contents);
      }
    }

    if ($generate) {
      if ($success) {
        drupal_set_message(t("Module @name written to @directory",
          array('@name' => $export['name'], '@directory' => $directory)));
        }
      else {
        drupal_set_message(
          t("Could not write module to @path. ", array('@path' => $directory)) .
          t("Ensure your file permissions allow the web server to write to that directory."), "error");
      }
    }
    else {
      print pack("a1024","");
      exit;
    }
  }
}

/**
 * Submit handler for generating excel template
 */
function we_content_migration_form_generate_excel($form, &$form_state) {
  // Require libraries
  $library_path = libraries_get_path('PHPExcel');
  require_once($library_path . '/Classes/PHPExcel.php');
  module_load_include('inc', 'we_content_migration', 'we_content_migration.export');
  module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.excel');

  $export = _we_content_migration_export_build($form, $form_state);
  $module_name = $export['machine_name'];
  $generate = $export['generate'];

  // Write the file
  $success = TRUE;
  $destination = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH);
  $directory = (!empty($export['generate_path'])) ? $export['generate_path'] . '/' . $module_name :
        $destination . '/' . $module_name;

  file_unmanaged_delete_recursive($directory);

  if (file_prepare_directory($directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY) === FALSE) {
    $success = FALSE;
  }

  try {
    $excelObj = we_content_migration_form_generate_excel_file($export['entity'], $directory);
    $objWriter = PHPExcel_IOFactory::createWriter($excelObj, "Excel2007");
    $objWriter->save("{$directory}/contents.xlsx");

    // Download zip file
    $directory_real_path = drupal_realpath($directory);
    $zip_file = $directory_real_path . '.zip';
    // print_r(basename($zip_file)); exit;
    we_content_migration_zip_create($directory_real_path, $zip_file);

    drupal_add_http_header('Content-type', 'application/zip');
    drupal_add_http_header('Content-Disposition', 'attachment; filename="'. basename($zip_file) .'"');
    drupal_send_headers();

    ob_clean();
    flush();

    readfile($zip_file);
    unlink($zip_file);
    file_unmanaged_delete_recursive($directory);
    exit;
  }
  catch (Exception $e) {
    $success = FALSE;
  }

  if ($success) {
    drupal_set_message(t("Module @name written to @directory",
      array('@name' => $export['name'], '@directory' => $directory)));
  }
  else {
    drupal_set_message(
      t("Could not write module to @path. ", array('@path' => $directory)) .
      t("Ensure your file permissions allow the web server to write to that directory."), "error");
  }
}

/**
* Load file info in output
*/
function we_content_migration_get_import_list($path) {
  $list_of_import = array();

  foreach(file_scan_directory($path, '/\.info.json$/', array('recurse' => TRUE)) as $file) {
    $info_json = file_get_contents($file->uri);
    $folder_path = str_replace("/{$file->filename}", '', $file->uri);
    $list_of_import[$folder_path] = drupal_json_decode($info_json);
  }

  return $list_of_import;
}

/**
 * Implements field instance.
 */
function field_instance_we_content_migration_export_options() {
  $options = array();
  foreach (field_info_fields() as $field_name => $field) {
    foreach ($field['bundles'] as $entity_type => $bundles) {
      foreach ($bundles as $bundle) {
        $identifier = "{$entity_type}-{$bundle}-{$field_name}";
        $options[$identifier] = $identifier;
      }
    }
  }
  ksort($options);
  return $options;
}

/**
 * Implements Migrate Content Submit.
 */
function _we_content_migration_export_build($form, &$form_state) {
  $export = array();
  $form_value = $form_state['values'];
  // Export infomation
  $export['name'] = $form_value['name'];
  $export['machine_name'] = $form_value['machine_name'];
  $export['version'] = $form_value['version'];
  $export['generate'] = $form_value['generate'];
  $export['generate_path'] = $form_value['generate_path'];
  $export['structure_only'] = $form_value['structure_only'];

  // Components
  $entity_info = entity_get_info();
  $non_export_entity = array('comment', 'user', 'file', 'taxonomy_term', 'taxonomy_vocabulary');
  $site_entity = array_keys($entity_info);
  $entity = array_diff($site_entity, $non_export_entity);

  foreach ($form_value['export'] as $component => $component_info) {
    $selected = array();
    foreach ($component_info['source']['selected'] as $key => $value) {
      if($key === $value) {
        $selected[] = $key;
      }
    }

    if(!empty($selected)) {
      if (in_array($component, $entity)) {
        $export['entity'][$component] = $selected;
      }
      else {
      $export[$component] = $selected;
      }
    }
  }

  return $export;
}

/**
 * Machine name existence callback for the module name.
 * Validate machine_name
 */
function we_content_migration_export_form_module_name_exists($value) {
  return FALSE;
}

/**
 * Get dependencies of entity.
 */
function _we_content_migration_get_dependencies() {
  $cache = cache_get('we_content_migration_dependencies', 'cache');
  if(!empty($cache)) {
    return $cache->data;
  }
  else {
    $excluded = drupal_required_modules();
    $module = system_rebuild_module_data();
    $dependencies = array();
    foreach ($module as $module_name => $info) {
      if (!in_array($module_name, $excluded) && $info->status && !empty($info->info)) {
        $dependencies[$module_name] = $info->info['name'];
      }
    }
    cache_set('we_content_migration_dependencies', $dependencies, 'cache');
    return $dependencies;
  }
}

/**
 * Ajax check require bundles.
 */
function we_content_migration_export_components_json() {
  if (!empty($_POST['entity']) && !empty($_POST['bundle'])) {
    $dependencies = _we_content_migration_export_components_json($_POST['entity'], $_POST['bundle']);
  }

  print drupal_json_encode($dependencies);
}

/**
 * Sub Ajax call
 * Get all dependencies
 */
function _we_content_migration_export_components_json($entity, $bundle) {
  $dependencies = array();

  foreach (field_info_instances($entity) as $sub_bundle => $value) {
    if( $sub_bundle == $bundle ) {
      //field
      $field_base = array_keys($value);
      $field_instance = array();
      $entity_dependencies = array();
      $bundle_require = array();
      foreach ($value as $key => $field) {
        $item = $entity . '-' . $bundle . '-' . $key;
        array_push($field_instance, $item);
        $field_info = field_info_field($key);
        // Add dependencies to return array
        if(isset($field_info['module'])) {
          array_push($entity_dependencies, $field_info['module']);
        }

        // Add taxonomy term bundle to return array
        if($field_info['module'] == 'taxonomy') {
          $list_term = $field_info['settings']['allowed_values'];
          foreach ($list_term as $key => $term) {
            array_push($bundle_require, $term['vocabulary']);
          }
        }

        // Add node bundle to return array
        if($field_info['module'] == 'entityreference') {
          $list_reference = $field_info['settings'];
          foreach ($list_reference['handler_settings']['target_bundles'] as $key => $reference) {
            if ($reference !== $bundle) {
              array_push($bundle_require, $reference);
              $bundles_target = _we_content_migration_export_components_json($list_reference['target_type'], $reference);
              $bundle_require = array_merge($bundle_require, $bundles_target['field']);
            }
          }
        }
      }
      $dependencies['field'] = array_merge($field_base, $field_instance, $entity_dependencies, $bundle_require);
      array_unique($dependencies['field']);
    }
  }

  return $dependencies;
}

/**
 * Render API callback: Validates a project field.
 *
 * This function is assigned as an #element_validate callback in
 * we_content_migration_export_form().
 */
function we_content_migration_export_form_validate_field($element, &$form_state) {
  switch ($element['#name']) {
    case 'version':
      preg_match('/^(?P<core>\d+\.x)-(?P<major>\d+)\.(?P<patch>\d+)-?(?P<extra>\w+)?$/', $element['#value'], $matches);

      if (!empty($element['#value']) && !isset($matches['core'], $matches['major'])) {
        form_error($element, t('Please enter a valid version with core and major version number. Example: @example', array('@example' => '7.x-1.0')));
      };
      break;
  }
}

/**
 * Validate handler for the 'export' form.
 */
function we_content_migration_form_validate($form, &$form_state) {
  $current_state = _we_content_migration_export_build($form, $form_state);
  if (sizeof($current_state) == 6) {
    form_set_error('status', t('Please select entity to export'));
  }
}


/**
 * Submit handler for the 'export content' form
 */

function we_content_migration_import_form_submit($form, &$form_state) {
  module_load_include('inc', 'we_content_migration', 'we_content_migration.import');

  $path = $form_state['values']['table'];
  we_content_migration_import_load($path);
  drupal_set_message('Imported');
}

/**
 * Validate handler for the 'manage features' form.
 */
function we_content_migration_form_import_validate($form, &$form_state) {
  if (empty($form_state['values']['table'])) {
    form_set_error('status',t("Please select folder to import."));
  }
}

/**
 * Create form page to import content form upload file

 * @return array $form
 */
function we_content_migration_import_from_upload_form($form, &$form_state) {
  $form['import_container'] = array(
    '#title' => 'Import from zip file',
    '#description' => 'Upload your zip file to import content',
    '#type' => 'fieldset',
  );

  $form['import_container']['file'] = array(
    '#type' => 'file',
    '#title' => t("Import file"),
  );

  $form['actions'] = array(
    '#type' => 'container',
  );

  $form['actions']['import'] = array(
    '#type' => 'submit',
    '#value' => t("Import"),
    '#submit' => array('we_content_migration_import_from_upload_form_submit'),
  );

  return $form;
}

function we_content_migration_import_from_upload_form_submit($form, &$form_state) {
  module_load_include('inc', 'we_content_migration', 'we_content_migration.import');

  $destination = variable_get('we_content_migration_default_export_path', WE_CONTENT_MIGRATION_DEFAULT_EXPORT_PATH);
  $directory = $destination . '/import';

  if(isset($_FILES['files']['name']['file'])) {
    $file = $_FILES['files'];
    $file_name = $file['name']['file'];
    $file_type = $file['type']['file'];
    $file_info = pathinfo($file_name);

    // Remove existed folder
    if (file_unmanaged_delete_recursive($directory) &&
        file_prepare_directory($directory, FILE_MODIFY_PERMISSIONS | FILE_CREATE_DIRECTORY) !== FALSE) {
      if ($file_info['extension'] == 'zip') {
        $zip = new ZipArchive;
        if ($zip->open($file['tmp_name']['file']) === TRUE) {
          $zip->extractTo($directory);
          $zip->close();

          // Check type of zip
          if (glob($directory . '/*.json')) {
            we_content_migration_import_load($directory);
          }
          else if (glob($directory . '/contents.xlsx')) {
            we_content_migration_import_excel($directory, 'xlsx');
          }
          else if (glob($directory . '/*.xls')) {
            we_content_migration_import_excel($directory, 'xls');
          }

        } else {
          drupal_set_message(
            t("Could not import the file") .
            t("Ensure your file permissions allow the web server to write to @path directory.", array('@path' => $directory)), "error");
        }
      }
    }
  }
}
