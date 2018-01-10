<?php

function template_preprocess_megamenu_vx(&$vars) {
  $menu_name = $vars['menu_name'];
  if($vars['section'] == 'backend') {
    $items = megamenu_vx_get_tree($menu_name);
  }
  else {
    $items = menu_tree_all_data($menu_name);
  }
  if (function_exists('i18n_menu_localize_tree')) {
    $items = i18n_menu_localize_tree($items);
  }
  #megamenu_vx_render_menu_struct($menu_name);
  $items_page = menu_tree_page_data($menu_name);
  $trail = megamenu_vx_build_page_trail($items_page);
  $menu_config = megamenu_vx_get_menu_config($menu_name, $items);
  $block_config = megamenu_vx_get_block_config($menu_name);
  megamenu_vx_sync_config_all($items, $menu_config, $vars['section']);
  megamenu_vx_edit_block_config($block_config);
  $vars['classes_array'][] = "megamenu-vx-" . str_replace('_', '-', $menu_name);
  if($vars['section'] == 'frontend' && $block_config->style != '') {
    drupal_add_css(drupal_get_path("module", "megamenu_vx") . "/stylesheets/presets/" . $block_config->style . ".css", array(
        'basename' => "megamenu-vx-style-" . $block_config->style . ".css",
      ));
    $vars['classes_array'][] = "style-" . $block_config->style;

  }

  $vars['attributes_array']['data-name'] = $menu_name;
  $vars['attributes_array']['data-preset'] = "";
  $vars['attributes_array']['data-animation'] = "";
  $vars['attributes_array']['data-auto-arrow'] = isset($block_config->{'auto-arrow'}) ? $block_config->{'auto-arrow'} : '';
  $vars['attributes_array']['data-direction'] = isset($block_config->{'direction'}) ? $block_config->{'direction'} : '';
  $vars['attributes_array']['data-always-show-submenu'] = isset($block_config->{'always-show-submenu'}) ? $block_config->{'always-show-submenu'} : '';
  $vars['attributes_array']['id'] = "megamenu-vx-" . str_replace('_', '-', $menu_name);
  if($block_config->animation != 'none') {
    $vars['classes_array'][] = "animate";
    $vars['classes_array'][] = isset($block_config->animation) ? $block_config->animation : '';
  }
  
  if(isset($block_config->{'auto-arrow'}) && $block_config->{'auto-arrow'}) {
    $vars['classes_array'][] = "auto-arrow";
  }

  if(isset($block_config->{'direction'}) && $block_config->{'direction'}) {
    $vars['classes_array'][] = $block_config->{'direction'};
  }

  if(isset($block_config->{'always-show-submenu'}) && $block_config->{'always-show-submenu'}) {
    $vars['classes_array'][] = "always-show-submenu";
  }

  $vars['content'] = theme('megamenu_vx_ul', array(
    'menu_config' => $menu_config,
    'menu_items' =>$items,
    'trail' => $trail,
  ));

  $vars['block_config'] = $block_config;
  if($vars['section'] == 'frontend' && $block_config->animation != 'none') {
    $css = array();
    $css[] = '.megamenu-vx.animate .mega > .mega-dropdown-menu, .megamenu-vx.animate.slide .mega > .mega-dropdown-menu > div {';
    $css[] = '  transition-delay: ' . $block_config->delay . 'ms;';
    $css[] = '  -webkit-transition-delay: ' . $block_config->delay . 'ms;';
    $css[] = '  -ms-transition-delay: ' . $block_config->delay . 'ms;';
    $css[] = '  -o-transition-delay: ' . $block_config->delay . 'ms;';
    $css[] = '  transition-duration: ' . $block_config->duration . 'ms;';
    $css[] = '  -webkit-transition-duration: ' . $block_config->duration . 'ms;';
    $css[] = '  -ms-transition-duration: ' . $block_config->duration . 'ms;';
    $css[] = '  -o-transition-duration: ' . $block_config->duration . 'ms;';
    $css[] = '}';
    drupal_add_css(implode("\n", $css), array('type' => 'inline'));
  }
}

function template_preprocess_megamenu_vx_ul(&$vars) {
  $menu_config = $vars['menu_config'];
  $menu_items = $vars['menu_items'];

  $level = 0;
  $lis = array();

  foreach ($menu_config as $item) {
    foreach ($item as $key => $value) {
      if ($key == 'submenu') {
        
        $lis[] = theme('megamenu_vx_li', array(
          'submenu' => $item->submenu,
          'menu_config' => $item->config,
          'menu_items' => $menu_items,
          'level' => $level + 1,
          'trail' => $vars['trail'],
        ));
      }
    }
  }

  $vars['lis'] = implode("\n", $lis);
  $vars['classes_array'][] = 'nav';
  $vars['classes_array'][] = 'navbar-nav';
  $vars['classes_array'][] = "level-" . $level;
  $vars['classes_array'][] = "items-" . count($lis);

}


