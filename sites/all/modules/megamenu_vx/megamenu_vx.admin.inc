<?php

include_once drupal_get_path('module', 'megamenu_vx') . '/megamenu_vx.inc';

function megamenu_vx_admin_settings($form) {
  $menus = megamenu_vx_get_megamenus();
  foreach ($menus as $menu) {
    $form[$menu->menu_name]['#megamenu_vx'] = $menu;
    $form[$menu->menu_name]['menu_name'] = array('#markup' => $menu->menu_name);
    $form[$menu->menu_name]['title'] = array('#markup' => $menu->title);
    $form[$menu->menu_name]['config_megamenu'] = array('#type' => 'link', '#title' => t('Config'), '#href' => "admin/structure/megamenu-vx/$menu->menu_name");
    $form[$menu->menu_name]['config_links'] = array('#type' => 'link', '#title' => t('Edit links'), '#href' => "admin/structure/menu/manage/$menu->menu_name");

    # If exits tb_menu & and have never import
    if (megamenu_vx_check_exits_tb_mega_menu($menu->menu_name) &&
        megamenu_vx_check_imported($menu->menu_name)) {
      $form[$menu->menu_name]['import_tb_mega_menu'] = array('#type' => 'link', '#title' => t('Import From TB Mega Menu'), '#href' => "admin/structure/megamenu-vx/$menu->menu_name/convert");
    }
  }
  return $form;
}

function theme_megamenu_vx_admin_settings($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#megamenu_vx'])) {
      $menu = &$form[$key];
      $menu['enabled']['#parents'] = array($key, 'enabled');
      $menu['enabled']['#name'] = $key . "[enabled]";
      $row = array();
      $row[] = drupal_render($menu['menu_name']);
      $row[] = drupal_render($menu['title']);
      $row[] = drupal_render($menu['config_megamenu']);
      $row[] = drupal_render($menu['config_links']);
      $row[] = drupal_render($menu['import_tb_mega_menu']);
      $rows[] = $row;
    }
  }
  $header = array(t('Menu Name'), t('Menu Title'));
  $header[] = array('data' => t('Operations'), 'colspan' => 3);
  return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => t('No MegaMenu block available. <a href="@link">Add MegaMenu Block</a>.', array('@link' => url('admin/config/user-interface/megamenu_vx/add'))), 'attributes' => array('id' => 'megamenu_vx'))) . drupal_render_children($form);
}

function megamenu_vx_configure_form($form, &$form_state, $menu_names = array()) {
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/stylesheets/base.css');
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/stylesheets/backend.css');  
  drupal_add_js(array('ajax_link' => (variable_get('clean_url', 0) ? '' : '?q=')), 'setting');  
  
  drupal_add_js(drupal_get_path('module', 'megamenu_vx') . '/js/megamenu-vx.js');
  if(!empty($menu_names)) {
    $form['megamenu_vx'] = array(
      '#markup' => theme('megamenu_vx_backend', array(
        'menu_name' => $menu_names[0],
      )),
    );
  }
  return $form;
}

