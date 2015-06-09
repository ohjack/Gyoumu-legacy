<?php
namespace Oerp\Gallery\Page;

class PhotoSubmit extends \Gulei\Page\Form\_Abstract
{
  private $isSubmitable;

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return array();
  }

  protected function hook_form_internal()
  {
    $h = new \Oerp\Gallery\Helper();
    $sub = $h->getPhotoSubmission();

    $form = array();
    $form['msg'] = array(
      '#type' => 'markup',
      '#value' => $sub->msg
    );

    $this->isSubmitable = $sub->isSubmitable;
    return $form;
  }

  protected function getToolbarBottom()
  {
    if($this->isSubmitable){
      return $this->getLocalCalleeEle(array('Submit'));
    }
    return array();
  }
}