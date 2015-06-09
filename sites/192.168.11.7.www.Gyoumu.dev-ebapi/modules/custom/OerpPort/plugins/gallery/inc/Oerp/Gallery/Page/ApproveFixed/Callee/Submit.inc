<?php
namespace Oerp\Gallery\Page\ApproveFixed\Callee;

class Submit extends \Gulei\Page\Form\Callee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  private function clear($root, $fn, $refixed = false, $inTofix = false)
  {
    $fn = $refixed ? '_' . $fn . '_' : $fn;

    if($inTofix){
      $exeRm = sprintf('rm "./%s/tofix/%s"*', $root, $fn);
    }
    else{
      $exeRm = sprintf('rm "./%s/tofix/fixed/%s"*', $root, $fn);
    }

    exec($exeRm);
  }

  private function getFixedFn($path)
  {
    preg_match('@(.*)/tofix/fixed/_?([^_]*_[^_]*)(_.*)?.jpg@', $path, $m);
    return $m;
  }

  private function reject($path, $note)
  {
    list($path, $root, $fn) = $this->getFixedFn($path);

    $exeCp = sprintf(
      'cp "./%s" "./%s/tofix/_%s_(%s).jpg"',
        $path, $root, $fn, $note
    );

    exec($exeCp);
    $this->clear($root, $fn, true);
  }

  private function approve($path)
  {
    list($path, $root, $fn) = $this->getFixedFn($path);

    $exeMv = sprintf('mv "./%s" "./%s/seed/%s.jpg"', $path, $root, $fn);
    exec($exeMv);

    $this->clear($root, $fn);
  }

  private function delete($path)
  {
    list($path, $root, $fn) = $this->getFixedFn($path);
    $this->clear($root, $fn);
    $this->clear($root, $fn, true);
    $this->clear($root, $fn, true, true);
  }

  public function execute($form, &$form_state)
  {
    $vals = $form_state['values'];

    foreach($vals as $key => $val){
      if($val && preg_match('/^(pic-\d)-delete$/', $key, $m)){
        $this->delete($form[$m[1]]['#path']);
        continue;
      }

      if(preg_match('/^pic-\d*$/', $key)){
        if($val){
          $path = $form[$key]['#refixed'];
          $path = $path ? $path : $form[$key]['#path'];
          $this->approve($path);
        }
        else{
          if(($note = $vals[$key . '-note']) && (!$form[$key]['#attributes']['disabled'])){
            $this->reject($form[$key]['#path'], $note);
          }
        }
      }
    }
  }
}