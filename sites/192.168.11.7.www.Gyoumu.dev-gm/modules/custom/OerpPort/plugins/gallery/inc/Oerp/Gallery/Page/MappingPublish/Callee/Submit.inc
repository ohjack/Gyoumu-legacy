<?php
namespace Oerp\Gallery\Page\MappingPublish\Callee;

class Submit extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $vals = $form_state['values'];
    $h = new \Oerp\Gallery\Helper();

    foreach($vals as $name => $val){
      $path = explode('?', $val);
      $path = $path[0];
      $accnt = $vals['accnt'] == 'All' ? '_all' : $vals['accnt'];

      if(($fn = $h->extractSlotFn($name)) && !$vals['del-' . $name] && $path){
        $wm = preg_match('/.*-0$/', $name) ? '0' : '1';
        $newFn = sprintf(
          './%s/.processing/(%s-%s)%s.jpg', $h->getRoot(), $accnt, $wm, $fn);

        copy('./' . $path, $newFn);
      }
      else if(strpos($name, 'del-') === 0 && $val){
        $newFn = sprintf(
          './%s/.processing/(%s-_del_)%s', $h->getRoot(), $accnt, $form[$name]['#fn']);

        touch($newFn);
      }
    }

    $cmd = sprintf('sh "./%s/procMapped.sh" gallery', drupal_get_path('module', 'oerp_gallery'));
    exec($cmd);

    $form_state['storage']['values'] = array(
      'sku' => $vals['sku'],
      'suffix' => $vals['suffix'],
    );

    drupal_set_message('Mapping submitted.');
    return true;
  }

  public function validate($form, &$form_state)  {
    $vals = $form_state['values'];
    $form_state['rebuild'] = true;
    $Form = $this->FormPage;

    foreach($vals as $name => $val){
      if((strpos($name, $Form::ELEPREFIX) === 0 && $val)
          || (strpos($name, 'del-' . $Form::ELEPREFIX) === 0 && $val)
      ) {
        $form_state['rebuild'] = false;
        break;
      }
    }
  }
}