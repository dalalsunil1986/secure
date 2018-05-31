<?php

/**
 * @file
 * Contains Configuration Forms.
 */

require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc'); 
 
/**
 * Custom form for adding field in secure configuration form.
 */
function _secure_configuration($form, &$form_state) {
	
  $form['secure'] = array(
    '#type' => 'fieldset',
    '#title' => t('Secure Page Configurations'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );

  $secure_page_list = variable_get('secure');	

  $form['secure']['secure_field'] = array (
    '#title' => t('Node IDs'),
    '#type' => 'textarea',
	'#description' => t('Enter the nid(s) by separating it with a comma.<br/>  Available token: <br/> %uid: cureent logged in userID.<br/> %nid: current page NodeID.'),
	'#default_value' => isset($secure_page_list['secure_field']) ? $secure_page_list['secure_field'] : '',
  );
    return system_settings_form($form);
}



function _secure_valid_form($form, &$form_state){
	
  $form['field_based_login_pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
	'#required' => TRUE,
    '#description' => t('Enter your login password to access page'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
    
  $form['#validate'][] = 'secure_page_password_validate';
  return $form;
}


function secure_page_password_validate(&$form, &$form_state){
		
	if (empty($form_state['values']['field_based_login_pass'])) {
		form_set_error('secure_field');
	} else {
		
		global $user;
		$account_pass = user_load($user->uid)->pass;
		$hash_pass = _password_crypt('sha512', trim($form_state['values']['field_based_login_pass']), $account_pass);
		if($account_pass === $hash_pass) {
		   	$params = drupal_get_query_parameters();
			cache_set('secure_page_uid-'.$user->uid, $params['destination'], 'cache', time() + 1*60);
			$rtrv_cache = cache_get('secure_page_uid-'.$user->uid, $bin = 'cache');

			if ($rtrv_cache) {
				//drupal_goto($params['destination']);
				secure_page_redirect($params['destination']);
            } else {
				$options = array('query' => array('destination' => $params['destination']));
				drupal_goto('secure/valid', $options);	
			}			
		} else {
			form_set_error('secure_field', t('invalid credential'));
		}
		
	}
}


// Page redirection for correct credential.
function secure_page_redirect($url) {
	drupal_goto($url);
}
	