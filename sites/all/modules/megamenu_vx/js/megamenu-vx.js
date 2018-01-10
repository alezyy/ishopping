Drupal.MegaMenuVX = Drupal.MegaMenuVX || {};

(function($) {
  Drupal.MegaMenuVX.options = {};
  Drupal.MegaMenuVX.currentSelected = null;
  Drupal.MegaMenuVX.admin = null;
  Drupal.MegaMenuVX.megamenu_vx = null;
  Drupal.MegaMenuVX.nav_items = null;
  Drupal.MegaMenuVX.nav_subs = null;
  Drupal.MegaMenuVX.nav_cols = null;
  Drupal.MegaMenuVX.nav_all = null;
  Drupal.MegaMenuVX.lockedAjax = false;
  var self = Drupal.MegaMenuVX;
  Drupal.MegaMenuVX.data = {};
  
  Drupal.MegaMenuVX.lockAjax = function() {
    Drupal.MegaMenuVX.lockedAjax = true;
  }

  Drupal.MegaMenuVX.isLockedAjax = function() {
    return Drupal.MegaMenuVX.lockedAjax;
  }
  
  Drupal.MegaMenuVX.releaseAjax = function() {
    Drupal.MegaMenuVX.lockedAjax = false;
  }
  
  Drupal.MegaMenuVX.init = function(options) {
    self.options = $.extend(self.options, options);

    self.admin = $('#megamenu-vx-config');
    self.megamenu_vx = self.admin.find('.megamenu-vx');
    self.nav_items = self.megamenu_vx.find('ul>li.megamenu-vx-li>:first-child');
    self.nav_subs = self.megamenu_vx.find('.megamenu-vx-submenu');
    self.nav_cols = self.megamenu_vx.find('.megamenu-vx-col');
    self.nav_all = self.nav_items.add(self.nav_subs).add(self.nav_cols);
    
    self.showConfigGroup(true);
    self.bindEvents(self.nav_all);
    $('.config-action, .config-toggle, .config-input, config-select').unbind("focus blur click change keydown");
    $('.megamenu-vx-config-header').click(function(event) {
      event.stopPropagation();
    });
    
    $(document.body).click(function(event) {
      self.showConfigGroup(true);
    });

    $('.config-action').click(function(event) {
      var action = $(this).data('action');
      if (action) {
        self.datas = $(this).data();
        self[action](this);
      }
      event.stopPropagation();
      return false;
    });

    $('.config-toggle, .config-select, .config-input').change(function(event) {
      var action = $(this).data('action');
      if (action) {
        self.datas = $(this).data();
        self[action](this);
      }
      event.stopPropagation();
      return false;
    });
    
    $('.config-input').bind('focus blur click', function(event) {
      event.stopPropagation();
      return false;
    });

    $('.config-input').bind('keydown', function(event) {
      if (event.keyCode === 13) {
        var action = $(this).data('action');
        if (action) {
          self.datas = $(this).data();
          self[action](this);
        }
        event.preventDefault();
      }
    });


    return this;
  };

  Drupal.MegaMenuVX.changeBlock = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var value = $('select[name="input-col-block"]').val();
    self.callAjax({
      'action': 'load_block', 
      'block_key': value, 
      'id': self.currentSelected.attr('id'), 
      'showblocktitle': parseInt(self.currentSelected.attr('data-showblocktitle'))
    });
  }

  Drupal.MegaMenuVX.changeSubmenuWidth = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var value = $(element).val();
    self.currentSelected.css({'width': value + "px"});
    self.currentSelected.attr('data-width', value + 'px');
  }

  Drupal.MegaMenuVX.hideWhenCollapse = function (element, type) {
    if (!self.currentSelected) {
      return ;
    }

    if (!type) {
      type = self.configType();
      console.log(type);
    }
    if (type == 'sub') {
      var li_item = self.currentSelected.closest('li');
      if (parseInt(li_item.attr('data-hidewsub'))) {
        li_item.attr('data-hidewsub', 0);
        li_item.removeClass ('sub-hidden-collapse');
      }
      else {
        li_item.attr('data-hidewsub', 1);
        li_item.addClass ('sub-hidden-collapse');
      }
    }
    else if (type == 'col') {
      if (parseInt(self.currentSelected.attr('data-hidewcol'))) {
        self.currentSelected.attr('data-hidewcol', 0);
        self.currentSelected.removeClass ('hidden-collapse');
      } 
      else {
        self.currentSelected.attr('data-hidewcol', 1);
        self.currentSelected.addClass ('hidden-collapse');      
      }
    }
    //self.updateConfig();
  }

  Drupal.MegaMenuVX.changeGrid = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var grid = $(element).val();
    var current_grid = self.currentSelected.data('grid');
    self.currentSelected.removeClass('col-md-' + current_grid).addClass('col-md-' + grid).attr('data-grid', grid);
  }

  Drupal.MegaMenuVX.changePreset = function(element) {
    $("#megamenu-vx-config-content .megamenu-vx").attr('data-preset', $(element).val());
  }

  Drupal.MegaMenuVX.changeDirection = function(element) {
    $("#megamenu-vx-config-content .megamenu-vx").attr('data-direction', $(element).val());

    $("#megamenu-vx-config-content .megamenu-vx").removeClass('vertical');
    $("#megamenu-vx-config-content .megamenu-vx").removeClass('horizontal');
    $("#megamenu-vx-config-content .megamenu-vx").addClass($(element).val());
  }

  Drupal.MegaMenuVX.changeClass = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var value = $(element).val();
    var current_class = self.currentSelected.attr('data-class');
    if(current_class !== undefined) {
      self.currentSelected.removeClass(current_class).addClass(value).attr('data-class', value);
    }
    else {
      self.currentSelected.addClass(value).attr('data-class', value);
    } 
  }

  Drupal.MegaMenuVX.changeAnimation = function(element) {
    var value = $(element).val();
    $("#megamenu-vx-config-content .megamenu-vx").attr('data-animation', value);
    if (value === 'none') {
      $('.megamenu-vx-config-header .animation-param').hide();
    }
    else {
      $('.megamenu-vx-config-header .animation-param').show();
    }
  }

  Drupal.MegaMenuVX.changeAnimationDelay = function(element) {
    var value = $(element).val();
    $("#megamenu-vx-config-content .megamenu-vx").attr('data-animation-delay', value);
  }
  
  Drupal.MegaMenuVX.changeAnimationDuration = function(element) {
    var value = $(element).val();
    $("#megamenu-vx-config-content .megamenu-vx").attr('data-animation-duration', value);
  }
  
  Drupal.MegaMenuVX.changeIcon = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var value = $(element).val();
    var current_icon = self.currentSelected.data('icon');
    self.currentSelected.removeClass(current_icon).attr('data-icon', value); 
    //self.currentSelected.removeClass(current_icon).addClass(value).attr('data-icon', value); 
    self.currentSelected.children('i').remove();
    self.currentSelected.prepend($('<i class="' + value + '"></i>'));
  }

  Drupal.MegaMenuVX.changeCaption = function(element) {
    if (!self.currentSelected) {
      return;
    }
    var value = $(element).val();
    var current_icon = self.currentSelected.data('caption');
    self.currentSelected.removeClass(current_icon).addClass(value).attr('data-caption', value); 
    self.currentSelected.children('span.caption').remove();
    self.currentSelected.append($('<span class="caption">' + value + '</span>'));
  }
  
  Drupal.MegaMenuVX.toggleSub = function(element) {
    if (!self.currentSelected) {
      return ;
    }
    var li_item = self.currentSelected.closest('li'),
    sub = li_item.find('.megamenu-vx-submenu:first');
    if (parseInt(li_item.attr('data-group'))) {
      return;
    }
    if (sub.length === 0 || sub.css('display') === 'none') {
      if (sub.length === 0) {
        var column = self.ElementsCounter['column'] + 1;
        self.ElementsCounter['column'] ++;
        sub = $('<div class="megamenu-vx-submenu dropdown-menu"><div class="row-fluid megamenu-vx-row clearfix"><div id="megamenu-vx-column-' + column + '" class="megamenu-vx-col col-md-12" data-grid="12"><div class="megamenu-vx-col-inner"></div></div></div></div>').appendTo(li_item);
        self.bindEvents(sub.find('.megamenu-vx-col'));
      }
      else {
        sub.css('display', '');
        li_item.attr('data-hidesub', 0);
      }
      li_item.attr('data-group', 0);
      self.currentSelected.addClass('dropdown-toggle').attr('data-toggle', 'dropdown');
      li_item.addClass(li_item.attr('data-level') == 1 ? 'dropdown' : 'dropdown-submenu');
      self.bindEvents(sub);
    }
    else {
      self.unbindEvents(sub);
      if (li_item.find('ul.megamenu-vx-subul').length) {
        sub.css({'display': 'none'});
        li_item.attr('data-hidesub', 1);
      }
      else {
        sub.remove();
      }
      li_item.attr('data-group', 0);
      self.currentSelected.removeClass('dropdown-toggle').attr('data-toggle', '');
      li_item.removeClass('dropdown dropdown-submenu');
    }
  }

  Drupal.MegaMenuVX.toggleAlwayShowSubmenu = function() {
    var navbar = $('.navbar-megamenu-vx');
    var arrowRadio = $('input[name=megamenu-vx-always-show-submenu]');
    var curValue = arrowRadio.filter(':checked').val();
    if (parseInt(curValue)) {
      navbar.addClass('always-show-submenu');
      navbar.attr('data-always-show-submenu', 1);
    } else {
      navbar.removeClass('always-show-submenu');
      navbar.attr('data-always-show-submenu', 0);
    };

    /*
    var toggle = $('.toolitem-always-show-submenu');
    toggle.find('label').removeClass('active btn-success btn-danger btn-primary');
    if (parseInt(toggle.attr('data-always-show-submenu'))) {
      self.updateToggle(toggle, 0);
      toggle.attr('data-always-show-submenu', 0);
    }
    else {
      self.updateToggle(toggle, 1);
      toggle.attr('data-always-show-submenu', 1);
    }      */
  }

  Drupal.MegaMenuVX.showBlockTitle = function() {
    if (!self.currentSelected) return ;
    var toggle = $('.toggle-col-show-title');
    toggle.find('label').removeClass('active btn-success btn-danger btn-primary');

    if (parseInt(self.currentSelected.attr('data-show-block-title'))) {
      self.updateToggle(toggle, 0);
      self.currentSelected.attr('data-show-block-title', 0);
      self.currentSelected.removeClass('show-block-title');
    }
    else {
      self.updateToggle(toggle, 1);
      self.currentSelected.attr('data-show-block-title', 1);
      self.currentSelected.addClass('show-block-title');
    }
  }

  Drupal.MegaMenuVX.toggleGroup = function() {
    if (!self.currentSelected) return ;
    var li_item = self.currentSelected.parent();
    var sub = li_item.find('.megamenu-vx-submenu:first');
    if (li_item.attr('data-level') == 1) {
      return;
    }
    if (parseInt(li_item.attr('data-group'))) {
      li_item.attr('data-group', 0);
      li_item.removeClass('submenu-group').addClass('dropdown-submenu');
      self.currentSelected.addClass('dropdown-toggle').attr('data-toggle', 'dropdown');
      sub.removeClass('submenu-group-ct').addClass('dropdown-menu mega-dropdown-menu');
      sub.css('width', sub.attr('data-grid'));
      rebindEvents(sub);
    } else {
      self.currentSelected.removeClass('dropdown-toggle').attr('data-toggle', '');
      li_item.attr('data-group', 1);
      li_item.removeClass('dropdown-submenu').addClass('submenu-group');
      sub.removeClass('dropdown-menu mega-dropdown-menu').addClass('submenu-group-ct');
      sub.css('width', '');
      rebindEvents(sub);
    }
    updateConfig();
  }

  Drupal.MegaMenuVX.alignment = function() {
    if (!self.currentSelected) return ;
    var li_item = self.currentSelected.closest('li');
    li_item.removeClass('submenu-align-left submenu-align-center submenu-align-right submenu-align-justify').addClass('submenu-align-' + self.datas.align);

    $('.btn-alignment-left').removeClass('active');
    $('.btn-alignment-right').removeClass('active');
    $('.btn-alignment-center').removeClass('active');
    $('.btn-alignment-justify').removeClass('active');
    
    $('.btn-alignment-left').click(function(){
      $(this).addClass('active');
    });
    $('.btn-alignment-right').click(function(){
      $(this).addClass('active');
    });
    $('.btn-alignment-center').click(function(){
      $(this).addClass('active');
    });
    $('.btn-alignment-justify').click(function(){
      $(this).addClass('active');
    });

    if (self.datas.align === 'justify') {
      self.currentSelected.css('width', '');
    } else if (self.currentSelected.attr('data-width')) {
      self.currentSelected.css('width', self.currentSelected.attr('data-width'));
    }
    li_item.attr('data-align-submenu', self.datas.align);
    self.updateConfig();
  }

  Drupal.MegaMenuVX.moveItemsLeft = function() {
    if (!self.currentSelected) {
      return ;
    }
    var $item = self.currentSelected.closest('li'),
    $liparent = $item.parent().closest('li'),
    level = $liparent.attr('data-level'),
    $col = $item.closest('.megamenu-vx-col'),
    $items = $col.find('ul:first > li'),
    itemidx = $items.index($item),
    $moveitems = $items.slice(0, itemidx+1),
    itemleft = $items.length - $moveitems.length,
    $rows = $col.parent().parent().children('.megamenu-vx-row'),
    $cols = $rows.children('.megamenu-vx-col').filter(function(){
      return $(this).children('.megamenu-vx-block').length === 0;
    });
    colidx = $cols.index($col);
    if (!$liparent.length) {
      return ;
    } // need make this is mega first

    if (colidx === 0) {
      var oldSelected = self.currentSelected;
      self.currentSelected = $col;
      self.datas.addfirst = true;
      self.addColumn();
      $cols = $rows.children('.megamenu-vx-col').filter(function(){
        return $(this).children('.megamenu-vx-block').length === 0;
      });
      self.currentSelected = oldSelected;
      colidx++;
    }
    var $tocol = $($cols[colidx-1]);
    var $ul = $tocol.find('ul:first');
    if (!$ul.length) {
      $ul = $('<ul class="level-'+level+' megamenu-vx-subul">').appendTo($tocol.children('.megamenu-vx-col-inner'));
    }
    $moveitems.appendTo($ul);
    if (itemleft === 0) {
      $col.find('ul:first').remove();
    }
    self.updateConfig();
  }

  Drupal.MegaMenuVX.moveItemsRight = function() {
    if (!self.currentSelected) {
      return ;
    }
    var $item = self.currentSelected.closest('li'),
    $liparent = $item.closest('li'),
    level = $liparent.data('level'),
    $col = $item.closest('.megamenu-vx-col'),
    $items = $col.find('ul:first > li'),
    itemidx = $items.index($item),
    $moveitems = $items.slice(itemidx),
    itemleft = $items.length - $moveitems.length,
    $rows = $col.parent().parent().children('.megamenu-vx-row'),
    $cols = $rows.children('.megamenu-vx-col').filter(function(){
      return $(this).children('.megamenu-vx-block').length === 0;
    });
    
    var colidx = $cols.index($col);
    if (!$liparent.length) {
      return ;
    }

    if (colidx === $cols.length - 1) {
      var oldSelected = self.currentSelected;
      self.currentSelected = $col;
      self.datas.addfirst = false;
      self.addColumn();
      $cols = $rows.children('.megamenu-vx-col').filter(function(){
        return $(this).children('.megamenu-vx-block').length === 0;
      }),
      self.currentSelected = oldSelected;
    }
    var $tocol = $($cols[colidx+1]);
    var $ul = $tocol.find('ul.megamenu-vx-subul:first');
    if (!$ul.length) {
      $ul = $('<ul class="level' + level + ' megamenu-vx-subul">').appendTo($tocol.children('.megamenu-vx-col-inner'));
    }
    $moveitems.prependTo($ul);
    if (itemleft === 0) {
      $col.find('ul:first').remove();
    }
    self.showConfig(self.currentSelected);
  }

  Drupal.MegaMenuVX.addRow = function() {
    if (!self.currentSelected) {
      return ;
    }
    var column = self.ElementsCounter['column'] + 1;
    self.ElementsCounter['column'] ++;
    var $row = $('<div class="row-fluid megamenu-vx-row clearfix"><div id=megamenu-vx-column-' + column + ' class="megamenu-vx-col col-md-12"><div class="megamenu-vx-col-inner"></div></div></div>').appendTo(self.currentSelected),
    $col = $row.children();
    self.bindEvents($col);
    self.currentSelected = null;
    self.showConfig($col);
  }

  Drupal.MegaMenuVX.addColumn = function() {
    if (!self.currentSelected) {
      return ;
    }
    var $cols = self.currentSelected.parent().children('.megamenu-vx-col');
    var colcount = $cols.length + 1;
    var colwidths = self.defaultColumnsWidth(colcount);

    var column = self.ElementsCounter['column'] + 1;
    self.ElementsCounter['column'] ++;

    var $col = $('<div id=megamenu-vx-column-' + column + ' class="megamenu-vx-col col-md-12" data-grid="12"><div class="megamenu-vx-col-inner"></div></div>');
    if (self.datas.addfirst) { 
      $col.insertBefore(self.currentSelected);
    }
    else {
      $col.insertAfter(self.currentSelected);
    }
    $cols = $cols.add($col);
    self.bindEvents($col);
    $cols.each(function(i) {
      $(this).removeClass('col-md-'+$(this).attr('data-grid')).addClass('col-md-'+colwidths[i]).attr('data-grid', colwidths[i]);
    });
    self.showConfig($col);
  }

  Drupal.MegaMenuVX.removeColumn = function() {
    if (!self.currentSelected) {
      return;
    }

    var $col = self.currentSelected,
      $row = $col.parent(),
      $rows = $row.parent().children('.megamenu-vx-row'),
      $allcols = $rows.children('.megamenu-vx-col'),
      $allmenucols = $allcols.filter(function(){return !$(this).attr('data-block')}),
      $haspos = $allcols.filter(function(){return $(this).attr('data-block')}).length,
      $cols = $row.children('.megamenu-vx-col'),
      colcount = $cols.length - 1,
      colwidths = self.defaultColumnsWidth(colcount),
      type_menu = $col.attr('data-block') ? false : true;

    if ((type_menu &&((!$haspos && $allmenucols.length === 1) ||($haspos && $allmenucols.length === 0))) || $allcols.length === 1) {
      showConfig($(self.currentSelected).closest('.megamenu-vx-item'));
      self.currentSelected = $(self.currentSelected).closest('.megamenu-vx-item')
      self.currentSelected.find('.megamenu-vx-submenu').remove();
      
    } // if this is the only one column left
    else {
      if (type_menu) {
        var colidx = $allmenucols.index($col),
        tocol = colidx == 0 ? $allmenucols[1] : $allmenucols[colidx-1];

        $col.find('ul:first > li').appendTo($(tocol).find('ul:first'));
      } 

      var colidx = $allcols.index($col),
        nextActiveCol = colidx == 0 ? $allcols[1] : $allcols[colidx-1];

      if (colcount < 1) {
        $row.remove();
      } else {
        $cols = $cols.not($col);
        $cols.each(function(i) {
          $(this).removeClass('col-md-'+$(this).attr('data-grid')).addClass('col-md-'+colwidths[i]).attr('data-grid', colwidths[i]);
        });
        $col.remove();
      }
      self.showConfig($(nextActiveCol));
    }
  }

  Drupal.MegaMenuVX.configType = function() {
    return self.currentSelected ? self.currentSelected.hasClass('megamenu-vx-submenu') ? 'sub' :(self.currentSelected[0].tagName == 'DIV' ? 'col':'item') : false;
  }

  Drupal.MegaMenuVX.showConfigGroup = function(_default) {
    $('#megamenu-vx-config-groups .config-group').hide();
    if (_default) {
      $('#megamenu-vx-config-menu').show();
    }
    else {
      $('#megamenu-vx-config-menu').hide();
    }
  }

  Drupal.MegaMenuVX.showConfig = function(selected) {
    self.showConfigGroup(false);
    if (selected) {
      self.currentSelected = selected;
    }
    self.megamenu_vx.find('.megamenu-vx-li').each(function(){
      if ($(this).has(self.currentSelected).length) {
        $(this).addClass('open');
      }
      else {
        $(this).removeClass('open');
      }
    });
    $(".megamenu-vx-active").removeClass("megamenu-vx-active");
    if (self.currentSelected.hasClass('megamenu-vx-submenu')) {
      $("#megamenu-vx-config-submenu").show();
      self.currentSelected.addClass("megamenu-vx-active");
    }
    else if (self.currentSelected.hasClass('megamenu-vx-col')) {
      $("#megamenu-vx-config-col").show();
      self.currentSelected.addClass("megamenu-vx-active");
    }
    else if (self.currentSelected.parent().hasClass('megamenu-vx-li')) {
      self.currentSelected.addClass("megamenu-vx-active");
      self.currentSelected.parent().addClass('open')
      $("#megamenu-vx-config-li").show();
    }
    self.updateConfig();
  }

  Drupal.MegaMenuVX.updateConfig = function(type) { 
    if (!type) {
      type = self.configType();
    }

    switch(type) {
      case 'item':
        var li_item = self.currentSelected.closest('li'),
        sub = li_item.find('.megamenu-vx-submenu');

        $('.input-item-class').attr('value', li_item.find('a:first').attr('data-class') || '');
        $('.input-item-icon').attr('value', li_item.find('a:first').attr('data-icon') || '');
        $('.input-item-caption').attr('value', li_item.find('a:first').attr('data-caption') || '');

        var toggleSub = $('.toggle-item-sub');
        if (parseInt(li_item.attr('data-group'))) {
          toggleSub.addClass('disabled');
          toggleSub.find('input').attr('disabled', 'disabled');
        } 
        else if (sub.length === 0 || sub.css('display') === 'none') {
          self.updateToggle(toggleSub, 0);
        }
        else {
          self.updateToggle(toggleSub, 1);
        }

        var toggleGroup = $('.toggle-item-group');
        if (parseInt(li_item.data('level')) === 1 || sub.length === 0 || parseInt(li_item.data('hidesub')) === 1) {
          toggleGroup.addClass('disabled');
          toggleGroup.find('input').attr('disabled', 'disabled');
        }
        else if (parseInt(li_item.attr('data-group'))) {
          self.updateToggle(toggleGroup, 1);
          toggleGroup.removeClass('disabled');
          toggleGroup.find('input').removeAttr('disabled');
        }
        else {
          self.updateToggle(toggleGroup, 0);  
          toggleGroup.removeClass('disabled');  
          toggleGroup.find('input').removeAttr('disabled');    
        }

        if (parseInt(li_item.data('level')) === 1) {
          $('.btn-move-left, .btn-move-right').addClass('disabled');
        }
        else {
          $('.btn-move-left, .btn-move-right').removeClass('disabled');
        }
        break;

      case 'sub':
        var li_item = self.currentSelected.closest('li');
        $('.input-submenu-class').attr('value', self.currentSelected.data('class') || '');
        if (parseInt(li_item.data('group'))) {
          $('.input-submenu-width').attr('value', '').addClass('disabled');
          $('.btn-submenu-alignment').addClass('disabled');
        }
        else {
          $('.input-submenu-width').val(self.currentSelected.attr('data-width'));
          if (parseInt(li_item.attr('data-level')) > 1) {
            $('.btn-alignment-center').addClass('disabled');
            $('.btn-alignment-justify').addClass('disabled');
          } 

          if (li_item.attr('data-align-submenu')) {
            $('.btn-alignment-'+li_item.data('align-submenu')).addClass('active');
            if (li_item.data('align-submenu') == 'justify') {
              $('.input-submenu-width').addClass('disabled');
            }
          }       
        }
        
        var toggleSub = $('.toggle-sub-hidewhencollapse');
        if (parseInt(self.currentSelected.parent().attr('data-hidewsub'))) {
          toggleSub.removeClass('disabled');
          toggleSub.find('input').removeAttr('disabled');
          self.updateToggle(toggleSub, 1);
        } else {
          toggleSub.removeClass('disabled');
          toggleSub.find('input').removeAttr('disabled');
          self.updateToggle(toggleSub, 0);
        }

        break;

      case 'col':
        $('.input-col-class').attr('value', self.currentSelected.data('class') || '');
        $('.input-col-block').val(self.currentSelected.children('.megamenu-vx-block').data('block') || '');
        $('.input-col-grid').val(self.currentSelected.attr('data-grid') || '');
        if (self.currentSelected.find('.megamenu-vx-subul').length > 0) {
          $('.input-col-block').parent().addClass('disabled');
        }
        if (self.currentSelected.parent().children().length === 1) {
          $('.input-col-grid').parent().addClass('disabled');
        }

        var toggle = $('.toggle-col-show-title');
        if(self.currentSelected.find('.megamenu-vx-subul').length) {
          toggle.addClass('disabled');
          toggle.find('input').attr('disabled', 'disabled');
        }
        else if (parseInt(self.currentSelected.attr('data-show-block-title'))) {
          toggle.removeClass('disabled');
          toggle.find('input').removeAttr('disabled');
          self.updateToggle(toggle, 1);
        } else {
          toggle.removeClass('disabled');
          toggle.find('input').removeAttr('disabled');
          self.updateToggle(toggle, 0);
        }

        var toggleCol = $('.toggle-col-hidewhencollapse');
        if (parseInt(self.currentSelected.attr('data-hidewcol'))) {
          toggleCol.removeClass('disabled');
          toggleCol.find('input').removeAttr('disabled');
          self.updateToggle(toggleCol, 1);
        } else {
          toggleCol.removeClass('disabled');
          toggleCol.find('input').removeAttr('disabled');
          self.updateToggle(toggleCol, 0);
        }


        break;
    }
  }

  Drupal.MegaMenuVX.toggleAutoArrow = function(input) {
    var navbar = $('.navbar-megamenu-vx');
    var arrowRadio = $('input[name=megamenu-vx-auto-arrow]');
    var curValue = arrowRadio.filter(':checked').val();
    if (parseInt(curValue)) {
      navbar.addClass('auto-arrow');
      navbar.attr('data-auto-arrow', 1);
    } else {
      navbar.removeClass('auto-arrow');
      navbar.attr('data-auto-arrow', 0);
    };
  }

  Drupal.MegaMenuVX.applyConfig = function(input) {
    var name = $(input).attr('data-name'), 
    value = input.value,
    type = self.configType();
    switch(name) {
    case 'width':
      value = parseInt(value);
      if (isNaN(value)) {
        value = "";
        if (type == 'sub') {
          self.currentSelected.width(value);
        }
        if (type == 'col') {
          self.currentSelected.removeClass('span'+self.currentSelected.attr('data-' + name));
        }
        self.currentSelected.attr('data-' + name, value);
      }
      else {
        if (type == 'sub') {
          self.currentSelected.width(value);
        }
        if (type == 'col') {
          self.currentSelected.removeClass('span'+self.currentSelected.attr('data-' + name)).addClass('span'+value);
        }
        self.currentSelected.attr('data-' + name, value);
      }
      $(input).val(value);
      break;
    case 'duration': 
      value = parseInt(value);
      if (isNaN(value)) {
        value = "";
      }
      $(input).val(value);        
      break;
    case 'delay': 
      value = parseInt(value);
      if (isNaN(value)) {
        value = "";
      }
      $(input).val(value);        
      break;
    case 'class':
      if (type == 'item') {
        var item = self.currentSelected.closest('li');
      } else {
        var item = self.currentSelected;
      }
      item.removeClass(item.attr('data-' + name) || '').addClass(value);
      item.attr('data-' + name, value);
      break;

    case 'icon':
      if (type == 'item') {
        self.currentSelected.closest('li').attr('data-' + name, value);
        self.currentSelected.find('i').remove();
        if (value) {
          self.currentSelected.prepend($('<i class="'+value+'"></i>'));
        }
      }
      break;

    case 'caption':
      if (type == 'item') {
        self.currentSelected.closest('li').attr('data-' + name, value);
        self.currentSelected.find('span.caption').remove();
        if (value) {
          self.currentSelected.append($('<span class="caption">'+value+'</span>'));
        }
      }
      break;

    case 'block':
      if (self.currentSelected.find('ul[class*="level"]').length == 0) {
        if (value) {
          $('.megamenu-vx-messages-group #toolbox-loading').show();
          self.callAjax({'action': 'load_block', 'block_key': value, 'id': self.currentSelected.attr('id'), 'showblocktitle': parseInt(self.currentSelected.attr('data-show-block-title'))});
        }
        else {
          self.currentSelected.find('.mega-inner').html('');
        }
        self.currentSelected.attr('data-' + name, value);
      }
      break;
    }
  }
  
  Drupal.MegaMenuVX.callAjax = function(data) {
    if (self.isLockedAjax()) {
      window.setTimeout(function() {
        self.callAjax(data);
      }, 200);
      return;
    }
    self.lockAjax();
    switch(data.action) {
    case 'load_block':
        $.ajax({
          type: "POST",
          url: Drupal.settings.basePath + Drupal.settings.ajax_link + "admin/structure/megamenu-vx/load_block",
          data: data,
          complete: function( msg ) {
            var resp = $.parseJSON(msg.responseText);
            var content = resp.content ? resp.content : "";
            var close_button = $('<span class="close icon-remove" title="' + Drupal.t("Remove this block") + '">&nbsp;</span>');
            var id = resp.id ? resp.id : "";
            var currentElement = $("#" + id);
            if (currentElement.length) {
              currentElement.children('.megamenu-vx-col-inner').html("").append(close_button).append($(content)).find(':input').removeAttr('name');
              currentElement.children('.megamenu-vx-col-inner').children('span.close').click(function() {
                $(this).parent().html("");
              });
            }
            $('.megamenu-vx-messages-group #toolbox-loading').hide();
            self.releaseAjax();
          }
        });
      break;
    case 'load':
      break;
    case '':
      break;
    }
  }
  
  Drupal.MegaMenuVX.resetConfig = function() {
    if (Drupal.MegaMenuVX.isLockedAjax()) {
      window.setTimeout(function() {
        actions.resetConfig();
      }, 200);
      return;
    }
    Drupal.MegaMenuVX.lockAjax();
    $('.megamenu-vx-messages-group #toolbox-message').html("").hide();
    $('.megamenu-vx-messages-group #toolbox-loading').show();
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + Drupal.settings.ajax_link + "admin/structure/megamenu-vx/load_menu",
      data: { 'action': 'load', 'menu_name': $('#megamenu-vx-config-content .megamenu-vx').data('name')},
      complete: function( msg ) {
        $('#megamenu-vx-config-content').html(msg.responseText);
        $('.megamenu-vx-messages-group #toolbox-loading').hide();
        $div = $('<div id="console" class="clearfix"><div class="messages status"><h2 class="element-invisible">Status message</h2>' + Drupal.t("Saved config sucessfully!") + '</div></div>');
        $('.megamenu-vx-messages-group #toolbox-message').html($div).show();
        window.setTimeout(function() {
          $('.megamenu-vx-messages-group #toolbox-message').html("").hide();
        }, 5000);
        Drupal.MegaMenuVX.init();
        Drupal.MegaMenuVX.releaseAjax();
      }
    });
  }

  Drupal.MegaMenuVX.getSubMenuConfig = function(sub) {
    var rows = [];
    $(sub).children('.megamenu-vx-row').each(function() {
      rows.push(Drupal.MegaMenuVX.getRowConfig(this));
    });

    var config = {};

    config['width'] = $(sub).css('width');
    config['class'] = $(sub).attr('data-class');

    /*config['class'] = $(sub).attr('data-class');*/
    //config['class'] = $(sub).data('class');

    return {rows_cols: rows, config: config};
  }

  Drupal.MegaMenuVX.getRowConfig = function(row) {
    var cols = [];
    $(row).children('.megamenu-vx-col').each(function() {
      cols.push(Drupal.MegaMenuVX.getColConfig(this));
    });
    return cols;
  }

  Drupal.MegaMenuVX.getColConfig = function(col) {
    var block = null;
    var ul = null;
    if($(col).children('.megamenu-vx-col-inner').children('.megamenu-vx-block').length) {
      block = Drupal.MegaMenuVX.getBlockConfig($(col).children('.megamenu-vx-col-inner').children('.megamenu-vx-block'));
    }
    else {
      ul = Drupal.MegaMenuVX.getMenuConfig($(col).children('.megamenu-vx-col-inner').children('.megamenu-vx-subul'));
    }
    var config = {};

    config['hidewcol'] = $(col).attr('data-hidewcol');
    config['grid'] = $(col).attr('data-grid');
    config['class'] = $(col).attr('data-class');
    config['show-block-title'] = $(col).attr('data-show-block-title');
    

    return {block: block, ul: ul, type: block ? "block" : (ul ? 'ul' : null), config: config};
  }
  
  Drupal.MegaMenuVX.getBlockConfig = function(block) {
    var config = {};
    config['block'] = $(block).data('block');
    return config;    
  }
  
  Drupal.MegaMenuVX.getItemConfig = function(item) {
    var submenu = $(item).children('.megamenu-vx-submenu');

    if(submenu.length) {
      submenu = Drupal.MegaMenuVX.getSubMenuConfig(submenu);
    }
    else {
      submenu = null;
    }

    var config = {};
    config['align-submenu'] = $(item).attr('data-align-submenu');
    config['hidewsub'] = $(item).attr('data-hidewsub');
    config['group'] = $(item).attr('data-group');
    config['caption'] = $(item).children("a").attr('data-caption');
    config['icon'] = $(item).children("a").attr('data-icon');
    config['a-class'] = $(item).children("a").attr('class');
    config['class'] = $(item).data('class');
    config['mlid'] = $(item).data('mlid');
    config['level'] = $(item).data('level');
    return {submenu: submenu, config: config};
  }
  
  Drupal.MegaMenuVX.getMenuConfig = function(ul) {
    var config = [];
    $(ul).children('.megamenu-vx-li').each(function() {
      config.push(Drupal.MegaMenuVX.getItemConfig(this));
    });
    return config;
  }

  Drupal.MegaMenuVX.getMegaMenuVXBlockConfig = function(ul) {
    var config = {};
    var navbar = $('.navbar-megamenu-vx');
    config['direction'] = $('select[name="megamenu-vx-direction"]').val();
    config['style'] = $('select[name="megamenu-vx-style"]').val();
    config['animation'] = $('select[name="megamenu-vx-animation"]').val();
    config['auto-arrow'] = parseInt($('input[name=megamenu-vx-auto-arrow]:checked').val());
    config['always-show-submenu'] = parseInt($('input[name=megamenu-vx-always-show-submenu]:checked').val());
    config['duration'] = parseInt($('input[name="megamenu-vx-duration"]').val());
    config['delay'] = parseInt($('input[name="megamenu-vx-delay"]').val());
    return config;
  }

  Drupal.MegaMenuVX.getGUIMenu = function() {
    var menu_config = {},
    items = self.megamenu_vx.find('ul[class*="level"] > li');
    items.each (function(){
      var $this = $(this),
      id = $this.attr('data-id'),
      rows = [];
      var level = parseInt($this.attr('data-level'));
      var $sub = $this.find('.dropdown-menu:first'); // /nav-child:first
      var $rows = $sub.find('[class*="row"]').parent().children('[class*="row"]');

      //var $rows = $sub.find('row-fluid');
      $rows.each (function () {
        var $cols = $(this).children('[class*="megamenu-vx-col"]');
        var cols = [];
        $cols.each (function(){
          var col_config = {};
          col_config['width'] = $(this).attr('data-width') ? $(this).attr('data-width') : "";
          col_config['class'] = $(this).attr('data-class') ? $(this).attr('data-class') : "";
          col_config['hidewcol'] = $(this).attr('data-hidewcol') ? $(this).attr('data-hidewcol') : "";
          col_config['showblocktitle'] = $(this).attr('data-showblocktitle') ? $(this).attr('data-showblocktitle') : "1";
          var col = {'col_content': [], 'col_config': col_config};
          $(this).find('ul[class*="level"] > li:first').each(function() {
            var sub_level = parseInt($(this).attr('data-level'));
            if(sub_level == level + 1) {
              var ele = {};
              ele['mlid'] = $(this).attr('data-id');
              ele['type'] = $(this).attr('data-type');
              ele['tb_item_config'] = {};
              col['col_content'].push(ele);
            }
          });
          $(this).children('[class*="megamenu-vx-col"]').children('[class*="megamenu-vx-block"]').each(function() {
            var ele = {};
            ele['block_key'] = $(this).attr('data-block');
            ele['type'] = $(this).attr('data-type');
            ele['tb_item_config'] = {};
            col['col_content'].push(ele);
          });
          if(col['col_content'].length) {
            cols.push(col);
          }
        });
        if(cols.length) {
          rows.push(cols);
        }
      });
      var submenu_config = {};
      submenu_config['width'] = $this.children('.mega-dropdown-menu').attr('data-width') ? $this.children('.mega-dropdown-menu').attr('data-width') : "";
      submenu_config['class'] = $this.children('.mega-dropdown-menu').attr('data-class') ? $this.children('.mega-dropdown-menu').attr('data-class') : "";
      submenu_config['group'] = $this.attr('data-group') ? $this.attr('data-group') : 0;
      var item_config = {};
      item_config['class'] = $this.attr('data-class') ? $this.attr('data-class') : "";
      item_config['xicon'] = $this.attr('data-xicon') ? $this.attr('data-xicon') : "";
      item_config['caption'] = $this.attr('data-caption') ? $this.attr('data-caption') : "";
      item_config['alignsub'] = $this.attr('data-alignsub') ? $this.attr('data-alignsub') : "";
      item_config['group'] = $this.attr('data-group') ? $this.attr('data-group') : "";
      item_config['hidewsub'] = $this.attr('data-hidewsub') ? $this.attr('data-hidewsub') : 1;
      //item_config['hidesub'] = $this.attr('data-hidesub') ? $this.attr('data-hidesub') : 1;
      config = {'rows_content': rows, 'submenu_config': submenu_config, 'item_config': item_config};
      menu_config[id] = config;
    });

    return menu_config;
  }

  Drupal.MegaMenuVX.saveConfig = function(options) {
    if (Drupal.MegaMenuVX.isLockedAjax()) {
      window.setTimeout(function() {
        actions.saveConfig(options);
      }, 200);
      return;
    }
    Drupal.MegaMenuVX.lockAjax();
    var menu_config = Drupal.MegaMenuVX.getMenuConfig(self.megamenu_vx.find('ul.megamenu-vx-ul'));
    var block_config = self.getMegaMenuVXBlockConfig();

    $('.megamenu-vx-messages-group #toolbox-message').html("").hide();
    $('.megamenu-vx-messages-group #toolbox-loading').show();
    var menu_name = $('#megamenu-vx-config-content > .megamenu-vx').data('name');
    
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + Drupal.settings.ajax_link + "admin/structure/megamenu-vx/save_config",
      data: {
        'action': 'save',
        'menu_name': menu_name,
        'menu_config': JSON.stringify(menu_config),
        'block_config': JSON.stringify(block_config)
      },
      complete: function( msg ) {
        $('.megamenu-vx-messages-group #toolbox-loading').hide();
        $div = $('<div id="console" class="clearfix"><div class="messages status"><h2 class="element-invisible">Status message</h2>' + Drupal.t("Saved config sucessfully!") + '</div></div>');
        $('.megamenu-vx-messages-group #toolbox-message').html($div).show();
        window.setTimeout(function() {
          $('.megamenu-vx-messages-group #toolbox-message').html("").hide();
        }, 5000);
        Drupal.MegaMenuVX.releaseAjax();
      }
    });
  }
  
  Drupal.MegaMenuVX.defaultColumnsWidth = function(count) {
    if (count < 1) {
      return null;
    }
    var total = 12,
    min = Math.floor(total / count),
    widths = [];
    for (var i=0;i<count;i++) {
      widths[i] = min;
    }
    widths[count - 1] = total - min*(count-1);
    return widths;
  }

  Drupal.MegaMenuVX.bindEvents = function(els) {
    var megamenu_vx = self.megamenu_vx;
    if (megamenu_vx.data('nav_all')) 
      megamenu_vx.data('nav_all', megamenu_vx.data('nav_all').add(els));
    else
      megamenu_vx.data('nav_all', els);

    els.mouseover(function(event) {
      megamenu_vx.data('nav_all').removeClass('hover');
      $this = $(this);
      clearTimeout(megamenu_vx.attr('data-hovertimeout'));
      megamenu_vx.attr('data-hovertimeout', setTimeout("$this.addClass('hover')", 100));
      event.stopPropagation();
    });
    els.mouseout(function(event) {
      clearTimeout(megamenu_vx.attr('data-hovertimeout'));
      $(this).removeClass('hover');
    });
    els.click(function(event){
      self.showConfig($(this));
      event.stopPropagation();        
      return false;
    });
  }

  Drupal.MegaMenuVX.updateToggle = function(toggle, val) {
    var $input = toggle.find('input[value="' + val + '"]');
    $input.attr('checked', 'checked');
    $input.trigger('update');
  }

  Drupal.MegaMenuVX.unbindEvents = function(els) {
    self.megamenu_vx.data('nav_all', self.megamenu_vx.data('nav_all').not(els));
    els.unbind('mouseover').unbind('mouseout').unbind('click');
  }

  Drupal.MegaMenuVX.rebindEvents = function(els) {
    self.unbindEvents(els);
    self.bindEvents(els);
  }
  
  Drupal.behaviors.simpleMenuAction = {
    attach: function(context) {
      Drupal.MegaMenuVX.init();
    }
  }  
  
})(jQuery);
