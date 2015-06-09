<?php
use Gulei as G;
use Oerp as O;

//module_load_include('inc', 'GuLib', '/include/GuTable');
//module_load_include('inc', 'GuLib', '/include/page');
//require_once 'provider.inc';

function _OerpViewSnippet_form(&$form_state, $eles)
{
  return array('view' => $eles);
}


class OerpViewController
{
  static public function getCustomTreeForm($module, $filepath){
    $arch = file_get_contents(
      drupal_get_path('module', $module) . $filepath);

    $form = oerp_ViewTree_form($form_state, 0, array('arch' => $arch));
    return $form;
  }
}