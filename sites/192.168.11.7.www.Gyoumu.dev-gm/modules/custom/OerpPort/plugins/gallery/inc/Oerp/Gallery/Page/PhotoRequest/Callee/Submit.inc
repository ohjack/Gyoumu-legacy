<?php
namespace Oerp\Gallery\Page\PhotoRequest\Callee;

class Submit extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function validate($form, &$form_state)
  {
    $vals = $form_state['values'];
    $lines = \Gulei\Text\Helper::explode($vals['input']);

    $q = new \Oerp\Query\Basic('product.product', array(array('state', '<>', 'obsolete')), array('default_code'));
    $skus = $q->raw()->exe('collect', array('default_code'));
    $skus = array_fill_keys($skus, true);
    $msg = '';

    $toPhotos = array();
    foreach($lines as $line){
      $sku = strtoupper(substr($line, 0, 8));
      if(!isset($skus[$sku])){
        $msg .= 'Unknown SKU: ' . $line . "<br/>";
      }
      else if(preg_match("@[:/\\\\*\\?]@", $line)){
        $msg .= 'Folder name has invalid characters: ' . $line . '<br/>';
      }
      else{
        $toPhotos[] = $sku . substr($line, 8);
      }
    }

    if($msg){
      form_error($form['input'], $msg);
    }
    else{
      $form_state['storage']['toPhoto'] = $toPhotos;
    }
  }

  public function execute($form, &$form_state)
  {
    $toPhotos = $form_state['storage']['toPhoto'];
    $fe = new \Oerp\Gallery\File\Editor\toPhoto();
    $fe->append($toPhotos);

    drupal_set_message('Photo request queued:<br/>' . implode('<br/>', $toPhotos));
  }
}