<?php
namespace Oerp\View;

class Helper
{
  static public function getCustomTreeForm($module, $filepath){
    $arch = file_get_contents(
      drupal_get_path('module', $module) . $filepath);

    $form = oerp_ViewTree_form($form_state, 0, array('arch' => $arch));
    return $form;
  }
}