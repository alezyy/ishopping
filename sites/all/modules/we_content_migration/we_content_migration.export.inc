   <?php

/**
 * Render export into an array representing its files.
 *
 * @param $export
 *  An exported definition.
 * @param $module_name
 *  The name of the module to be exported.
 *
 * @return array of info file and module file contents.
 */
function we_content_migration_export_render($export, $options = NULL) {
  $content = array();

  $machine_name = isset($export['machine_name']) ? $export['machine_name'] : time();

  $export_default_files = array(
    "info" => "{$machine_name}.info.json",
    "entity" => "{$machine_name}.entity.json",
    "field_base" => "{$machine_name}.field_base.json",
    "field_instance" => "{$machine_name}.field_instance.json",
    "taxonomy" => "{$machine_name}.taxonomy.json"
  );

  foreach ($export_default_files as $type => $file_name) {
    $func = '_we_content_migration_export_' . $type;
    $content[$file_name] = $func($export);
  }

  $content_data = _we_content_migration_export_content($export, $options);
  $content += $content_data;

  return $content;
}

/**
 * Get json output for version file
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_info($export) {
  $output = array(
    "name" => isset($export['name']) ? $export['name'] : time(),
    "version" => isset($export['version']) ? $export['version'] : time(),
  );

  return we_content_migration_json_encode($output);
}

/**
 * Get json output for all site entities
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_entity($export) {
  $output = array();
  $entity = $export['entity'];

  foreach ($entity as $type => $bundles) {
    $default_entity = array('node', 'user', 'comment');
    $include_name = in_array($type, $default_entity) ? $type : 'another'; // Will change 'another' later
    $inc = module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.' . $include_name);
    if ($inc) {
      // Load entity type and its properties, exclude some default entities
      if (!in_array($type, $default_entity)) {
        // @TO-DO Should have a function to handle this, just leave empty now
      }

      // Get all bundles config of the entity
      $render_func = $type . '_we_content_migration_export_bundle';
      $output[$type]['bundles'] = $render_func($bundles);
    }
  }

  return we_content_migration_json_encode($output);
}

/**
 * Get json output for field base
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_field_base($export) {
  $output = array();
  $field_base = $export['field_base'];

  $inc = module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.field');
  if ($inc) {
    $output = field_base_we_content_migration_export($field_base);
  }

  return we_content_migration_json_encode($output);
}

/**
 * Get json output for field instance
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_field_instance($export) {
  $output = array();
  $field_instance = $export['field_instance'];

  $inc = module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.field');
  if ($inc) {
    $output = field_instance_we_content_migration_export($field_instance);
  }

  return we_content_migration_json_encode($output);
}

/**
 * Get json output for all site vocabularies
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_taxonomy($export) {
  $output = array();
  if (isset($export['taxonomy_term'])) {
    $taxonomy = $export['taxonomy_term'];

    $inc = module_load_include('inc', 'we_content_migration', 'includes/we_content_migration.taxonomy');
    if ($inc) {
      $output = taxonomy_we_content_migration_export($taxonomy);
    }
    return we_content_migration_json_encode($output);
  }

  return '';
}

/**
 * Get json output for all site content
 *
 * @param $export
 *  An exported definition.
 *
 * @return json string.
 */
function _we_content_migration_export_content($export, $options = NULL) {
  // Load the export field file
  module_load_include('inc', 'we_content_migration', 'generator/we_content_migration.field_export');
  $output = array();

  $entity_data = $export['entity'];

  if (isset($export['taxonomy_term']))
    $entity_data['taxonomy_term'] = $export['taxonomy_term'];

  foreach ($entity_data as $entity_type => $bundles) {
    foreach ($bundles as $bundle) {
      // Get all entity id
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $entity_type)
            ->entityCondition('bundle', $bundle);
      $result = $query->execute();

      if (isset($result[$entity_type])) {
        $eids = array_keys($result[$entity_type]);
        $ewrapper = entity_metadata_wrapper($entity_type, NULL, array('bundle' => $bundle));
        $property_info = $ewrapper->getPropertyInfo();
        $field_instance = field_info_instances($entity_type, $bundle);
        $export_data = array();

        foreach ($eids as $eid) {
          $entity = entity_load_single($entity_type, $eid);
          $export_data[$eid] = _we_content_migration_export_entity_data($entity, $property_info, $field_instance, $options);
        }

        $export_file_name = "contents/{$entity_type}-{$bundle}.json";
        $output[$export_file_name] = we_content_migration_json_encode($export_data);
      }
    }
  }

  return $output;
}

