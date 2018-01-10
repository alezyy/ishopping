<div 
	<?php if (!empty($width)): ?>
		style="width: <?=$width?>;"
		data-width="<?=$width?>"
	<?php endif ?>

	<?php if (!empty($data_class)): ?>
		data-class="<?=$data_class?>"
	<?php endif ?>
	class="<?php print $classes;?>">
  <?php print $rows;?>
</div>

