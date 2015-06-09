<?php
namespace Gulei\Page;

abstract class Abstract_Content extends \Gulei\Page\Form\_Abstract
{
  abstract protected function content();

  protected function hook_form_internal()
  {
    $form = array();
    $form['content'] = array(
      '#type' => 'markup',
      '#value' => $this->content(),
    );
    return $form;
  }

  protected function getToolbarBottom()
  {
    return array();
  }

  protected function getParam()
  {
    return array();
  }
}