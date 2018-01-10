<li <?php print $attributes;?> class="<?php print $classes;?>">
  <a href="<?php isset($menu_item['link']['href']) ? print in_array($menu_item['link']['href'], array('<nolink>')) ? "#" : url($menu_item['link']['href']) : '';?>"  

    <?php if (!empty($caption)): ?>
      data-caption="<?=t($caption)?>"
    <?php endif ?>

    <?php if (!empty($icon)): ?>
      data-icon="<?=t($icon)?>"
    <?php endif ?>

    <?php if (!empty($a_class)): ?>
      data-class="<?=$a_class?>"
    <?php endif ?>

    <?php if (!empty($a_class)): ?>
      class="<?=$a_class?>"
    <?php endif ?>
  >

    <?php if(!empty($icon)) : ?>
      <i class="<?php print $icon;?>"></i>
    <?php endif;?>    

    <?=isset($menu_item['link']['link_title']) ? t($menu_item['link']['link_title']) : '' ?>

    <?php
      if (isset($sub_menu_icon)) {
        echo '<span class="caret"></span>';
      }
    ?>

    <?php if(!empty($caption)) : ?>
      <span class="caption"><?php print t($caption);?></span>
    <?php endif;?>
  </a>
  <?php !empty($content) ? print $content : '' ; ?>
</li>
