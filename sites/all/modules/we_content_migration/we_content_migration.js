(function ($) {
  Drupal.behaviors.migrate_content = {
    attach: function(context, settings) {
      function updateComponentCountInfo(item, section) {
        switch (section) {
          case 'select':
            var parent = $(item).closest('.migrate-content-export-list').siblings('.migrate-content-export-component');
            $('.component-count', parent).text(function (index, text) {
                return +text + 1;
              }
            );
            break;
          case 'added':
          case 'detected':
            var parent = $(item).closest('.migrate-content-export-component');
            $('.component-count', parent).text(function (index, text) {
              return text - 1;
            });
        }
      }

      function moveCheckbox(item, section, value) {
        updateComponentCountInfo(item, section);
        var curParent = item;
        curParent = $(item).parents('.form-type-checkbox');
        var newParent = $(curParent).parents('.migrate-content-export-parent').find('.form-checkboxes.component-'+section);
        $(curParent).detach();
        $(curParent).appendTo(newParent);
        var list = ['select', 'added', 'detected', 'included'];
        for (i in list) {
          $(curParent).removeClass('component-' + list[i]);
          $(item).removeClass('component-' + list[i]);
        }
        $(curParent).addClass('component-'+section);
        $(item).addClass('component-'+section);
        if (value) {
          $(item).attr('checked', 'checked');
        }
        else {
          $(item).removeAttr('checked')
        }
/*        $(newParent).parent().removeClass('features-export-empty');*/
      }



      $('#migrate-content-export-wrapper .migrate-content-export-parent input[type=checkbox]:not(.processed)', context).addClass('processed').click(function() {
         var $entity = $(this).parents('.migrate-content-export-parent').attr('value');
         var $bundle = $(this).val();
         if ($(this).hasClass('component-select')) {
            moveCheckbox(this, 'added', true);
            _updateDetected($entity, $bundle, true);
          }
          else if ($(this).hasClass('component-included')) {
            moveCheckbox(this, 'added', false);
          }
          else if ($(this).hasClass('component-added')) {
            if ($(this).is(':checked')) {
              moveCheckbox(this, 'included', true);
            }
            else {
              moveCheckbox(this, 'select', false);
              _updateDetected($entity, $bundle, false);
            }
          }
      });

      function _updateDetected($entity, $bundle, $value) {
        var url = Drupal.settings.basePath + 'migrate_content/ajaxcallback/mcontent';
        var postData = {'entity': $entity, 'bundle': $bundle};
        jQuery.post(url, postData, function(data) {
            for (var component in data) {
              var itemList = data['field'];
              $('#migrate-content-export-wrapper .migrate-content-export-parent input[type=checkbox]', context).each(function() {
                var key = $(this).attr('value');
                // first remove any auto-detected items that are no longer in component
                if ($value == false) {
                  if ($.inArray(key, itemList) != -1) {
                    moveCheckbox(this, 'select', false);
                    $(this).parent().show(); // make sure it's not hidden from filter
                  }
                }
                // next, add any new auto-detected items
                else {
                  if ($.inArray(key, itemList) != -1) {
                    moveCheckbox(this, 'added', true);
                    $(this).parent().show(); // make sure it's not hidden from filter
                  }
                }
              });
            }
        }, "json");

      }
    }
  }
})(jQuery);