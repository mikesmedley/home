<?php

/**
 * @file
 * Contains form alter callback bodies.
 */

/**
 * Menu callback to configure the default behaviour of the module.
 */
function readmorecontrol_admin_settings_form($form, &$form_state) {
  // Enable users to select what type of behaviour the module should use by
  // default for new content types.
  $form['readmorecontrol_default_node_behaviour'] = array(
    '#type' => 'radios',
    '#title' => t('Default "Read more" link behaviour'),
    '#options' => array(
      'always' => t('Always show link<br/><small>Standard Drupal 7 behaviour.</small>'),
      'when_required' => t('Show link when required by any supported fields'),
      'when_required_text' => t('Show link when required by any supported text based fields'),
      'when_required_body' => t('Show link when required by the Body (body) field<br/><small>Standard Drupal 6 behaviour.</small>'),
      'never' => t('Never show link'),
    ),
    '#default_value' => variable_get('readmorecontrol_default_node_behaviour', 'always'),
    '#required' => TRUE,
    '#description' => t('This setting can be overridden by individual content types and fields. Note that most themes will link teaser titles to the node irrespective of these settings.'),
  );
  return system_settings_form($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function _readmorecontrol_form_node_type_form_alter(&$form, &$form_state, $form_id) {
  $type = $form['#node_type'];

  // Add readmorecontrol_node setting to node_type_form to control the
  // circumstances when the Read more link should be displayed.
  $form['readmorecontrol'] = array(
    '#type' => 'fieldset',
    '#title' => t('Read More Settings'),
    '#descriptions' => t('Settings to control when Read more link is displayed.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );

  $form['readmorecontrol']['readmorecontrol_node'] = array(
    '#type' => 'radios',
    '#title' => t('Display Read More'),
    '#options' => array(
      'default' => t('Use the global default behaviour'),
      'always' => t('Always show link<br/><small>Standard Drupal 7 behaviour.</small>'),
      'when_required' => t('Show link when required by any supported fields'),
      'when_required_text' => t('Show link when required by any supported text based fields'),
      'when_required_body' => t('Show link when required by the Body (body) field<br/><small>Standard Drupal 6 behaviour.</small>'),
      'never' => t('Never show link'),
    ),
    '#default_value' => variable_get('readmorecontrol_node_' . $type->type, 'default'),
    '#required' => TRUE,
    '#description' => t('Note that most themes will link teaser titles to the node irrespective of these settings.'),
  );
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function _readmorecontrol_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  // Add readmore_behaviour flag to field_ui_field_edit_form to set fields
  // specific behaviour on the read more link (if applicable).
  $instance = $form['#instance'];
  $field = $form['#field'];
  $entity_type = $instance['entity_type'];
  if ($entity_type == 'node') {
    $form['instance']['readmore_behaviour'] = array(
      '#type' => 'radios',
      '#title' => t('Read More Behaviour'),
      '#description' => readmorecontrol_field_readmore_settings($field, $instance),
      '#default_value' => empty($instance['readmore_behaviour']) ? 'default' : $instance['readmore_behaviour'],
      '#options' => array(
        'default' => t('Use the default behaviour'),
        'process' => t('Check this field when determining the Read more link'),
        'ignore' => t('Ignore this field when determining the Read more link'),
      ),
      '#weight' => -9,
    );
  }
}

/**
 * Helper function to explain how the current instance setting would be used.
 */
function readmorecontrol_field_readmore_settings($field, $instance) {
  $bundle = $instance['bundle'];
  $entity_type = $instance['entity_type'];

  $system_default = $entity_type == 'node' ? 'always' : 'never';
  $entity_default = variable_get("readmorecontrol_default_{$entity_type}_behaviour", $system_default);
  $bundle_default = variable_get("readmorecontrol_{$entity_type}_{$bundle}", $entity_default);
  $entity_info = entity_get_info($entity_type);
  $t_args = array(
    '%field' => $instance['label'],
    '%display_read_more' => t('Display Read More'),
    '%entity_label' => $entity_info['label'],
  );

  if ($bundle_default == 'default') {
    $actual_behaviour = $entity_default;
    $t_args['!settings_source'] = t('default');
  }
  else {
    $actual_behaviour = $bundle_default;
    $bundle_info = field_info_bundles($entity_type);
    $t_args['!settings_source'] = t('@bundle', array('@bundle' => $bundle_info[$bundle]['label']));
  }

  $output = '';
  switch ($actual_behaviour) {
    case 'always':
      $t_args['%behaviour'] = t('always show the Read more link');
      $used = FALSE;
      break;

    case 'when_required':
      $t_args['%behaviour'] = t('conditionally show the Read more link based on any field');
      $used = TRUE;
      break;

    case 'when_required_text':
      $t_args['%behaviour'] = t('conditionally show the Read more link based on any text fields');
      $used = in_array($field['type'], array('text_with_summary', 'text_long'));
      break;

    case 'when_required_body':
      $t_args['%behaviour'] = t('conditionally show the Read more link based on the body field');
      $used = $field['field_name'] == 'body';
      break;

    case 'never':
      $t_args['%behaviour'] = t('never show the Read more link');
      $used = FALSE;
      break;
  }
  if ($used) {
    $output .= t('This setting is currently being used.');
  }
  else {
    $output .= t('This setting is currently <strong>not used</strong>.');
  }
  $output .= '<br/>' . t('This behaviour, %behaviour, is set using the %entity_label types !settings_source %display_read_more settings.', $t_args);
  return $output;
}
