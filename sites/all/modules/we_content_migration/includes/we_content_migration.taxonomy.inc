<?php

/**
 * Handle field base export to get neccessary information.
 */
function taxonomy_we_content_migration_export($taxonomy) {
  $output = array();
  $vocabularies = taxonomy_get_vocabularies();

  foreach ($taxonomy as $machine_name) {
    foreach ($vocabularies as $vocabulary) {
      if ($vocabulary->machine_name == $machine_name) {
        // We don't want to break the entity cache, so we need to clone the
        // vocabulary before unsetting the id.
        $vocabulary = clone $vocabulary;
        unset($vocabulary->vid);
        $output[$machine_name] = $vocabulary;
      }
    }
  }

  return $output;
}

/**
 *
 * Import Taxonomy vocabularies from code defaults.
 */
function _we_content_migration_import_taxonomy($path, $import_name) {
  $file_path = "{$path}/$import_name.taxonomy.json";
  if(file_exists($file_path)) {
    $vocabularies = file_get_contents($file_path);
    $vocabularies = drupal_json_decode($vocabularies);
    if ($vocabularies) {
      $existing = taxonomy_get_vocabularies();
      foreach ($vocabularies as $vocabulary) {
        $vocabulary = (object) $vocabulary;
        foreach ($existing as $existing_vocab) {
          if ($existing_vocab->machine_name === $vocabulary->machine_name) {
            $vocabulary->vid = $existing_vocab->vid;
          }
        }
        taxonomy_vocabulary_save($vocabulary);
      }
    }
  }
}