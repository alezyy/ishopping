<?php

function we_content_migration_mapping_save($entity_type, $bundle, $oeid, $neid) {
  $id = db_insert('we_content_migration_mapping')
        ->fields(array(
          'old_entity_type' => $entity_type,
          'old_bundle' => $bundle,
          'old_entity_id' => $oeid,
          'new_entity_type' => $entity_type,
          'new_bundle' => $bundle,
          'new_entity_id' => $neid,
        ))
        ->execute();

  return $id;
}

function we_content_migration_mapping_save_reference($entity_type, $bundle, $eid, $field_name,
                                          $field_value, $language = LANGUAGE_NONE) {
  $id = db_insert('we_content_migration_mapping_reference')
        ->fields(array(
          'entity_type' => $entity_type,
          'bundle' => $bundle,
          'eid' => $eid,
          'field_name' => $field_name,
          'field_value' => $field_value,
          'language' => $language,
        ))
        ->execute();

  return $id;
}

function we_content_migration_mapping_load_multiple($conditions = array(), $fields = array(), $limit = NULL) {
  $query = db_select('we_content_migration_mapping', 'mapping');

  if ($fields) {
    $query->fields('mapping', $fields);
  }
  else {
    $query->fields('mapping');
  }

  foreach ($conditions as $condition) {
    // Continue with next condition if 'field' or 'value' not exist
    if (!isset($condition['field']) || !isset($condition['value']))
      continue;

    $operator = '=';
    if (!empty($condition['operator'])) {
      $operator = $condition['operator'];
    }

    $query->condition($condition['field'], $condition['value'], $operator);
  }

  if ($limit)
    $query->range(0, $limit);

  $records = $query->execute()->fetchAll();
  if ($records) {
    return $records;
  }

  return FALSE;
}

function we_content_migration_mapping_get_new_id($oeid, $entity_type = 'node', $bundle = array()) {
  $conditions = array(
    array('field' => 'old_entity_type', 'value' => $entity_type),
    array('field' => 'old_entity_id', 'value' => $oeid),
  );

  if ($bundle) {
    $conditions[] = array('field' => 'old_bundle', 'value' => $bundle, 'operator' => 'IN');
  }

  $results = we_content_migration_mapping_load_multiple($conditions);
  if ($results) {
    $result = reset($results);
    return $result->new_entity_id;
  }

  return FALSE;
}

function we_content_migration_mapping_field_reference_list($limit = NULL) {
  $query = db_select('we_content_migration_mapping_reference', 'mref');
  $query->fields('mref');

  if ($limit)
    $query->range(0, $limit);

  $records = $query->execute()->fetchAll();
  if ($records) {
    return $records;
  }

  return FALSE;
}