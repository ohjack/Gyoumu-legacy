<?php
namespace Oerp\Delivery\Page\Callee;

abstract class Abstract_Download extends \Gulei\Page\Form\Callee\_Abstract
{
  abstract protected function getQueryParam();

  protected function getArch() {
    return file_get_contents(__DIR__ . '/Abstract_Download.xml');
  }

  public function execute($form, &$form_state)
  {
    $arch = $this->getArch();
    $param = $this->getQueryParam();
    $param['no-blank-tail'] = true;

    $t = new \Oerp\Table\Arch($arch, $param);
    $fname = $t->getCsvFile($param, $this->getCaption());
    $form_state['storage']['redirect'] = $fname;
  }
}