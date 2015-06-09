<?php
namespace Yauc\Lister\Page;

class Queue extends \Gulei\Page\Form\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return null;
  }

  private function isStageQueue($status)
  {
    return !in_array(\Yauc\Lister\Provider\_Abstract::Q_DONE, $status)
        && !in_array(\Yauc\Lister\Provider\_Abstract::Q_CANCEL, $status);
  }

  protected function getToolbarBottom()
  {
    $calls = array('Mark_as_CANCEL', 'Mark_as_NEW', 'Trial_upload', 'Upload');
    return $this->getLocalCalleeEle($calls);
  }

  public function hook_form_internal()
  {
    list($sellerId, $status) = $this->args;
    $arch = \Gulei\File\Helper::getPathContent('module', 'yauc_lister', '/xml/viewQueue.xml');
    $prov = new \Yauc\Lister\Provider\Queue($sellerId, $status);

    $form = array();
    $form['sel_id'] = array(
      '#type' => 'value',
      '#value' => $sellerId,
    );

    $form['title'] = array(
      '#type' => 'markup',
      '#value' => "<h1><em>$sellerId</em></h1>",
    );

    if($this->isStageQueue($status)){
      $t = new \Yauc\Lister\Table\QueueSelect($arch, $prov->getData());
      $form['view'] = $t->getFormEles();
      $form['preview'] = array(
        '#type' => 'markup',
        '#value' => '<iframe id="preview-iframe"></iframe>',
      );
    }
    else{
      $t = new \Gulei\Table\Arch($arch, $prov);
      $form['view'] = array(
        '#type' => 'markup',
        '#value' => $t->theme(),
      );
    }

    drupal_add_css(drupal_get_path('module', 'yauc_lister') . '/yauc_lister.css');
    drupal_add_js(drupal_get_path('module', 'yauc_lister') . '/yauc_lister.js');
    return $form;
  }
}