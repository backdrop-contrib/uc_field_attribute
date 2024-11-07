<?php

/**
 * @file uc_field_attributes_for
 * Field Attribute administration menu items.
 */

/**
 * Form to associate field attributes with classes.
 *
 * @ingroup forms
 */
function uc_field_attribute_form($form, &$form_state, $object, $type, $view = 'overview') {
  $class = $object;
  $bundle = $class->pcid;
  if (empty($class->name)) {
    backdrop_goto('admin/store/products/classes/' . $bundle);
  }
  backdrop_set_title($class->name);

  $instances = field_info_instances('node', $class->pcid);

  $header = array(
    'label' => t('Field name'),
    'operations' => t('Operations'),
  );

  $form = array(
    array('#markup' => t('<p>Select the fields to make available as product attributes. Not all fields work well as attributes. Typically the field should allow for multiple values.</p>'))
  );

  $form['#tree'] = TRUE;
  $options = array();
  foreach ($instances as $field_name => $instance) {
    $field_name = $instance['field_name'];
    $links = array(
      'edit' => array(
        'title' => t('Edit'),
        'href' => 'admin/structure/types/manage/' . str_replace('_', '-', $bundle) . '/fields/' . $field_name,
      ),
    );
    $options[$field_name] = array(
      'label' => check_plain($instance['label']),
      'operations' => array(
        'data' => array(
          '#type' => 'operations',
          '#links' => $links,
        ),
      ),
    );

  }
  $attribute_settings = (object) config_get('uc_field_attribute.settings', "uc_field_attribute_{$class->pcid}");
  $form['fields'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('There are no fields available for this content type. Add fields to the content type first and then enable them here.'),
    '#default_value' => (isset($attribute_settings->fields) ? (array) $attribute_settings->fields : array()),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}


/**
 * Form submission handler for uc_object_attributes_form().
 *
 * @see uc_object_attributes_form()
 */
function uc_field_attribute_form_submit($form, &$form_state) {
  $settings = array(
    'fields' =>  $form_state['values']['fields'],
  );
  $bundle_name = arg(4);
  config_set('uc_field_attribute.settings', "uc_field_attribute_$bundle_name", (array) $settings);
  backdrop_set_message(t('The changes have been saved.'));
}
