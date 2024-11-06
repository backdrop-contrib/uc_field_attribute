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
  $id = $class->pcid;
  if (empty($class->name)) {
    backdrop_goto('admin/store/products/classes/' . $id);
  }
  backdrop_set_title($class->name);

  $instances = field_info_instances('node', $class->pcid);

  $header = array(
    "label" => t("Field name"),
  );

  $form = array(
    array('#markup' => t("Select the fields to make available as product attributes."))
  );

  $form['#tree'] = TRUE;
  $options = array();
  foreach ($instances as $field_name => $instance) {
    $options[$field_name] = array(
      "label" => check_plain($instance['label']),
    );

  }
  $attribute_settings = uc_field_attribute_load_settings($class->pcid);
  $form['fields'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t("There are no fields available for this content type. Add fields to the content type first and then enable them here."),
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
  uc_field_attribute_save_settings(arg(4), $settings);
  backdrop_set_message(t("The changes have been saved."));
}
