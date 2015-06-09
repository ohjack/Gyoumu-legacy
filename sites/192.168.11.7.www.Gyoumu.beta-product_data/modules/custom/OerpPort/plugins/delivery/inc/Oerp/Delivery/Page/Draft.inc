<?php
namespace Oerp\Delivery\Page;

class Draft extends \Gulei\Page\Form\Abstract_WithArch
{
  const ENTRYNAME = 'order_list';

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getFilepath()
  {
    return __DIR__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function getToolbarBottom()
  {
    $calls = array(
      'Mark_as_READY',
      'Delete',
    );
    return $this->getLocalCalleeEle($calls);
  }

  protected function hook_form_internal()
  {
    $Form = new \Oerp\Arch\Form($this->getArch());

    $form = $Form->getFormEntry(self::ENTRYNAME);
    $form['#attributes']['class'] = 'oerp-formview';
    $form['#attributes']['enctype'] = 'multipart/form-data';
    return $form;
  }

  public function getSelection()
  {
    $sels = parent::getSelection(array(self::ENTRYNAME, 'sels'));
    return array_map('intval', $sels);
  }
}