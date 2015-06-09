<?php
namespace Gulei\Page\Form;

abstract class Abstract_WithArch extends \Gulei\Page\Form\_Abstract
{
  abstract protected function getDirPath();
  abstract protected function getEntryName();

  protected function getArch()
  {
    $toks = explode("\\", $this->getClassname());
    return file_get_contents($this->getDirPath() . '/' . array_pop($toks) . '.xml');
  }

  protected function hook_form_internal()
  {
    $Form = new \Oerp\Arch\Form($this->getArch());

    $form = $Form->getFormEntry($this->getEntryName());
    $form['#attributes']['class'] = 'oerp-formview';
    $form['#attributes']['enctype'] = 'multipart/form-data';
    return $form;
  }

  public function getSelection()
  {
    $sels = parent::getSelection(array($this->getEntryName(), 'sels'));
    return array_map('intval', $sels);
  }
}