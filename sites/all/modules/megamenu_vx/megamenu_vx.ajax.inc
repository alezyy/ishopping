<?php

function megamenu_vx_ajax_save_config() {
  $menu_config = isset($_POST['menu_config']) ? $_POST['menu_config'] : NULL;
  $block_config = isset($_POST['block_config']) ? $_POST['block_config'] : NULL;
  $menu_name = isset($_POST['menu_name']) ? $_POST['menu_name'] : NULL;
  if($menu_config && $menu_name && $block_config) {
    $megamenu_vx = db_select('megamenu_vxs', 't')->fields('t')->condition('menu_name', $menu_name)->execute()->fetchObject();
    if($megamenu_vx) {
      db_update('megamenu_vxs')->fields(array('menu_config' => $menu_config, 'block_config' => $block_config))->condition('menu_name', $menu_name)->execute();
    }
    else {
      db_insert('megamenu_vxs')->fields(array('menu_name' => $menu_name, 'block_config' => $block_config, 'menu_config' => $menu_config))->execute();
    }
  }
  $content = json_encode(array('status' => 'success'));
  print($content);
}

function megamenu_vx_ajax_load_menu() {
  $block = megamenu_vx_render_block($_POST['menu_name']);
  print($block['content']['#markup']);
}

function megamenu_vx_ajax_load_block() {
  $block_key = isset($_POST['block_key']) ? $_POST['block_key'] : NULL;
  $id = isset($_POST['id']) ? $_POST['id'] : NULL;
  $showblocktitle = isset($_POST['showblocktitle']) ? $_POST['showblocktitle'] : NULL;
  if($block_key) {
    $content = json_encode(array('content' => theme('megamenu_vx_block', array('block_key' => $block_key, 'section' => 'backend', 'showblocktitle' => $showblocktitle)), 'id' => $id));
    print($content);
  }
}
