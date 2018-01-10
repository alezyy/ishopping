<div <?php print $attributes;?> class="navbar navbar-megamenu-vx <?php print $classes;?>">
  <div class="container-fluid">
    <div class="navbar-header">
      <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#wp-sm-main-menu-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
    </div>

    <div class="collapse navbar-collapse" id="wp-sm-main-menu-1">
      <?php print $content;?>
    </div>
  </div>
</div>
