<?php
/**
 * @file uc_field_attribute.inc
 * Node functions for uc_field_attribute
 */

/**
 * Sync attributes between attributes and field attributes
 *
 * @param Node $node
 */
function _uc_field_attribute_sync($node) {
  if (!array_key_exists($node->type, uc_product_class_load())) {
    return;
  }
  $attribute_settings = (object) config_get('uc_field_attribute.settings', "uc_field_attribute_{$node->type}");
  if (!$attribute_settings) {
    return;
  }
  foreach ($attribute_settings->fields as $fiid => $enabled) {
    if (!$enabled) {
      continue;
    }
    $bundle = field_extract_bundle('node', $node);
    $instances = field_info_instances('node', $bundle);
    $found = false;
    foreach ($instances as $field_name => $instance) {
      if ($field_name == $fiid) {
        $found = true;
        break;
      }
    }
    if (!$found) {
      continue; // @todo warning?
    }
    $instance = (object) $instance;
    $attribute = uc_field_attribute_load($node->nid, $instance->field_name);
    if (!$attribute) {
      $attribute = $instance;
      $attribute->display = 1;
      $attribute->name = $instance->field_name;
      $attribute->options = array();
      uc_attribute_save($attribute);
      $attribute = (object) $attribute;
      $field_attribute = array(
        'aid' => $attribute->aid,
        'nid' => $node->nid,
        'field_instance_id' => $instance->field_name,
      );
      backdrop_write_record('uc_field_attribute_node', $field_attribute);
    }
    $product_attribute = (object) $attribute;
    $attributes = uc_product_get_attributes($node->nid);
    // UPDATE or INSERT
    $exists = isset($attributes[$product_attribute->aid]);
    $product_attribute->nid = $node->nid;
    // @todo no API for this?
    if (!$exists) {
      backdrop_write_record('uc_product_attributes', $product_attribute);
      $product_attribute = (object) $product_attribute;
    }

    $items = field_get_items('node', $node, $field_name);

    foreach ($items as $index => $item) {
      // This returns markup in an array.
      // Could probably make the $display configurable.
      $value = field_view_value('node', $node, $field_name, $item);
      switch (TRUE) {
        case isset($value['#title']):
          // Taxonomy title
          $value = strip_tags($value['#title']);
          break;
        case isset($value['#label']):
          // Entity Reference label
          $value = strip_tags($value['#label']);
          break;
        case isset($value['#markup']):
          $value = strip_tags($value['#markup']);
          break;
      }
      $product_attribute_oid = 0;
      foreach ($product_attribute->options as $oid => $option) {
        if ($option->name == $value) {
          $product_attribute_oid = $oid;
          unset($product_attribute->options[$oid]);
          break;
        }
      }
      if (!$product_attribute_oid) {
        // @todo could this already exist - should probably check through the
        // table first
        $option = array(
          'aid' => $product_attribute->aid,
          'name' => $value,
          'ordering' => $index,
        );
        $option = (object) $option;
        uc_attribute_option_save($option);
        $option = (object) $option;
        $option->nid = $node->nid;
        backdrop_write_record('uc_product_options', $option);
        backdrop_write_record('uc_field_attribute_options', $option);
      }
    }
    // At this point $product_attribute->options holds all options that have
    // "disappeared" from the Field API field.
    foreach ($product_attribute->options as $oid => $option) {
      db_delete('uc_product_options')
        ->condition('oid', $oid)
        ->condition('nid', $node->nid)
        ->execute();
      db_delete('uc_attribute_options')
        ->condition('oid', $oid)
        ->execute();
      db_delete('uc_field_attribute_options')
        ->condition('aid', $product_attribute->aid)
        ->condition('oid', $oid)
        ->execute();
    }
  }
}

/**
 * Delete attributes when a node is deleted.
 *
 * @param Node $node
 */
function _uc_field_attribute_node_delete($node) {
  $result = db_select('uc_field_attribute_node', 'n')
    ->fields('n', array('aid'))
    ->condition('n.nid', $node->nid)
    ->execute();
  foreach ($result as $row) {
    db_delete('uc_attribute_options')
      ->condition('aid', $row->aid)
      ->execute();
    db_delete('uc_field_attribute_options')
      ->condition('aid', $row->aid)
      ->execute();
    db_delete('uc_attributes')
      ->condition('aid', $row->aid)
      ->execute();
  }
  db_delete('uc_field_attribute_node')
    ->condition('nid', $node->nid)
    ->execute();
}
/**
 * Options load
 *
 * @param int $aid
 *
 * @return array
 */
function uc_field_attribute_options_load($aid) {
  $options = array();

  $result = db_select('uc_field_attribute_options', 'o')
    ->condition('o.aid', $aid)
    ->execute();
  foreach ($result as $option) {
    $options[$option->oid] = $option;
  }
  return $options;

}
/**
 * Save attribute
 *
 * @param object $attribute
 *
 * @return bool
 */
function uc_field_attribute_save($attribute) {
  // Insert or update?
  $key = empty($attribute->aid) ? array() : 'aid';
  return backdrop_write_record('uc_field_attribute_node', $attribute, $key);
}

/**
 * Load the field attribute
 *
 * @param int $nid
 * @param int $fiid
 *   Field instance
 */
function uc_field_attribute_load($nid, $fiid) {
  $aid = db_select('uc_field_attribute_node', 'n')
    ->fields('n', array('aid'))
    ->condition('n.nid', $nid)
    ->condition('field_instance_id', $fiid)
    ->execute()
    ->fetchField();
  if (!$aid) {
    return false;
  }
  return uc_attribute_load($aid);
}
