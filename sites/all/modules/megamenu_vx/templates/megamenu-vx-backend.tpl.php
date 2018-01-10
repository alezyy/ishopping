<div id="megamenu-vx-config">
  <div class="megamenu-vx-config-header">
    <div id="megamenu-vx-config-groups">
        <div id="megamenu-vx-config-menu" class="config-group">
          <h3><?php print t('Menu Config') ?></h3>
          <p><?php print t('This toolbox includes all settings of megamenu, just select menu then configure. There are 3 level of configuration: sub-megamenu setting, column setting and menu item setting.') ?></p>
          <ul>
            <li>
              <label class="hasTip" title="<?php print t('Select Direction for this Menu');?>"><?php print t('Direction') ?></label>
              <fieldset class="select-group">
                <select name="megamenu-vx-direction" class="config-select" data-action="changeDirection">
                  <?php print $direction_options;?>
                </select>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Select Preset for this Menu');?>"><?php print t('Presets') ?></label>
              <fieldset class="select-group">
                <select name="megamenu-vx-style" class="config-select" data-action="changePreset">
                  <?php print $preset_options;?>
                </select>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Select animation for this megamenu - CSS3 Animation');?>"><?php print t('Animation') ?></label>
              <fieldset class="select-group">
                <select name="megamenu-vx-animation" class="config-select" data-action="changeAnimation">
                  <?php print $animation_options;?>
                </select>
              </fieldset>
            </li>
            <li class="animation-param" <?php print $block_config->animation == 'none' ? 'style="display: none"' : '';?>>
              <label class="hasTip" title="<?php print t('Delay time of the animation - this field must be an integer.');?>"><?php print t('Delay (ms)') ?></label>
              <fieldset class="text-group">
                <input class="input-medium config-input" name="megamenu-vx-delay" type="text" value="<?php print $block_config->delay;?>" data-name="delay" data-action="changeAnimationDelay"></input>
              </fieldset>
            </li>
            <li class="animation-param" <?php print $block_config->animation == 'none' ? 'style="display: none"' : '';?>>
              <label class="hasTip" title="<?php print t('Duration of the animation - this field must be an integer.');?>"><?php print t('Duration (ms)') ?></label>
              <fieldset class="text-group">
                <input class="input-medium config-input" name="megamenu-vx-duration" type="text" value="<?php print $block_config->duration;?>" data-name="duration" data-action="changeAnimationDuration"></input>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Show/hide the arrow next to items that have submenu.');?>"><?php print t('Auto arrow') ?></label>
              <fieldset class="radio btn-group toggle-auto-arrow" data-auto-arrow="<?php print $block_config->{'auto-arrow'};?>">
                <input type="radio" <?php print $block_config->{'auto-arrow'} ? '' : 'checked="checked"';?> value="0" name="megamenu-vx-auto-arrow" data-action="toggleAutoArrow" class="config-toggle megamenu-vx-auto-arrow" id="toggleAutoArrow0" value="0">
                <label for="toggleAutoArrow0" title="<?php print t('Hide the arrow next to items that have submenu.');?>">No</label>

                <input type="radio" <?php print $block_config->{'auto-arrow'} ? 'checked="checked"' : '';?> value="1" name="megamenu-vx-auto-arrow" data-action="toggleAutoArrow" class="config-toggle megamenu-vx-auto-arrow" id="toggleAutoArrow1" value="1">
                <label for="toggleAutoArrow1" title="<?php print t('Show the arrow next to items that have submenu.');?>">Yes</label>
              </fieldset>        
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Show or collapse submenus when browsing on small screens');?>"><?php print t('Always show submenu') ?></label>
              <fieldset class="radio btn-group toolitem-always-show-submenu" data-always-show-submenu="<?php print $block_config->{'always-show-submenu'};?>">
                <input type="radio" <?php print $block_config->{'always-show-submenu'} ? '' : 'checked="checked"';?> value="0" name="megamenu-vx-always-show-submenu" data-action="toggleAlwayShowSubmenu" class="config-toggle megamenu-vx-always-show-submenu" id="toggleAlwayShowSubmenu0">
                <label for="toggleAlwayShowSubmenu0" title="<?php print t('Collapse submenus when browsing on small screens');?>">No</label>
                <input type="radio" <?php print $block_config->{'always-show-submenu'} ? 'checked="checked"' : '';?> value="1" name="megamenu-vx-always-show-submenu" data-action="toggleAlwayShowSubmenu" class="config-toggle megamenu-vx-always-show-submenu" id="toggleAlwayShowSubmenu1">
                <label for="toggleAlwayShowSubmenu1" title="<?php print t('Show submenus when browsing on small screens');?>">Yes</label>
              </fieldset>
            </li>
          </ul>
        </div>
        
        <div id="megamenu-vx-config-li" class="config-group">
          <h3><?php print t('Item Configuration') ?></h3>
          <p><?php print t('This allows you to configure each link you added in the Drupal core menu. You can: add block, have it styled by adding extra class, set icon (Bootstrap icons) and add description.') ?></p>
          <ul>
            <li>
              <label class="hasTip" title="<?php print t('Submenu') . ' - ' . t('Enable or disable submenu') ?>"><?php print t('Submenu') ?></label>
              <fieldset class="radio-group toggle-item-sub">
                <input type="radio" id="toggleSub0" class="config-toggle" data-action="toggleSub" name="toggleSub" value="0"/>
                <label for="toggleSub0" title="<?php print t('Disable submenu') ?>"><?php print t('No') ?></label>
                <input type="radio" id="toggleSub1" class="config-toggle" data-action="toggleSub" name="toggleSub" value="1" checked="checked"/>
                <label for="toggleSub1" title="<?php print t('Enable submenu') ?>"><?php print t('Yes') ?></label>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Group') . ' - ' . t('Configure how this item’s submenu display.') ?>"><?php print t('Group') ?></label>
              <fieldset class="radio-group toggle-item-group">
                <input type="radio" id="toggleGroup0" class="config-toggle" data-action="toggleGroup" name="toggleGroup" value="0" checked="checked"/>
                <label for="toggleGroup0" title="<?php print t('Submenu items show only when hover/click on this tem.') ?>"><?php print t('No') ?></label>
                <input type="radio" id="toggleGroup1" class="config-toggle" data-action="toggleGroup" name="toggleGroup" value="1"/>
                <label for="toggleGroup1" title="<?php print t('Submenu items are visible under this item.') ?>"><?php print t('Yes') ?></label>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Break column') . ' - ' . t('Move the item to the left/right column, create new column if there’s none on the chosen side.') ?>"><?php print t('Break column') ?></label>
              <fieldset class="btn-group">
                <a href="" class="btn config-action btn-move-left" data-action="moveItemsLeft" title="<?php print t('Move the items to the left column.') ?>"><i class="fa fa-arrow-left"></i></a>
                <a href="" class="btn config-action btn-move-right" data-action="moveItemsRight" title="<?php print t('Move the items to the right column.') ?>"><i class="fa fa-arrow-right"></i></a>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Extra class') . ' - ' . t('Add extra class to style megamenu.') ?>"><?php print t('Extra class') ?></label>
              <fieldset class="text-group">
                <input type="text" class="input-medium config-input input-item-class" name="input-item-class" data-name="class" value="" data-action="changeClass" />
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Icon') . ' - ' . t('Add Icon for Menu Item. Click Icon label to visit Bootstrap icons page and get Icon Class. E.g.: icon-search') ?>"><a href="http://fortawesome.github.io/Font-Awesome/icons/" target="_blank"><i class="icon-search"></i><?php print t('Icon') ?></a></label>
              <fieldset class="text-group">
                <input type="text" class="input-medium config-input input-item-icon" name="input-item-icon" data-name="icon" value="" data-action="changeIcon" />
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Item caption') . ' - ' . t('Add caption to this item'); ?>"><?php print t('Item caption') ?></label>
              <fieldset class="text-group">
                <input type="text" class="input-large config-input input-item-caption" name="input-item-caption" data-name="caption" value="" data-action="changeCaption" />
              </fieldset>
            </li>
          </ul>
        </div>
        
        <div id="megamenu-vx-config-submenu" class="config-group">
          <h3><?php print t('Submenu Configuration') ?></h3>
          <p><?php print t('Contains all the level 2+ items. Allows you to: add and remove row, set the submenu’s position, have it styled, edit its width...') ?></p>
          <ul>
            <li>
              <label class="hasTip" title="<?php print t('Add row') . ' - ' . t('Add a new row to the selected submenu') ?>"><?php print t('Add row') ?></label>
              <fieldset class="btn-group">
                <a href="" class="btn config-action" data-action="addRow"><i class="fa fa-plus-square"></i></a>
              </fieldset>
            </li>

            <li>
              <label class="hasTip" title="<?php print t('Hide when collapse') . ' - ' . t('Hide this column when the menu is collapsed on small screens') ?>"><?php print t('Hide when collapse') ?></label>
              <fieldset class="radio-group toggle-sub-hidewhencollapse">
                <input type="radio" id="togglesubHideWhenCollapse0" class="config-toggle" data-action="hideWhenCollapse" name="togglesubHideWhenCollapse" value="0" checked="checked"/>
                <label for="togglesubHideWhenCollapse0" title="<?php print t('Keep showing this submenu when the menu is collapsed on small screens') ?>"><?php print t('No') ?></label>
                <input type="radio" id="togglesubHideWhenCollapse1" class="config-toggle" data-action="hideWhenCollapse" name="togglesubHideWhenCollapse" value="1"/>
                <label for="togglesubHideWhenCollapse1" title="<?php print t('Hide this submenu when the menu is collapsed on small screens') ?>"><?php print t('Yes') ?></label>
              </fieldset>
            </li>
            
            <li>
              <label class="hasTip" title="<?php print t('Submenu width (px)') . ' - ' . t('Set submenu width (in pixel), this field must be a integer') ?>"><?php print t('Submenu width (px)') ?></label>
              <fieldset class="text-group">
                <input type="text" class="config-input input-small input-submenu-width" name="input-submenu-width" data-name="width" value="" data-action="changeSubmenuWidth"/>
              </fieldset>
            </li>

            <li>
              <label class="hasTip" title="<?php print t('Alignment') . ' - ' . t('Align this submenu') ?>"><?php print t('Alignment') ?></label>
              <fieldset class="btn-group btn-alignment btn-submenu-alignment">
                <a class="btn config-action btn-alignment-left" href="#" data-action="alignment" data-align="left" title="<?php print t('Left') ?>"><i class="fa fa-align-left"></i></a>
                <a class="btn config-action btn-alignment-right" href="#" data-action="alignment" data-align="right" title="<?php print t('Right') ?>"><i class="fa fa-align-right"></i></a>
                <a class="btn config-action btn-alignment-center" href="#" data-action="alignment" data-align="center" title="<?php print t('Center') ?>"><i class="fa fa-align-center"></i></a>
                <a class="btn config-action btn-alignment-justify" href="#" data-action="alignment" data-align="justify" title="<?php print t('Justify') ?>"><i class="fa fa-align-justify"></i></a>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Extra class') . ' - ' . t('Add extra class to style megamenu') ?>"><?php print t('Extra class') ?></label>
              <fieldset class="text-group">
                <input type="text" class="config-input input-medium input-submenu-class" name="input-submenu-class" data-name="class" value="" data-action="changeClass" />
              </fieldset>
            </li>
          </ul>
        </div>
        
        <div id="megamenu-vx-config-col" class="config-group">
          <h3><?php print t('Column Configuration') ?></h3>
          <p><?php print t('Allows you to: add and remove column, set grid, add block to column and style the column with extra class') ?></p>
          <ul>
            <li>
              <label class="hasTip" title="<?php print t('Add/remove Column') . ' - ' . t('Click + to add a new column on the right of the selected column. Click - to remove the selected column') ?>"><?php print t('Add/remove Column') ?></label>
              <fieldset class="btn-group">
                <a href="" class="btn config-action" data-action="addColumn" title="<?php print t('Add a new column on the right of the selected column') ?>"><i class="fa fa-plus-square"></i></a>
                <a href="" class="btn config-action" data-action="removeColumn" title="<?php print t('Remove the selected column') ?>"><i class="fa fa-minus-square"></i></a>
              </fieldset>
            </li>

            <li>
              <label class="hasTip" title="<?php print t('Hide when collapse') . ' - ' . t('Hide this column when the menu is collapsed on small screens') ?>"><?php print t('Hide when collapse') ?></label>
              <fieldset class="radio-group toggle-col-hidewhencollapse">
                <input type="radio" id="toggleHideColWhenCollapse0" class="config-toggle" data-action="hideWhenCollapse" name="toggleHideColWhenCollapse" value="0" checked="checked"/>
                <label for="toggleHideColWhenCollapse0" title="<?php print t('Keep showing this submenu when the menu is collapsed on small screens') ?>"><?php print t('No') ?></label>
                <input type="radio" id="toggleHideColWhenCollapse1" class="config-toggle" data-action="hideWhenCollapse" name="toggleHideColWhenCollapse" value="1"/>
                <label for="toggleHideColWhenCollapse1" title="<?php print t('Hide this submenu when the menu is collapsed on small screens') ?>"><?php print t('Yes') ?></label>
              </fieldset>
            </li>

            <!--
            <li>
              <label class="hasTip" title="<?php print t('Hide when collapse') . ' - ' . t('Hide this column when the menu is collapsed on small screens') ?>"><?php print t('Hide when collapse') ?></label>
              <fieldset class="radio btn-group toolsub-hidewhencollapse">
                <input type="radio" id="togglesubHideWhenCollapse0" class="toolbox-toggle" data-action="hideWhenCollapse" name="togglesubHideWhenCollapse" value="0" checked="checked"/>
                <label for="togglesubHideWhenCollapse0" title="<?php print t('Keep showing this submenu when the menu is collapsed on small screens') ?>"><?php print t('No') ?></label>
                <input type="radio" id="togglesubHideWhenCollapse1" class="toolbox-toggle" data-action="hideWhenCollapse" name="togglesubHideWhenCollapse" value="1"/>
                <label for="togglesubHideWhenCollapse1" title="<?php print t('Hide this submenu when the menu is collapsed on small screens') ?>"><?php print t('Yes') ?></label>
              </fieldset>
            </li>
            -->

            <li>
              <label class="hasTip" title="<?php print t('Grid (1-12)') . ' - ' . t('Set number of grid columns the selected column spans') ?>"><?php print t('Grid (1-12)') ?></label>
              <fieldset class="select-group">
                <select class="config-select input-col-grid" name="input-col-grid" data-name="width" data-action="changeGrid">
                  <option value="1">1</option>
                  <option value="2">2</option>
                  <option value="3">3</option>
                  <option value="4">4</option>
                  <option value="5">5</option>
                  <option value="6">6</option>
                  <option value="7">7</option>
                  <option value="8">8</option>
                  <option value="9">9</option>
                  <option value="10">10</option>
                  <option value="11">11</option>
                  <option value="12">12</option>
                </select>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Blocks') . ' - ' . t('Add/replace a block to the selected column') ?>"><?php print t('Blocks') ?></label>
              <fieldset class="select-group">
                <select class="config-select input-col-block" name="input-col-block" data-name="block" data-action="changeBlock" data-placeholder="<?php print t('Select Block') ?>" style="width: 200px;">
                  <option value=""></option>
                  <?php
                  foreach ($block_options as $block_key => $block) {
                    print "<option value=\"{$block_key}\">" . $block . "</option>\n";
                  }
                  ?>
                </select>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Show block title');?>"><?php print t('Show block title') ?></label>
              <fieldset class="radio-group toggle-col-show-title">
                <input type="radio" id="toggleShowBlockTitle0" class="config-toggle" data-action="showBlockTitle" name="toggleShowBlockTitle" value="0"/>
                <label for="toggleShowBlockTitle0"><?php print t('No') ?></label>
                <input type="radio" id="toggleShowBlockTitle1" class="config-toggle" data-action="showBlockTitle" name="toggleShowBlockTitle" value="1" checked="checked"/>
                <label for="toggleShowBlockTitle1"><?php print t('Yes') ?></label>
              </fieldset>
            </li>
            <li>
              <label class="hasTip" title="<?php print t('Extra class') . ' - ' . t('Add extra class to style megamenu') ?>"><?php print t('Extra class') ?></label>
              <fieldset class="text-group">
                <input type="text" class="input-medium config-input input-col-class" name="input-col-class" data-name="class" value="" data-action="changeClass" />
              </fieldset>
            </li>
          </ul>
        </div>    
    </div>
      
    <div class="megamenu-vx-messages-group">
      <div id="toolbox-loading" class="toolbox-loading">&nbsp;</div>
      <div id="toolbox-message" class="toolbox-message">&nbsp;</div>
    </div>

    <div class="megamenu-vx-actions-group">
      <button class="btn btn-success config-action" data-action="saveConfig"><i class="icon-save"></i><?php print t('Save') ?></button>
      <button class="btn btn-danger config-action" data-action="resetConfig"><i class="icon-undo"></i><?php print t('Reset') ?></button>
    </div>
    <div class="megamenu-vx-links-group">
      <a href="<?php print $edit_menu;?>" target="_blank">Edit menu</a>
      <a href="<?php print $edit_links;?>" target="_blank">Edit links</a>
    </div>
  </div>

  <div id="megamenu-vx-config-content">
    <?php print $menu_content;?>
  </div>
</div>