function template_preprocess_megamenu_vx_li(&$vars) {
  /*$vars['a_attributes'] = array();*/
  $submenu = $vars['submenu'];
  $menu_items = $vars['menu_items'];
  $menu_config = $vars['menu_config'];
  $vars['content'] = "";
  $level = $vars['level'];
  $trail = $vars['trail'];

  static $t = 1;
  $t ++;
  foreach ($menu_items as $item) {
    if ($item['link']['mlid'] == $menu_config->mlid) {
      
      if (sizeof($item['below']) > 0) {
        $vars['sub_menu_icon'] = TRUE;
      }

      $vars['menu_item'] = $item;
      if(!empty($item['below']) || !empty($submenu->rows_cols)) {
        $vars['content'] = theme('megamenu_vx_submenu', array(
          'rows_cols' => (isset($submenu->rows_cols) ? $submenu->rows_cols : (new stdClass())),
          'menu_config' => (isset($submenu->config) ? $submenu->config : (new stdClass())),
          'parent_config' => $menu_config,
          'menu_items' => $item,
          'level' => $level,
          'trail' => $vars['trail'],
        ));
      }
    }
  }



  if (isset($menu_config->caption)) {
    $vars['caption'] = $menu_config->caption;
  }
  
  $vars['icon'] = isset($menu_config->icon) ? $menu_config->icon : ($menu_config->icon = '');
  
  if (isset($menu_config->{'a-class'})) {
    $vars['a_class'] = $menu_config->{'a-class'};
  }
  
  $vars['classes_array'][] = 'level-' . $level;
  
  if (!empty($menu_config->{'align-submenu'})) {
    $vars['classes_array'][] = 'submenu-align-' . $menu_config->{'align-submenu'};
  }

  if (!empty($menu_config->hidewsub)) {
    $vars['classes_array'][] = 'sub-hidden-collapse';
  }
  
  if ($menu_config->group) {
    $vars['classes_array'][] = 'submenu-group';    
  }

  if($level == 1) {
    $vars['classes_array'][] = 'dropdown';
  }
  else {
    $vars['classes_array'][] = 'dropdown-submenu';
  }
  
  if(isset($trail[$menu_config->mlid])) {
    $vars['classes_array'][] = 'active';
  }
  
  # add other attr
  $vars['attributes_array']['data-hidewsub'] = isset($menu_config->hidewsub) ? $menu_config->hidewsub : '';
  $vars['attributes_array']['data-mlid'] = isset($menu_config->mlid) ? $menu_config->mlid : '';
  $vars['attributes_array']['data-level'] = $level++;
  $vars['attributes_array']['data-align-submenu'] = isset($menu_config->{'align-submenu'}) ? $menu_config->{'align-submenu'} : '';
  $vars['attributes_array']['data-group'] = isset($menu_config->group) ? $menu_config->group : '';
}

function template_preprocess_megamenu_vx_submenu(&$vars) {
  $menu_items = $vars['menu_items'];
  $rows_cols = $vars['rows_cols'];
  $menu_config = $vars['menu_config'];
  $parent_config = $vars['parent_config'];
  $level = $vars['level'];

  $rows = array();
  foreach ($rows_cols as $item) {
    $rows[] = theme('megamenu_vx_row', array(
      'menu_config' => $item,
      'menu_items' => $menu_items,
      'level' => $level,
      'trail' => $vars['trail'],
    ));
  }

  $vars['rows'] = implode("\n", $rows);
  if(isset($menu_config->{'class'}) && $menu_config->{'class'} != "") {
    $vars['classes_array'][] = $menu_config->{'class'};
  }
  if($parent_config->group) {
    $vars['classes_array'][] = 'submenu-group-ct';
  }
  else {
    $vars['classes_array'][] = 'dropdown-menu';
  }

  $vars['width'] = isset($menu_config->width) ? $menu_config->width : ('');

  if (isset($menu_config->{'class'})) {
    $vars['data_class'] = $menu_config->{'class'};
  }
  
}

function template_preprocess_megamenu_vx_row(&$vars) {
  $row = $vars['menu_config'];
  $level = $vars['level'];
  $menu_items = $vars['menu_items'];
  $cols = array();
  foreach($row as $col) {
    $cols[] = theme('megamenu_vx_col', array(
      'menu_config' => $col,      
      'menu_items' => $menu_items,
      'level' => $level,
      'trail' => $vars['trail'],
    ));
  }

  $vars['columns'] = implode("\n", $cols);
}