function _we_content_migration_export_entity_data($entity, $property_info, $field_instance, $options = NULL) {
  $output = array();
  // Convert entity object to array
  $entity = (array) $entity;

  // Remove rdf mapping
  unset($entity['rdf_mapping']);

  // Just get all properties first
  $output = array_diff_key($entity, $field_instance);

  if (!empty($field_instance)) {
    foreach ($field_instance as $field_name => $instance_info) {
      $field_info = field_info_field($field_name);
      $field_module = $field_info['module'];
      $field_type = $field_info['type'];

      $handle_func = 'we_content_migration_field_export_' . $field_module;
      if (function_exists($handle_func))
        $field_data = $handle_func($entity[$field_name], $field_type, $field_module, $options);
      else
        $field_data = we_content_migration_field_export($entity[$field_name], $field_type, $field_module, $options);

      $output[$field_name] = $field_data;
    }
  }

  return $output;
}

/**
 * Override default json encode off drupal to add pretty print option.
 *
 * @param $var
 *  Value being encoded
 * @param $pretty
 *  Encode the $var with pretty print or not
 *
 * @return json string
 */
function we_content_migration_json_encode($var, $pretty = true) {
  // The PHP version cannot change within a request.
  static $php530, $php540;

  if (!isset($php530)) {
    $php530 = version_compare(PHP_VERSION, '5.3.0', '>=');
  }

  if (!isset($php540)) {
    $php540 = version_compare(PHP_VERSION, '5.4.0', '>=');
  }

  if ($php540 && $pretty) {
    // Encode <, >, ', &, and " using the json_encode() options parameter.
    // Print json with pretty print
    return json_encode($var, JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_AMP | JSON_HEX_QUOT | JSON_PRETTY_PRINT);
  }

  if ($php530) {
    // Encode <, >, ', &, and " using the json_encode() options parameter.
    return json_encode($var, JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_AMP | JSON_HEX_QUOT);
  }

  // json_encode() escapes <, >, ', &, and " using its options parameter, but
  // does not support this parameter prior to PHP 5.3.0.  Use a helper instead.
  include_once DRUPAL_ROOT . '/includes/json-encode.inc';
  return drupal_json_encode_helper($var);
}

/**
 * Tar creation function. Written by dmitrig01.
 *
 * @param $name
 *   Filename of the file to be tarred.
 * @param $contents
 *   String contents of the file.
 *
 * @return
 *   A string of the tar file contents.
 */
function we_content_migration_tar_create($name, $contents) {
  /* http://www.mkssoftware.com/docs/man4/tar.4.asp */
  /* http://www.phpclasses.org/browse/file/21200.html */
  $tar = '';
  $bigheader = $header = '';
  if (strlen($name) > 100) {
    $bigheader = pack("a100a8a8a8a12a12a8a1a100a6a2a32a32a8a8a155a12",
        '././@LongLink', '0000000', '0000000', '0000000',
        sprintf("%011o", strlen($name)), '00000000000',
        '        ', 'L', '', 'ustar ', '0',
        '', '', '', '', '', '');

    $bigheader .= str_pad($name, floor((strlen($name) + 512 - 1) / 512) * 512, "\0");

    $checksum = 0;
    for ($i = 0; $i < 512; $i++) {
      $checksum += ord(substr($bigheader, $i, 1));
    }
    $bigheader = substr_replace($bigheader, sprintf("%06o", $checksum)."\0 ", 148, 8);
  }
 $header = pack("a100a8a8a8a12a12a8a1a100a6a2a32a32a8a8a155a12", // book the memorie area
    substr($name,0,100),  //  0     100     File name
    '100644 ',            // File permissions
    '   765 ',            // UID,
    '   765 ',            // GID,
    sprintf("%11s ", decoct(strlen($contents))), // Filesize,
    sprintf("%11s", decoct(REQUEST_TIME)),       // Creation time
    '        ',        // 148     8         Check sum for header block
    '',                // 156     1         Link indicator / ustar Type flag
    '',                // 157     100     Name of linked file
    'ustar ',          // 257     6         USTAR indicator "ustar"
    ' ',               // 263     2         USTAR version "00"
    '',                // 265     32         Owner user name
    '',                // 297     32         Owner group name
    '',                // 329     8         Device major number
    '',                // 337     8         Device minor number
    '',                // 345     155     Filename prefix
    '');               // 500     12         ??

  $checksum = 0;
  for ($i = 0; $i < 512; $i++) {
    $checksum += ord(substr($header, $i, 1));
  }
  $header = substr_replace($header, sprintf("%06o", $checksum)."\0 ", 148, 8);
  $tar = $bigheader.$header;

  $buffer = str_split($contents, 512);
  foreach ($buffer as $item) {
    $tar .= pack("a512", $item);
  }
  return $tar;
}

function we_content_migration_zip_create($source_path, $out_zip_path) {
  $zip = new ZipArchive();
  $zip->open($out_zip_path, ZipArchive::CREATE | ZipArchive::OVERWRITE);

  $files = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator($source_path),
    RecursiveIteratorIterator::LEAVES_ONLY
  );

  foreach ($files as $name => $file) {
    if (!$file->isDir()) {
      // Get real and relative path for current file
      $filePath = $file->getRealPath();
      $relativePath = substr($filePath, strlen($source_path) + 1);

      // Add current file to archive
      $zip->addFile($filePath, $relativePath);
    }
  }

  $zip->close();
}