(function ($) {

Drupal.WeebPalBackend = Drupal.WeebPalBackend || {};

Drupal.behaviors.actionWeebPalBackend = {
  attach: function (context) {
    Drupal.WeebPalBackend.tab();
  }
};

Drupal.WeebPalBackend.tab = function() {
  $tabs = $("#content .section .tabs");
  if ($tabs.find("ul.secondary").length) {
    $tabs.addClass("tab-secondary");
  }
}

})(jQuery);
