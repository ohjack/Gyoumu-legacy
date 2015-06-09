<?php

$path = getcwd();
preg_match('@^(.*)/sites/@', $path, $m);
chdir($m[1]);

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

chdir($path);

function OerpProduct_OverviewSupplier_form(&$form_state)
{
  $arch = GuFile::getPathContent('module', 'oerp', '/tests/xml/SearchPanelTest.xml');
  $form = oerp_ViewTree_form($form_state, 0, array('arch' => $arch));
  return $form;
}

class SearchPanelTest extends PHPUnit_Framework_TestCase
{
}

?>