function megamenu_vx_convert_from_tb_menu_confirm($form, &$form_state, $menu_names = array()) {
  $menu_name = $menu_names[0];

  if (user_access('administer megamenu_vx')) {
    $form['menu_name'] = array(
      '#type'  => 'hidden',
      '#value' => $menu_name,
    );

    $output = confirm_form($form,
      t('Are you sure you want to convert from Mega TB Menu %title?', array('%title' => $menu_name)),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/megamenu-vx');
  }
  return $output;
}

function megamenu_vx_convert_from_tb_menu_confirm_submit($form, &$form_state) {
  megamenu_vx_convert_from_tb_menu($form_state['input']['menu_name']);
  $form_state['redirect'] = 'admin/structure/megamenu-vx';
  drupal_set_message('Import Success !', $type = 'status');
}

function megamenu_vx_convert_from_tb_menu($menu_name) {
  # Get config tb megamenu
  $tb_mega_menu = db_select('tb_megamenus', 't')
                        ->fields('t')
                        ->condition('menu_name', $menu_name,'=')
                        ->execute()
                        ->fetchAssoc();
  $tb_mega_menu_config = $tb_mega_menu['menu_config'];
  $tb_mega_block_config = $tb_mega_menu['block_config'];
  $tb_mega_menu_config = json_decode($tb_mega_menu_config, true);
  $tb_mega_block_config = json_decode($tb_mega_block_config, true);



  $menu_struct = menu_tree_all_data($menu_name);
  $new_menu_config = array();
  $new_block_config = array();

  # Convert Menu Config
  foreach ($menu_struct as $key => $value) {
    $new_menu_config[] = megamenu_vx_convert_menu_config($value, 1, $tb_mega_menu_config);
  }

  # Convert Blog Config
  $new_block_config = megamenu_vx_convert_block_config($tb_mega_block_config);

  # Convert Json Encode
  $new_menu_config = json_encode($new_menu_config);
  $new_block_config = json_encode($new_block_config);

  # Insert to MegaMenu VX
  # Check Exist Old Config (If not exist)
  $mega_menu_vx = db_select('megamenu_vxs', 't')
                  ->fields('t')
                  ->condition('menu_name', $menu_name,'=')
                  ->execute()
                  ->fetchAssoc();

  
  if (empty($mega_menu_vx['menu_name'])) {
    # If Empty -> Add New
    $nid = db_insert('megamenu_vxs') // Table name no longer needs {}
          ->fields(array(
            'menu_name' => $menu_name,
            'block_config' => $new_block_config,
            'menu_config' => $new_menu_config,
          ))
          ->execute();
  } else {
    # Update
    $num_updated = db_update('megamenu_vxs') // Table name no longer needs {}
                  ->fields(array(
                      'block_config' => $new_block_config,
                      'menu_config' => $new_menu_config,
                    ))
                  ->condition('menu_name', $menu_name, '=')
                  ->execute();
  }
}

function megamenu_vx_convert_block_config ($tb_mega_block_config) {

  $result = new stdClass();
  $result->direction = 'horizontal';
  $result->style = 'none';
  $result->animation = $tb_mega_block_config['animation'];
  $result->{'auto-arrow'} = $tb_mega_block_config['auto-arrow'];
  $result->{'always-show-submenu'} = $tb_mega_block_config['always-show-submenu'];
  $result->duration = $tb_mega_block_config['duration'];
  $result->delay = $tb_mega_block_config['delay'];
  $result->import_from_tb_menu = '1';

  return $result;
}

function megamenu_vx_check_menu_break ($value_rows) {
  $result = new stdClass();
  $result->break = 0;
  $result->cols = array();

  $arr_cols_break = array();
  foreach ($value_rows as $key_rows_break => $value_rows_break) {
    if ($value_rows_break['col_content'][0]['type'] == 'menu_item') {
      $arr_cols_break[$key_rows_break] = $value_rows_break['col_content'][0]['mlid'];
    }
  }

  $count = 0;
  foreach ($arr_cols_break as $key => $value) {
    if ($value != 0) {
      $count++;
    }
  }

  $result->break = $count > 1 ? 1 : 0;
  $result->cols = $arr_cols_break;

  return $result;
}

function megamenu_vx_convert_menu_config ($item, $level, $tb_mega_menu_config) {
  $arr_past = array();
  $cur_menu_id = $item['link']['mlid'];

  if (!empty($tb_mega_menu_config[$cur_menu_id])) {
    $cur_item_config = $tb_mega_menu_config[$cur_menu_id]['item_config'];
    $cur_submenu_config = $tb_mega_menu_config[$cur_menu_id]['submenu_config'];
    $cur_rows_content = $tb_mega_menu_config[$cur_menu_id]['rows_content'];
  }

  $li_config = new stdClass();
  if (!empty($item['below']) || !empty($cur_rows_content)) {

    $li_config->submenu = new stdClass();
    $li_config->submenu->rows_cols = array();
    $li_config->submenu->config = array(
      'width' => $cur_submenu_config['width'] . 'px',
      'class' => $cur_submenu_config['class'],
    );

    $items = $item['below'];
    $arr_rows_cols = array();
    foreach ($cur_rows_content as $key_rows => $value_rows) {
      $cur_check = megamenu_vx_check_menu_break($value_rows);
      $arr_cols = array();
      
      foreach ($value_rows as $key_cols => $value_cols) {
        $cur_rows_cols = new stdClass();
        
        if ($value_cols['col_content'][0]['type'] == 'menu_item') {
          $cur_rows_cols->block = null;
          $cur_rows_cols->ul = array();
          $cur_rows_cols->type = 'ul';

          if ($cur_check->break) {
            array_shift($cur_check->cols);
          }

          $keys = array_keys($items);
          foreach(array_keys($keys) AS $k ){
            $this_value = $items[$keys[$k]];

            if (!in_array($this_value['link']['mlid'], $arr_past) && $this_value['link']['hidden'] == 0) {
              $cur_rows_cols->ul[] = megamenu_vx_convert_menu_config($this_value, $level + 1, $tb_mega_menu_config);
              $arr_past[] = $this_value['link']['mlid'];
            }
            
            $nextval = '';

            if (isset($keys[$k+1])) {
              $nextval = $items[$keys[$k+1]];

              if (in_array($nextval['link']['mlid'], $cur_check->cols)) {
                break;
              }
            }
          }
        } else {
          $cur_rows_cols->block = new stdClass();
          $cur_rows_cols->block->block = $value_cols['col_content'][0]['block_key'];
          $cur_rows_cols->ul = null;
          $cur_rows_cols->type = 'block';
        }
        
        $cur_rows_cols->config = new stdClass();
        $cur_rows_cols->config->hidewcol = $cur_item_config['hidewcol'];
        $cur_rows_cols->config->grid = $value_cols['col_config']['width'];
        $cur_rows_cols->config->level = $level;
        $cur_rows_cols->config->class = $value_cols['col_config']['class'];
        $cur_rows_cols->config->{'show-block-title'} = $value_cols['col_config']['showblocktitle'];
        
        $arr_cols[] = $cur_rows_cols;
      }
      $arr_rows_cols[] = $arr_cols;
    }

    $li_config->submenu->rows_cols = $arr_rows_cols;
    $li_config->config = new stdClass();
    $li_config->config->{'align-submenu'} = !empty($cur_item_config['alignsub']) ? $cur_item_config['alignsub'] : 0;
    $li_config->config->hidewsub = $cur_item_config['hidesub'];
    $li_config->config->group = $cur_item_config['group'];
    $li_config->config->{'a-class'} = '';
    $li_config->config->mlid = $item['link']['mlid'];
    $li_config->config->level = $level;

  } else {
    if (!empty($cur_submenu_config['width']) || !empty($cur_submenu_config['class'])) {
      $li_config->submenu = new stdClass();
      $li_config->submenu->rows_cols = array();
      $li_config->submenu->config = array(
        'width' => $cur_submenu_config['width'] . 'px',
        'class' => $cur_submenu_config['class'],
      );
    } else {
      $li_config->submenu = null;
    }
    

    $li_config->config = new stdClass();
    $li_config->config->{'align-submenu'} = !empty($cur_item_config['alignsub']) ? $cur_item_config['alignsub'] : 0;
    $li_config->config->hidewsub = isset($cur_item_config['hidesub']) ? $cur_item_config['hidesub'] : '' ;
    $li_config->config->group = isset($cur_item_config['group']) ? $cur_item_config['group'] : '' ;
    $li_config->config->{'a-class'} = '';
    $li_config->config->mlid = $item['link']['mlid'];
    $li_config->config->level = $level;
  }
  
  return $li_config;
}