function template_preprocess_megamenu_vx_col(&$vars) {
  $menu_items = $vars['menu_items'];
  $col_config = $vars['menu_config'];

  $level = $vars['level'];
  $result = array();

  if ($col_config->block == '') {
    $result[] = theme('megamenu_vx_subul', array(
      'subcol' => $col_config,
      'menu_items' => $menu_items,
      'level' => $level,
      'trail' => $vars['trail'],
    ));
  }
  else {
    $result[] = theme('megamenu_vx_block', array(
      'block_key' => $col_config->block->block,
      'showblocktitle' => isset($col_config->showblocktitle) ? $col_config->showblocktitle : ($col_config->showblocktitle = ''),
      'level' => $level,
    ));
  }

  $vars['tb_items'] = implode("\n", $result);

  $vars['classes_array'][] = 'col-md-' . (isset($col_config->config->grid) ? $col_config->config->grid : '');
  if(!empty($col_config->class)) {
    $vars['classes_array'][] = $col_config->class;
  }
  if(!empty($col_config->config->class)) {
    $vars['classes_array'][] = $col_config->config->class;
  }  
  if(!empty($col_config->config->hidewcol)) {
    $vars['classes_array'][] = 'hidden-collapse';
  }
  if(isset($col_config->config->{'show-block-title'}) && $col_config->config->{'show-block-title'}) {
    $vars['classes_array'][] = 'show-block-title';
  }

  $vars['attributes_array']['data-class'] = isset($col_config->class) ? $col_config->class : ($col_config->class = '');
  $vars['attributes_array']['data-grid'] = isset($col_config->config->grid) ? $col_config->config->grid : '';
  $vars['attributes_array']['data-hidewcol'] = isset($col_config->config->hidewcol) ? $col_config->config->hidewcol : ($col_config->config->hidewcol = '');
  $vars['attributes_array']['data-id'] = megamenu_vx_get_counter("column");
  $vars['attributes_array']['id'] = megamenu_vx_get_counter("column");
  $vars['attributes_array']['data-block'] = isset($col_config->block->block) ? ($col_config->block->block) : ($col_config->block = new StdClass());
  $vars['attributes_array']['data-show-block-title'] = isset($col_config->config->{'show-block-title'}) ? $col_config->config->{'show-block-title'} : ($col_config->config->{'show-block-title'} = '');
  $vars['attributes_array']['data-class'] = isset($col_config->config->class) ? $col_config->config->class : ($col_config->config->class = '');
}

function template_preprocess_megamenu_vx_subul(&$vars) {
  $items = $vars['subcol']->ul;
  $level = $vars['level'];
  $menu_items = $vars['menu_items'];

  $lis = array();
  foreach ($items as $item) {
    foreach ($item as $key => $value) {
      if ($key == 'submenu') {
        $lis[] = theme('megamenu_vx_li', array(
          'submenu' => $item->submenu,
          'flag' => TRUE,
          'menu_config' => $item->config,
          'menu_items' => $menu_items['below'],
          'level' => $level + 1,
          'li_active' => 0,
          'trail' => $vars['trail'],
        ));
      }
    }
  }

  $vars['lis'] = implode("\n", $lis);
  
  $vars['classes_array'][] = "level-" . $level;
  $vars['classes_array'][] = "items-" . count($items);
  $vars['theme_hook_suggestions'][] = 'megamenu-vx-subul';
}

function template_preprocess_megamenu_vx_block(&$vars) {
  $block = megamenu_vx_load_block($vars['block_key']);
  
  
  if($block) {
    $module = $block->module;
    $delta = $block->delta;
    $content = module_invoke($module, 'block_view', $delta);

    /*
    if(isset($vars['showblocktitle']) && $vars['showblocktitle']) {
      $block->subject = empty($content['subject']) ? $block->title : $content['subject'];
    }*/

    if(empty($content['subject'])) {
      $block->subject = $block->title;
    } else {
      $block->subject = $content['subject'];
    }
    if(empty($content) && $vars['section'] == 'backend') {

      $vars['content'] = megamenu_vx_get_block_title($vars['block_key']);
    }
    elseif($content) {
      $vars['content'] = theme("block", array('elements' => array('content' => $content['content'], '#children' => (is_array($content['content']) ? drupal_render($content['content']) : $content['content']), '#block' => $block)));
    }
    else {
      $vars['content'] = NULL;
    }
    $vars['classes_array'][] = "megamenu-vx-block";
    $vars['attributes_array']['data-type'] = "block";
    $vars['attributes_array']['data-block'] = $vars['block_key'];
  }
  else {
    $vars['content'] = NULL;
  }
}

function template_preprocess_megamenu_vx_backend(&$vars) {
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/bootstrap/css/bootstrap.css');
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/bootstrap/css/bootstrap-theme.css');
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/font-awesome/css/font-awesome.css');
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/stylesheets/base.css');
  megamenu_vx_add_css(drupal_get_path('module', 'megamenu_vx') . '/stylesheets/backend.css');  
  drupal_add_js(drupal_get_path('module', 'megamenu_vx') . '/bootstrap/js/bootstrap.js');    
  $menu_name = $vars['menu_name'];
  $block_config = megamenu_vx_get_block_config($menu_name);
  megamenu_vx_edit_block_config($block_config);
  $menu_content = megamenu_vx_render_block($menu_name);
  $vars['menu_content'] = $menu_content['content']['#markup'];
  $vars['block_config'] = $block_config;
  $vars['animation_options'] = megamenu_vx_animation_options($block_config);
  $vars['direction_options'] = megamenu_vx_direction_options($block_config);
  $vars['preset_options'] = megamenu_vx_preset_options($block_config);
  $vars['edit_links'] = base_path() . "admin/structure/menu/manage/$menu_name";
  $vars['edit_menu'] = base_path() . "admin/structure/menu/manage/$menu_name/edit";
  $vars['block_options'] = megamenu_vx_get_blocks_options();
}
